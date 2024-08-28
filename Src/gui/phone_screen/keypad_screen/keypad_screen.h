//
// Created by andrew on 24.08.24.
//

#ifndef KEYPAD_SCREEN_H
#define KEYPAD_SCREEN_H

#include <QWidget>
#include <QLineEdit>

#include "controller/controller.h"
#include "gui/phone_screen/keypad_screen/ussd_dialog.h"

class KeypadScreen : public QWidget {
Q_OBJECT

    QLineEdit *numberField;
    Controller *controller;
    USSDDialog *ussdDialog;
    QTimer *zeroLongClickTimer;

public:
    explicit KeypadScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

private slots:

    void makeCall();

    void handleUSSDResponse(const QString &response) const;

    void zeroReleased() const;

    void zeroLongClick() const;
};

#endif //KEYPAD_SCREEN_H
