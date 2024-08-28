//
// Created by andrew on 24.08.24.
//

#include <QVBoxLayout>

#include "gui/messages_screen/chat_screen/message_entry.h"
#include "gui/utils/fa.h"


MessageEntryWidget::MessageEntryWidget(const Message &message, QWidget *parent) : QWidget(parent), message(message) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *messageLayout = new QVBoxLayout(this);
    messageLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *messageBoxWidget = new QWidget();
    QHBoxLayout *messageBoxLayout = new QHBoxLayout(messageBoxWidget);
    messageBoxLayout->setContentsMargins(0, 0, 0, 0);
    messageBoxWidget->setMaximumWidth(this->width() * 0.75);

    messageLayout->setSpacing(3);

    QLabel *messageLabel = new QLabel();
    messageLabel->setWordWrap(true);
    messageLabel->setText(message.message);

    QHBoxLayout *messageLabelLayout = new QHBoxLayout();
    messageLabelLayout->setContentsMargins(0, 0, 0, 0);
    messageLabelLayout->setSpacing(5);

    QLabel *timestampLabel = new QLabel(message.dateTime.toString("hh:mm"), this);
    timestampLabel->setObjectName("messageTimestampLabel");
    timestampLabel->setContentsMargins(0, 0, 0, 0);

    messageLayout->setAlignment(Qt::AlignRight);

    if (message.messageDirection == messageDirection::MD_OUTGOING) {
        messageLabel->setObjectName("outgoingMessageLabel");
        timestampLabel->setAlignment(Qt::AlignRight);
        messageLayout->setAlignment(Qt::AlignRight);
        messageLabelLayout->setAlignment(Qt::AlignRight);
        messageBoxLayout->setAlignment(Qt::AlignRight);
    } else {
        messageLabel->setObjectName("incomingMessageLabel");
        timestampLabel->setAlignment(Qt::AlignLeft);
        messageLayout->setAlignment(Qt::AlignLeft);
        messageLabelLayout->setAlignment(Qt::AlignLeft);
        messageBoxLayout->setAlignment(Qt::AlignLeft);
    }

    messageBoxLayout->addWidget(messageLabel);
    messageLabelLayout->addWidget(timestampLabel);

    if (message.messageDirection == messageDirection::MD_OUTGOING) {
        deliveryStatus = new QLabel("", this);

        setDeliveryStatus(message.deliveryStatus);
        messageLabelLayout->addWidget(deliveryStatus);
    }

    messageLayout->addWidget(messageBoxWidget);
    messageLayout->addLayout(messageLabelLayout);
}

void MessageEntryWidget::setDeliveryStatus(delivery_status_t status) const {
    QVariantMap options;
    QIcon icon;

    switch (status) {
        case delivery_status_t::DS_PENDING:
            options.insert("color", "#4d4f4f");
        icon = FA::getInstance()->icon(fa::fa_regular, fa::fa_clock, options);
        break;
        case delivery_status_t::DS_SENT:
            options.insert("color", "#4d4f4f");
        icon = FA::getInstance()->icon(fa::fa_solid, fa::fa_check, options);
        break;
        case delivery_status_t::DS_DELIVERED:
            options.insert("color", "#2064b4");
        icon = FA::getInstance()->icon(fa::fa_solid, fa::fa_check, options);
        break;
        case delivery_status_t::DS_FAILED:
            options.insert("color", "#a82414");
        icon = FA::getInstance()->icon(fa::fa_solid, fa::fa_circle_exclamation, options);
        break;
        default:
            break;

    }

    deliveryStatus->setPixmap(icon.pixmap(15, 15));
}
