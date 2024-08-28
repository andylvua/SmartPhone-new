//
// Created by andrew on 24.08.24.
//

#ifndef AT_CONSOLE_WINDOW_H
#define AT_CONSOLE_WINDOW_H

#include <QLineEdit>
#include <QListWidgetItem>

#include "gui/application_screen/application_screen.h"
#include "controller/controller.h"


class ATConsoleScreen : public ApplicationScreen {
Q_OBJECT

    QLineEdit *commandInput;
    QListWidget *commandList;
    Controller *controller;
    QListWidgetItem *currentCommand;

    QStringList commandHistory;
    int historyIndex = -1;

public:

    explicit ATConsoleScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

signals:
    void easter();

protected:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void showContextMenu(const QPoint &pos);

    void addCommandResponse(const ATCommand &response);

    QListWidgetItem *addCommand(const QString &command, bool isURC = false, const QString &response = "");

    void addURC(const QString &urc);

    void clearConsole() const;

    void process(const QString &command);
};

#endif //AT_CONSOLE_WINDOW_H
