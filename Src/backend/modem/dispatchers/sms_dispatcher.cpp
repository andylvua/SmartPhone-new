//
// Created by andrew on 26.08.24.
//

#include "backend/modem/dispatchers/sms_dispatcher.h"
#include "backend/utils/cache_manager.h"


SMSDispatcher::SMSDispatcher(Modem *modem) : modem(modem) {
    SMSTimeoout = new QTimer(this);
    SMSTimeoout->setSingleShot(true);

    connect(modem->getNotificationManager(), &NotificationDispatcher::cmgs, this, &SMSDispatcher::onCMGS);
    connect(modem->getNotificationManager(), &NotificationDispatcher::cds, this, &SMSDispatcher::onCDS);
    connect(modem->getNotificationManager(), &NotificationDispatcher::cmti, this, &SMSDispatcher::onCMTI);

    connect(SMSTimeoout, &QTimer::timeout, this, &SMSDispatcher::SMSReadyTimeout);
}

void SMSDispatcher::checkForIncomingSMS() {
    modem->getATChat()->chat("AT+CMGL=\"REC UNREAD\"", this, SLOT(onReadListSMS(const ATCommand &)));
}


void SMSDispatcher::sendSMS(const QUuid &uuid, const QString &recipient, const QString &message) {
    SMSPending *smsPending = new SMSPending();
    smsPending->uuid = uuid;
    smsPending->recipient = recipient;
    smsPending->message = message;
    smsPending->retries = 2;
    smsPending->result = at_command_res_t::AT_OK;
    smsQueue.enqueue(smsPending);

    if (smsQueue.head() == smsPending) {
        sendNextSMS();
    }
}

