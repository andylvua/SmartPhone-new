//
// Created by andrew on 26.08.24.
//

#include <QStateMachine>
#include <QState>
#include <QJsonDocument>
#include <QJsonArray>
#include <QList>

#include "backend/modem/dispatchers/call_dispatcher.h"
#include "backend/utils/cache_manager.h"


CallDispatcher::CallDispatcher(Modem *modem) : modem(modem) {
    connect(modem->getNotificationManager(), &NotificationDispatcher::clip, this, &CallDispatcher::handleClip);
    connect(modem->getNotificationManager(), &NotificationDispatcher::ciev, this, &CallDispatcher::handleCiev);
    connect(modem->getNotificationManager(), &NotificationDispatcher::ring, this, &CallDispatcher::handleRing);

    loadCountryDialCodes();

    ringTimeout = new QTimer(this);
    ringTimeout->setInterval(1000);
    ringTimeout->setSingleShot(true);

    stateMachine = new QStateMachine(this);

    idleState = new QState(stateMachine);
    incomingCallState = new QState(stateMachine);
    outgoingCallState = new QState(stateMachine);
    inCallState = new QState(stateMachine);

    idleState->addTransition(this, &CallDispatcher::clipHandled, incomingCallState);
    idleState->addTransition(ringTimeout, &QTimer::timeout, incomingCallState);

    idleState->addTransition(this, &CallDispatcher::callStateEntered, outgoingCallState);

    incomingCallState->addTransition(this, &CallDispatcher::callStateEntered, inCallState);
    incomingCallState->addTransition(this, &CallDispatcher::callStateExited, idleState);

    outgoingCallState->addTransition(this, &CallDispatcher::callAnswered, inCallState);
    outgoingCallState->addTransition(this, &CallDispatcher::callStateExited, idleState);

    inCallState->addTransition(this, &CallDispatcher::callStateExited, idleState);

    // on enter to incoming call state, call handler handleIncomingCall
    connect(incomingCallState, &QState::entered, this, &CallDispatcher::handleIncomingCall);
    connect(inCallState, &QState::entered, this, &CallDispatcher::handleInCall);
    connect(idleState, &QState::entered, this, &CallDispatcher::handleIdle);

    connect(this, &CallDispatcher::callStateExited, this, &CallDispatcher::callEnded);

    stateMachine->setInitialState(idleState);
    stateMachine->start();
}


void CallDispatcher::callRequestHandler(const QString &number) {
    if (!stateMachine->configuration().contains(idleState)) {
        return;
    }

    QString modifiedNumber = number;
    const QList<decltype(countryDialCodes)::key_type> &keys = countryDialCodes.keys();

    for (const QString &dialCode : keys) {
        if (number.startsWith(dialCode.mid(1))) {
            qDebug() << "Call manager: Dial code found. Country: " << countryDialCodes[dialCode];
            modifiedNumber = "+" + number;
            break;
        }
    }

    currentCall = Call{};
    currentCall.callDirection = callDirection::CD_OUTGOING;
    currentCall.callResult = callResult::CR_NO_ANSWER;
    currentCall.contact.number = modifiedNumber;
    currentCall.startTime = QDateTime::currentDateTime();

    modem->getATChat()->chat("ATD" + modifiedNumber + ";",
        this, SLOT(handleCallCommandResponse(const ATCommand &)));
}

void CallDispatcher::acceptCall() {
    modem->getATChat()->chat("ATA"); // maybe later handle call command response
}

void CallDispatcher::rejectCall() {
    modem->getATChat()->chat("AT+CHUP");
}

void CallDispatcher::loadCountryDialCodes() {
    QFile file(":/assets/country_codes.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open country dial codes JSON file");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        qWarning("Invalid JSON format for country dial codes");
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    for (const auto &value : jsonArray) {
        QJsonObject obj = value.toObject();
        QString dialCode = obj["dial_code"].toString();
        QString countryName = obj["name"].toString();
        countryDialCodes.insert(dialCode, countryName);
    }
}

void CallDispatcher::handleClip(const QString &notification) {
    QString number = notification.split(",")[0].remove("\"").trimmed();
    if (ringTimeout->isActive()) {
        ringTimeout->stop();
    }

    clipNumber.clear();
    clipNumber = number;
    emit clipHandled();
}

void CallDispatcher::handleCiev(const QString &notification) {
    if (notification.trimmed().startsWith("\"CALL\"")) {
        int state = notification.split(",")[1].toInt();
        if (state == 0) {
            emit callStateExited();
        } else if (state == 1) {
            emit callStateEntered();
        }
    }

    if (notification.trimmed().startsWith("\"SOUNDER\"")) {
        int state = notification.split(",")[1].toInt();
        if (state == 0) {
            if (sounder1) {
                QTimer::singleShot(500, this, [this]() {
                    if (!stateMachine->configuration().contains(idleState)) {
                        currentCall.callResult = callResult::CR_ANSWERED;
                        emit callAnswered();
                    }
                });
                sounder1 = false;
            } else {
                qDebug() << "Call failed: NO SOUNDER1";
            }
        } else if (state == 1) {
            sounder1 = true;
        }
    }
}

void CallDispatcher::handleRing() {
    qDebug() << "Call manager: RING";
    if (stateMachine->configuration().contains(idleState)) {
        ringTimeout->start();
    }
}

void CallDispatcher::handleIncomingCall() {
    currentCall = Call{};
    currentCall.callDirection = callDirection::CD_INCOMING;
    currentCall.contact.number = clipNumber;
    currentCall.startTime = QDateTime::currentDateTime();
    currentCall.callResult = callResult::CR_NO_ANSWER;
    qDebug() << "Call manager: Incoming call from: " << clipNumber;
    emit incomingCall(clipNumber);
}

void CallDispatcher::handleInCall() {
    currentCall.callResult = callResult::CR_ANSWERED;
    callTimer.start();
    qDebug() << "Call manager: Call answered";
}

void CallDispatcher::handleIdle() {
    if (!init) {
        init = true;
        return;
    }
    sounder1 = false;
    currentCall.duration = callTimer.isValid() ? callTimer.elapsed() / 1000 : 0;
    currentCall.uuid = QUuid::createUuid();
    callTimer.invalidate();

    qDebug() << "Call manager: adding call to cache: " << currentCall.contact.number;
    CacheManager::addCall(currentCall);

    // if missed call -- notify others
    if (currentCall.callDirection == callDirection::CD_INCOMING &&
        currentCall.callResult == callResult::CR_NO_ANSWER) {
        emit callMissed(currentCall.contact.number);
    }
}

void CallDispatcher::handleCallCommandResponse(const ATCommand &command) {
    if (command.result != AT_OK) {
        emit callFailed();
    }
}
