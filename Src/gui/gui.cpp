//
// Created by andrew on 26.08.24.
//

#include <QMessageBox>
#include <QGraphicsBlurEffect>

#include "gui/gui.h"
#include "gui/utils/fa.h"
#include "backend/utils/cache_manager.h"
#include "gui/utils/gui_utils.h"


MainWindow::MainWindow(Controller* controller, QWidget* parent)
    : QMainWindow(parent), controller(controller) {
    setFixedSize(480, 900);
    setWindowTitle("Phone App");

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("MainWindow");

    statusBar = new StatusBar(this, controller);
    addToolBar(Qt::TopToolBarArea, statusBar);

    centralStack = new QStackedWidget(this);
    centralStack->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(centralStack);
    centralStack->setObjectName("CentralStack");

    mainScreen = new MainScreen(this, controller);
    phoneScreen = new PhoneScreen(this, controller);
    smsScreen = new MessagesScreen(this, controller);
    smsChatScreen = new ChatScreen(this, controller);
    callScreen = new CallScreen(this);
    settingsScreen = new SettingsScreen(this, controller);
    aboutDeviceScreen = new AboutDeviceScreen(this, controller);
    atConsoleScreen = new ATConsoleScreen(this, controller);
    browserScreen = new BrowserScreen(this);

    centralStack->addWidget(mainScreen);
    centralStack->addWidget(phoneScreen);
    centralStack->addWidget(smsScreen);
    centralStack->addWidget(smsChatScreen);
    centralStack->addWidget(callScreen);
    centralStack->addWidget(settingsScreen);
    centralStack->addWidget(aboutDeviceScreen);
    centralStack->addWidget(atConsoleScreen);
    centralStack->addWidget(browserScreen); // todo: maybe destroy this screen when not in use, browser is heavy
    centralStack->setCurrentWidget(mainScreen);

    performConnects();
}

void MainWindow::performConnects() {
    // Module connections
    connect(controller, &Controller::initComplete, this, [this]() {
        if (loadingOverlay) {
            centralStack->setGraphicsEffect(nullptr);
            loadingOverlay->deleteLater();
            goToMainScreen();
        }
    });

    // Navigation connections
    connect(mainScreen, &MainScreen::phoneScreenRequested, this, [this]() {
        pushScreen(phoneScreen);
    });

    connect(mainScreen, &MainScreen::smsScreenRequested, this, [this]() {
        pushScreen(smsScreen);
    });

    connect(mainScreen, &MainScreen::browserScreenRequested, this, [this]() {
        pushScreen(browserScreen);
    });

    connect(smsScreen, &MessagesScreen::smsChatRequested, this, [this](const QString& number) {
        smsChatScreen->setNewChat(false);
        smsChatScreen->setContactOrNumber(number);
        pushScreen(smsChatScreen);
    });

    connect(smsScreen, &MessagesScreen::newSmsScreenRequested, this, [this]() {
        smsChatScreen->setNewChat(true);
        pushScreen(smsChatScreen);
    });

    connect(mainScreen, &MainScreen::settingsScreenRequested, this, [this]() {
        pushScreen(settingsScreen);
    });

    connect(mainScreen, &MainScreen::consolesScreenRequested, this, [this]() {
        pushScreen(atConsoleScreen);
    });

    connect(settingsScreen, &SettingsScreen::aboutDeviceRequested, this, [this]() {
        aboutDeviceScreen->updateInfo();
        pushScreen(aboutDeviceScreen);
    });

    connect(BackButtonHandler::instance(), &BackButtonHandler::backRequested, this, [this]() {
        popScreen();
    });

    connect(phoneScreen->getContactsScreen(), &ContactsScreen::smsChatRequested, this, &MainWindow::onChatRequested);

    // Controller connections
    //     Message related
    connect(controller, &Controller::smsReceived, this, &MainWindow::onMessageReceived);

    connect(controller, &Controller::simCardReady, this, [this](bool ready) {
        if (!ready) {
            QMessageBox::warning(this, "No SIM", "SIM card not detected.");
        }
    });

    //     Call related
    connect(controller, &Controller::incomingCall, this,
            [this](const QString& callerNumber, const QString& callerName) {
                callScreen->handleIncomingCall(callerNumber, callerName);
                pushScreen(callScreen);
            });
    connect(controller, &Controller::outgoingCall, this,
            [this](const QString& receiverNumber, const QString& receiverName) {
                callScreen->handleOutgoingCall(receiverNumber, receiverName);
                pushScreen(callScreen);
            });

    connect(controller, &Controller::callAnswered, callScreen, &CallScreen::handleAnswerCall);
    connect(controller, &Controller::callEnded, callScreen, &CallScreen::handleRejectCall);

    connect(callScreen, &CallScreen::callRejected, controller, &Controller::rejectCall);
    connect(callScreen, &CallScreen::callAnswered, controller, &Controller::acceptCall);

    connect(callScreen, &CallScreen::callEnded, this, [this]() {
        popScreen();
    });
    connect(controller, &Controller::callMissed, this, &MainWindow::onCallMissed);

    // Misc connections
    connect(settingsScreen, &SettingsScreen::restartRequested, this, &MainWindow::onRestartRequested);
    connect(atConsoleScreen, &ATConsoleScreen::easter, this, [this]() {
        pushScreen(browserScreen);
        browserScreen->openEasterEgg();
    });
}

