//
// Created by andrew on 24.08.24.
//

#ifndef ABOUT_DEVICE_SCREEN_H
#define ABOUT_DEVICE_SCREEN_H

#include "gui/application_screen/application_screen.h"
#include "controller/controller.h"

class InfoEntry : public QWidget {

    QFrame *separator;
    QLabel *valueLabel;

public:
    InfoEntry(const QString &title, const QString &value, QWidget *parent) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *itemLayout = new QHBoxLayout();

        QLabel *titleLabel = new QLabel(title, this);
        titleLabel->setObjectName("InfoEntryTitleLabel");
        itemLayout->addWidget(titleLabel);

        valueLabel = new QLabel(value, this);
        valueLabel->setObjectName("InfoEntryValueLabel");
        valueLabel->setAlignment(Qt::AlignRight);
        itemLayout->addWidget(valueLabel);

        itemLayout->setContentsMargins(5, 10, 5, 10);

        mainLayout->addLayout(itemLayout);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        itemLayout->setSpacing(9);

        QHBoxLayout *separatorLayout = new QHBoxLayout();
        separatorLayout->setContentsMargins(0, 0, 0, 0);

        separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Plain);
        separator->setObjectName("InfoEntrySeparator");
        separator->setFixedHeight(1);

        separatorLayout->addWidget(separator);

        mainLayout->addLayout(separatorLayout);
    }

    void removeSeparator() {
        separator->hide();
    }

    void setValue(const QString &value) {
        valueLabel->setText(value);
    }
};

class AboutDeviceScreen : public ApplicationScreen {
Q_OBJECT

    Modem::ModemInfo modemInfo;
    Controller *controller;

    InfoEntry *manufacturerEntry;
    InfoEntry *modelEntry;
    InfoEntry *revisionEntry;
    InfoEntry *numberEntry;
    InfoEntry *operatorEntry;
    InfoEntry *registrationEntry;

public:
    explicit AboutDeviceScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    void updateInfo();
};


#endif //ABOUT_DEVICE_SCREEN_H
