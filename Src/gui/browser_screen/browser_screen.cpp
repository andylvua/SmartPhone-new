//
// Created by andrew on 25.08.24.
//

#include <QHBoxLayout>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QTimer>
#include <QMetaObject>


#include "gui/browser_screen/browser_screen.h"
#include "gui/utils/fa.h"


BrowserScreen::BrowserScreen(QWidget *parent) : ApplicationScreen("Browser", parent, true) {
    setAttribute(Qt::WA_StyledBackground);

    this->setObjectName("BrowserScreen");

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());

    // Initialize the browser
    browser = new QWebEngineView(this);
    browser->page()->profile()->setHttpUserAgent(
            "Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Mobile Safari/537.36");

    browser->page()->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
    browser->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    browser->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    browser->page()->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    browser->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);

    browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    browser->load(QUrl("https://google.com"));

    QHBoxLayout *bottomLayout = new QHBoxLayout();

    QPushButton *backButton = new QPushButton("", this);
    backButton->setFixedSize(40, 40);
    QIcon backIcon = FA::getInstance()->icon(fa::fa_solid, fa::fa_chevron_left);
    backButton->setIcon(backIcon);
    connect(backButton, &QPushButton::clicked, browser, &QWebEngineView::back);
    bottomLayout->addWidget(backButton);

    QPushButton *forwardButton = new QPushButton("", this);
    forwardButton->setFixedSize(40, 40);
    QIcon forwardIcon = FA::getInstance()->icon(fa::fa_solid, fa::fa_chevron_right);
    forwardButton->setIcon(forwardIcon);
    connect(forwardButton, &QPushButton::clicked, browser, &QWebEngineView::forward);
    bottomLayout->addWidget(forwardButton);

    addressBar = new QLineEdit(this);
    addressBar->setFixedHeight(40);
    addressBar->setPlaceholderText("Enter URL...");
    addressBar->setObjectName("browserAddressBar");
    connect(addressBar, &QLineEdit::returnPressed, this, &BrowserScreen::loadPageFromAddressBar);
    bottomLayout->addWidget(addressBar);

    QPushButton *enterButton = new QPushButton("", this);
    enterButton->setFixedSize(40, 40);
    QIcon enterIcon = FA::getInstance()->icon(fa::fa_solid, fa::fa_magnifying_glass);
    enterButton->setIcon(enterIcon);
    connect(enterButton, &QPushButton::clicked, this, &BrowserScreen::loadPageFromAddressBar);
    bottomLayout->addWidget(enterButton);

    layout->addLayout(bottomLayout);
    layout->addWidget(browser);

    connect(browser, &QWebEngineView::urlChanged, this, &BrowserScreen::updateAddressBar);
}

void BrowserScreen::loadPageFromAddressBar() {
    QString input = addressBar->text();
    QRegExp urlRegex("^(http://|https://)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/[^\\s]*)?$");
    if (urlRegex.exactMatch(input)) {
        QUrl url = QUrl::fromUserInput(input);
        browser->load(url);
    } else {
        QUrl url = QUrl("https://www.google.com/search?q=" + input);
        browser->load(url);
    }
}

void BrowserScreen::updateAddressBar(const QUrl &url) {
    addressBar->setText(url.toString());
}

void BrowserScreen::openEasterEgg() {
    browser->load(QUrl("https://www.youtube.com/watch?v=dQw4w9WgXcQ"));

    auto *connection = new QMetaObject::Connection;
    *connection = connect(browser, &QWebEngineView::loadFinished, [this, connection](){
        browser->page()->runJavaScript(R"(
            let video = document.querySelector('video');
            video.muted = false;
            video.play();
        )");
        disconnect(*connection);
        delete connection;
    });
}
