//
// Created by andrew on 26.08.24.
//

#ifndef COMMAND_ENTRY_H
#define COMMAND_ENTRY_H

#include <QLabel>
#include <QString>


class CommandEntryWidget : public QWidget {
    Q_OBJECT

    QLabel *commandLabel;
    QLabel *responseLabel;
    bool urc;

public:

    CommandEntryWidget(const QString& command, bool urc, const QString& response);

    void setResponse(const QString &response) {
        responseLabel->setText(response);
    }

    [[nodiscard]] bool isURC() const {
        return urc;
    }

    QString getCommand() const {
        return commandLabel->text();
    }

    QString getResponse() const {
        return responseLabel->text();
    }
};
#endif //COMMAND_ENTRY_H
