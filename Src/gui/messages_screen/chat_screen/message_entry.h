//
// Created by andrew on 24.08.24.
//

#ifndef MESSAGE_ENTRY_H
#define MESSAGE_ENTRY_H

#include <QWidget>
#include <QLabel>

#include "backend/models/models.h"


class MessageEntryWidget : public QWidget {
Q_OBJECT

    QLabel *header = nullptr;
    QLabel *deliveryStatus = nullptr;
    MessageEntryWidget *nextMessage = nullptr;
    Message message;

public:
    MessageEntryWidget(const Message &message, QWidget *parent = nullptr);

    void setDeliveryStatus(delivery_status_t status) const;

    void setHeader(QLabel *header) {
        this->header = header;
    }

    QLabel *getHeader() const {
        return header;
    }

    void setNextMessage(MessageEntryWidget *nextMessage) {
        this->nextMessage = nextMessage;
    }

    MessageEntryWidget *getNextMessage() const {
        return nextMessage;
    }

    Message &getMessage() {
        return message;
    }
};

#endif //MESSAGE_ENTRY_H