void SMSDispatcher::sendNextSMS() {
    sending = true;
    SMSTimeoout->start(15'000);

    if (smsQueue.isEmpty()) {
        return;
    }

    SMSPending *smsPending = smsQueue.head();
    qDebug() << "Sending SMS (MANAGER): " << smsPending->message;
    modem->getATChat()->chat("AT+CMGS=" + smsPending->recipient, smsPending->message, this,
                      SLOT(onTransmitSMS(const ATCommand &)), 100000);
}

void SMSDispatcher::SMSReadyTimeout() {
    while (!smsQueue.isEmpty()) {
        SMSPending *smsPending = smsQueue.dequeue();
        smsPending->result = at_command_res_t::AT_OK;
        emit smsStatusChanged(smsPending->uuid, delivery_status_t::DS_FAILED);
        CacheManager::updateMessageStatus(smsPending->uuid, delivery_status_t::DS_FAILED);
        qDebug() << "SMS failed: TIMEOUT";
        delete smsPending;
    }
}

void SMSDispatcher::onTransmitSMS(const ATCommand &command) {
    SMSTimeoout->stop();

    if (command.result == at_command_res_t::AT_OK) {
        emit smsStatusChanged(smsQueue.head()->uuid, delivery_status_t::DS_SENT);
        CacheManager::updateMessageStatus(smsQueue.head()->uuid, delivery_status_t::DS_SENT);
        qDebug() << "SMS sent";
        smsQueue.dequeue();
        sendNextSMS();
        return;
    }

    SMSPending *smsPending = smsQueue.head();
    if (--smsPending->retries > 0) {
        qDebug() << "Retries left: " << smsPending->retries;
        QTimer::singleShot(1000, this, SLOT(sendNextSMS()));
        return;
    }

    smsPending->result = command.result;
    CacheManager::updateMessageStatus(smsPending->uuid, delivery_status_t::DS_FAILED);
    emit smsStatusChanged(smsPending->uuid, delivery_status_t::DS_FAILED);
    smsQueue.dequeue();
    sendNextSMS();
}

void SMSDispatcher::onCMGS(const QString &notification) {
    if (smsQueue.isEmpty()) {
        return;
    }

    SMSPending *smsPending = smsQueue.head();
    waitingDeliveryReport[notification.toInt()] = smsPending->uuid;
}

void SMSDispatcher::onCDS(const QString &notification) {
    QStringList parts = notification.split(",");
    if (parts.size() < 2) {
        return;
    }

    int internalModemSmsId = parts[1].toInt();
    QUuid smsId = waitingDeliveryReport.value(internalModemSmsId);
    if (!smsId.isNull()) {
        waitingDeliveryReport.remove(internalModemSmsId);
        emit smsStatusChanged(smsId, delivery_status_t::DS_DELIVERED);
        CacheManager::updateMessageStatus(smsId, delivery_status_t::DS_DELIVERED);
        qDebug() << "SMS delivered";
    }
}

void SMSDispatcher::onCMTI(const QString &notification) {
    QStringList parts = notification.split(",");
    if (parts.size() < 2) {
        return;
    }

    int internalModemSmsId = parts[1].toInt();
    readIncomingSMS(internalModemSmsId);
}

QDateTime dateTimeFromSMSHeader(const QString &header) {
    QRegExp timeRe("\\d{4}/\\d{2}/\\d{2},\\d{2}:\\d{2}:\\d{2}[+-]\\d{2}");
    timeRe.indexIn(header);
    return QDateTime::fromString(timeRe.cap(0), "yyyy/MM/dd,HH:mm:sszzz");
}

QStringList headerPartsFromSMSHeader(const QString &header) {
    QRegExp re("(\\+\\w+:)\\s*(.+)");
    if (re.indexIn(header) < 0) {
        qDebug() << "Failed to parse SMS";
        return {};
    }
    QString value = re.cap(2);
    return value.split(",");
}

void SMSDispatcher::processSMS(const QString &header, const QString &message, int internalModemSmsId = -1) {
    auto dateTime = dateTimeFromSMSHeader(header);

    QRegExp re("(\\+\\w+:)\\s*(.+)");
    if (re.indexIn(header) < 0) {
        qDebug() << "Failed to parse SMS";
        return;
    }
    auto type = re.cap(1);
    auto value = re.cap(2);
    auto parts = value.split(",");

    if (type == "+CMGL:") {
        internalModemSmsId = parts[0].toInt();
        // delete 0 entry from parts
        parts.removeAt(0);
    } else if (type != "+CMGR:") {
        qDebug() << "Unknown SMS notification: " << type;
        return;
    }

    bool alreadyRead = parts[0].contains("REC READ");
    if (alreadyRead) {
        return;
    }

    auto number = parts[1].remove("\"").trimmed();

    Message msg = Message(QUuid::createUuid().toString(), number, message, dateTime, messageDirection::MD_INCOMING);
    qDebug() << "Incoming SMS: " <<
                "Number: " << number <<
                "Message: " << message <<
                "DateTime: " << dateTime.toString("dd.MM.yyyy hh:mm:ss");

    CacheManager::addMessage(msg);
    emit smsReceived(msg);

    deleteMessage(internalModemSmsId);
}

void SMSDispatcher::onReadSMS(ATCommand *command) {
    if (command->result != at_command_res_t::AT_OK) {
        qDebug() << "Failed to read SMS";
        return;
    }
    qDebug() << "Read SMS: " << command->response;

    QStringList notification = command->response.split("\r\n");
    QString message = notification[1];

    processSMS(notification[0], message, command->command.split("=")[1].toInt());
}

void SMSDispatcher::onReadListSMS(const ATCommand &command) {
    if (command.result != at_command_res_t::AT_OK) {
        qDebug() << "Failed to read SMS list";
        return;
    }

    QStringList notifications = command.response.split("\r\n");
    auto lineIdx = 0;
    while (lineIdx < notifications.size()) {
        if (!notifications[lineIdx].startsWith("+CMGL:")) {
            lineIdx++;
            continue;
        }

        // todo: parse SMS function
        QString header = notifications[lineIdx];
        QString message = notifications[++lineIdx];
        processSMS(header, message);
    }
}

void SMSDispatcher::onDeleteSMS(ATCommand *command) {
    if (command->result != at_command_res_t::AT_OK) {
        qDebug() << "Failed to delete SMS with id: " << command->command.split("=")[1].toInt();
        return;
    }
}

void SMSDispatcher::readIncomingSMS(int internalModemSmsId) {
    modem->getATChat()->chat("AT+CMGR=" + QString::number(internalModemSmsId), this, SLOT(onReadSMS(ATCommand * )));
}

void SMSDispatcher::deleteMessage(int internalModemSmsId) {
    modem->getATChat()->chat("AT+CMGD=" + QString::number(internalModemSmsId), this, SLOT(onDeleteSMS(ATCommand * )));
}
