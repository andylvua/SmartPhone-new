//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "gui/phone_screen/keypad_screen/ussd_dialog.h"


USSDDialog::USSDDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("USSD");
    setFixedWidth(200);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *loadingLayout = new QHBoxLayout();
    loadingLayout->setAlignment(Qt::AlignCenter);

    loadingLabel = new QLabel("Loading...", this);
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLayout->addWidget(loadingLabel);

    spinner = new WaitingSpinnerWidget(this);
    spinner->setRoundness(15.0);
    spinner->setMinimumTrailOpacity(15.0);
    spinner->setTrailFadePercentage(70.0);
    spinner->setNumberOfLines(12);
    spinner->setLineLength(5);
    spinner->setLineWidth(2);
    spinner->setInnerRadius(5);
    spinner->setRevolutionsPerSecond(1);
    spinner->setColor(QColor(81, 81, 81));

    loadingLayout->addWidget(spinner, 0, Qt::AlignCenter);

    layout->addLayout(loadingLayout);

    closeButton = new QPushButton("Close", this);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &USSDDialog::close);

    spinner->start();
}

void USSDDialog::setResponse(const QString &response) const {
    loadingLabel->setText(response);
    spinner->stop();
    spinner->hide();
}
