//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QCompleter>
#include <QMenu>
#include <QAction>
#include <QStringListModel>
#include <QClipboard>
#include <qevent.h>

#include "gui/at_console/at_console.h"
#include "gui/utils/fa.h"
#include "gui/at_console/command_entry.h"
#include "controller/controller.h"

ATConsoleScreen::ATConsoleScreen(QWidget *parent, Controller *controller)
    : ApplicationScreen("AT Console", parent), controller(controller) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("ATConsoleScreen");

    connect(controller, &Controller::urcReceived, this, &ATConsoleScreen::addURC);

    QVBoxLayout *layout = getContentLayout();

    commandList = new QListWidget(this);
    commandList->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout *inputLayout = new QHBoxLayout();

    commandInput = new QLineEdit(this);
    commandInput->setPlaceholderText("Enter AT command...");
    commandInput->setFixedHeight(40);
    commandInput->setValidator(new QRegExpValidator(QRegExp("^AT.*", Qt::CaseInsensitive), this));

    QStringListModel *completerModel = new QStringListModel(this);
    QFile atCommandsFile("/home/andrew/CLionProjects/SmartPhone-dev/assets/at_commands.txt");
    if (!atCommandsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open completions file";
    }

    QTextStream in(&atCommandsFile);
    QStringList atCommands;
    while (!in.atEnd()) {
        QString line = in.readLine();
        atCommands.append(line);
    }

    completerModel->setStringList(atCommands);

    QCompleter *completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setMaxVisibleItems(5);
    commandInput->setCompleter(completer);

    QPushButton *sendButton = new QPushButton("", this);
    sendButton->setFixedSize(60, 40);
    sendButton->setIconSize(QSize(20, 20));
    QIcon sendIcon = FA::getInstance()->icon(fa::fa_regular, fa::fa_turn_down);

    QTransform transform;
    transform.rotate(90);
    sendButton->setIcon(sendIcon.pixmap(20, 20).transformed(transform));


    inputLayout->addWidget(commandInput);
    inputLayout->addWidget(sendButton);

    layout->addWidget(commandList);
    layout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, [this]() {
        QString command = commandInput->text();
        if (command.isEmpty()) {
            return;
        }

        commandHistory.prepend(command);
        historyIndex = -1;

        this->controller->sendATCommand(command, this, SLOT(addCommandResponse(const ATCommand &)));
        commandInput->clear();
        currentCommand = addCommand(command);
    });

    connect(commandInput, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

    connect(commandList->verticalScrollBar(), &QScrollBar::rangeChanged, [this](int, int max) {
        commandList->verticalScrollBar()->setValue(max);
    });

    connect(commandList, &QListWidget::customContextMenuRequested, this, &ATConsoleScreen::showContextMenu);
    commandList->setMouseTracking(true); // Enable tracking of the mouse over the widget

    connect(commandList, &QWidget::customContextMenuRequested, this, &ATConsoleScreen::showContextMenu);
}

void ATConsoleScreen::clearConsole() const {
    commandList->clear();
}

void ATConsoleScreen::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        if (!commandHistory.isEmpty() && historyIndex < commandHistory.size() - 1) {
            historyIndex++;
            commandInput->setText(commandHistory.at(historyIndex));
        }
    } else if (event->key() == Qt::Key_Down) {
        if (historyIndex > 0) {
            historyIndex--;
            commandInput->setText(commandHistory.at(historyIndex));
        } else if (historyIndex == 0) {
            historyIndex--;
            commandInput->clear();
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void ATConsoleScreen::showEvent(QShowEvent *event) {
    controller->setATConsoleMode(true);
    commandInput->setFocus();
}

void ATConsoleScreen::hideEvent(QHideEvent *event) {
    controller->setATConsoleMode(false);
}

QListWidgetItem *ATConsoleScreen::addCommand(const QString &command, bool isURC, const QString &response) {
    QListWidgetItem *item = new QListWidgetItem(commandList);

    CommandEntryWidget *widget = new CommandEntryWidget(command, isURC, response);

    item->setSizeHint(widget->sizeHint());

    commandInput->setDisabled(true);
    commandList->setItemWidget(item, widget);

    return item;
}

void ATConsoleScreen::addCommandResponse(const ATCommand &response) {
    CommandEntryWidget *widget = qobject_cast<CommandEntryWidget *>(commandList->itemWidget(currentCommand));

    widget->setResponse(response.response);
    widget->adjustSize();
    currentCommand->setSizeHint(widget->sizeHint());

    if (response.result != at_command_res_t::AT_OK) {
        widget->setObjectName("consoleError");
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }

    commandInput->setDisabled(false);
    commandInput->setFocus();
    update();
    process(response.command);
}

void ATConsoleScreen::addURC(const QString &urc) {
    addCommand("", true, urc);
}

void ATConsoleScreen::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = commandList->itemAt(pos);

    QMenu contextMenu(this);
    QAction *copyCommandAction = nullptr;
    QAction *copyResponseAction;

    if (item) {
        CommandEntryWidget *widget = qobject_cast<CommandEntryWidget *>(commandList->itemWidget(item));

        if (widget->isURC()) {
            copyResponseAction = contextMenu.addAction("Copy URC");
        } else {
            copyCommandAction = contextMenu.addAction("Copy Command");
            copyResponseAction = contextMenu.addAction("Copy Response");
        }

        connect(copyCommandAction, &QAction::triggered, this, [widget]() {
            QApplication::clipboard()->setText(widget->getCommand());
        });

        connect(copyResponseAction, &QAction::triggered, this, [widget]() {
            QApplication::clipboard()->setText(widget->getResponse());
        });

        contextMenu.addSeparator();
    }

    QAction *clearConsoleAction = contextMenu.addAction("Clear Console");

    connect(clearConsoleAction, &QAction::triggered, this, &ATConsoleScreen::clearConsole);

    contextMenu.exec(commandList->mapToGlobal(pos));
}

void ATConsoleScreen::process(const QString &command) {
    if (command.toUpper() == "AT?") {
        emit easter();
    }
}
