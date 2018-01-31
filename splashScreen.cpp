#include <functional>
#include <QDebug>

#include "splashScreen.h"
#include "ui_splashScreen.h"
#include "stateMachine.h"

#include "logger.h"

using namespace std::placeholders;

SplashScreen::SplashScreen(QWidget *parent) :
    Page(parent),
    _ui(new Ui::SplashScreen)
{
    _ui->setupUi(this);
}

SplashScreen::~SplashScreen()
{
    delete _ui;
}

void SplashScreen::init(MainWindow *mainWindow)
{
    Page::init(mainWindow);

    QObject::connect(_ui->frameButton, &QPushButton::released, [=] {
        _mainWindow->goToState(START);
    });
}

QString SplashScreen::getName() const
{
    return "splashScreen";
}

QList<Transition*> SplashScreen::getTransitions()
{
    Transition* transition = new Transition(SPLASH_SCREEN, START, [=](QEvent*){
        _mainWindow->setPage(START_PAGE);
    });

    return QList<Transition*>({transition});
}

void SplashScreen::onEntry()
{
    connectToDeviceChecker();
    _mainWindow->getDevicesChecker().startDevices();

    _mainWindow->getSessionData().reset();
}

void SplashScreen::onDevicesStarted()
{
    // TODO: logging
    _mainWindow->getDevicesChecker().checkDevicesStatus();
}

void SplashScreen::onSomeDevicesNotStarted()
{
    // TODO: logging
    disconnectFromDeviceChecker();
    _mainWindow->goToState(CRITICAL_ERROR);
}

void SplashScreen::onDevicesStatusOk()
{
    // TODO: logging
    disconnectFromDeviceChecker();
}

void SplashScreen::onSomeDevicesStatusNotOk()
{
    // TODO: logging
    disconnectFromDeviceChecker();
    _mainWindow->goToState(CRITICAL_ERROR);
}

void SplashScreen::connectToDeviceChecker()
{
    DevicesChecker* devicesChecker = &_mainWindow->getDevicesChecker();

    QObject::connect(devicesChecker, &DevicesChecker::devicesStarted, this, &SplashScreen::onDevicesStarted);
    QObject::connect(devicesChecker, &DevicesChecker::someDevicesNotStarted, this, &SplashScreen::onSomeDevicesNotStarted);
    QObject::connect(devicesChecker, &DevicesChecker::devicesStatusOk, this, &SplashScreen::onDevicesStatusOk);
    QObject::connect(devicesChecker, &DevicesChecker::someDevicesStatusNotOk, this, &SplashScreen::onSomeDevicesStatusNotOk);
}

void SplashScreen::disconnectFromDeviceChecker()
{
    DevicesChecker* devicesChecker = &_mainWindow->getDevicesChecker();

    QObject::disconnect(devicesChecker, &DevicesChecker::devicesStarted, 0, 0);
    QObject::disconnect(devicesChecker, &DevicesChecker::someDevicesNotStarted, 0, 0);
    QObject::disconnect(devicesChecker, &DevicesChecker::devicesStatusOk, 0, 0);
    QObject::disconnect(devicesChecker, &DevicesChecker::someDevicesStatusNotOk, 0, 0);
}
