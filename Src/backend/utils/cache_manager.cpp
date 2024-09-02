//
// Created by Andrew Yaroshevych on 26.01.2023.
//

#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QVector>
#include <QDataStream>

#include "backend/utils/cache_manager.h"

QString CacheManager::CONTACTS_FILEPATH;
QString CacheManager::CALLS_FILEPATH;
QString CacheManager::MESSAGES_FILEPATH;
QVector<Call> CacheManager::CALLS = {};
QMap<QString, QVector<Message>> CacheManager::MESSAGES = {};
QMap<QString, Contact> CacheManager::CONTACTS = {};

//
//const auto cacheLogger = spdlog::basic_logger_mt("cache",
//                                                 LOGS_FILEPATH, true);

void createIfNotExists(const QString &fileName) {
//    SPDLOG_LOGGER_INFO(cacheLogger, "Checking file: {}", fileName.toStdString());
    QFile file(fileName);

    if (!file.exists()) {
//        SPDLOG_LOGGER_INFO(cacheLogger, "File {} does not exist, creating", fileName.toStdString());
        file.open(QIODevice::ReadWrite);
        file.close();
    }
}

void CacheManager::checkCacheFiles() {
    createIfNotExists(MESSAGES_FILEPATH);
    createIfNotExists(CALLS_FILEPATH);
    createIfNotExists(CONTACTS_FILEPATH);
}

void CacheManager::startupPopulate() {
    getContacts();
    getCalls();
    getMessages();
}
// CONTACTS

bool CacheManager::addContact(const QString &name, const QString &number) {
    if (name.isEmpty() || number.isEmpty()) {
        return false;
    }

    if (CONTACTS.contains(number)) {
        return false;
    }
//   SPDLOG_LOGGER_INFO(cacheLogger, "Adding contact: {} {}", name.toStdString(), number.toStdString());
    CONTACTS.insert(number, {name, number});
    return saveContacts();
}

Contact CacheManager::getContact(const QString &info) {
    if (info.isEmpty()) {
        return {};
    }

    if (getContacts().contains(info)) {
//    SPDLOG_LOGGER_INFO(cacheLogger, "Getting contact by name or number: {}", info.toStdString());
        return CONTACTS[info];
    }

    return {};
}

bool CacheManager::removeContact(const Contact &contact) {

    if (CONTACTS.contains(contact.number)) {
//        SPDLOG_LOGGER_INFO(cacheLogger, "Removing contact: {} {}", contact.name.toStdString(),
//        contact.number.toStdString());
        CONTACTS.remove(contact.number);
        return saveContacts();
    }

    return false;
}

bool CacheManager::editContact(const Contact &contact, const QString &newName, const QString &newNumber) {

    if (CONTACTS.contains(contact.number)) {
//        SPDLOG_LOGGER_INFO(cacheLogger, "Editing contact: {} {} to {} {}", contact.name.toStdString(),
//        contact.number.toStdString(), newName.toStdString(), newNumber.toStdString());
        CONTACTS.remove(contact.number);
        CONTACTS.insert(newNumber, {newName, newNumber});
        return saveContacts();
    }

    return false;
}

