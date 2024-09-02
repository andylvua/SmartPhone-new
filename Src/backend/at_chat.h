//
// Created by andrew on 26.08.24.
//

#ifndef AT_CHAT_H
#define AT_CHAT_H

#include <QObject>
#include <QQueue>
#include <QTimer>

#include "backend/at_command.h"
#include "backend/serial_port.h"


enum prefix_t {
    TERMINATOR,
    NOTIFICATION,
    PDU_NOTIFICATION,
    TERMINATOR_OR_NOTIFICATION,
    COMMAND_ECHO,
    UNKNOWN
};

class ATChat : public QObject {
Q_OBJECT

    QQueue<ATCommand*> commandQueue;
    SerialPort *m_serialPort = nullptr;
    QByteArray m_readData;

    QString lastNotification;
    QTimer *timeoutTimer;
    int timeout = 500;
    int retries = 0;

    enum OperatingMode {
        NORMAL,
        AT_CONSOLE,
    } mode = NORMAL;

public:
    explicit ATChat(SerialPort *serialPort);

    void chat(const QString &command);

    void chat(const QString &command, QObject *receiver, const char *slot,
              int timeout = 500, int retries = 0);

    void chat(const QString &command, const QString &pdu, QObject *receiver, const char *slot,
              int timeout = 500, int retries = 0);

public slots:
    void setATConsoleMode(bool enabled);

signals:
    void notification(const QString &type, const QString &notification);
    void pduNotification(const QString &type, const QByteArray &notification);
    void callNotification(const QString &notification);
    void urc(const QString &notification);


private:
    void done(ATCommand *command);

    bool processLine(const QString &line);

    void execute(bool force = false);

private slots:
    void handleReadyRead();

    void handleTimeout();

    static void handleError(QSerialPort::SerialPortError serialPortError);

    void sendCommand(const QString &command) const;

signals:
    void commandRequested(const QString &command);
};


#endif //AT_CHAT_H
