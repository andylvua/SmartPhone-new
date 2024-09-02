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
    static bool saveContacts();
    static const QMap<QString, Contact>& getContacts();

    static bool addCall(Call &call);
    static bool removeCall(const Call &call);
    static bool saveCalls();
    static const QVector<Call>& getCalls();

    static bool addMessage(const Message &message);
    static bool updateMessageStatus(const QUuid &uuid, delivery_status_t status);
    static bool updateMessageStatus(const QString& info, const QUuid &uuid, delivery_status_t status);
    static bool updateMessageStatus(const QString& info, const QUuid &uuid, read_status_t status);
    static bool saveMessages(const QString &info);
    static bool removeMessage(const QString &info, const QUuid &uuid);
    static bool removeMessages(const QString &info);
    static const QMap<QString, QVector<Message>>& getMessages();
    static const QVector<Message>& getMessages(const QString &info);

    static void checkCacheFiles();
    static void startupPopulate();

    static void setCacheDir(const QString &dir) {
        CONTACTS_FILEPATH = dir + "/contacts.json";
        CALLS_FILEPATH = dir + "/calls.json";
        MESSAGES_FILEPATH = dir + "/chats/";
    }

private:
    static QString CONTACTS_FILEPATH;
    static QString CALLS_FILEPATH;
    static QString MESSAGES_FILEPATH;
    static QVector<Call> CALLS;
    static QMap<QString, QVector<Message>> MESSAGES; // key is contact number
    static QMap<QString, Contact> CONTACTS; // key is contact number
};

#endif //PHONE_CACHE_MANAGER_HPP
