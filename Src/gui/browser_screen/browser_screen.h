//
// Created by andrew on 25.08.24.
//

#ifndef BROWSER_SCREEN_H
#define BROWSER_SCREEN_H

#include <QWebEngineView>
#include <QLineEdit>

#include "gui/application_screen/application_screen.h"

class BrowserScreen : public ApplicationScreen {
Q_OBJECT

    QWebEngineView *browser;
    QLineEdit *addressBar;

public:
    explicit BrowserScreen(QWidget *parent = nullptr);

public slots:
    void openEasterEgg();

private slots:

    void loadPageFromAddressBar();

    void updateAddressBar(const QUrl &url);

};


#endif //BROWSER_SCREEN_H
