//
// Created by andrew on 26.08.24.
//

#include <QHBoxLayout>

#include "gui/at_console/command_entry.h"


CommandEntryWidget::CommandEntryWidget(const QString& command, bool urc, const QString& response) : urc(urc) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QVBoxLayout *itemLayout = new QVBoxLayout();

    commandLabel = new QLabel(command, this);
    commandLabel->setObjectName("consoleCommandLabel");
    if (urc) {
        commandLabel->setObjectName("consoleURCLabel");
        commandLabel->setText("URC:");
    }

    responseLabel = new QLabel("", this);
    responseLabel->setTextFormat(Qt::PlainText);

    if (!response.isEmpty()) {
        responseLabel->setText(response);
    }
    responseLabel->setWordWrap(true);

    responseLabel->setObjectName("consoleResponseLabel");
    itemLayout->addWidget(commandLabel);
    itemLayout->addWidget(responseLabel);

    QHBoxLayout *separatorLayout = new QHBoxLayout();
    separatorLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setObjectName("consoleSeparator");
    separator->setFixedHeight(1);

    separatorLayout->addWidget(separator);

    itemLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(0);
    itemLayout->setSpacing(9);

    mainLayout->addLayout(itemLayout);
    mainLayout->addLayout(separatorLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}
