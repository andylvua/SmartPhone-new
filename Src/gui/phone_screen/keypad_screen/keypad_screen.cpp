//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

#include "gui/phone_screen/keypad_screen/keypad_screen.h"
#include "gui/utils/fa.h"


KeypadScreen::KeypadScreen(QWidget *parent, Controller *controller) : QWidget(parent), controller(controller) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);

    numberField = new QLineEdit(this);
    zeroLongClickTimer = new QTimer(this);

    numberField->setPlaceholderText("Enter number");
    numberField->setAlignment(Qt::AlignCenter);
    numberField->setObjectName("keypadNumberField");
    numberField->setFixedHeight(40);

    zeroLongClickTimer->setSingleShot(true);

    QPushButton *backspaceButton = new QPushButton(
            FA::getInstance()->icon(fa::fa_solid, fa::fa_backspace), "", this);
    backspaceButton->setFixedSize(60, 40);
    backspaceButton->setIconSize(QSize(20, 20));
    QHBoxLayout *numberFieldLayout = new QHBoxLayout();
    numberFieldLayout->addWidget(numberField);
    numberFieldLayout->addWidget(backspaceButton);

    connect(backspaceButton, &QPushButton::clicked, this, [this]() {
        numberField->backspace();
    });

    QGridLayout *keypadLayout = new QGridLayout();
    keypadLayout->setHorizontalSpacing(30);
    keypadLayout->setVerticalSpacing(15);
    keypadLayout->setAlignment(Qt::AlignCenter);

    const QStringList keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
    for (const QString &key: keys) {
        QPushButton *button = new QPushButton(key, this);
        button->setObjectName("keypadButton");
        button->setFixedSize(74, 74);

        keypadLayout->addWidget(button, (keys.indexOf(key) / 3), (keys.indexOf(key) % 3));

        if (key == "0") {
            connect(button, &QPushButton::pressed, [&]{ zeroLongClickTimer->start(1000); });
            connect(button, &QPushButton::released, this, &KeypadScreen::zeroReleased);
            connect(zeroLongClickTimer, &QTimer::timeout, this, &KeypadScreen::zeroLongClick);
        } else {
            connect(button, &QPushButton::clicked, this, [this, key]() {
                numberField->insert(key);
            });
        }
    }

    QHBoxLayout *callButtonLayout = new QHBoxLayout();
    QPushButton *callButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_phone), "",
                                              this);
    callButton->setFixedSize(74, 74);
    callButton->setIconSize(QSize(25, 25));
    callButton->setObjectName("keypadCallButton");

    callButtonLayout->addWidget(callButton);

    layout->addStretch();
    layout->addLayout(numberFieldLayout);
    layout->addStretch();
    layout->addLayout(keypadLayout);
    layout->addSpacing(10);
    layout->addLayout(callButtonLayout);
    layout->addStretch();

    connect(callButton, &QPushButton::clicked, this, &KeypadScreen::makeCall);

    connect(controller, &Controller::ussdResponse, this, &KeypadScreen::handleUSSDResponse);
}


void KeypadScreen::makeCall() {
    QString number = numberField->text();
    numberField->clear();
    QRegularExpression ussdPattern(R"(^\*[\d*]+#$)");
    if (ussdPattern.match(number).hasMatch()) {
        ussdDialog = new USSDDialog(this);
        qDebug() << "USSD code entered: " << number;
        controller->makeUSSDRequest(number);
        ussdDialog->exec();
    } else {
        qDebug() << "Calling number: " << number;
        controller->makeCall(number);
    }
}

void KeypadScreen::handleUSSDResponse(const QString &response) const {
    if (ussdDialog) {
        ussdDialog->setResponse(response);
    }
}

void KeypadScreen::zeroReleased() const {
    if(zeroLongClickTimer->isActive()){
        zeroLongClickTimer->stop();
        numberField->insert("0");
    }
}

void KeypadScreen::zeroLongClick() const {
    numberField->insert("+");
}
