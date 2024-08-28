//
// Created by andrew on 24.08.24.
//

#ifndef CONTACT_ENTRY_H
#define CONTACT_ENTRY_H

#include <QWidget>
#include <QLabel>

#include "controller/controller.h"
#include "backend/models/models.h"


class ContactEntryWidget : public QWidget {
Q_OBJECT

    Contact contact;
    Controller *controller;
    QLabel *contactLabel;

public:
    explicit ContactEntryWidget(const Contact &entry, Controller *controller, QWidget *parent = nullptr);

    [[nodiscard]] Contact getContact() const {
        return contact;
    }

    void updateContact(const Contact &updatedContact) {
        contact = updatedContact;
        contactLabel->setText(contact.name);
    }

signals:
    void smsChatRequested(const QString &number);
};

#endif //CONTACT_ENTRY_H
