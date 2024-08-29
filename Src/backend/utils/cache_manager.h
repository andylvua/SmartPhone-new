//
// Created by Andrew Yaroshevych on 26.01.2023.
//

#ifndef PHONE_CACHE_MANAGER_HPP
#define PHONE_CACHE_MANAGER_HPP

#include <QCoreApplication>
#include <QString>
#include <QStandardPaths>

#include "backend/models/models.h"

class CacheManager {
public:
    static bool addContact(const QString &name, const QString &number);
    static Contact getContact(const QString &info);
    static bool removeContact(const Contact &contact);
    static bool editContact(const Contact &contact, const QString &newName, const QString &newNumber);
    static bool saveContacts(const QVector<Contact> &contacts);
    static QVector<Contact> getContacts();

    static bool addCall(Call &call);
    static bool removeCall(const Call &call);
    static bool saveCalls(const QVector<Call> &calls);
    static QVector<Call> getCalls();

    static bool addMessage(const Message &message);
    static bool updateMessageStatus(const QUuid &uuid, delivery_status_t status);
    static bool updateMessageStatus(const QUuid &uuid, read_status_t status);
    static bool saveMessages(const QVector<Message> &messages);
    static bool removeMessage(const QUuid &uuid);
    static bool removeMessages(const QString &info);
    static QVector<Message> getMessages();
    static QVector<Message> getMessages(const QString &info);

    static void checkCacheFiles();

    static void setCacheDir(const QString &dir) {
        CONTACTS_FILEPATH = dir + "/contacts.json";
        CALLS_FILEPATH = dir + "/calls.json";
        MESSAGES_FILEPATH = dir + "/messages.json";
    }

private:
    static QString CONTACTS_FILEPATH;
    static QString CALLS_FILEPATH;
    static QString MESSAGES_FILEPATH;
};

#endif //PHONE_CACHE_MANAGER_HPP
