//
// Created by andrew on 24.08.24.
//

#ifndef RESTART_OVERLAY_H
#define RESTART_OVERLAY_H


#include <QLabel>

#include "gui/utils/waitingspinnerwidget.h"

class OverlayWidget : public QWidget {
public:
    explicit OverlayWidget(QWidget *parent = {}) : QWidget{parent} {
        setAttribute(Qt::WA_NoSystemBackground);

        if (!this->parent()) {
            return;
        }
        this->parent()->installEventFilter(this);
        resize(parentWidget()->size());
        raise();
    }
};

class RestartOverlay : public OverlayWidget {

    QLabel *loadingLabel;
    WaitingSpinnerWidget *spinner;

public:
    RestartOverlay(QWidget *parent = nullptr);
};

#endif //RESTART_OVERLAY_H
