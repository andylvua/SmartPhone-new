//
// Created by andrew on 26.08.24.
//

#include <QRegularExpression>

#include "backend/at_chat.h"

namespace {
    QMap<at_command_res_t, QString> commandResReverse = {
        {AT_OK, "OK"},
        {AT_ERROR, "ERROR"},
        {AT_CONNECT, "CONNECT"},
        {AT_NO_CARRIER, "NO CARRIER"},
        {AT_NO_ANSWER, "NO ANSWER"},
        {AT_BUSY, "BUSY"},
        {AT_NO_DIALTONE, "NO DIALTONE"},
        {AT_TIMEOUT, "TIMEOUT"}
    };

    QMap<QString, at_command_res_t> commandRes = {
        {"OK", AT_OK},
        {"ERROR", AT_ERROR},
        {"CONNECT", AT_CONNECT},
        {"NO CARRIER", AT_NO_CARRIER},
        {"NO ANSWER", AT_NO_ANSWER},
        {"BUSY", AT_BUSY},
        {"NO DIALTONE", AT_NO_DIALTONE},
        {"TIMEOUT", AT_TIMEOUT}
    };

    QMap<QString, prefix_t> prefixes = {
        {"OK", TERMINATOR},
        {"ERROR", TERMINATOR},
        {"+CME ERROR:", TERMINATOR},
        {"+CMS ERROR:", TERMINATOR},
        {"+EXT ERROR:", TERMINATOR},
        {"CONNECT", TERMINATOR_OR_NOTIFICATION},
        {"NO CARRIER", TERMINATOR_OR_NOTIFICATION},
        {"NO ANSWER", TERMINATOR_OR_NOTIFICATION},
        {"BUSY", TERMINATOR_OR_NOTIFICATION},
        {"NO DIALTONE", TERMINATOR_OR_NOTIFICATION},
        {"RING", NOTIFICATION},
        {"+CIEV:", NOTIFICATION},
        {"+CLIP:", NOTIFICATION},
        {"+CDS:", PDU_NOTIFICATION},
        {"+CMGS:", NOTIFICATION},
        {"+CMTI:", NOTIFICATION},
        {"+CUSD:", NOTIFICATION},
        {"VCON", TERMINATOR_OR_NOTIFICATION},
        {"AT", COMMAND_ECHO},
        {"at", COMMAND_ECHO}
    };

    prefix_t getPrefixType(const QString &response) {
        for (int i = response.length(); i > 0; i--) {
            QString prefix = response.left(i);
            if (prefixes.contains(prefix)) {
                return prefixes[prefix];
            }
        }
        return UNKNOWN;
    }

    at_command_res_t getCommandRes(const QString &response) {
        return commandRes.contains(response) ? commandRes[response] : AT_UNKNOWN_ERROR;
    }

    QByteArray fromHex(const QString& hex) {
        QByteArray bytes;
        int nibble;

        int flag = 0;
        int value = 0;
        int size = 0;
        for (int posn = 0; posn < hex.length(); ++posn) {
            if (uint ch = hex[posn].unicode(); ch >= '0' && ch <= '9' ) {
                nibble = ch - '0';
            } else if ( ch >= 'A' && ch <= 'F' ) {
                nibble = ch - 'A' + 10;
            } else if ( ch >= 'a' && ch <= 'f' ) {
                nibble = ch - 'a' + 10;
            } else {
                continue;
            }
            value = (value << 4) | nibble;
            flag = !flag;
            if ( !flag ) {
                bytes.resize( size + 1 );
                bytes[size++] = static_cast<char>(value);
                value = 0;
            }
        }

        return bytes;
    }
}

ATChat::ATChat(SerialPort *serialPort) :
        m_serialPort(serialPort) {
    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);

    connect(m_serialPort, &QSerialPort::readyRead, this, &ATChat::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &ATChat::handleError);
    connect(timeoutTimer, &QTimer::timeout, this, &ATChat::handleTimeout);

    connect(this, &ATChat::commandRequested, this, &ATChat::sendCommand);
}

void ATChat::chat(const QString &command) {
    if (!m_serialPort->isOpen()) {
        return;
    }

    ATCommand *atCommand = new ATCommand(command);
    commandQueue.enqueue(atCommand);

    if (commandQueue.head() == atCommand) {
        execute();
    }
}

void ATChat::chat(const QString &command, QObject *receiver, const char *slot,
                  int timeout, int retries) {
    if (!m_serialPort->isOpen()) {
        return;
    }

    ATCommand *atCommand = new ATCommand(command);
    this->timeout = timeout;
    this->retries = retries;
    commandQueue.enqueue(atCommand);
    connect(atCommand, SIGNAL(done(const ATCommand &)), receiver, slot);

    qDebug() << "Chatting: " << command;
    if (commandQueue.head() == atCommand) {
        qDebug() << "Executing command: " << command;
        execute();
    } else {
        qDebug() << "Waiting for command: " << command << " since command queue head is: " << commandQueue.head()->command;
    }
}

void ATChat::chat(const QString &command, const QString &pdu, QObject *receiver, const char *slot,
                  int timeout, int retries) {
    if (!m_serialPort->isOpen()) {
        return;
    }

    ATCommand *atCommand = new ATCommand(command, pdu);
    this->timeout = timeout;
    this->retries = retries;
    commandQueue.enqueue(atCommand);
    connect(atCommand, SIGNAL(done(const ATCommand &)), receiver, slot);

    if (commandQueue.head() == atCommand) {
        execute();
    }
}