void MainWindow::pushScreen(QWidget* screen) {
    screenStack.push(centralStack->currentWidget());
    centralStack->setCurrentWidget(screen);
}

void MainWindow::popScreen() {
    if (!screenStack.isEmpty()) {
        QWidget* previousScreen = screenStack.pop();
        centralStack->setCurrentWidget(previousScreen);
    }
}

void MainWindow::goToMainScreen() {
    while (!screenStack.isEmpty()) {
        screenStack.pop();
    }
    centralStack->setCurrentWidget(mainScreen);
}

void MainWindow::onChatRequested(const QString& number) {
    QMap<QString, QVector<Message>> messages = CacheManager::getMessages();

    if (messages.contains(number)) {
        smsChatScreen->setNewChat(false);
    } else {
        smsChatScreen->setNewChat(true);
    }
    smsChatScreen->setContactOrNumber(number);
    pushScreen(smsChatScreen);
}

void MainWindow::onMessageReceived(const Message& message) {
    if (smsChatScreen->isChatVisible(message.number)) {
        return;
    }

    Contact contact = CacheManager::getContact(message.number);
    QString contactName = contact.name.isEmpty() ? message.number : contact.name;

    auto* notification = mainScreen->addNotification(FA::getInstance()->icon(fa::fa_solid, fa::fa_comment),
                                                     contactName, message.message);

    notification->setAction([message, this]() {
        smsChatScreen->setNewChat(false);
        smsChatScreen->setContactOrNumber(message.number);
        pushScreen(smsChatScreen);
    });

    notification->setRemoveEvent(smsChatScreen, &ChatScreen::smsRead, [message](const QUuid& uuid) {
        return message.uuid == uuid;
    });
}

void MainWindow::onCallMissed(const QString& callerNumber, const QString& callerName) {
    QString contactName = callerName.isEmpty() ? callerNumber : callerName;
    auto* notification = mainScreen->addNotification(
        FA::getInstance()->icon(fa::fa_solid, fa::fa_phone),
        contactName, "Missed call");

    notification->setAction([callerNumber, this]() {
        this->controller->makeCall(callerNumber);
    });

    notification->setRemoveEvent(phoneScreen->getHistoryScreen(), &HistoryScreen::screenShown);
}

void MainWindow::onRestartRequested() {
    QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect;
    blurEffect->setBlurRadius(5);
    centralStack->setGraphicsEffect(blurEffect);

    loadingOverlay = new RestartOverlay(this);
    loadingOverlay->show();
}
