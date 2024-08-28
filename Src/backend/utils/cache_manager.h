//
// Created by Andrew Yaroshevych on 26.01.2023.
//

#ifndef PHONE_CACHE_MANAGER_HPP
#define PHONE_CACHE_MANAGER_HPP

#include <QString>
#include <string>

#include "backend/models/models.h"

constexpr const char *CONTACTS_FILEPATH = "../cache/contacts.json";
constexpr const char *MESSAGES_FILEPATH = "../cache/messages.json";
constexpr const char *CALLS_FILEPATH = "../cache/calls.json";
#define LOGS_FILEPATH "../logs/log.txt"

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
};

#endif //PHONE_CACHE_MANAGER_HPP
