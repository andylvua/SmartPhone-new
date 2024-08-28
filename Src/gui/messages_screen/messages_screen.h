//
// Created by andrew on 24.08.24.
//

#ifndef MESSAGES_SCREEN_H
#define MESSAGES_SCREEN_H

#include <QListWidget>

#include "gui/application_screen/application_screen.h"
#include "controller/controller.h"


class MessagesScreen : public ApplicationScreen {
Q_OBJECT

    QListWidget *smsList;
    Controller *controller;

public:
    explicit MessagesScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

signals:
    void newSmsScreenRequested();
    void smsChatRequested(const QString &number);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void populateSmsChats();

private slots:
    void handleIncomingMessage(const Message &message);

    void handleSmsChatRequest(QListWidgetItem *item);

    void showContextMenu(const QPoint &pos);
};

#endif //MESSAGES_SCREEN_H
