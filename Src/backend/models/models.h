//
// Created by Andrew Yaroshevych on 26.01.2023.
//

#ifndef PHONE_MEDIA_TYPES_HPP
#define PHONE_MEDIA_TYPES_HPP

#include <QString>
#include <QJsonObject>
#include <QUuid>
#include <QDateTime>
#include <utility>

using callResult_t = enum class callResult {
    CR_ANSWERED,
    CR_NO_ANSWER
};

using callDirection_t = enum class callDirection {
    CD_INCOMING,
    CD_OUTGOING
};

using messageDirection_t = enum class messageDirection {
    MD_INCOMING,
    MD_OUTGOING
};

using delivery_status_t = enum class delivery_status {
    DS_PENDING,
    DS_SENT,
    DS_DELIVERED,
    DS_FAILED
};

using read_status_t = enum class read_status {
    RS_UNAPPLICABLE,
    RS_READ,
    RS_UNREAD
};

enum class ussdEncoding {
    UE_GSM7,
    UE_UCS2,
    UE_UNKNOWN
};

struct Contact {
    QString name;
    QString number;

    // Constructors
    Contact() = default;
    Contact(QString name, QString number) : name(std::move(name)), number(std::move(number)) {}

    // Serialization
    QJsonObject toJson() const {
        QJsonObject jsonObj;
        jsonObj["name"] = name;
        jsonObj["number"] = number;
        return jsonObj;
    }

    // Deserialization
    static Contact fromJson(const QJsonObject& jsonObj) {
        return Contact(jsonObj["name"].toString(), jsonObj["number"].toString());
    }
};

struct Call {
    Contact contact;
    QDateTime startTime;
    uint16_t duration;
    callResult_t callResult;
    callDirection_t callDirection;
    QUuid uuid;

    // Constructors
    Call() = default;

    QJsonObject toJson() const {
        QJsonObject jsonObj;
        jsonObj["contact"] = contact.toJson();
        jsonObj["startTime"] = startTime.toString("dd.MM.yyyy hh:mm:ss");
        jsonObj["duration"] = duration;
        jsonObj["callResult"] = callResult == callResult::CR_ANSWERED ? "ANSWERED" : "NO_ANSWER";
        jsonObj["callDirection"] = callDirection == callDirection::CD_INCOMING ? "INCOMING" : "OUTGOING";
        jsonObj["uuid"] = uuid.toString();
        return jsonObj;
    }

    static Call fromJson(const QJsonObject& jsonObj) {
        Call call;
        call.contact = Contact::fromJson(jsonObj["contact"].toObject());
        call.startTime = QDateTime::fromString(jsonObj["startTime"].toString(), "dd.MM.yyyy hh:mm:ss");
        call.duration = jsonObj["duration"].toInt();
        call.callResult = jsonObj["callResult"].toString() == "ANSWERED" ? callResult::CR_ANSWERED : callResult::CR_NO_ANSWER;
        call.callDirection = jsonObj["callDirection"].toString() == "INCOMING" ? callDirection::CD_INCOMING : callDirection::CD_OUTGOING;
        call.uuid = QUuid(jsonObj["uuid"].toString());
        return call;
    }
};

struct Message {
    Message(QUuid uuid, QString number, QString message, QDateTime dateTime, messageDirection_t messageDirection) :
            number(std::move(number)), message(std::move(message)), dateTime(dateTime), messageDirection(messageDirection), uuid(std::move(uuid)){}
    QString number;
    QString message;
    QDateTime dateTime;
    messageDirection_t messageDirection;
    QUuid uuid;
    delivery_status_t deliveryStatus = delivery_status::DS_PENDING;
    read_status_t readStatus = read_status::RS_UNREAD;

    // Serialization
    QJsonObject toJson() const {
        QJsonObject jsonObj;
        jsonObj["number"] = number;
        jsonObj["message"] = message;
        jsonObj["dateTime"] = dateTime.toString("dd.MM.yyyy hh:mm:ss");
        jsonObj["messageDirection"] = messageDirection == messageDirection::MD_INCOMING ? "INCOMING" : "OUTGOING";
        jsonObj["uuid"] = uuid.toString();
        switch (deliveryStatus) {
            case delivery_status::DS_SENT:
                jsonObj["deliveryStatus"] = "SENT";
                break;
            case delivery_status::DS_DELIVERED:
                jsonObj["deliveryStatus"] = "DELIVERED";
                break;
            case delivery_status::DS_FAILED:
                jsonObj["deliveryStatus"] = "FAILED";
                break;
            default:
                jsonObj["deliveryStatus"] = "PENDING";
                break;
        }
        jsonObj["read"] = readStatus == read_status::RS_READ;
        return jsonObj;
    }

    // Deserialization
    static Message fromJson(const QJsonObject& jsonObj) {
        Message msg = Message(QUuid(jsonObj["uuid"].toString()),
                        jsonObj["number"].toString(),
                       jsonObj["message"].toString(),
                       QDateTime::fromString(jsonObj["dateTime"].toString(), "dd.MM.yyyy hh:mm:ss"),
                       jsonObj["messageDirection"].toString() == "INCOMING" ? messageDirection::MD_INCOMING : messageDirection::MD_OUTGOING);

        switch (jsonObj["deliveryStatus"].toString().toStdString()[0]) {
            case 'S':
                msg.deliveryStatus = delivery_status::DS_SENT;
                break;
            case 'D':
                msg.deliveryStatus = delivery_status::DS_DELIVERED;
                break;
            case 'F':
                msg.deliveryStatus = delivery_status::DS_FAILED;
                break;
            default:
                msg.deliveryStatus = delivery_status::DS_PENDING;
                break;
        }
        msg.readStatus = jsonObj["read"].toBool() ? read_status::RS_READ : read_status::RS_UNREAD;
        return msg;
    }
};

Q_DECLARE_METATYPE(Contact)
Q_DECLARE_METATYPE(Call)

#endif //PHONE_MEDIA_TYPES_HPP
