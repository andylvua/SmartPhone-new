//
// Created by andrew on 24.08.24.
//

#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <QWidget>
#include <QIcon>
#include <QListWidget>
#include <QListWidgetItem>

#include "controller/controller.h"
#include "gui/main_screen/notification.h"


class MainScreen : public QWidget {
Q_OBJECT

    QListWidget *notificationsList;
    Controller *controller;

public:
    explicit MainScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    NotificationWidget* addNotification(const QIcon &icon, const QString &title, const QString &message);

signals:
    void phoneScreenRequested();
    void smsScreenRequested();
    void browserScreenRequested();
    void consolesScreenRequested();
    void settingsScreenRequested();

private slots:
    void handleNotificationClick(QListWidgetItem *item) const;
};

#endif //MAIN_SCREEN_H
