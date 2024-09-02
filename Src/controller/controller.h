//
// Created by andrew on 24.08.24.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "backend/modem/modem.h"
#include "backend/modem/dispatchers/notification_dispatcher.h"
#include "backend/modem/dispatchers/call_dispatcher.h"
#include "backend/modem/dispatchers/sms_dispatcher.h"
#include "backend/utils/network_manager.h"


class Controller : public QObject {
Q_OBJECT

    Modem *modem;
    NotificationDispatcher *notificationManager;
    CallDispatcher *callManager;
    SMSDispatcher *smsManager;
    NetworkManager *networkManager;

public:
    explicit Controller(Modem *modem, QObject *parent = nullptr);

    Modem::ModemInfo getModemInfo() const;
    void getSignalStrength();

    void makeCall(const QString &number);
    void sendSMS(const QUuid &uuid, const QString &contactOrNumber, const QString &message);

    void makeUSSDRequest(const QString &ussdCode) const;

    void setATConsoleMode(bool enabled) const;
    void sendATCommand(const QString &command, QObject *receiver, const char *slot) const;

    void restart() const;

    void turnOnMobileData() const;
    void turnOffMobileData() const;

public slots:
    void rejectCall() const;
    void acceptCall() const;

signals:
    void incomingCall(const QString &callerNumber, const QString &callerName = "");
    void outgoingCall(const QString &calleeNumber, const QString &calleeName = "");
    void callAnswered();
    void callEnded();
    void callMissed(const QString &callerNumber, const QString &callerName = "");

    void smsStatusChanged(const QUuid &uuid, delivery_status_t status);
    void smsReceived(const Message &message);

    void initComplete();
    void ussdResponse(const QString &response);
    void urcReceived(const QString &urc);
    void operatorChanged(const QString &operatorName);
    void signalStrengthChanged(int signalStrength);
    void simCardReady(bool ready);

    void GPRSConnected();
    void GPRSError(PppdExitStatus exitCode);
    void GPRSDisconnected();
    void GPRSTerminated();

private slots:
    void onSignalStrengthResponse(const ATCommand &command);

    void handleInitComplete();
    void handleIncomingCall(const QString &callerNumber);
    void handleOutgoingCall(const QString &calleeNumber);
    void handleMissedCall(const QString &callerNumber);

    void handleUSSDResponse(const QString &response);

signals:
    void callRequested(const QString &contactOrNumber);
    void smsRequested(const QUuid &uuid, const QString &contactOrNumber, const QString &message);
};

#endif //CONTROLLER_H
