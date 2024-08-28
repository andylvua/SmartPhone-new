//
// Created by andrew on 24.08.24.
//

#ifndef CONTACTS_SCREEN_H
#define CONTACTS_SCREEN_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>

#include "controller/controller.h"


class ContactsScreen : public QWidget {
Q_OBJECT

    QListWidget *contactsList;
    Controller *controller;
    QPushButton *newContactButton;
    QWidget *parent;

public:
    explicit ContactsScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    void addContact(const Contact &contact);

    void populateContacts();


signals:
    void smsChatRequested(const QString &number);

protected:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

private slots:

    void onNewContactClicked();

    void onContactItemDoubleClicked(QListWidgetItem *item);
};

#endif //CONTACTS_SCREEN_H
