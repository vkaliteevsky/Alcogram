#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QLinearGradient>

#include "alcoTestPageWidget.h"
#include "ui_alcoTestPageWidget.h"
#include "utils.h"

AlcoTestPageWidget::AlcoTestPageWidget(QWidget *parent) :
    Page(parent),
    _ui(new Ui::AlcoTestPageWidget)
{
    _ui->setupUi(this);
}

AlcoTestPageWidget::~AlcoTestPageWidget()
{
    delete _ui;
}

void AlcoTestPageWidget::init(MainWindow *mainWindow)
{
    Page::init(mainWindow);

    _alcotester = _mainWindow->getDeviceManager()->getAlcotesterDevice();
    _camera = _mainWindow->getDeviceManager()->getCameraDevice();
    _faceDetector = _mainWindow->getFaceDetector();
}

QString AlcoTestPageWidget::getName() const
{
    return "test";
}

QList<Transition*> AlcoTestPageWidget::getTransitions()
{
    QList<Transition*> transitions;

    // ALCOTEST -> ALCOTEST
    transitions.append(new Transition(ALCOTEST, ALCOTEST, [this](QEvent*) {
        circleCurrentPerson();
        setTimer("recognized");
    }));

    // ALCOTEST -> ALCOTEST_INACTION
    transitions.append(new Transition(ALCOTEST, ALCOTEST_INACTION, [this](QEvent*) {
        // TODO: to inaction warning
    }));

    // ALCOTEST -> DRUNKENESS_NOT_RECOGNIZED
    transitions.append(new Transition(ALCOTEST, DRUNKENESS_NOT_RECOGNIZED, [this](QEvent*) {
        circleCurrentPerson();
        setTimer("drunkenessNotRecognized");
    }));

    // ALCOTEST -> FINAL_PHOTO
    transitions.append(new Transition(ALCOTEST, FINAL_PHOTO, [this](QEvent*) {
        _mainWindow->setPage(PHOTO_PRINT_PAGE);
    }));

    // ALCOTEST_INACTION -> SPLASH_SCREEN
    transitions.append(new Transition(ALCOTEST_INACTION, SPLASH_SCREEN, [this](QEvent*) {
        _mainWindow->setPage(SPLASH_SCREEN_PAGE);
    }));

    // ALCOTEST_INACTION -> ALCOTEST
    transitions.append(new Transition(ALCOTEST_INACTION, ALCOTEST, [this](QEvent*) {
        // TODO: show regular message
    }));

    // DRUNKENESS_NOT_RECOGNIZED -> ALCOTEST
    transitions.append(new Transition(DRUNKENESS_NOT_RECOGNIZED, ALCOTEST, [this](QEvent*) {
        test(_currentPerson);
    }));

    return transitions;
}

void AlcoTestPageWidget::onEntry()
{
    test(0);
}

void AlcoTestPageWidget::initInterface()
{
    updateTexts(_ui->frame);
}

void AlcoTestPageWidget::setTimer(const QString& durationName)
{
    _timer.stop();

    int timeMs = _mainWindow->getConfigManager()->getTimeDuration(getName(), durationName) * 1000;

    _timer.setInterval(timeMs);

    QObject::disconnect(&_timer, &QTimer::timeout, 0, 0);
    QObject::connect(&_timer, &QTimer::timeout, [this]{
        _timer.stop();

        switch (_mainWindow->getCurrentStateName()) {
            case ALCOTEST:
                switch (_circleState) {
                    case TEST:
                        _mainWindow->goToState(ALCOTEST_INACTION);
                        break;
                    case SUCCESS:
                        test(_currentPerson + 1);
                        break;
                }
                break;
            case ALCOTEST_INACTION:
                _mainWindow->goToState(SPLASH_SCREEN);
                break;
            case DRUNKENESS_NOT_RECOGNIZED:
                _mainWindow->goToState(ALCOTEST);
                break;
            default:
                // TODO: handle
                break;
        }
    });

    _timer.start();
}

void AlcoTestPageWidget::test(int i)
{
    if (i == _faceDetector->facesNumber()) {
        _mainWindow->goToState(FINAL_PHOTO);
        return;
    }

    _circleState = TEST;
    _currentPerson = i;

    circleCurrentPerson();

    _alcotester->test([=](int statusCode, double value) {
        switch (statusCode) {
            case OK:
                _circleState = SUCCESS;
                _lastPersonValue = value;
                _mainWindow->goToState(ALCOTEST);
                break;
            case NOT_RECOGNIZED:
                _circleState = FAIL;
                _mainWindow->goToState(DRUNKENESS_NOT_RECOGNIZED);
                break;
            case ERROR:
                break;
        }
    });

    setTimer("inactionAlcotest");
}

void AlcoTestPageWidget::circleCurrentPerson()
{
    QPixmap image = QPixmap::fromImage(_camera->getCapturedImage());
    QRect faceRect = _faceDetector->faceRects().at(_currentPerson);
    int radius = std::max(faceRect.width(), faceRect.height()) / 2;

    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);

    QPen pen;

    switch (_circleState) {
        case TEST:
        case FAIL:
        {
            QLinearGradient gradient;
            gradient.setColorAt(0, QColor("#ffdd33"));
            gradient.setColorAt(1, QColor("#ffb932"));

            pen = QPen(QBrush(gradient), radius * 0.2);
            break;
        }

        case SUCCESS:
        {
            pen = QPen(QColor("#71b732"));
            pen.setWidth(radius * 0.2);
            break;
        }
    }


    p.setPen(pen);

    QPoint center = faceRect.center();

    p.drawEllipse(center, radius, radius);

    QPoint upperCircleCenter(center.x() + radius * 1.2, center.y() - radius * 1.2);
    int upperCircleRadius = radius * 0.23;

    switch (_circleState) {
        case TEST:
        {
            pen.setWidth(5);
            p.setPen(pen);
            p.drawEllipse(upperCircleCenter, upperCircleRadius, upperCircleRadius);
            break;
        }

        case SUCCESS:
        {
            p.setBrush(QBrush(QColor("#71b732")));
            p.drawEllipse(upperCircleCenter, upperCircleRadius, upperCircleRadius);
            break;
        }

        case FAIL:
        {
            pen.setWidth(5);
            p.setPen(pen);
            p.drawEllipse(upperCircleCenter, upperCircleRadius, upperCircleRadius);
        }
    }

    int w = _ui->photo->width();
    int h = _ui->photo->height();

    _ui->photo->setPixmap(image.scaled(w, h, Qt::KeepAspectRatioByExpanding));
}
