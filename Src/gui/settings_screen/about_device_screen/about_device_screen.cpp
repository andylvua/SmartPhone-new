//
// Created by andrew on 24.08.24.
//

#include "gui/settings_screen/about_device_screen/about_device_screen.h"


AboutDeviceScreen::AboutDeviceScreen(QWidget *parent, Controller *controller) : ApplicationScreen(
        "About Device", parent), controller(controller) {
    QVBoxLayout *layout = getContentLayout();

    QWidget *container = new QWidget(this);
    container->setObjectName("AboutDeviceContainer");

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(10, 5, 10, 5);
    containerLayout->setSpacing(0);

    manufacturerEntry = new InfoEntry("Manufacturer", "", container);
    modelEntry = new InfoEntry("Model", "", container);
    revisionEntry = new InfoEntry("Revision", "", container);
    numberEntry = new InfoEntry("Number", "", container);
    operatorEntry = new InfoEntry("Operator", "", container);
    registrationEntry = new InfoEntry("Registration", "", container);
    registrationEntry->removeSeparator();

    containerLayout->addWidget(manufacturerEntry);
    containerLayout->addWidget(modelEntry);
    containerLayout->addWidget(revisionEntry);
    containerLayout->addWidget(numberEntry);
    containerLayout->addWidget(operatorEntry);
    containerLayout->addWidget(registrationEntry);

    layout->addWidget(container);
}

void AboutDeviceScreen::updateInfo() {
    auto getValue = [](const QString &value) -> QString {
        return value.isEmpty() ? "Unknown" : value;
    };

    modemInfo = controller->getModemInfo();

    manufacturerEntry->setValue(getValue(modemInfo.manufacturer));
    modelEntry->setValue(getValue(modemInfo.model));
    revisionEntry->setValue(getValue(modemInfo.revision));
    numberEntry->setValue(getValue(modemInfo.number));
    operatorEntry->setValue(getValue(modemInfo.operatorName));
    registrationEntry->setValue(modemInfo.registrationStatus ? "Registered" : "Not Registered");
}
