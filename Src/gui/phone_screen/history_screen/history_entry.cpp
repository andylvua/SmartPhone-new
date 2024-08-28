//
// Created by andrew on 25.08.24.
//

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "gui/phone_screen/history_screen/history_entry.h"
#include "gui/utils/fa.h"
#include "gui/utils/gui_utils.h"


namespace {
    QIcon getDirectionIcon(callDirection direction, callResult result) {
        QIcon directionIcon;
        if (direction == callDirection::CD_INCOMING) {
            if (result == callResult::CR_ANSWERED) {
                directionIcon = FA::getInstance()->icon(fa::fa_thin, fa::fa_phone_arrow_down_left, {{"scale-factor", 0.7}});
            } else {
                directionIcon = FA::getInstance()->icon(fa::fa_regular, fa::fa_phone_missed, {{"color", "#a82414"}});
            }
        } else {
            directionIcon = FA::getInstance()->icon(fa::fa_thin, fa::fa_phone_arrow_up_right, {{"scale-factor", 0.7}});
        }

        return directionIcon;
    }
}

HistoryEntryWidget::HistoryEntryWidget(const Call &entry, Controller *controller, QWidget *parent)
    : QWidget(parent), entry(entry), controller(controller) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *itemLayout = new QHBoxLayout();

    QLabel *historyLabel = new QLabel(entry.contact.name.isEmpty() ? entry.contact.number : entry.contact.name, this);
    historyLabel->setObjectName("historyLabel");

    QLabel *directionLabel = new QLabel(this);
    QIcon directionIcon = getDirectionIcon(entry.callDirection, entry.callResult);
    directionLabel->setPixmap(directionIcon.pixmap(20, 20));

    QLabel *timeLabel = new QLabel(Utils::getTimeString(entry.startTime), this);
    timeLabel->setObjectName("historyTimeLabel");

    QPushButton *callButton = new QPushButton(FA::getInstance()->icon(fa::fa_solid, fa::fa_phone, {{"color", "#008dff"}}), "", this);
    callButton->setFixedSize(40, 40);
    callButton->setIconSize(QSize(20, 20));
    callButton->setObjectName("historyCallButton");

    itemLayout->addWidget(directionLabel);
    itemLayout->addSpacing(5);
    itemLayout->addWidget(historyLabel);
    itemLayout->addStretch();
    itemLayout->addWidget(timeLabel);
    itemLayout->addSpacing(5);
    itemLayout->addWidget(callButton);
    itemLayout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *separatorLayout = new QHBoxLayout();
    separatorLayout->setContentsMargins(35, 0, 50, 0);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setObjectName("historySeparator");
    separator->setFixedHeight(1);

    separatorLayout->addWidget(separator);

    mainLayout->addLayout(itemLayout);
    mainLayout->addLayout(separatorLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    itemLayout->setSpacing(9);

    connect(callButton, &QPushButton::clicked, this, [this, entry]() {
        this->controller->makeCall(entry.contact.number);
    });
}
