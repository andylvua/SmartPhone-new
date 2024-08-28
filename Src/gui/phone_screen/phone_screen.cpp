//
// Created by andrew on 24.08.24.
//

#include <QTabWidget>
#include <QVBoxLayout>

#include "gui/phone_screen/phone_screen.h"
#include "gui/utils/fa.h"


PhoneScreen::PhoneScreen(QWidget *parent, Controller *controller)
        : ApplicationScreen("Phone", parent), controller(controller) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("PhoneScreen");

    QVBoxLayout *layout = getContentLayout();

    QTabWidget *tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget);

    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setDocumentMode(true);
    tabWidget->setContentsMargins(0, 0, 0, 0);

    historyScreen = new HistoryScreen(this, controller);
    contactsScreen = new ContactsScreen(this, controller);
    keypadScreen = new KeypadScreen(this, controller);

    tabWidget->addTab(historyScreen, "History");
    tabWidget->setTabIcon(0, FA::getInstance()->icon(fa::fa_solid, fa::fa_history));
    tabWidget->addTab(contactsScreen, "Contacts");
    tabWidget->setTabIcon(1, FA::getInstance()->icon(fa::fa_solid, fa::fa_address_book));
    tabWidget->addTab(keypadScreen, "Keypad");
    tabWidget->setTabIcon(2, FA::getInstance()->icon(fa::fa_solid, fa::fa_grid));

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        switch (index) {
            case 0:
                historyScreen->populateHistory();
                break;
            case 1:
                contactsScreen->populateContacts();
                break;
            case 2:
                // Keypad screen
                    break;
            }
        });
}
