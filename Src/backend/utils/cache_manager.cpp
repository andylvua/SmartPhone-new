//
// Created by Andrew Yaroshevych on 26.01.2023.
//

#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QVector>
#include <QDateTime>

#include "backend/utils/cache_manager.h"


#define SEMICOLON_SEPARATOR(...) QStringList{__VA_ARGS__}.join("; ")
//
//const auto cacheLogger = spdlog::basic_logger_mt("cache",
//                                                 LOGS_FILEPATH, true);

void createIfNotExists(const QString &fileName) {
//    SPDLOG_LOGGER_INFO(cacheLogger, "Checking file: {}", fileName.toStdString());
    QFile file(fileName);

    if (!file.exists()) {
//        SPDLOG_LOGGER_INFO(cacheLogger, "File {} does not exist, creating", fileName.toStdString());
        file.open(QIODevice::WriteOnly);
        file.close();
    }
}

void CacheManager::checkCacheFiles() {
    createIfNotExists(MESSAGES_FILEPATH);
    createIfNotExists(CALLS_FILEPATH);
    createIfNotExists(CONTACTS_FILEPATH);
}

bool CacheManager::addContact(const QString &name, const QString &number) {
    QVector<Contact> contacts = getContacts();
    for (const Contact& contact : contacts) {
        if (contact.name == name || contact.number == number) {
            return false;
        }
    }
    contacts.append({name, number});
    return saveContacts(contacts);
}

Contact CacheManager::getContact(const QString &info) {
    if (info.isEmpty()) {
        return {};
    }

//    SPDLOG_LOGGER_INFO(cacheLogger, "Getting contact by name or number: {}", info.toStdString());
    QFile file(CONTACTS_FILEPATH);
    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        Contact contact = Contact::fromJson(value.toObject());
        if (contact.name == info || contact.number.contains(info)) {
            return contact;
        }
    }

    return {};
}

bool CacheManager::removeContact(const Contact &contact) {
    QVector<Contact> contacts = getContacts();

    auto it = std::remove_if(contacts.begin(), contacts.end(), [&contact](const Contact& c) {
        return c.name == contact.name && c.number == contact.number;
    });

    if (it != contacts.end()) {
        contacts.erase(it, contacts.end());
        return saveContacts(contacts);
    }

    return false;
}

bool CacheManager::editContact(const Contact &contact, const QString &newName, const QString &newNumber) {
    QVector<Contact> contacts = getContacts();

    for (Contact& c : contacts) {
        if (c.name == contact.name && c.number == contact.number) {
            c.name = newName;
            c.number = newNumber;
            return saveContacts(contacts);
        }
    }

    return false;
}

bool CacheManager::saveContacts(const QVector<Contact>& contacts) {
    QFile file(CONTACTS_FILEPATH);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const Contact& contact : contacts) {
        jsonArray.append(contact.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

QVector<Contact> CacheManager::getContacts() {
    QFile file(CONTACTS_FILEPATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVector<Contact> contacts;
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        contacts.append(Contact::fromJson(value.toObject()));
    }

    return contacts;
}


// CALLS

bool CacheManager::addCall(Call &call) {
    QVector<Call> calls = getCalls();

    Contact contact = getContact(call.contact.number);
    if (!contact.name.isEmpty()) {
        call.contact = contact;
    }

    calls.append(call);

    return saveCalls(calls);
}

bool CacheManager::removeCall(const Call &call) {
    QVector<Call> calls = getCalls();

    auto it = std::remove_if(calls.begin(), calls.end(), [&call](const Call& c) {
        return c.uuid == call.uuid;
    });

    if (it != calls.end()) {
        calls.erase(it, calls.end());
        return saveCalls(calls);
    }

    return false;
}

bool CacheManager::saveCalls(const QVector<Call> &calls) {
    QFile file(CALLS_FILEPATH);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const Call& call : calls) {
        jsonArray.append(call.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

QVector<Call> CacheManager::getCalls() {
    QFile file(CALLS_FILEPATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVector<Call> calls;
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        calls.append(Call::fromJson(value.toObject()));
    }

    return calls;
}


// MESSAGES

bool CacheManager::addMessage(const Message &message) {
    QVector<Message> messages = getMessages();

    messages.append(message);

    return saveMessages(messages);
}

bool CacheManager::updateMessageStatus(const QUuid &uuid, delivery_status_t status) {
    QVector<Message> messages = getMessages();

    for (Message& message : messages) {
        if (message.uuid == uuid) {
            message.deliveryStatus = status;
            return saveMessages(messages);
        }
    }

    return false;
}

bool CacheManager::updateMessageStatus(const QUuid &uuid, read_status_t status) {
    QVector<Message> messages = getMessages();

    for (Message& message : messages) {
        if (message.uuid == uuid) {
            message.readStatus = status;
            return saveMessages(messages);
        }
    }

    return false;
}


QVector<Message> CacheManager::getMessages() {
    QFile file(MESSAGES_FILEPATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVector<Message> messages;
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        messages.append(Message::fromJson(value.toObject()));
    }

    return messages;
}

QVector<Message> CacheManager::getMessages(const QString &info) {
    QFile file(MESSAGES_FILEPATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVector<Message> messages;
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        Message message = Message::fromJson(value.toObject());
        if (message.number == info) {
            messages.append(message);
        }
    }

    return messages;
}

bool CacheManager::saveMessages(const QVector<Message> &messages) {
    QFile file(MESSAGES_FILEPATH);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const Message &message: messages) {
        jsonArray.append(message.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool CacheManager::removeMessage(const QUuid &uuid) {
    QVector<Message> messages = getMessages();

    auto it = std::remove_if(messages.begin(), messages.end(), [&uuid](const Message& m) {
        return m.uuid == uuid;
    });

    if (it != messages.end()) {
        messages.erase(it, messages.end());
        return saveMessages(messages);
    }

    return false;
}

bool CacheManager::removeMessages(const QString &info) {
    QVector<Message> messages = getMessages();

    auto it = std::remove_if(messages.begin(), messages.end(), [&info](const Message& m) {
        return m.number == info;
    });

    if (it != messages.end()) {
        messages.erase(it, messages.end());
        return saveMessages(messages);
    }

    return false;
}
