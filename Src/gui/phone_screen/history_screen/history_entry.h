//
// Created by andrew on 25.08.24.
//

#ifndef HISTORY_ENTRY_H
#define HISTORY_ENTRY_H


#include <QWidget>

#include "controller/controller.h"
#include "backend/models/models.h"


class HistoryEntryWidget : public QWidget {
Q_OBJECT

    Call entry;
    Controller *controller;

public:
    explicit HistoryEntryWidget(const Call &entry, Controller *controller, QWidget *parent = nullptr);

    Call getEntry() const {
        return entry;
    }
};

#endif //HISTORY_ENTRY_H
