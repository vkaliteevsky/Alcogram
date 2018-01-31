#include "criticalErrorPage.h"
#include "ui_criticalErrorPage.h"

CriticalErrorPage::CriticalErrorPage(QWidget *parent) :
    Page(parent),
    ui(new Ui::CriticalErrorPage)
{
    ui->setupUi(this);
}

CriticalErrorPage::~CriticalErrorPage()
{
    delete ui;
}

void CriticalErrorPage::init(MainWindow *mainWindow)
{
    Page::init(mainWindow);

    QObject::connect(_mainWindow, &MainWindow::criticalError, this, &CriticalErrorPage::onCriticalError);
}

QString CriticalErrorPage::getName() const
{
    return "criticalError";
}

void CriticalErrorPage::onCriticalError(StateName fromState)
{
    _fromState = fromState;

    stopTimer();

    // TODO: move to xml
    int timeMs = 180000;

    _timer.setInterval(timeMs);

    QObject::disconnect(&_timer, &QTimer::timeout, 0, 0);
    QObject::connect(&_timer, &QTimer::timeout, [=]{
        stopTimer();
        _checkTimer.stop();
        disconnectFromDeviceChecker();
        _mainWindow->goToState(SPLASH_SCREEN_ETERNAL_SLEEP);
    });

    startTimer("criticalPageTimer", timeMs / 1000);

    _mainWindow->setPage(CRITICAL_ERROR_PAGE);
}

void CriticalErrorPage::setCheckTimer()
{
    // TODO: move to xml
    _checkTimer.setInterval(5000);
    _checkTimer.setSingleShot(true);

    QObject::disconnect(&_checkTimer, &QTimer::timeout, 0, 0);
    QObject::connect(&_checkTimer, &QTimer::timeout, [=]{
        if (!_mainWindow->getDevicesChecker().getDisabledDevices().isEmpty()) {
            _mainWindow->getDevicesChecker().checkDisabledDevicesStatus();
        } else {
            onDevicesStatusOk();
        }
    });
}

void CriticalErrorPage::onDevicesStatusOk()
{
    disconnectFromDeviceChecker();
    _mainWindow->goToState(_fromState);
}

void CriticalErrorPage::onSomeDevicesStatusNotOk()
{
    setCheckTimer();
}

void CriticalErrorPage::connectToDeviceChecker()
{
    DevicesChecker* devicesChecker = &_mainWindow->getDevicesChecker();

    QObject::connect(devicesChecker, &DevicesChecker::devicesStatusOk, this, &CriticalErrorPage::onDevicesStatusOk);
    QObject::connect(devicesChecker, &DevicesChecker::someDevicesStatusNotOk
                     , this, &CriticalErrorPage::onSomeDevicesStatusNotOk);
}

void CriticalErrorPage::disconnectFromDeviceChecker()
{
    DevicesChecker* devicesChecker = &_mainWindow->getDevicesChecker();

    QObject::disconnect(devicesChecker, &DevicesChecker::devicesStatusOk, 0, 0);
    QObject::disconnect(devicesChecker, &DevicesChecker::someDevicesStatusNotOk, 0, 0);
}
