//
// Created by andrew on 24.08.24.
//

#ifndef HISTORY_SCREEN_H
#define HISTORY_SCREEN_H

#include <QWidget>
#include <QListWidget>

#include "controller/controller.h"
#include "backend/models/models.h"

class HistoryScreen : public QWidget {
Q_OBJECT

    QListWidget *historyList;
    Controller *controller;

public:
    explicit HistoryScreen(QWidget *parent = nullptr, Controller *controller = nullptr);

    void populateHistory();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void screenShown();

private slots:

    void showContextMenu(const QPoint &pos);
};


#endif //HISTORY_SCREEN_H
