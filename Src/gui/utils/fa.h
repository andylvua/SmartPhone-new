//
// Created by andrew on 24.08.24.
//

#ifndef FA_HPP
#define FA_HPP

#include <QApplication>
#include <QtAwesome.h>

class FA {
public:
    static fa::QtAwesome *getInstance() {
        if (!awesome) {
            awesome = new fa::QtAwesome(qApp);
            awesome->initFontAwesome();
        }
        return awesome;
    }

private:
    static fa::QtAwesome *awesome;
};

#endif //FA_HPP
