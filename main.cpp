#include <QApplication>

#include "backend/serial_port.h"
#include "backend/modem/modem.h"
#include "controller/controller.h"
#include "backend/utils/cache_manager.h"
#include "backend/utils/logging.h"
#include "gui/utils/fa.h"

#include "gui/gui.h"

fa::QtAwesome* FA::awesome = nullptr;


//#define DEBUG

int main(int argc, char *argv[]) {
//    qInstallMessageHandler(logOutputHandler);
    QApplication app(argc, argv);

    QFile file("../Src/style/stylesheet.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(styleSheet);

    std::setlocale(LC_ALL, "en_US.UTF-8");
    QLocale locale = QLocale(QLocale::English, QLocale::UnitedStates);
    QLocale::setDefault(locale);

    auto mainLogger = spdlog::basic_logger_mt("main",
                                              LOGS_FILEPATH,
                                              true);

    // spdlog::flush_on(spdlog::level::debug);
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [function %!] [line %#] %v");

    #ifdef __APPLE__
        const char* portName = "/dev/tty.usbserial-1410";
    #elif __linux__
        const char* portName = "/dev/ttyUSB0";
    #endif

    SerialPort *serial = new SerialPort(portName,
                      500,
                      115200,
                      QSerialPort::Data8,
                      QSerialPort::NoParity,
                      QSerialPort::OneStop,
                      QSerialPort::NoFlowControl);

    #ifndef DEBUG
    if (!serial->openSerialPort()) {
        qDebug() << "Serial port error: " << serial->errorString();
        SPDLOG_LOGGER_ERROR(mainLogger, "Serial port was not opened: \"{}\"", serial->errorString().toStdString());
        return 1;
    }
    SPDLOG_LOGGER_INFO(mainLogger, "Serial port was opened successfully. Starting modem");
    #endif


    Modem *modem = new Modem(serial);
    Controller *controller = new Controller(modem);
    MainWindow mainWindow(controller);

    mainWindow.show();
    modem->init();

    return QApplication::exec();
}
