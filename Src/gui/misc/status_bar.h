//
// Created by andrew on 24.08.24.
//

#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QToolBar>
#include <QLabel>

#include "controller/controller.h"


class StatusBar : public QToolBar {
Q_OBJECT

    QLabel *timeLabel;
    QLabel *operatorLabel;
    QLabel *mobileDataLabel;
    QLabel *connectionLabel;
    QLabel *initStatus;
    Controller *controller;

public:
    explicit StatusBar(QWidget *parent = nullptr, Controller *controller = nullptr);

private slots:

    void updateTime() const;

    void updateOperator(const QString &operatorName) const;

    void updateSignalStrengthRequest() const;

    void onSignalStrengthUpdated(int strength) const;
};

#endif //STATUS_BAR_H
