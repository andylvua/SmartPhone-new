//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "gui/main_screen/main_screen.h"
#include "gui/utils/fa.h"

MainScreen::MainScreen(QWidget *parent, Controller *controller) : QWidget(parent), controller(controller) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("MainScreen");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    auto createButton = [&]<typename Icon>(const Icon &iconName, const QString &labelText) {
        QPushButton *button = new QPushButton(FA::getInstance()->icon(fa::fa_solid, iconName), "", this);
        button->setIconSize(QSize(30, 30));
        button->setFixedSize(70, 70);
        button->setObjectName("ApplicationButton");

        QLabel *label = new QLabel(labelText, this);
        label->setObjectName("ApplicationButtonLabel");
        label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *buttonLayout = new QVBoxLayout();
        buttonLayout->setAlignment(Qt::AlignCenter);
        buttonLayout->setSpacing(5);
        buttonLayout->addWidget(button);
        buttonLayout->addWidget(label);

        return std::make_pair(button, buttonLayout);
    };

    auto [phoneButton, phoneButtonLayout] = createButton(fa::fa_phone, "Phone");
    auto [smsButton, smsButtonLayout] = createButton(fa::fa_comment, "Messages");
    auto [browserButton, browserButtonLayout] = createButton(fa::fa_compass, "Browser");
    auto [consolesButton, consolesButtonLayout] = createButton(fa::fa_rectangle_terminal, "Console");
    auto [settingsButton, settingsButtonLayout] = createButton(fa::fa_cog, "Settings");

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addLayout(phoneButtonLayout);
    buttonsLayout->addLayout(smsButtonLayout);
    buttonsLayout->addLayout(browserButtonLayout);
    buttonsLayout->addLayout(consolesButtonLayout);
    buttonsLayout->addLayout(settingsButtonLayout);

    notificationsList = new QListWidget(this);

    notificationsList->setFrameStyle(QFrame::NoFrame);
    notificationsList->setSpacing(5);
    notificationsList->setContentsMargins(0, 0, 0, 0);

    QLabel *notificationsLabel = new QLabel("Notifications", this);
    notificationsLabel->setObjectName("NotificationsLabel");
    notificationsLabel->setAlignment(Qt::AlignLeft);
    notificationsLabel->setContentsMargins(5, 0, 5, 0);

    layout->addSpacing(10);
    layout->addWidget(notificationsLabel);
    layout->addWidget(notificationsList);
    layout->addLayout(buttonsLayout);

    connect(phoneButton, &QPushButton::clicked, this, &MainScreen::phoneScreenRequested);
    connect(smsButton, &QPushButton::clicked, this, &MainScreen::smsScreenRequested);
    connect(browserButton, &QPushButton::clicked, this, &MainScreen::browserScreenRequested);
    connect(consolesButton, &QPushButton::clicked, this, &MainScreen::consolesScreenRequested);
    connect(settingsButton, &QPushButton::clicked, this, &MainScreen::settingsScreenRequested);

    connect(notificationsList, &QListWidget::itemDoubleClicked, this, &MainScreen::handleNotificationClick);
}

NotificationWidget* MainScreen::addNotification(const QIcon &icon, const QString &title, const QString &message) {
    QListWidgetItem *item = new QListWidgetItem(notificationsList);
    NotificationWidget *notification = new NotificationWidget(item, icon, title, message, this);
    item->setSizeHint(notification->sizeHint());
    notificationsList->setItemWidget(item, notification);

    return notification;
}

void MainScreen::handleNotificationClick(QListWidgetItem *item) const {
    auto *notification = qobject_cast<NotificationWidget *>(notificationsList->itemWidget(item));
    notification->handleAction();
}
