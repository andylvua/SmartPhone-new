//
// Created by andrew on 24.08.24.
//

#include "gui/misc/status_bar.h"
#include "gui/utils/fa.h"


StatusBar::StatusBar(QWidget *parent, Controller *controller) : QToolBar(parent),
                                                                          controller(controller) {
    connect(controller, &Controller::operatorChanged, this, &StatusBar::updateOperator);
    connect(controller, &Controller::signalStrengthChanged, this, &StatusBar::onSignalStrengthUpdated);

    setAttribute(Qt::WA_StyledBackground);
    setContextMenuPolicy(Qt::PreventContextMenu);

    this->setObjectName("StatusBar");

    setFixedHeight(30);
    setFloatable(false);
    setMovable(false);

    timeLabel = new QLabel();
    timeLabel->setObjectName("statusLabel");

    operatorLabel = new QLabel("");
    operatorLabel->setObjectName("statusLabel");

    QVariantMap options;
    options.insert("duotone-color", QColor(100, 100, 100));
    auto connectionLabelPixmap = FA::getInstance()->icon(fa::fa_duotone, fa::fa_signal_bars_slash,
                                                         options).pixmap(19, 19);
    connectionLabel = new QLabel();
    connectionLabel->setPixmap(connectionLabelPixmap);


    this->addWidget(timeLabel);

    QWidget *spacer1 = new QWidget();
    spacer1->setFixedSize(10, 10);
    this->addWidget(spacer1);

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->addWidget(spacer);

    this->addWidget(operatorLabel);

    QWidget *spacer2 = new QWidget();
    spacer2->setFixedSize(10, 10);
    this->addWidget(spacer2);

    mobileDataLabel = new QLabel();
    auto mobileDataLabelPixmap = FA::getInstance()->icon(fa::fa_regular, fa::fa_arrow_up_arrow_down,
                                                         options).pixmap(16, 16);
    mobileDataLabel->setPixmap(mobileDataLabelPixmap);
    QAction *mobileDataAction = this->addWidget(mobileDataLabel);
    mobileDataAction->setVisible(false);

    QWidget *spacer3 = new QWidget();
    spacer3->setFixedSize(10, 10);
    QAction *mobileDataSpacerAction = this->addWidget(spacer3);
    mobileDataSpacerAction->setVisible(false);
    this->addWidget(connectionLabel);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateTime);
    timer->start(1000);
    updateTime();

    QTimer *signalStrengthTimer = new QTimer(this);
    connect(signalStrengthTimer, &QTimer::timeout, this, &StatusBar::updateSignalStrengthRequest);
    signalStrengthTimer->start(10000);
    connect(this->controller, &Controller::initComplete, this, &StatusBar::updateSignalStrengthRequest);
    connect(this->controller, &Controller::GPRSConnected, this, [mobileDataAction, mobileDataSpacerAction]() {
        mobileDataSpacerAction->setVisible(true);
        mobileDataAction->setVisible(true);
    });
    connect(this->controller, &Controller::GPRSDisconnected, this, [mobileDataAction, mobileDataSpacerAction]() {
        mobileDataSpacerAction->setVisible(false);
        mobileDataAction->setVisible(false);
    });
}

void StatusBar::updateTime() const {
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm");
    timeLabel->setText(currentTime);
}

void StatusBar::updateOperator(const QString &operatorName) const {
    operatorLabel->setText(operatorName);
}

void StatusBar::updateSignalStrengthRequest() const {
    controller->getSignalStrength();
}

void StatusBar::onSignalStrengthUpdated(int strength) const {
    QVariantMap options;

    fa::fa_pro_icons icon;

    if (strength < 10) {
        icon = fa::fa_signal_bars_weak;
    } else if (strength < 15) {
        icon = fa::fa_signal_bars;
    } else if (strength < 20) {
        icon = fa::fa_signal_bars_good;
    } else {
        icon = fa::fa_signal_bars_strong;
    }

    if (icon != fa::fa_signal_bars_strong) {
        options.insert("duotone-color", QColor(100, 100, 100));
    }

    auto connectionLabelPixmap =
        FA::getInstance()->icon(fa::fa_duotone, icon, options).pixmap(19, 19);
    connectionLabel->setPixmap(connectionLabelPixmap);
}