bool CacheManager::saveContacts() {
    QFile file(CONTACTS_FILEPATH);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QJsonArray jsonArray;
    for (const Contact &contact: CONTACTS) {
        jsonArray.append(contact.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

const QMap<QString, Contact> &CacheManager::getContacts() {
    if (!CONTACTS.isEmpty()) {
        return CONTACTS;
    }

    QFile file(CONTACTS_FILEPATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return CONTACTS;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray jsonArray = doc.array();
    for (const auto &value: jsonArray) {
        CONTACTS.insert(value.toObject()["number"].toString(), Contact::fromJson(value.toObject()));
    }

    return CONTACTS;
}

// CALLS

bool CacheManager::addCall(Call &call) {
    Contact contact = getContact(call.contact.number);
    if (!contact.name.isEmpty()) {
        call.contact = contact;
    }

    CALLS.append(call);

    return saveCalls();
}

bool CacheManager::removeCall(const Call &call) {
    auto it = std::remove_if(CALLS.begin(), CALLS.end(), [&call](const Call &c) {
        return c.uuid == call.uuid;
    });

    if (it != CALLS.end()) {
        CALLS.erase(it, CALLS.end());
        return saveCalls();
    }

    return false;
}

bool CacheManager::saveCalls() {
    QFile file(CALLS_FILEPATH);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const Call &call: CALLS) {
        jsonArray.append(call.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    return true;
}

const QVector<Call> &CacheManager::getCalls() {
    if (!CALLS.isEmpty()) {
        return CALLS;
    }

    QFile file(CALLS_FILEPATH);

    if (!file.open(QIODevice::ReadOnly)) {
        return CALLS;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray jsonArray = doc.array();
    for (const auto &value: jsonArray) {
        CALLS.append(Call::fromJson(value.toObject()));
    }

    return CALLS;
}

// MESSAGES

bool CacheManager::addMessage(const Message &message) {
    MESSAGES[message.number].append(message);

    return saveMessages(message.number);
}

bool CacheManager::updateMessageStatus(const QUuid &uuid, delivery_status_t status) {
    for (auto &chat: MESSAGES) {
        for (auto &message: chat) {
            if (message.uuid == uuid) {
                message.deliveryStatus = status;
                return saveMessages(message.number);
            }
        }
    }

    return false;
}

bool CacheManager::updateMessageStatus(const QString &info, const QUuid &uuid, delivery_status_t status) {
    if (!MESSAGES.contains(info)) {
        return false;
    }

    for (auto &message: MESSAGES[info]) {
        if (message.uuid == uuid) {
            message.deliveryStatus = status;
            return saveMessages(info);
        }
    }
    return false;
}

bool CacheManager::updateMessageStatus(const QString &info, const QUuid &uuid, read_status_t status) {
    if (!MESSAGES.contains(info)) {
        return false;
    }

    for (auto &message: MESSAGES[info]) {
        if (message.uuid == uuid) {
            message.readStatus = status;
            return saveMessages(info);
        }
    }

    return false;
}


const QMap<QString, QVector<Message>> &CacheManager::getMessages() {
    if (!MESSAGES.isEmpty()) {
        return MESSAGES;
    }

    if (!QDir(MESSAGES_FILEPATH).exists()) {
        // create directory
        QDir().mkdir(MESSAGES_FILEPATH);
    }

    QDir dir(MESSAGES_FILEPATH);
    for (const QString &file: dir.entryList(QDir::Files)) {
        getMessages(file.split(".")[0]);
    }
    return MESSAGES;
}


const QVector<Message> &CacheManager::getMessages(const QString &info) {
    if (info.isEmpty()) {
        return QVector<Message>();
    }
    if (MESSAGES.contains(info)) {
        return MESSAGES[info];
    }

    QFile file(MESSAGES_FILEPATH + info + ".json");
    if (!file.open(QIODevice::ReadOnly)) {
//        SPDLOG_LOGGER_INFO(cacheLogger, "No messages file found");
        return QVector<Message>();
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray jsonArray = doc.array();
    for (const auto& value: jsonArray) {
        MESSAGES[info].append(Message::fromJson(value.toObject()));
    }

    return MESSAGES[info];
}

bool CacheManager::saveMessages(const QString &info) {
    if (QDir(MESSAGES_FILEPATH).exists()) {
        QDir().mkdir(MESSAGES_FILEPATH);
    }

    QFile file(MESSAGES_FILEPATH + info + ".json");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const Message &message: MESSAGES[info]) {
        jsonArray.append(message.toJson());
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool CacheManager::removeMessage(const QString &info, const QUuid &uuid) {
    if (!MESSAGES.contains(info)) {
        return false;
    }
    auto it = std::remove_if(MESSAGES[info].begin(), MESSAGES[info].end(), [&uuid](const Message &m) {
        return m.uuid == uuid;
    });

    if (it != MESSAGES[info].end()) {
        MESSAGES[info].erase(it, MESSAGES[info].end());
        return saveMessages(info);
    }

    return false;
}

bool CacheManager::removeMessages(const QString &info) {
    if (MESSAGES.contains(info)) {
        MESSAGES.remove(info);
        return saveMessages(info);
    }

    return false;
}
