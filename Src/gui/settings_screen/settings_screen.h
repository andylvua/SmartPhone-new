//
// Created by andrew on 24.08.24.
//

#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "gui/application_screen/application_screen.h"
#include "controller/controller.h"

class SettingsScreen : public ApplicationScreen {
Q_OBJECT

    Controller *controller;
    bool cellularNetworkEnabled = false;

public:
    explicit SettingsScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

signals:
    void aboutDeviceRequested();

    void restartRequested();

private slots:
    void handleRestartRequest();
};


#endif //SETTINGS_SCREEN_H
