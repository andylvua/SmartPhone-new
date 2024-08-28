//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>

#include "gui/misc/restart_overlay.h"


RestartOverlay::RestartOverlay(QWidget *parent) : OverlayWidget{parent} {
    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *loadingLayout = new QHBoxLayout();
    loadingLayout->setAlignment(Qt::AlignCenter);
    loadingLabel = new QLabel("Restarting...", this);
    loadingLabel->setObjectName("loadingLabel");
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
    spinner->setColor(Qt::white);

    loadingLayout->addWidget(spinner, 0, Qt::AlignCenter);

    layout->addLayout(loadingLayout);
    spinner->start();
}
