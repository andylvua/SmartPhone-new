//
// Created by andrew on 27.08.24.
//

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QFile>


enum class PppdExitStatus : std::uint8_t {
    SUCCESS = 0,                             // Connection established and terminated successfully
    FATAL_ERROR = 2,                         // Immediate fatal error
    OPTION_ERROR = 3,                        // Error processing options
    PERMISSION_ERROR = 4,                     // Not setuid-root and user is not root
    KERNEL_SUPPORT_ERROR = 5,                 // Kernel does not support PPP
    SIGNAL_TERMINATION = 6,                   // Terminated by signal (SIGINT, SIGTERM, SIGHUP)
    SERIAL_LOCK_ERROR = 7,                    // Serial port could not be locked
    SERIAL_OPEN_ERROR = 8,                    // Serial port could not be opened
    CONNECT_SCRIPT_FAILURE = 9,               // Connect script failed
    PTY_COMMAND_ERROR = 10,                   // Command specified with pty option could not run
    NEGOTIATION_FAILURE = 11,                 // PPP negotiation failed
    PEER_AUTH_FAILURE = 12,                   // Peer system failed to authenticate
    IDLE_TIMEOUT = 13,                        // Terminated due to idle timeout
    CONNECT_TIME_LIMIT = 14,                  // Terminated due to connect time limit
    CALLBACK_PENDING = 15,                    // Callback negotiated, incoming call expected
    PEER_UNRESPONSIVE = 16,                   // Peer not responding to echo requests
    MODEM_HANGUP = 17,                        // Terminated by modem hangup
    SERIAL_LOOPBACK_DETECTED = 18,            // Negotiation failed due to loopback
    INIT_SCRIPT_FAILURE = 19,                 // Init script failed
    LOCAL_AUTH_FAILURE = 20,                  // Failed to authenticate to peer
    DETACHED = 1, // Connection detached or terminated by peer
};

inline QString toString(PppdExitStatus status) {
    switch (status) {
        case PppdExitStatus::SUCCESS: return "Success";
        case PppdExitStatus::FATAL_ERROR: return "Immediate fatal error";
        case PppdExitStatus::OPTION_ERROR: return "Error processing options";
        case PppdExitStatus::PERMISSION_ERROR: return "Permission error";
        case PppdExitStatus::KERNEL_SUPPORT_ERROR: return "Kernel does not support PPP";
        case PppdExitStatus::SIGNAL_TERMINATION: return "Terminated by signal";
        case PppdExitStatus::SERIAL_LOCK_ERROR: return "Serial port could not be locked";
        case PppdExitStatus::SERIAL_OPEN_ERROR: return "Serial port could not be opened";
        case PppdExitStatus::CONNECT_SCRIPT_FAILURE: return "Connect script failed";
        case PppdExitStatus::PTY_COMMAND_ERROR: return "Command specified with pty option could not run";
        case PppdExitStatus::NEGOTIATION_FAILURE: return "PPP negotiation failed";
        case PppdExitStatus::PEER_AUTH_FAILURE: return "Peer system failed to authenticate";
        case PppdExitStatus::IDLE_TIMEOUT: return "Terminated due to idle timeout";
        case PppdExitStatus::CONNECT_TIME_LIMIT: return "Terminated due to connect time limit";
        case PppdExitStatus::CALLBACK_PENDING: return "Callback negotiated, incoming call expected";
        case PppdExitStatus::PEER_UNRESPONSIVE: return "Peer not responding to echo requests";
        case PppdExitStatus::MODEM_HANGUP: return "Terminated by modem hangup";
        case PppdExitStatus::SERIAL_LOOPBACK_DETECTED: return "Negotiation failed due to loopback";
        case PppdExitStatus::INIT_SCRIPT_FAILURE: return "Init script failed";
        case PppdExitStatus::LOCAL_AUTH_FAILURE: return "Failed to authenticate to peer";
        case PppdExitStatus::DETACHED: return "Connection detached or terminated by peer";
        default: return "Unknown error";
    }
}

class NetworkManager : public QObject {
    Q_OBJECT

    QLocalServer *server;
    QProcess *pppdProcess = nullptr;

public:
    NetworkManager();

public slots:

    void turnOnMobileData();

    void turnOffMobileData() const;

signals:

    void connected();
    void error(PppdExitStatus exitCode);
    void disconnected();
    void terminated();

private slots:

    void onNewPPPConnection();
};

#endif //NETWORK_MANAGER_H
