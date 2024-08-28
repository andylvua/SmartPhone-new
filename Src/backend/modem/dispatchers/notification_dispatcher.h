//
// Created by andrew on 26.08.24.
//

#ifndef NOTIFICATION_DISPATCHER_H
#define NOTIFICATION_DISPATCHER_H

#include <QObject>
#include <QString>

#include "backend/at_chat.h"

class NotificationDispatcher : public QObject {
    Q_OBJECT

public:
    explicit NotificationDispatcher(ATChat *atChat);

signals:
    void clip(const QString &notification);
    void ciev(const QString &notification);
    void ring();
    void cmgs(const QString &notification);
    void cds(const QString &notification);
    void cmti(const QString &notification);
    void cusd(const QString &notification);

private slots:
    void handleNotification(const QString &type, const QString &notification);

    void handleCallNotification(const QString &notification);
};

#endif //NOTIFICATION_DISPATCHER_H
