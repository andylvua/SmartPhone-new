//
// Created by andrew on 24.08.24.
//

#ifndef PHONE_SCREEN_H
#define PHONE_SCREEN_H

#include "gui/application_screen/application_screen.h"
#include "gui/phone_screen/contacts_screen/contacts_screen.h"
#include "gui/phone_screen/history_screen/history_screen.h"
#include "gui/phone_screen/keypad_screen/keypad_screen.h"
#include "controller/controller.h"

class PhoneScreen : public ApplicationScreen {

Q_OBJECT

    HistoryScreen *historyScreen;
    ContactsScreen *contactsScreen;
    KeypadScreen *keypadScreen;
    Controller *controller;

public:
    explicit PhoneScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    ContactsScreen *getContactsScreen() const {
        return contactsScreen;
    }

    HistoryScreen *getHistoryScreen() const {
        return historyScreen;
    }

    KeypadScreen *getKeypadScreen() const {
        return keypadScreen;
    }
};

#endif //PHONE_SCREEN_H
