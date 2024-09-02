//
// Created by andrew on 26.08.24.
//

#include "backend/modem/dispatchers/sms_dispatcher.h"
#include "backend/utils/cache_manager.h"
#include "libs/pdulib/pdulib.h"


SMSDispatcher::SMSDispatcher(Modem *modem) : modem(modem) {
    SMSTimeoout = new QTimer(this);
    SMSTimeoout->setSingleShot(true);

    connect(modem->getNotificationManager(), &NotificationDispatcher::cmgs, this, &SMSDispatcher::onCMGS);
    connect(modem->getNotificationManager(), &NotificationDispatcher::cds, this, &SMSDispatcher::onCDS);
    connect(modem->getNotificationManager(), &NotificationDispatcher::cmti, this, &SMSDispatcher::onCMTI);

    connect(SMSTimeoout, &QTimer::timeout, this, &SMSDispatcher::SMSReadyTimeout);
}

void SMSDispatcher::checkForIncomingSMS() {
    modem->getATChat()->chat("AT+CMGL=0", this, SLOT(onReadListSMS(const ATCommand &)));
}


void SMSDispatcher::sendSMS(const QUuid &uuid, const QString &recipient, const QString &message) {
    SMSPending *smsPending = new SMSPending();
    smsPending->uuid = uuid;
    smsPending->recipient = recipient;
    smsPending->message = message;
    smsPending->retries = 2;
    smsPending->result = AT_OK;
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
    PDU pdu;
    int length = pdu.encodePDU(smsPending->recipient.toStdString().c_str(),
                                    smsPending->message.toStdString().c_str(), messageReference);


    modem->getATChat()->chat("AT+CMGS=" + QString::number(length), pdu.getSMS(), this,
                      SLOT(onTransmitSMS(const ATCommand &)), 100000);
}

void SMSDispatcher::SMSReadyTimeout() {
    while (!smsQueue.isEmpty()) {
        SMSPending *smsPending = smsQueue.dequeue();
        smsPending->result = AT_OK;
        emit smsStatusChanged(smsPending->uuid, delivery_status_t::DS_FAILED);
        CacheManager::updateMessageStatus(smsPending->uuid, delivery_status_t::DS_FAILED);
        qDebug() << "SMS failed: TIMEOUT";
        delete smsPending;
    }
}

void SMSDispatcher::onTransmitSMS(const ATCommand &command) {
    SMSTimeoout->stop();

    if (command.result == AT_OK) {
        waitingDeliveryReport[messageReference++] = smsQueue.head()->uuid;
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
    Q_DECL_UNUSED_MEMBER
}

void SMSDispatcher::onCDS(const QByteArray &notification) {
    qDebug() << "SMS delivery report: " << notification;

    int index = 1;
    index += notification[0] & 0xff;
    ++index;

    if (notification.size() < index + 1)
        return;

    int internalModemSmsId = notification[index] & 0xff;
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

void SMSDispatcher::processSMS(const QString &header, const QString &message, int internalModemSmsId = -1) {
    QRegularExpression re("(\\+\\w+:)\\s*(.+)");
    QRegularExpressionMatch match = re.match(header);
    if (!match.hasMatch()) {
        qDebug() << "Failed to parse SMS";
        return;
    }
    auto type = match.captured(1);
    auto value = match.captured(2);
    auto parts = value.split(",");

    if (type == "+CMGL:") {
        internalModemSmsId = parts[0].toInt();
        // delete 0 entry from parts
        parts.removeAt(0);
    } else if (type != "+CMGR:") {
        qDebug() << "Unknown SMS notification: " << type;
        return;
    }

    bool alreadyRead = parts[0].toInt() == 1;
    if (alreadyRead) {
        return;
    }

    PDU pdu;
    pdu.decodePDU(message.toStdString().c_str());
    QString number = pdu.getSender();
    QString message_text = pdu.getText();
    QDateTime dateTime = QDateTime::fromString(pdu.getTimeStamp(), "yyMMddHHmmsszz");
    dateTime = dateTime.addYears(100);
    Message msg = Message(QUuid::createUuid(), number, message_text, dateTime, messageDirection::MD_INCOMING);
    qDebug() << "Incoming SMS: " <<
                "Number: " << number <<
                "Message: " << message_text <<
                "DateTime: " << dateTime.toString("dd.MM.yyyy hh:mm:ss");

    CacheManager::addMessage(msg);
    emit smsReceived(msg);

    deleteMessage(internalModemSmsId);
}

void SMSDispatcher::onReadSMS(const ATCommand &command) {
    if (command.result != AT_OK) {
        qDebug() << "Failed to read SMS";
        return;
    }
    qDebug() << "Read SMS: " << command.response;

    QStringList notification = command.response.split("\r\n");
    const QString& message = notification[1];

    processSMS(notification[0], message, command.command.split("=")[1].toInt());
}

void SMSDispatcher::onReadListSMS(const ATCommand &command) {
    if (command.result != AT_OK) {
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

        const QString& header = notifications[lineIdx];
        const QString& message = notifications[++lineIdx];
        processSMS(header, message);
    }
}

void SMSDispatcher::onDeleteSMS(const ATCommand &command) {
    if (command.result != AT_OK) {
        qDebug() << "Failed to delete SMS with id: " << command.command.split("=")[1].toInt();
    }
}

void SMSDispatcher::readIncomingSMS(int internalModemSmsId) {
    modem->getATChat()->chat("AT+CMGR=" + QString::number(internalModemSmsId), this, SLOT(onReadSMS(const ATCommand &)));
}

void SMSDispatcher::deleteMessage(int internalModemSmsId) {
    modem->getATChat()->chat("AT+CMGD=" + QString::number(internalModemSmsId), this, SLOT(onDeleteSMS(const ATCommand &)));
}
