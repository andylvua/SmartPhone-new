//
// Created by andrew on 24.08.24.
//

#ifndef CHAT_SCREEN_H
#define CHAT_SCREEN_H

#include <QComboBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QClipboard>
#include <QMenu>

#include "gui/application_screen/application_screen.h"
#include "controller/controller.h"
#include "message_entry.h"

class ChatScreen : public ApplicationScreen {
Q_OBJECT

    QWidget *contactSelectionWidget;
    QComboBox *contactPicker;
    QString contactOrNumber;
    QVBoxLayout *messagesLayout;
    QLineEdit *inputField;
    QScrollArea *scrollArea;
    QHash<QUuid, MessageEntryWidget *> waitingForDelivery;

    QDateTime lastMessageTime;

    bool newChat = false;
    bool scrollOnAdd = true;

    Controller *controller;

public:
    ChatScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    void setNewChat(bool newChat);

    void setContactOrNumber(const QString &contactOrNumber);

    bool isChatVisible(const QString &contactOrNumber) const;

private:

    void loadChatHistory(const QString &contactOrNumber);

    void clearChatHistory();

    QLabel *addTimestampHeader(const QDateTime &dateTime);

    void addMessage(const Message &message);

signals:

    void messageSent(const QString &message);

    void incomingMessage(const QString &message);

    void smsRead(const QUuid &uuid);

private slots:
    void onContactSelectionChanged(const QString &selectedContact);

    void showMessageContextMenu(const QPoint &pos);

    void sendMessage();

    void handleIncomingMessage(const Message &message);

    void handleStatusChange(const QUuid &uuid, delivery_status_t status);

};

#endif //CHAT_SCREEN_H
