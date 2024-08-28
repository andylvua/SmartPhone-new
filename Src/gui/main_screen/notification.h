//
// Created by andrew on 24.08.24.
//

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <QListWidgetItem>

class NotificationWidget : public QWidget {
Q_OBJECT

    std::function<void()> action;
    QListWidgetItem *parentItem;

public:
    explicit NotificationWidget(QListWidgetItem *item, const QIcon &icon, const QString &title,
                                const QString &message, QWidget *parent = nullptr);

    template<typename Callable>
    void setAction(Callable&& slot) {
        this->action = std::forward<Callable>(slot);
    }

    template<typename Object, typename Signal, typename Condition = std::function<bool()>>
    void setRemoveEvent(Object *sender, Signal signal, Condition condition = [](auto&&...) { return true; }) {
        connect(sender, signal, this, [this, condition](auto&&... args) {
            if (condition(std::forward<decltype(args)>(args)...)) {
                this->remove();
            }
        });
    }

    void handleAction();

private:
    void remove();
};

#endif //NOTIFICATION_H
