//
// Created by andrew on 26.08.24.
//

#ifndef CALL_DISPATCHER_H
#define CALL_DISPATCHER_H

#include <QObject>
#include <QStateMachine>
#include <QState>

#include "backend/modem/modem.h"
#include "backend/models/models.h"


class CallDispatcher : public QObject {
Q_OBJECT

    Modem *modem;

    QTimer *ringTimeout;

    Call currentCall;
    QElapsedTimer callTimer;
    QString clipNumber;
    bool sounder1 = false;

    QStateMachine *stateMachine;
    QState *idleState;
    QState *incomingCallState;
    QState *outgoingCallState;
    QState *inCallState;

    bool init = false;

public:
    explicit CallDispatcher(Modem *modem);

public slots:
    void callRequestHandler(const QString &number);

    void acceptCall();

    void rejectCall();

signals:
    void incomingCall(const QString &number);
    void callEnded();
    void callAnswered();
    void callFailed();
    void callMissed(const QString &number);

private slots:
    void handleClip(const QString &notification);

    void handleCiev(const QString &notification);

    void handleRing();

    void handleIncomingCall();

    void handleInCall();

    void handleIdle();

    void handleCallCommandResponse(ATCommand *command);

signals:
    void clipHandled();
    void callStateEntered();
    void callStateExited();
};


#endif //CALL_DISPATCHER_H
