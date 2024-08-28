//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include "gui/phone_screen/contacts_screen/contact_entry.h"
#include "gui/utils/fa.h"


ContactEntryWidget::ContactEntryWidget(const Contact &entry, Controller *controller, QWidget *parent)
    : QWidget(parent), contact(entry), controller(controller) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *itemLayout = new QHBoxLayout();

    contactLabel = new QLabel(contact.name, this);
    contactLabel->setObjectName("contactLabel");
    contactLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    itemLayout->addWidget(contactLabel);

    QPushButton *callButton = new QPushButton(
            FA::getInstance()->icon(fa::fa_solid, fa::fa_phone, {{"color", "#008dff"}}), "",
            this);
    QPushButton *smsButton = new QPushButton(
            FA::getInstance()->icon(fa::fa_solid, fa::fa_comment, {{"color", "#008dff"}}), "",
            this);

    callButton->setFixedSize(40, 40);
    callButton->setObjectName("contactButton");
    smsButton->setFixedSize(40, 40);
    smsButton->setObjectName("contactButton");
    callButton->setIconSize(QSize(20, 20));
    smsButton->setIconSize(QSize(20, 20));

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    itemLayout->addItem(spacer);
    itemLayout->addWidget(callButton);
    itemLayout->addWidget(smsButton);

    itemLayout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *separatorLayout = new QHBoxLayout();
    separatorLayout->setContentsMargins(0, 0, 100, 0);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setObjectName("contactSeparator");
    separator->setFixedHeight(1);

    separatorLayout->addWidget(separator);

    mainLayout->addLayout(itemLayout);
    mainLayout->addLayout(separatorLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    itemLayout->setSpacing(9);

    connect(callButton, &QPushButton::clicked, this, [this]() {
        this->controller->makeCall(contact.number);
    });
    connect(smsButton, &QPushButton::clicked, this, [this]() {
        emit smsChatRequested(contact.number);
    });
}
