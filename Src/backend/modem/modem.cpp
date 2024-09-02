//
// Created by andrew on 26.08.24.
//

#include <QRegularExpression>

#include "backend/modem/modem.h"


Modem::Modem(SerialPort *serialPort) : serialPort(serialPort) {
    atChat = new ATChat(serialPort);
    notificationManager = new NotificationDispatcher(atChat);
}

void Modem::init() {
    initPos = 0;
    atChat->chat(initCommands.at(initPos), this, SLOT(onInitResponse(const ATCommand &)));
}

void Modem::onInitResponse(const ATCommand &command) {
    if (command.result == AT_OK) {
        if (command.command == "AT+CPIN?") {
            emit simCardReady(command.response.contains("READY"));
        } else if (command.command == "AT+CGMI") {
            qDebug() << "Manufacturer: " << command.response;
            modemInfo.manufacturer = command.response;
        } else if (command.command == "AT+CGMM") {
            qDebug() << "Model: " << command.response;
            modemInfo.model = command.response;
        } else if (command.command == "AT+CGMR") {
            qDebug() << "Revision: " << command.response;
            modemInfo.revision = command.response;
        } else if (command.command == "AT+CGSN") {
            qDebug() << "Number: " << command.response;
            modemInfo.number = command.response;
        } else if (command.command == "AT+COPS?") {
            QString operatorName;
            QRegularExpression re("^\\+COPS:.*\"(.*)\"");
            QRegularExpressionMatch match = re.match(command.response);
            if (match.hasMatch()) {
                operatorName = match.captured(1);
            }
            qDebug() << "Operator: " << operatorName;
            modemInfo.operatorName = operatorName;
            emit operatorNameChanged(operatorName);
        } else if (command.command == "AT+CREG?") {
            QRegularExpression re("^\\+CREG: *\\d+ *,\\s*(\\d+)\\s*$");
            QRegularExpressionMatch match = re.match(command.response);
            if (match.hasMatch()) {
                int status = match.captured(1).toInt();
                qDebug() << "Registration status: " << status;
                modemInfo.registrationStatus = status == 1 || status == 5;
            }
        }
    } else if (command.result == AT_ERROR) {
        qDebug() << "Initialization failed, command: " << command.command;
    } else if (command.result == AT_TIMEOUT) {
        qDebug() << "Initialization failed, command: " << command.command << " timeout";
    }

    initPos++;
    if (initPos >= initCommands.size()) {
        qDebug() << "Initialization complete";
        emit initComplete();
    } else {
        qDebug() << "Sending next command: " << initCommands.at(initPos);
        atChat->chat(initCommands.at(initPos), this, SLOT(onInitResponse(ATCommand )));
    }
}

void Modem::closeSerialPort() const {
    serialPort->close();
}

void Modem::openSerialPort() const {
    serialPort->openSerialPort();
}
