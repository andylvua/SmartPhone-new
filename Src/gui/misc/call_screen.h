//
// Created by andrew on 24.08.24.
//

#ifndef CALL_SCREEN_H
#define CALL_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QElapsedTimer>
#include <QTimer>


class CallScreen : public QWidget {
Q_OBJECT

    QLabel *callerNameLabel;
    QLabel *callStatusLabel;
    QWidget *answerWidget;
    QWidget *rejectWidget;
    QPushButton *rejectButton;
    QLabel *endCallLabel;
    QElapsedTimer *callTimer;
    QTimer *hideTimer;

public:
    explicit CallScreen(QWidget *parent = nullptr);

    void setCallerInfo(const QString &name, const QString &number) {
        callerNameLabel->setText(name.isEmpty() ? number : name);
    }

signals:

    void callAnswered();

    void callRejected();

    void callEnded();

private slots:

    void answerCall();

    void rejectCall();

public slots:

    void handleIncomingCall(const QString &callerNumber, const QString &callerName);

    void handleOutgoingCall(const QString &receiverNumber, const QString &receiverName);

    void handleAnswerCall();

    void handleRejectCall() const;

private:

    void setupInCall();

    void setupOutCall() const;

    void setupIncomingCall() const;

    void setupEndCall() const;
};

#endif //CALL_SCREEN_H
