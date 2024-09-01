//
// Created by andrew on 24.08.24.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>

#include "chat_screen.h"
#include "backend/utils/cache_manager.h"
#include "gui/utils/fa.h"
#include "gui/utils/gui_utils.h"


ChatScreen::ChatScreen(QWidget *parent, Controller *controller): ApplicationScreen("Chat", parent, true), controller(controller) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("ChatScreen");


    QVBoxLayout *mainLayout = getContentLayout();

    contactSelectionWidget = new QWidget();
    QHBoxLayout *contactSelectionLayout = new QHBoxLayout(contactSelectionWidget);

    QLabel *toWhomLabel = new QLabel("Send To:", this);
    toWhomLabel->setObjectName("chatSmallLabel");
    // make it take only the space it needs
    toWhomLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    contactPicker = new QComboBox(this);
    contactPicker->setEditable(true);

    contactSelectionLayout->addWidget(toWhomLabel);
    contactSelectionLayout->addWidget(contactPicker);
    contactSelectionWidget->setObjectName("contactSelectionWidget");
    contactSelectionWidget->setLayout(contactSelectionLayout);
    mainLayout->addWidget(contactSelectionWidget);

    contactSelectionWidget->hide();

    scrollArea = new QScrollArea();
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *messagesWidget = new QWidget();
    messagesLayout = new QVBoxLayout(messagesWidget);
    messagesLayout->setAlignment(Qt::AlignTop);
    messagesLayout->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidget(messagesWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setObjectName("chatScrollArea");
    messagesLayout->setSpacing(7);

    mainLayout->addWidget(scrollArea);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputField = new QLineEdit();
    inputField->setPlaceholderText("Type a message...");
    inputField->setFixedHeight(40);
    QPushButton *sendButton = new QPushButton(
            FA::getInstance()->icon(fa::fa_solid, fa::fa_paper_plane_top), "", this);
    sendButton->setFixedSize(60, 40);
    sendButton->setIconSize(QSize(20, 20));
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);
    mainLayout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, &ChatScreen::sendMessage);
    connect(inputField, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

    connect(scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](int, int max) {
        if (scrollOnAdd) {
            scrollArea->verticalScrollBar()->setValue(max);
            scrollOnAdd = false;
        }
    });

    connect(contactPicker, &QComboBox::currentTextChanged, this, &ChatScreen::onContactSelectionChanged);

    connect(this->controller, &Controller::smsStatusChanged, this, &ChatScreen::handleStatusChange);
    connect(this->controller, &Controller::smsReceived, this, &ChatScreen::handleIncomingMessage);
}

void ChatScreen::setNewChat(bool newChat) {
    this->newChat = newChat;
    if (newChat) {
        clearChatHistory();
        contactPicker->clear();
        contactPicker->setPlaceholderText("Select a contact or type a number");
        contactPicker->setCurrentIndex(-1);
        QVector<Contact> contacts = CacheManager::getContacts();
        for (const auto &contact: contacts) {
            contactPicker->addItem(contact.name, contact.number);
        }
        contactSelectionWidget->show();
        setScreenTitle("New message");
    } else {
        contactSelectionWidget->hide();
    }
}

void ChatScreen::setContactOrNumber(const QString &contactOrNumber) {
    this->contactOrNumber = contactOrNumber;
    loadChatHistory(contactOrNumber);

    if (newChat) {
        int index = contactPicker->findData(contactOrNumber);
        if (index != -1) {
            contactPicker->setCurrentIndex(index);
        }
    } else {
        Contact contact = CacheManager::getContact(contactOrNumber);
        setScreenTitle(contact.name.isEmpty() ? contactOrNumber : contact.name);
    }
}

bool ChatScreen::isChatVisible(const QString &contactOrNumber) const {
    return this->contactOrNumber == contactOrNumber && isVisible();
}

void ChatScreen::loadChatHistory(const QString &contactOrNumber) {
    clearChatHistory();
    QVector<Message> messages = CacheManager::getMessages(contactOrNumber);
    for (const auto &message: messages) {
        addMessage(message);
    }
}

