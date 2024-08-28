//
// Created by andrew on 24.08.24.
//

#ifndef GUI_UTILS_HPP
#define GUI_UTILS_HPP

#include <QObject>
#include <QEvent>
#include <QDateTime>
#include <QLocale>

class BackButtonHandler : public QObject {
    Q_OBJECT
    public:
    static BackButtonHandler *instance() {
        static BackButtonHandler handler;
        return &handler;
    }

    signals:

        void backRequested();

    public slots:

        void handleBack() {
        emit backRequested();
    }
};

class HoverEventFilter : public QObject {
    Q_OBJECT

    protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        Q_UNUSED(obj)
        if (event->type() == QEvent::HoverEnter) {
            return true;
        }

        return false;
    }
};

enum class DateFormat {
    DATE_ONLY,
    TIME_ONLY,
};

namespace Utils {
    inline QString getTimeString(const QDateTime& time, DateFormat format = DateFormat::TIME_ONLY) {
        QString timeString;
        if (const QDateTime currentTime = QDateTime::currentDateTime(); time.daysTo(currentTime) == 0) {
            timeString = format == DateFormat::TIME_ONLY ? QLocale().toString(time, "hh:mm") : "Today";
        } else if (time.daysTo(currentTime) == 1) {
            timeString = "Yesterday";
        } else if (time.daysTo(currentTime) < 7) {
            timeString = QLocale().toString(time, "dddd");
        } else {
            timeString = QLocale().toString(time, "dd.MM.yyyy");
        }
        return timeString;
    }
}

#endif //GUI_UTILS_HPP
