//
// Created by andrew on 24.08.24.
//

#ifndef USSD_DIALOG_H
#define USSD_DIALOG_H

#include <QDialog>
#include <QLabel>

#include "gui/utils/waitingspinnerwidget.h"

class USSDDialog : public QDialog {
Q_OBJECT

    QLabel *loadingLabel;
    WaitingSpinnerWidget *spinner;
    QPushButton *closeButton;

public:
    explicit USSDDialog(QWidget *parent = nullptr);

    void setResponse(const QString &response) const;
};

#endif //USSD_DIALOG_H
