//
// Created by andrew on 10.08.24.
//

#ifndef PHONE_GUI_HPP
#define PHONE_GUI_HPP

#include <QStackedWidget>
#include <QStack>

#include "gui/main_screen/main_screen.h"
#include "gui/messages_screen/messages_screen.h"
#include "gui/messages_screen/chat_screen/chat_screen.h"
#include "gui/settings_screen/settings_screen.h"
#include "gui/phone_screen/phone_screen.h"
#include "gui/misc/status_bar.h"
#include "gui/settings_screen/about_device_screen/about_device_screen.h"
#include "gui/misc/call_screen.h"
#include "gui/at_console/at_console.h"
#include "gui/misc/restart_overlay.h"
#include "gui/browser_screen/browser_screen.h"

#include "controller/controller.h"


class MainWindow : public QMainWindow {
Q_OBJECT

    QStackedWidget* centralStack;
    QStack<QWidget *> screenStack;

    StatusBar* statusBar;
    MainScreen* mainScreen;
    PhoneScreen* phoneScreen;
    MessagesScreen* smsScreen;
    ChatScreen* smsChatScreen;
    BrowserScreen* browserScreen;
    ATConsoleScreen* atConsoleScreen;
    SettingsScreen* settingsScreen;
    AboutDeviceScreen* aboutDeviceScreen;
    RestartOverlay* loadingOverlay = nullptr;
    CallScreen* callScreen;

    Controller* controller;

public:
    explicit MainWindow(Controller* controller, QWidget* parent = nullptr);

private:
    void performConnects();

    void pushScreen(QWidget* screen);
    void popScreen();
    void goToMainScreen();

private slots:
    void onChatRequested(const QString& number);

    void onMessageReceived(const Message& message);

    void onCallMissed(const QString& callerNumber, const QString& callerName);

    void onRestartRequested();
};

#endif //PHONE_GUI_HPP
