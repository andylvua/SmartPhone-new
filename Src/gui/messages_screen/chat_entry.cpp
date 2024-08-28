//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>

#include "gui/messages_screen/chat_entry.h"
#include "backend/utils/cache_manager.h"
#include "gui/utils/fa.h"
#include "gui/utils/gui_utils.h"


ChatEntryWidget::ChatEntryWidget(const QString &number, const Message &lastMessage, QWidget *parent)
    : QWidget(parent), number(number) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *itemLayout = new QHBoxLayout();
    QVBoxLayout *messageLayout = new QVBoxLayout();
    QHBoxLayout *messageContentLayout = new QHBoxLayout();

    messageContentLayout->setContentsMargins(0, 0, 0, 0);
    messageContentLayout->setAlignment(Qt::AlignLeft);

    Contact contact = CacheManager::getContact(number);
    QString displayText = contact.name.isEmpty() ? number : contact.name;

    QLabel *contactLabel = new QLabel(displayText, this);
    contactLabel->setObjectName("chatContactLabel");

    messageLayout->addWidget(contactLabel);

    unreadLabel = new QLabel("", this);
    QIcon unreadIcon = FA::getInstance()->icon(fa::fa_solid, fa::fa_circle, {{"color", "#2064b4"}});
    unreadLabel->setPixmap(unreadIcon.pixmap(8, 8));
    messageContentLayout->addWidget(unreadLabel);
    unreadLabel->hide();

    messageLabel = new QLabel("", this);
    messageLabel->setObjectName("chatSmallLabel");

    messageContentLayout->addWidget(messageLabel);
    messageLayout->addLayout(messageContentLayout);
    itemLayout->addLayout(messageLayout);

    timeLabel = new QLabel("", this);
    timeLabel->setObjectName("chatSmallLabel");
    itemLayout->addStretch();
    itemLayout->addWidget(timeLabel);

    QLabel *chevronLabel = new QLabel("", this);
    QIcon chevronIcon = FA::getInstance()->icon(fa::fa_light, fa::fa_chevron_right);
    chevronLabel->setPixmap(chevronIcon.pixmap(15, 15));
    itemLayout->addWidget(chevronLabel);

    updateLastMessage(lastMessage);

    itemLayout->setContentsMargins(0, 5, 0, 5);

    QHBoxLayout *separatorLayout = new QHBoxLayout();
    separatorLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setObjectName("chatSeparator");
    separator->setFixedHeight(1);

    separatorLayout->addWidget(separator);

    mainLayout->addLayout(itemLayout);
    mainLayout->addLayout(separatorLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    itemLayout->setSpacing(9);
}

void ChatEntryWidget::updateLastMessage(const Message &lastMessage) const {
    QString messageText = lastMessage.message;
    QFontMetrics metrics(messageLabel->font());
    messageText = metrics.elidedText(messageText, Qt::ElideRight, 200);
    messageLabel->setText(messageText);
    timeLabel->setText(Utils::getTimeString(lastMessage.dateTime));
    if (lastMessage.messageDirection == messageDirection::MD_INCOMING &&
             lastMessage.readStatus == read_status_t::RS_UNREAD) {
        unreadLabel->show();
             } else {
                 unreadLabel->hide();
             }
}
