//
// Created by andrew on 24.08.24.
//

#ifndef APPLICATION_SCREEN_H
#define APPLICATION_SCREEN_H

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

class ApplicationScreen : public QWidget {
Q_OBJECT

    QGridLayout *headerLayout;
    QVBoxLayout *contentLayout;
    QPushButton *backButton;
    QPushButton *actionButton;
    QLabel *screenTitle;

public:
    explicit ApplicationScreen(const QString &title, QWidget *parent = nullptr, bool titleOnTop = false);

    void addActionButton(QPushButton *button);

    void showActionButton() const;

    void hideActionButton() const;

    void setScreenTitle(const QString &title) const;

    QVBoxLayout *getContentLayout() const { return contentLayout; }
};

#endif //APPLICATION_SCREEN_H
