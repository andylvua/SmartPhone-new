//
// Created by andrew on 24.08.24.
//

#include "gui/application_screen/application_screen.h"
#include "gui/utils/gui_utils.h"
#include "gui/utils/fa.h"


ApplicationScreen::ApplicationScreen(const QString& title, QWidget *parent, bool titleOnTop) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("ApplicationScreen");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(5, 0, 5, 9);

    headerLayout = new QGridLayout();
    headerLayout->setColumnStretch(0, 1);
    headerLayout->setColumnStretch(1, 0);
    headerLayout->setColumnStretch(2, 1);
    headerLayout->setColumnStretch(3, 0);
    headerLayout->setColumnStretch(4, 1);

    backButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_chevron_left, {{"color", "#008dff"}, {"color-active", "#008dff"}}), "Back", this);
    backButton->installEventFilter(new HoverEventFilter());
    backButton->setFixedSize(65, 30);
    backButton->setFlat(true);
    backButton->setObjectName("BackButton");

    headerLayout->addWidget(backButton, 0, 0, Qt::AlignLeft);
    layout->addLayout(headerLayout);

    if (titleOnTop) {
        screenTitle = new QLabel(title, this);
        screenTitle->setObjectName("ScreenTitleTop");
        headerLayout->addWidget(screenTitle, 0, 2, Qt::AlignCenter);
    } else {
        screenTitle = new QLabel(title, this);
        screenTitle->setObjectName("ScreenTitle");
        screenTitle->setAlignment(Qt::AlignLeft);
        screenTitle->setStyleSheet("font-weight: bold; font-size: 28px;");
        screenTitle->setContentsMargins(10, 0, 10, 0);
        layout->addWidget(screenTitle);
    }

    contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop);
    contentLayout->setContentsMargins(10, 0, 10, 0);
    layout->addLayout(contentLayout);

    setLayout(layout);

    connect(backButton, &QPushButton::clicked, BackButtonHandler::instance(), &BackButtonHandler::handleBack);
}

void ApplicationScreen::addActionButton(QPushButton *button) {
    actionButton = button;
    QSizePolicy spRetain = button->sizePolicy();
    spRetain.setRetainSizeWhenHidden(true);
    button->setSizePolicy(spRetain);
    headerLayout->addWidget(button, 0, 4, Qt::AlignRight);
}

void ApplicationScreen::showActionButton() const {
    if (actionButton) {
        actionButton->show();
    }
}

void ApplicationScreen::hideActionButton() const {
    if (actionButton) {
        actionButton->hide();
    }
}

void ApplicationScreen::setScreenTitle(const QString &title) const {
    screenTitle->setText(title);
}
