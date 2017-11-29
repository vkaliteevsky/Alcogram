#pragma once

#include <QMainWindow>
#include <QMap>

#include "interfaces/mainwindowinterface.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
                , public MainWindowInterface
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setPage(PageName pageName) override;

    ConfigManager* getConfigManager() const override;
    DeviceManager* getDeviceManager() const override;

    FaceDetectionInterface* getFaceDetector() const override;
    void goToState(StateName targetState) override;
    StateName getCurrentStateName() const override
    {
        return _stateMachine->getCurrentStateName();
    }

    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // load fonts from resources
    void loadFonts();

    void removeFonts();

    Ui::MainWindow*_ui;
    QVector<int> _fonts;
    ConfigManager* _configManager;
    StateMachine* _stateMachine;
    DeviceManager* _deviceManager;
    FaceDetectionInterface* _faceDetector;
};
