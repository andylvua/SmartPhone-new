//
// Created by andrew on 24.08.24.
//

#include <QHBoxLayout>
#include <QMenu>

#include "backend/utils/cache_manager.h"
#include "gui/phone_screen/history_screen/history_screen.h"
#include "gui/utils/fa.h"
#include "gui/utils/gui_utils.h"
#include "gui/phone_screen/history_screen/history_entry.h"


HistoryScreen::HistoryScreen(QWidget *parent, Controller *controller)
        : QWidget(parent), controller(controller) {
    historyList = new QListWidget(this);
    historyList->setFrameStyle(QFrame::NoFrame);
    historyList->setContextMenuPolicy(Qt::CustomContextMenu);
    historyList->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(historyList);
    layout->setContentsMargins(0, 0, 0, 10);

    connect(historyList, &QListWidget::customContextMenuRequested, this, &HistoryScreen::showContextMenu);
}

void HistoryScreen::populateHistory() {
    historyList->clear();
    QVector<Call> callHistory = CacheManager::getCalls();
    for (int i = callHistory.size() - 1; i >= 0; i--) {
        Call &entry = callHistory[i];

        QListWidgetItem *item = new QListWidgetItem(historyList);

        HistoryEntryWidget *entryWidget = new HistoryEntryWidget(entry, controller, this);
        item->setSizeHint(entryWidget->sizeHint());
        historyList->setItemWidget(item, entryWidget);
    }
}

void HistoryScreen::showEvent(QShowEvent *event) {
    emit screenShown();
    populateHistory();
    QWidget::showEvent(event);
}


void HistoryScreen::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = historyList->itemAt(pos);
    if (!item) {
        return;
    }

    HistoryEntryWidget *entryWidget = qobject_cast<HistoryEntryWidget *>(historyList->itemWidget(item));
    if (!entryWidget) {
        return;
    }

    Call entry = entryWidget->getEntry();

    QMenu contextMenu;
    QAction *deleteAction = contextMenu.addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, [entry, item]() {
        CacheManager::removeCall(entry);
        delete item;
    });

    contextMenu.exec(historyList->mapToGlobal(pos));
}
