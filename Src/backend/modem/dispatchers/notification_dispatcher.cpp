//
// Created by andrew on 26.08.24.
//

#include <QDebug>

#include "backend/modem/dispatchers/notification_dispatcher.h"


NotificationDispatcher::NotificationDispatcher(ATChat *atChat) {
    connect(atChat, &ATChat::notification, this, &NotificationDispatcher::handleNotification);
    connect(atChat, &ATChat::callNotification, this, &NotificationDispatcher::handleCallNotification);
}


void NotificationDispatcher::handleNotification(const QString &type, const QString &notification) {
    if (type == "+CLIP:") {
        qDebug() << "Notification Manager: Incoming call: " << notification;
        emit clip(notification);
    }
    if (type == "+CIEV:") {
        qDebug() << "Notification Manager: CIEV: " << notification;
        emit ciev(notification);
    }
    if (type == "+CMGS:") {
        qDebug() << "Notification Manager: CMGS: " << notification;
        emit cmgs(notification);
    }
    if (type == "+CDS:") {
        qDebug() << "Notification Manager: CDS: " << notification;
        emit cds(notification);
    }
    if (type == "+CMTI:") {
        qDebug() << "Notification Manager: CMTI: " << notification;
        emit cmti(notification);
    }
    if (type == "+CUSD:") {
        qDebug() << "Notification Manager: CUSD: " << notification;
        emit cusd(notification);
    }
}

void NotificationDispatcher::handleCallNotification(const QString &notification) {
    if (notification == "RING") {
        qDebug() << "Notification Manager: RING";
        emit ring();
    }
}
