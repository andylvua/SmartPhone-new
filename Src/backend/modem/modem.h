//
// Created by andrew on 26.08.24.
//

#ifndef MODEM_H
#define MODEM_H

#include <QObject>
#include <QString>

#include "backend/at_chat.h"
#include "backend/modem/dispatchers/notification_dispatcher.h"

class Modem : public QObject {
Q_OBJECT

public:
    struct ModemInfo {
        QString manufacturer;
        QString model;
        QString revision;
        QString number;
        QString operatorName;
        bool registrationStatus = false;
    };

private:
    QStringList initCommands = {
            "AT",
            "AT+CPIN?",
            "AT+CMEE=1",
            "AT+CMGF=1",
            R"(AT+CPMS="ME","ME","ME")",
            R"(AT+CSCS="HEX")",
            "AT+CGMI",
            "AT+CGMM",
            "AT+CGMR",
            "AT+CGSN",
            "AT+CNMI=2,1,0,1,0",
            "AT+CLIP=1",
            "AT+COPS=3,0",
            "AT+COPS?",
            "AT+CREG?"
    };
    int initPos = 0;

    ModemInfo modemInfo;
    SerialPort *serialPort;
    ATChat *atChat;
    NotificationDispatcher *notificationManager;

public:
    explicit Modem(SerialPort *serialPort);

    void init();

    [[nodiscard]] ATChat *getATChat() const {
        return atChat;
    }

    [[nodiscard]] NotificationDispatcher *getNotificationManager() const {
        return notificationManager;
    }

    ModemInfo &getModemInfo() {
        return modemInfo;
    }

    void closeSerialPort();
    void openSerialPort();

signals:
    void initComplete();

    void operatorNameChanged(const QString &operatorName);
    void simCardReady(bool ready);

private slots:
    void onInitResponse(const ATCommand &command);

};


#endif //MODEM_H
