//
// Created by andrew on 27.08.24.
//

#include "network_manager.h"


NetworkManager::NetworkManager() {
    QFile::remove("/tmp/ppp_socket.sock");

    server = new QLocalServer(this);
    if (!server->listen("/tmp/ppp_socket.sock")) {
        qDebug() << "Unable to start the server: " << server->errorString();
        return;
    }

    connect(server, &QLocalServer::newConnection, this, &NetworkManager::onNewPPPConnection);
}

void NetworkManager::turnOnMobileData() {
    if (pppdProcess) {
        qDebug() << "PPP connection already established!";
        return;
    }

    pppdProcess = new QProcess(this);
    pppdProcess->start("/usr/bin/sudo", QStringList() << "/usr/bin/pppd" << "call" << "gsm" << "nodetach" << "maxfail" << "2");

    connect(pppdProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus) {
        uint8_t exitCodeU8 = static_cast<uint8_t>(exitCode);
        exitCodeU8 += 1;

        switch (exitCodeU8) {
            case 0:
                break;
            case 6:
                emit terminated();
                break;
            default:
                emit error(static_cast<PppdExitStatus>(exitCodeU8));
        }

        pppdProcess->deleteLater();
        pppdProcess = nullptr;
    });
}

void NetworkManager::turnOffMobileData() const {
    pppdProcess->terminate();
}

void NetworkManager::onNewPPPConnection() {
    QLocalSocket *clientConnection = server->nextPendingConnection();
    connect(clientConnection, &QLocalSocket::readyRead, this, [this, clientConnection]() {
        QByteArray data = clientConnection->readAll();
        QString status = QString::fromUtf8(data).trimmed();
        if (status == "CONNECT") {
            qDebug() << "PPP connection successful!";
            emit connected();
        } else if (status == "DISCONNECT") {
            emit disconnected();
        }
        clientConnection->disconnectFromServer();
    });
}
