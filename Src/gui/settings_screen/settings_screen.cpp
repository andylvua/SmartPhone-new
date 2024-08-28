//
// Created by andrew on 24.08.24.
//

#include <QVBoxLayout>
#include <QMessageBox>
#include <QStyle>

#include "gui/settings_screen/settings_screen.h"
#include "gui/utils/waitingspinnerwidget.h"
#include "gui/utils/fa.h"


SettingsScreen::SettingsScreen(QWidget *parent, Controller *controller) :
ApplicationScreen("Settings", parent), controller(controller) {
    QVBoxLayout *layout = getContentLayout();

    QWidget *cellularNetwork = new QWidget(this);
    cellularNetwork->setObjectName("cellularNetworkSetting");
    cellularNetwork->setFixedHeight(40);
    QHBoxLayout *cellularNetworkLayout = new QHBoxLayout(cellularNetwork);
    cellularNetworkLayout->setContentsMargins(15, 5, 10, 5);

    QLabel *cellularNetworkLabel = new QLabel("Cellular Network", this);

    WaitingSpinnerWidget *spinner = new WaitingSpinnerWidget(this, false, false);
    spinner->setRoundness(12.0);
    spinner->setMinimumTrailOpacity(15.0);
    spinner->setTrailFadePercentage(70.0);
    spinner->setNumberOfLines(8);
    spinner->setLineLength(4);
    spinner->setLineWidth(2);
    spinner->setInnerRadius(4);
    spinner->setRevolutionsPerSecond(1);
    spinner->setColor(Qt::gray);

    QPushButton *enableButton = new QPushButton("Connect", this);
    enableButton->setObjectName("networkEnableButton");

    QPushButton *infoButton = new QPushButton(FA::getInstance()->icon(fa::fa_thin, fa::fa_circle_info, {{"color", "#008dff"},{"color-active", "#008dff"}}), "", this);
    infoButton->setIconSize(QSize(20, 20));
    infoButton->setFlat(true);
    infoButton->setObjectName("infoButton");

    cellularNetwork->setStyleSheet("");
    cellularNetworkLayout->addWidget(cellularNetworkLabel);
    cellularNetworkLayout->addStretch();
    cellularNetworkLayout->addWidget(spinner);
    cellularNetworkLayout->addSpacing(5);
    cellularNetworkLayout->addWidget(enableButton);
    cellularNetworkLayout->addSpacing(5);
    cellularNetworkLayout->addWidget(infoButton);

    layout->addWidget(cellularNetwork);

    QLabel *descriptionForCellularNetwork = new QLabel("Connects the phone to the GPRS cellular network. "
                                                       "Calls and SMS will not be available when enabled.", this);
    descriptionForCellularNetwork->setObjectName("settingDescriptionLabel");
    descriptionForCellularNetwork->setWordWrap(true);
    descriptionForCellularNetwork->setContentsMargins(15, 0, 0, 0);
    layout->addWidget(descriptionForCellularNetwork);
    layout->addSpacing(10);

    QPushButton *aboutDeviceButton = new QPushButton("About Device", this);
    aboutDeviceButton->setObjectName("settingsButton");
    aboutDeviceButton->setFixedHeight(40);
    layout->addWidget(aboutDeviceButton);

    layout->addStretch();

    QPushButton *restartButton = new QPushButton("Restart", this);
    layout->addWidget(restartButton);
    restartButton->setObjectName("restartButton");
    restartButton->setFixedHeight(40);

    QLabel *descriptionForRestart = new QLabel("Issues a restart command to the device. "
                                               "It may take up to 30 seconds to complete.", this);
    descriptionForRestart->setObjectName("settingDescriptionLabel");
    descriptionForRestart->setWordWrap(true);
    descriptionForRestart->setContentsMargins(15, 0, 0, 0);
    layout->addWidget(descriptionForRestart);

    connect(enableButton, &QPushButton::clicked, this, [this, spinner, enableButton]() {
        if (!cellularNetworkEnabled) {
            this->controller->turnOnMobileData();
        } else {
            this->controller->turnOffMobileData();
        }
        enableButton->setEnabled(false);
        spinner->start();
    });

    connect(this->controller, &Controller::GPRSConnected, this, [this, spinner, enableButton]() {
        spinner->stop();
        enableButton->setEnabled(true);
        cellularNetworkEnabled = true;
        enableButton->setText("Disconnect");
        enableButton->setObjectName("networkDisableButton");
        enableButton->style()->unpolish(enableButton);
        enableButton->style()->polish(enableButton);
    });

    connect(this->controller, &Controller::GPRSDisconnected, this, [this, spinner, enableButton]() {
        spinner->stop();
        enableButton->setEnabled(true);
        cellularNetworkEnabled = false;
        enableButton->setText("Connect");
        enableButton->setObjectName("networkEnableButton");
        enableButton->style()->unpolish(enableButton);
        enableButton->style()->polish(enableButton);
    });

    connect(this->controller, &Controller::GPRSError, this, [this, spinner, enableButton](PppdExitStatus exitCode) {
        spinner->stop();
        enableButton->setEnabled(true);
        QMessageBox::critical(this, "Error", "An error occurred while trying to connect to the cellular network: " + toString(exitCode));
    });

    connect(infoButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this,
            "Cellular Network",
            "Uses a PPPoS (Point-to-Point Protocol over Serial) connection "
            "to attach the device to the GPRS cellular network. "
            "As the A9 modem used in this device does not support separate data line, "
            "it will not be able to execute any commands while connected to the network.");
    });

    connect(aboutDeviceButton, &QPushButton::clicked, this, &SettingsScreen::aboutDeviceRequested);
    connect(restartButton, &QPushButton::clicked, this, &SettingsScreen::handleRestartRequest);
}

void SettingsScreen::handleRestartRequest() {
    controller->restart();
    emit restartRequested();
}
