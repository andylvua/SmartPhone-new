//
// Created by andrew on 24.08.24.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QShowEvent>


#include "gui/messages_screen/messages_screen.h"
#include "gui/messages_screen/chat_entry.h"
#include "backend/utils/cache_manager.h"
#include "gui/utils/fa.h"
#include "gui/utils/gui_utils.h"


MessagesScreen::MessagesScreen(QWidget *parent, Controller *controller)
        : ApplicationScreen("Messages", parent), controller(controller) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("MesssagesScreen");

    smsList = new QListWidget(this);
    smsList->setFrameStyle(QFrame::NoFrame);
    smsList->setContextMenuPolicy(Qt::CustomContextMenu);
    smsList->setObjectName("SmsList");

    QVBoxLayout *layout = getContentLayout();
    QPushButton *newSmsButton = new QPushButton("", this);
    newSmsButton->setIcon(
            FA::getInstance()->icon(fa::fa_solid, fa::fa_pen_to_square, {{"color", "#2064b4"}}));
    newSmsButton->setIconSize(QSize(25, 25));
    newSmsButton->installEventFilter(new HoverEventFilter());
    newSmsButton->setFlat(true);
    newSmsButton->setFixedSize(30, 30);

    layout->addWidget(smsList);
    addActionButton(newSmsButton);

    connect(newSmsButton, &QPushButton::clicked, this, &MessagesScreen::newSmsScreenRequested);
    connect(smsList, &QListWidget::itemClicked, this, &MessagesScreen::handleSmsChatRequest);

    connect(smsList, &QListWidget::customContextMenuRequested, this, &MessagesScreen::showContextMenu);

    connect(this->controller, &Controller::smsReceived, this, &MessagesScreen::handleIncomingMessage);
}

void MessagesScreen::showEvent(QShowEvent *event) {
    populateSmsChats();
    QWidget::showEvent(event);
}

void MessagesScreen::populateSmsChats() {
    smsList->clear();
    QVector<Message> messages = CacheManager::getMessages();

    QHash<QString, QVector<Message>> messagesByNumber;
    for (const auto &message: messages) {
        messagesByNumber[message.number].append(message);
    }

    QList<QString> numbers = messagesByNumber.keys();
    std::sort(numbers.begin(), numbers.end(), [&messagesByNumber](const QString &a, const QString &b) {
        return messagesByNumber[a].last().dateTime > messagesByNumber[b].last().dateTime;
    });

    for (const auto &number: numbers) {
        const Message &lastMessage = messagesByNumber.value(number).last();

        QListWidgetItem *item = new QListWidgetItem(smsList);

        ChatEntryWidget *chatEntry = new ChatEntryWidget(number, lastMessage, this);
        item->setSizeHint(chatEntry->sizeHint());

        smsList->setItemWidget(item, chatEntry);
    }
}

void MessagesScreen::handleIncomingMessage(const Message &message) {
    for (int i = 0; i < smsList->count(); i++) {
        QListWidgetItem *item = smsList->item(i);
        ChatEntryWidget *chatEntry = qobject_cast<ChatEntryWidget *>(smsList->itemWidget(item));
        if (chatEntry->getNumber() == message.number) {
            chatEntry->updateLastMessage(message);
            return;
        }
    }

    populateSmsChats();
}

void MessagesScreen::handleSmsChatRequest(QListWidgetItem *item) {
    ChatEntryWidget *chatEntry = qobject_cast<ChatEntryWidget *>(smsList->itemWidget(item));
    QString number = chatEntry->getNumber();
    qDebug() << "Opening SMS chat with:" << number;
    emit smsChatRequested(number);
}

void MessagesScreen::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = smsList->itemAt(pos);

    if (item) {
        ChatEntryWidget *chatEntry = qobject_cast<ChatEntryWidget *>(smsList->itemWidget(item));
        QString number = chatEntry->getNumber();

        QMenu contextMenu;
        QAction *deleteAction = contextMenu.addAction("Delete Chat");

        connect(deleteAction, &QAction::triggered, this, [this, number, item]() {
            CacheManager::removeMessages(number);
            delete item;
        });

        contextMenu.exec(smsList->mapToGlobal(pos));
    }
}
