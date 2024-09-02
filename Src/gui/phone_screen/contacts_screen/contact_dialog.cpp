//
// Created by andrew on 24.08.24.
//

#include <QLabel>
#include <QRegularExpressionValidator>

#include "gui/phone_screen/contacts_screen/contact_dialog.h"
#include "gui/utils/fa.h"


ContactDialog::ContactDialog(QWidget *parent, const Contact &contact, Mode mode)
        : QDialog(parent), contact(contact), mode(mode) {

    setWindowTitle(mode == EditMode ? "Contact" : "Create Contact");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *nameLabel = new QLabel("Name:", this);
    nameEdit = new QLineEdit(contact.name, this);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);

    QLabel *numberLabel = new QLabel("Number:", this);
    numberEdit = new QLineEdit(contact.number, this);

    QRegularExpression rx("^\\+380\\d{9}$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    numberEdit->setValidator(validator);
    mainLayout->addWidget(numberLabel);
    mainLayout->addWidget(numberEdit);

    buttonLayout = new QHBoxLayout();

    if (mode == EditMode) {
        nameEdit->setReadOnly(true);
        numberEdit->setReadOnly(true);

        editButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_edit), "", this);
        deleteButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_trash), "", this);
        saveButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_save), "", this);
        cancelButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_times), "", this);

        saveButton->hide();
        cancelButton->hide();

        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);

        connect(editButton, &QPushButton::clicked, this, &ContactDialog::onEditClicked);
        connect(deleteButton, &QPushButton::clicked, this, &ContactDialog::onDeleteClicked);
        connect(saveButton, &QPushButton::clicked, this, &ContactDialog::onSaveClicked);
        connect(cancelButton, &QPushButton::clicked, this, &ContactDialog::onCancelClicked);
    } else {
        saveButton = new QPushButton("Save", this);
        cancelButton = new QPushButton("Cancel", this);

        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(cancelButton);

        connect(saveButton, &QPushButton::clicked, this, &ContactDialog::onSaveClicked);
        connect(cancelButton, &QPushButton::clicked, this, &ContactDialog::reject);
    }

    mainLayout->addLayout(buttonLayout);
}

void ContactDialog::onEditClicked() {
    nameEdit->setReadOnly(false);
    numberEdit->setReadOnly(false);

    saveButton->show();
    cancelButton->show();

    editButton->hide();
    deleteButton->hide();

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
}

void ContactDialog::onSaveClicked() {
    contact = getUpdatedContact();
    if (mode == EditMode) {
        emit contactUpdated(contact);
    } else {
        emit contactCreated(contact);
    }
    accept();
}

void ContactDialog::onDeleteClicked() {
    emit contactDeleted();
    accept();
}

void ContactDialog::onCancelClicked() {
    nameEdit->setReadOnly(true);
    numberEdit->setReadOnly(true);

    saveButton->hide();
    cancelButton->hide();

    editButton->show();
    deleteButton->show();
}
