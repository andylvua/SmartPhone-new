//
// Created by andrew on 24.08.24.
//

#include "controller/controller.h"
#include "backend/utils/cache_manager.h"
#include "backend/utils/decoder.h"


Controller::Controller(Modem *modem, QObject *parent) : QObject(parent) {
    this->modem = modem;
    notificationManager = new NotificationDispatcher(modem->getATChat());
    callManager = new CallDispatcher(modem);
    smsManager = new SMSDispatcher(modem);
    networkManager = new NetworkManager();

    connect(this, &Controller::callRequested, callManager, &CallDispatcher::callRequestHandler);
    connect(this, &Controller::callRequested, this, &Controller::handleOutgoingCall);
    connect(this, &Controller::smsRequested, smsManager, &SMSDispatcher::sendSMS);

    connect(modem, &Modem::initComplete, this, &Controller::handleInitComplete);
    connect(modem, &Modem::operatorNameChanged, this, &Controller::operatorChanged);
    connect(modem, &Modem::simCardReady, this, &Controller::simCardReady);
    connect(modem->getATChat(), &ATChat::urc, this, &Controller::urcReceived);

    connect(smsManager, &SMSDispatcher::smsStatusChanged, this, &Controller::smsStatusChanged);
    connect(smsManager, &SMSDispatcher::smsReceived, this, &Controller::smsReceived);

    connect(callManager, &CallDispatcher::incomingCall, this, &Controller::handleIncomingCall);
    connect(callManager, &CallDispatcher::callAnswered, this, &Controller::callAnswered);
    connect(callManager, &CallDispatcher::callEnded, this, &Controller::callEnded);
    connect(callManager, &CallDispatcher::callMissed, this, &Controller::handleMissedCall);

    connect(notificationManager, &NotificationDispatcher::cusd, this, &Controller::handleUSSDResponse);

    connect(networkManager, &NetworkManager::connected, this, &Controller::GPRSConnected);
    connect(networkManager, &NetworkManager::error, this, &Controller::GPRSError);
    connect(networkManager, &NetworkManager::disconnected, this, &Controller::GPRSDisconnected);
    connect(networkManager, &NetworkManager::disconnected, modem, &Modem::openSerialPort);
    connect(networkManager, &NetworkManager::terminated, this, &Controller::GPRSTerminated);

    CacheManager::checkCacheFiles();
    CacheManager::startupPopulate();

    //simulate incoming sms
    QTimer::singleShot(5000, this, [this]() {
        Message message{QUuid::createUuid(), "+380679027620", "Hello, this is a test message", QDateTime::currentDateTime(), messageDirection::MD_INCOMING};
    CacheManager::addMessage(message);
        emit smsReceived(message);
    });
    QTimer::singleShot(5000, this, [this]() {
        Message message{QUuid::createUuid(), "+380679027620", "Hello, this is a test message", QDateTime::currentDateTime(), messageDirection::MD_INCOMING};
    CacheManager::addMessage(message);
        emit smsReceived(message);
    });
}

Modem::ModemInfo Controller::getModemInfo() {
    return modem->getModemInfo();
}

void Controller::makeCall(const QString &number) {
    emit callRequested(number);
}

void Controller::makeUSSDRequest(const QString &ussdCode) {
    modem->getATChat()->chat("AT+CUSD=1,\"" + ussdCode + "\",15");
}

void Controller::sendSMS(const QUuid &uuid, const QString &contactOrNumber, const QString &message) {
    qDebug() << "Controller: Sending SMS to:" << contactOrNumber << "Message:" << message;
    emit smsRequested(uuid, contactOrNumber, message);
}

void Controller::setATConsoleMode(bool enabled) {
    modem->getATChat()->setATConsoleMode(enabled);
}

void Controller::sendATCommand(const QString &command, QObject *receiver, const char *slot) {
    modem->getATChat()->chat(command, receiver, slot);
}

void Controller::getSignalStrength() {
    modem->getATChat()->chat("AT+CSQ", this, SLOT(onSignalStrengthResponse(const ATCommand &)));
}

void Controller::restart() {
    modem->getATChat()->chat("AT+RST=1");
    QTimer::singleShot(17000, this, [this]() {
        modem->init();
    });
}

void Controller::turnOnMobileData() {
    modem->closeSerialPort();
    networkManager->turnOnMobileData();
}

void Controller::turnOffMobileData() {
    networkManager->turnOffMobileData();
}

void Controller::rejectCall() {
    callManager->rejectCall();
}

void Controller::acceptCall() {
    callManager->acceptCall();
}

void Controller::onSignalStrengthResponse(const ATCommand &command) {
    if (command.result == AT_OK) {
        QRegExp re("^\\+CSQ: (\\d+),\\d+");
        if (re.indexIn(command.response, 0) >= 0) {
            qDebug() << "Signal strength: " << re.cap(1).toInt();
            emit signalStrengthChanged(re.cap(1).toInt());
        }
    }
}

void Controller::handleInitComplete() {
    QTimer::singleShot(3000, this, [this]() {
        smsManager->checkForIncomingSMS();
    });
    emit initComplete();
}
void Controller::handleIncomingCall(const QString &callerNumber) {
    if (callerNumber.isEmpty()) {
        emit incomingCall(callerNumber, "Unknown");
        return;
    }
    Contact contact = CacheManager::getContact(callerNumber);
    if (contact.name.isEmpty()) {
        emit incomingCall(callerNumber);
    } else {
        emit incomingCall(callerNumber, contact.name);
    }
}

void Controller::handleOutgoingCall(const QString &calleeNumber) {
    Contact contact = CacheManager::getContact(calleeNumber);
    if (contact.name.isEmpty()) {
        emit outgoingCall(calleeNumber);
    } else {
        emit outgoingCall(calleeNumber, contact.name);
    }
}

void Controller::handleMissedCall(const QString &callerNumber) {
    Contact contact = CacheManager::getContact(callerNumber);
    if (contact.name.isEmpty()) {
        emit callMissed(callerNumber);
    } else {
        emit callMissed(callerNumber, contact.name);
    }
}

void Controller::handleUSSDResponse(const QString &response) {
    qDebug() << "USSD response:" << response;

    ussdEncoding encoding;

    if (response.right(3) == ",15") {
        encoding = ussdEncoding::UE_GSM7;
    } else if (response.right(3) == ",72") {
        encoding = ussdEncoding::UE_UCS2;
    } else {
        encoding = ussdEncoding::UE_UNKNOWN;
    }

    QString debugEncoding = response.right(3);
    qDebug() << "USSD encoding:" << debugEncoding;

    QString body = response.split("\"")[1];
    qDebug() << "USSD body:" << body;
    QString decoded;

    if (encoding == ussdEncoding::UE_GSM7) {
        decoded = Decoder::decode7Bit(body);
    } else {
        qDebug() << "USSD UCS2 decoded:" << Decoder::decodeUCS2(body);
        decoded = Decoder::decodeUCS2(body);
    }

    qDebug() << "USSD decoded:" << decoded;
    emit ussdResponse(decoded);
}


