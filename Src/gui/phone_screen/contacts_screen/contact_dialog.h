//
// Created by andrew on 24.08.24.
//

#ifndef CONTACT_DIALOG_H
#define CONTACT_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>

#include "backend/models/models.h"


class ContactDialog : public QDialog {
Q_OBJECT

public:
    enum Mode {
        CreateMode,
        EditMode
    };

private:
    Contact contact;
    QLineEdit *nameEdit;
    QLineEdit *numberEdit;
    QPushButton *editButton = nullptr;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QPushButton *deleteButton = nullptr;
    QHBoxLayout *buttonLayout;
    Mode mode;

public:
    explicit ContactDialog(QWidget *parent = nullptr, const Contact &contact = Contact(), Mode mode = EditMode);

    Contact getUpdatedContact() const {
        return Contact(nameEdit->text(), numberEdit->text());
    }

signals:
    void contactDeleted();
    void contactUpdated(const Contact &contact);
    void contactCreated(const Contact &contact);

private slots:

    void onEditClicked();

    void onSaveClicked();

    void onDeleteClicked();

    void onCancelClicked();
};

#endif //CONTACT_DIALOG_H