void ATChat::setATConsoleMode(bool enabled) {
    if (enabled) {
        mode = AT_CONSOLE;
    } else {
        mode = NORMAL;
    }
}


void ATChat::done(ATCommand *command) {
    qDebug() << "Command done: " << command->command << " result: " << commandResReverse[command->result];
    timeoutTimer->stop();

    emit command->done(*command);
    delete command;
}

bool ATChat::processLine(const QString &line) {
    if (line.isEmpty()) {
        return false;
    }
    qDebug() << "Received: " << m_readData;

    ATCommand *command = nullptr;
    if (!commandQueue.isEmpty()) {
        command = commandQueue.head();
    }

    prefix_t prefixType = getPrefixType(m_readData);

    if (mode == AT_CONSOLE && prefixType != TERMINATOR && prefixType != COMMAND_ECHO) {
        if (command != nullptr) {
            command->response.append(command->response.isEmpty() ? "" : "\r\n");
            command->response.append(m_readData);
        }

        if (prefixType == NOTIFICATION || prefixType == TERMINATOR_OR_NOTIFICATION) {
            qDebug() << "Notification: " << m_readData;
            emit urc(m_readData);
        }
        m_readData.clear();
        return true;
    }

    switch (prefixType) {
        case UNKNOWN:
            if (!lastNotification.isEmpty()) {
                qDebug() << "PDU notification on Unknown: " << lastNotification;
                QString type = lastNotification;
                QByteArray pdu;

                if (type.count(',') >= 2) {
                    pdu = line.toLatin1();
                } else {
                    pdu = fromHex(line);
                }

                lastNotification.clear();

                emit pduNotification(type, pdu);
                return false;
            }

            if (command != nullptr) {
                command->response.append(command->response.isEmpty() ? "" : "\r\n");
                command->response.append(m_readData);
                return true;
            }

            qDebug() << "Unknown prefix type: " << m_readData;
            break;
        case TERMINATOR:
            if (command != nullptr) {
                commandQueue.dequeue();
                command->result = getCommandRes(m_readData);
                if (mode == AT_CONSOLE) {
                    command->response.append(command->response.isEmpty() ? "" : "\r\n");
                    command->response.append(m_readData);
                    qDebug() << "In TERMINATOR: appending response: " << m_readData;
                }
                done(command);
                execute();
                return true;
            }
            break;
        case COMMAND_ECHO:
            // We can consider retrying the command if the echo is not received
            break;

        case PDU_NOTIFICATION: {
            lastNotification = line;
            qDebug() << "PDU notification: " << line;
        }

        case NOTIFICATION: {
            if (m_readData.startsWith("RING")) {
                qDebug() << "Call notification: " << m_readData;
                emit callNotification(m_readData);
                break;
            }
            QRegularExpression re("(\\+\\w+:)\\s*(.+)");
            QRegularExpressionMatch match = re.match(m_readData);
            if (match.hasMatch()) {
                QString type = match.captured(1);
                QString value = match.captured(2);
                qDebug() << "Notification: " << type << " " << value;
                emit notification(type, value);
            }
            break;
        }

        case TERMINATOR_OR_NOTIFICATION:
            if (command != nullptr &&
                (command->command.startsWith("ATD") ||
                 command->command.startsWith("ATA") ||
                 command->command.startsWith("ATH") ||
                 command->command.startsWith("ATO") ||
                 command->command.startsWith("AT+CHLD="))) {
                commandQueue.dequeue();
                command->result = getCommandRes(m_readData);
                done(command);
                execute();
                return true;
            } else {
                qDebug() << "Call notification: " << m_readData;
                emit callNotification(m_readData);
            }
            break;
    }

    return false;
}

void ATChat::execute(bool force) {
    if (commandQueue.isEmpty()) {
        return;
    }

    ATCommand *command = commandQueue.head();
    if (command->executed && !force) {
        return;
    }

    qDebug() << "Executing command: " << commandQueue.head()->command;
    emit commandRequested(command->command);
    command->executed = true;
    if (timeout != -1) {
        timeoutTimer->start(timeout);
    }
}


void ATChat::handleReadyRead() {
    bool reset = false;
    char c;
    while (m_serialPort->getChar(&c)) {
        if (c == '\n') {
            reset |= processLine(m_readData);
            m_readData.clear();
        } else if (c != '\0' && c != '\r') {
            m_readData.append(c);
        }
    }

    if (reset && timeoutTimer->isActive()) {
        timeoutTimer->start(timeout);
    }

    if (m_readData.startsWith("> ") && !commandQueue.isEmpty()) {
        qDebug() << "Sending PDU: " << commandQueue.head()->pdu;
        ATCommand *command = commandQueue.head();
        m_serialPort->write(command->pdu);
        m_readData.clear();
    }
}

void ATChat::handleTimeout() {
    if (commandQueue.isEmpty()) {
        return;
    }

    ATCommand *command = commandQueue.head();
    if (--retries >= 0) {
        qDebug() << "Retrying command: " << command->command;
        execute(true);
    } else {
        qDebug() << "Command timeout: " << command->command;
        commandQueue.dequeue();
        command->result = AT_TIMEOUT;
        done(command);
        execute();
    }
}

void ATChat::handleError(QSerialPort::SerialPortError serialPortError) {
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << "An I/O error occurred while reading the data from the modem";
        QCoreApplication::exit(1);
    }
}

void ATChat::sendCommand(const QString &command) const {
    QByteArray data = command.toUtf8() + "\r\n";
    m_serialPort->write(data);
}
