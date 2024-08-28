//
// Created by andrew on 26.08.24.
//

#ifndef AT_COMMAND_H
#define AT_COMMAND_H

#include <QObject>
#include <QString>
#include <QByteArray>


enum at_command_res_t {
    AT_OK,
    AT_ERROR,
    AT_CONNECT,
    AT_NO_CARRIER,
    AT_NO_ANSWER,
    AT_BUSY,
    AT_NO_DIALTONE,
    AT_TIMEOUT,
    AT_UNKNOWN_ERROR
};

struct ATCommand : QObject {
    Q_OBJECT


public:
    explicit ATCommand(QString command) : command(std::move(command)) {}

    ATCommand(QString command, const QString& pdu) : command(std::move(command)) {
        this->pdu = pdu.toLatin1();
        this->pdu.append('\x1A');
    }

    QString command;
    QByteArray pdu;
    QString response;
    at_command_res_t result = AT_UNKNOWN_ERROR;
    bool executed = false;

    signals:
        void done(const ATCommand &command);
};

#endif //AT_COMMAND_H
