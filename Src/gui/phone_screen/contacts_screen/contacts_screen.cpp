//
// Created by andrew on 24.08.24.
//

#include <QHideEvent>

#include "contacts_screen.h"
#include "contact_entry.h"
#include "contact_dialog.h"
#include "gui/utils/fa.h"
#include "backend/utils/cache_manager.h"
#include "gui/application_screen/application_screen.h"
#include "gui/utils/gui_utils.h"


ContactsScreen::ContactsScreen(QWidget *parent, Controller *controller)
        : QWidget(parent), controller(controller), parent(parent) {
    contactsList = new QListWidget(this);
    contactsList->setFrameStyle(QFrame::NoFrame);
    contactsList->setObjectName("ContactsList");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);

    newContactButton = new QPushButton("", this);
    newContactButton->setIcon(
            FA::getInstance()->icon(fa::fa_regular, fa::fa_plus, {{"color", "#008dff"}}));
    newContactButton->setIconSize(QSize(30, 30));
    newContactButton->installEventFilter(new HoverEventFilter());
    newContactButton->setFlat(true);
    newContactButton->setFixedSize(30, 30);
    if (ApplicationScreen *serviceScreen = qobject_cast<ApplicationScreen *>(parent)) {
        serviceScreen->addActionButton(newContactButton);
        serviceScreen->hideActionButton();
    }

    layout->addWidget(contactsList);

    connect(newContactButton, &QPushButton::clicked, this, &ContactsScreen::onNewContactClicked);
    connect(contactsList, &QListWidget::itemDoubleClicked, this, &ContactsScreen::onContactItemDoubleClicked);
}

void ContactsScreen::addContact(const Contact &entry) {
    QListWidgetItem *item = new QListWidgetItem(contactsList);

    ContactEntryWidget *contactWidget = new ContactEntryWidget(entry, controller, this);
    connect(contactWidget, &ContactEntryWidget::smsChatRequested, this, &ContactsScreen::smsChatRequested);

    item->setSizeHint(contactWidget->sizeHint());
    contactsList->setItemWidget(item, contactWidget);
}

void ContactsScreen::populateContacts() {
    contactsList->clear();
    QVector<Contact> contactsEntries = CacheManager::getContacts();
    for (const auto &entry: contactsEntries) {
        addContact(entry);
    }
}

void ContactsScreen::showEvent(QShowEvent *event) {
    populateContacts();

    ApplicationScreen *serviceScreen = qobject_cast<ApplicationScreen *>(parent);
    if (serviceScreen) {
        serviceScreen->showActionButton();
    }

    QWidget::showEvent(event);
}

void ContactsScreen::hideEvent(QHideEvent *event) {
    ApplicationScreen *serviceScreen = qobject_cast<ApplicationScreen *>(parent);
    if (serviceScreen) {
        serviceScreen->hideActionButton();
    }

    QWidget::hideEvent(event);
}


void ContactsScreen::onNewContactClicked() {
    ContactDialog dialog(this, Contact(), ContactDialog::CreateMode);

    connect(&dialog, &ContactDialog::contactCreated, this, [this](const Contact &newContact) {
        CacheManager::addContact(newContact.name, newContact.number);
        addContact(newContact);
    });

    dialog.exec();
}

void ContactsScreen::onContactItemDoubleClicked(QListWidgetItem *item) {
    ContactEntryWidget *contactWidget = qobject_cast<ContactEntryWidget *>(contactsList->itemWidget(item));
    if (!contactWidget) {
        return;
    }

    Contact contact = contactWidget->getContact();
    ContactDialog dialog(this, contact, ContactDialog::EditMode);

    connect(&dialog, &ContactDialog::contactUpdated, this, [this, contact, contactWidget](const Contact &updatedContact) {
        CacheManager::editContact(contact, updatedContact.name, updatedContact.number);
        contactWidget->updateContact(updatedContact);
    });

    connect(&dialog, &ContactDialog::contactDeleted, this, [item, contact]() {
        CacheManager::removeContact(contact);
        delete item;
    });

    dialog.exec();
}
