//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>

#include "gui/main_screen/notification.h"
#include "gui/utils/fa.h"

NotificationWidget::NotificationWidget(QListWidgetItem *parentItem, const QIcon &icon, const QString &title,
                            const QString &message, QWidget *parent) : QWidget(parent), parentItem(parentItem) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("NotificationWidget");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel *iconLabel = new QLabel(this);
    iconLabel->setFixedSize(45, 45);
    iconLabel->setPixmap(QPixmap(icon.pixmap(25, 25)));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setObjectName("NotificationIconLabel");
    layout->addWidget(iconLabel);
    layout->addSpacing(5);

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(0);
    textLayout->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("NotificationTitleLabel");
    textLayout->addWidget(titleLabel);

    QLabel *messageLabel = new QLabel("", this);
    QFontMetrics metrics(messageLabel->font());
    auto messageText = metrics.elidedText(message, Qt::ElideRight, 270);
    messageLabel->setText(messageText);
    messageLabel->setObjectName("NotificationMessageLabel");
    textLayout->addWidget(messageLabel);

    layout->addLayout(textLayout);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(spacer);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);

    layout->addLayout(buttonLayout);
    layout->addSpacing(20);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(5);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm"), this);
    timeLabel->setObjectName("NotificationTimeLabel");
    timeLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(timeLabel);

    QPushButton *closeButton = new QPushButton(this);
    closeButton->setIcon(FA::getInstance()->icon(fa::fa_solid, fa::fa_times));
    closeButton->setFixedSize(40, 25);
    closeButton->setObjectName("NotificationCloseButton");
    closeButton->setCursor(Qt::PointingHandCursor);

    connect(closeButton, &QPushButton::clicked, this, &NotificationWidget::remove);
    rightLayout->addWidget(closeButton);

    layout->addLayout(rightLayout);
}

void NotificationWidget::remove() {
    this->hide();
    this->deleteLater();
    delete parentItem;
}

void NotificationWidget::handleAction() {
    if (action) {
        action();
    }
}
