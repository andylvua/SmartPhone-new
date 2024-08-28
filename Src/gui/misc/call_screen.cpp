//
// Created by andrew on 24.08.24.
//

#include <QVBoxLayout>
#include <QHBoxLayout>


#include "gui/misc/call_screen.h"
#include "gui/utils/fa.h"


CallScreen::CallScreen(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("CallScreen");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignTop);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignCenter);

    callerNameLabel = new QLabel("", this);
    callerNameLabel->setAlignment(Qt::AlignCenter);
    callerNameLabel->setObjectName("callCallerNameLabel");

    callStatusLabel = new QLabel("", this);
    callStatusLabel->setObjectName("callStatusLabel");
    callStatusLabel->setAlignment(Qt::AlignCenter);

    answerWidget = new QWidget(this);
    rejectWidget = new QWidget(this);

    QPushButton *answerButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_phone_flip), "",
                                   this);
    rejectButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_phone_hangup), "",
                                   this);

    QVBoxLayout *answerButtonLayout = new QVBoxLayout();
    QVBoxLayout *rejectButtonLayout = new QVBoxLayout();

    answerButton->setFixedSize(74, 74);
    rejectButton->setFixedSize(74, 74);
    answerButton->setIconSize(QSize(25, 25));
    rejectButton->setIconSize(QSize(32, 32));

    answerButton->setObjectName("callAnswerButton");
    rejectButton->setObjectName("callRejectButton");

    answerButtonLayout->addWidget(answerButton);
    rejectButtonLayout->addWidget(rejectButton);

    QLabel *answerButtonLabel = new QLabel("Answer", this);
    endCallLabel = new QLabel("Reject", this);
    answerButtonLabel->setAlignment(Qt::AlignCenter);
    endCallLabel->setAlignment(Qt::AlignCenter);
    answerButtonLabel->setObjectName("callAnswerButtonLabel");
    endCallLabel->setObjectName("callEndCallLabel");

    answerButtonLayout->addWidget(answerButtonLabel);
    rejectButtonLayout->addWidget(endCallLabel);

    answerWidget->setLayout(answerButtonLayout);
    rejectWidget->setLayout(rejectButtonLayout);

    buttonsLayout->setSpacing(50);

    buttonsLayout->addWidget(answerWidget);
    buttonsLayout->addWidget(rejectWidget);

    layout->addSpacing(70);
    layout->addWidget(callerNameLabel);
    layout->addWidget(callStatusLabel);
    layout->addStretch();
    layout->addLayout(buttonsLayout);
    layout->addSpacing(70);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    connect(hideTimer, &QTimer::timeout, this, [this]() {
        emit callEnded();
    });
    callTimer = new QElapsedTimer();

    connect(answerButton, &QPushButton::clicked, this, &CallScreen::answerCall);
    connect(rejectButton, &QPushButton::clicked, this, &CallScreen::rejectCall);
}


void CallScreen::answerCall() {
    setupInCall();
    emit callAnswered();
}

void CallScreen::rejectCall() {
    handleRejectCall();
    emit callRejected();
}


void CallScreen::handleIncomingCall(const QString &callerNumber, const QString &callerName) {
    setCallerInfo(callerName, callerNumber);
    setupIncomingCall();
    hideTimer->stop();
}

void CallScreen::handleOutgoingCall(const QString &receiverNumber, const QString &receiverName) {
    setCallerInfo(receiverName, receiverNumber);
    setupOutCall();
    hideTimer->stop();
}

void CallScreen::handleAnswerCall() {
    setupInCall();
}

void CallScreen::handleRejectCall() const {
    setupEndCall();
}

void CallScreen::setupInCall() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!callTimer->isValid()) {
            return;
        }
        int elapsed = callTimer->elapsed() / 1000;
        callStatusLabel->setText(
                QString("%1:%2").arg(elapsed / 60, 2, 10, QChar('0')).arg(elapsed % 60, 2, 10, QChar('0')));
    });
    timer->QTimer::qt_metacall(QMetaObject::InvokeMetaMethod, 5, {});
    timer->start(1000);

    callTimer->start();
    callStatusLabel->setText("00:00");
    answerWidget->hide();
    endCallLabel->setText("End Call");
    rejectWidget->show();
}

void CallScreen::setupOutCall() const {
    callStatusLabel->setText("Calling");
    answerWidget->hide();
    endCallLabel->setText("End Call");
    rejectWidget->show();
}

void CallScreen::setupIncomingCall() const {
    callStatusLabel->setText("Incoming Call");
    answerWidget->show();
}

void CallScreen::setupEndCall() const {
    callStatusLabel->setText("Call Ended");
    callTimer->invalidate();
    answerWidget->hide();
    rejectButton->setDisabled(true);
    hideTimer->start(1500);
}