void ChatScreen::clearChatHistory() {
    waitingForDelivery.clear();

    QLayoutItem *child;
    while ((child = messagesLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    lastMessageTime = QDateTime();
}

QLabel *ChatScreen::addTimestampHeader(const QDateTime &dateTime) {
    QString timeString = Utils::getTimeString(dateTime, DateFormat::DATE_ONLY);

    QLabel *timestampLabel = new QLabel(timeString, this);
    timestampLabel->setObjectName("chatSmallLabel");
    timestampLabel->setAlignment(Qt::AlignCenter);
    messagesLayout->addWidget(timestampLabel);

    return timestampLabel;
}


void ChatScreen::addMessage(const Message &message) {
    QLabel *header = nullptr;
    if (!lastMessageTime.isValid() || lastMessageTime.daysTo(message.dateTime) > 0) {
        lastMessageTime = message.dateTime;
        header = addTimestampHeader(message.dateTime);
    }

    MessageEntryWidget *messageContainer = new MessageEntryWidget(message);
    messageContainer->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(messageContainer, &QWidget::customContextMenuRequested, this, &ChatScreen::showMessageContextMenu);

    if (header) {
        messageContainer->setHeader(header);
    }

    if (MessageEntryWidget *lastMessage =
        qobject_cast<MessageEntryWidget *>(messagesLayout->itemAt(messagesLayout->count() - 1)->widget())) {
        lastMessage->setNextMessage(messageContainer);
        }


    if (message.messageDirection == messageDirection::MD_INCOMING) {
        if (message.readStatus != read_status_t::RS_READ) {
            CacheManager::updateMessageStatus(message.uuid, read_status_t::RS_READ);
            emit smsRead(message.uuid);
        }
    } else {
        if (message.deliveryStatus != delivery_status_t::DS_DELIVERED) {
            waitingForDelivery.insert(message.uuid, messageContainer);
        }
    }

    scrollOnAdd = true;
    messagesLayout->addWidget(messageContainer);
}

void ChatScreen::onContactSelectionChanged(const QString &selectedContact) {
    QString selectedNumber = contactPicker->currentData().toString();
    if (selectedNumber.isEmpty()) {
        selectedNumber = selectedContact;
    }
    setContactOrNumber(selectedNumber);
}

void ChatScreen::showMessageContextMenu(const QPoint &pos) {
    MessageEntryWidget *message = qobject_cast<MessageEntryWidget *>(sender());
    QUuid uuid = message->getMessage().uuid;
    QLabel *header = message->getHeader();
    MessageEntryWidget *nextMessage = message->getNextMessage();

    QMenu contextMenu;
    contextMenu.menuAction()->setIconVisibleInMenu(true);

    QAction *deleteAction = contextMenu.addAction("Delete");
    QAction *copyAction = contextMenu.addAction("Copy");

    connect(deleteAction, &QAction::triggered, this, [=, this]() {
        CacheManager::removeMessage(uuid);
        delete message;
        scrollOnAdd = false;

        if (nextMessage && header) {
            if (nextMessage->getHeader()) {
                delete header;
            } else {
                nextMessage->setHeader(header);
            }
        } else {
            delete header;
        }
    });

    connect(copyAction, &QAction::triggered, this, [this, message]() {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(message->getMessage().message);
    });

    if (message->getMessage().messageDirection == messageDirection::MD_OUTGOING &&
        message->getMessage().deliveryStatus == delivery_status_t::DS_FAILED) {
        contextMenu.addSeparator();
        QAction *resendAction = contextMenu.addAction("Retry");
        connect(resendAction, &QAction::triggered, this, [this, message]() {
            waitingForDelivery.insert(message->getMessage().uuid, message);
            message->setDeliveryStatus(delivery_status_t::DS_PENDING);
            CacheManager::updateMessageStatus(message->getMessage().uuid, delivery_status_t::DS_PENDING);
            controller->sendSMS(message->getMessage().uuid, message->getMessage().number, message->getMessage().message);
        });
    }
    contextMenu.exec(message->mapToGlobal(pos));
}

void ChatScreen::sendMessage() {
    QString message = inputField->text();
    if (!message.isEmpty()) {
        Message newMessage = Message(QUuid::createUuid(), contactOrNumber, message, QDateTime::currentDateTime(),
                                     messageDirection::MD_OUTGOING);
        addMessage(newMessage);
        inputField->clear();
        controller->sendSMS(newMessage.uuid, contactOrNumber, message);
        CacheManager::addMessage(newMessage);
    }

    if (newChat) {
        setNewChat(false);
        Contact contact = CacheManager::getContact(contactOrNumber);
        setScreenTitle(contact.name.isEmpty() ? contactOrNumber : contact.name);
    }
}

void ChatScreen::handleIncomingMessage(const Message &message) {
    if (isVisible() && message.number == contactOrNumber) {
        addMessage(message);
    }
}

void ChatScreen::handleStatusChange(const QUuid &uuid, delivery_status_t status) {
    if (MessageEntryWidget *messageContainer = waitingForDelivery.value(uuid)) {
        messageContainer->setDeliveryStatus(status);
        if (status == delivery_status_t::DS_DELIVERED) {
            waitingForDelivery.remove(uuid);
        }
    }
}
