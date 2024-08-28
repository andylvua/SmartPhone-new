//
// Created by andrew on 24.08.24.
//

#ifndef CHAT_ENTRY_H
#define CHAT_ENTRY_H

#include <QString>
#include <QLabel>

#include "backend/models/models.h"

class ChatEntryWidget : public QWidget {
Q_OBJECT

    QString number;
    QLabel *messageLabel;
    QLabel *timeLabel;
    QLabel *unreadLabel;

public:
    explicit ChatEntryWidget(const QString &number, const Message &lastMessage, QWidget *parent = nullptr);

    QString getNumber() const {
        return number;
    }

    void updateLastMessage(const Message &lastMessage) const;
};
#endif //CHAT_ENTRY_H
