//
// Created by andrew on 26.08.24.
//

#ifndef SMS_DISPATCHER_H
#define SMS_DISPATCHER_H

#include <QUuid>
#include <QQueue>
#include <QTimer>
#include <QHash>

#include "backend/modem/modem.h"
#include "backend/models/models.h"


struct SMSPending {
    QUuid uuid;
    QString message;
    QString recipient;
    int retries;
    at_command_res_t result;
};

class SMSDispatcher : public QObject {
Q_OBJECT

    Modem *modem;
    QQueue<SMSPending*> smsQueue;
    QTimer *SMSTimeoout;
    QHash<int, QUuid> waitingDeliveryReport;
    bool sending = false;

public:
    explicit SMSDispatcher(Modem *modem);

    void checkForIncomingSMS();

public slots:
    void sendSMS(const QUuid &uuid, const QString &recipient, const QString &message);

private slots:
    void sendNextSMS();

    void SMSReadyTimeout();

    void onTransmitSMS(const ATCommand &command);

    void onCMGS(const QString &notification);

    void onCDS(const QString &notification);

    void onCMTI(const QString &notification);

    void onReadSMS(ATCommand *command);

    void onReadListSMS(const ATCommand &command);

    void onDeleteSMS(ATCommand *command);

signals:
    void smsStatusChanged(const QUuid &uuid, delivery_status_t status);

    void smsReceived(const Message &message);

private:

    void readIncomingSMS(int internalModemSmsId);

    void deleteMessage(int internalModemSmsId);

    void processSMS(const QString &header, const QString &message, int internalModemSmsId);
};

#endif //SMS_DISPATCHER_H
