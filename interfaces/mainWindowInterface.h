#pragma once

#include "configManager.h"
#include "stateMachine.h"
#include "devices/deviceManager.h"
#include "interfaces/faceDetectionInterface.h"

enum PageName {
    NONCRITICAL_ERROR_PAGE = 0
    , CRITICAL_ERROR_PAGE = 1
    , SPLASH_SCREEN_NONCRITICAL_ERROR_PAGE = 2
    , SPLASH_SCREEN_ETERNAL_SLEEP_PAGE = 3
    , SPLASH_SCREEN_PAGE = 4
    , START_PAGE = 5
    , TAKE_PHOTO_PAGE = 6
    , PAY_PAGE = 7
    , ALCOTEST_PAGE = 8
    , PHOTO_PRINT_PAGE = 9
};

class MainWindowInterface {
public:
    virtual void setPage(PageName pageName) = 0;

    virtual ConfigManager* getConfigManager() const = 0;
    virtual DeviceManager* getDeviceManager() const = 0;

    virtual FaceDetectionInterface* getFaceDetector() const = 0;
    virtual void goToState(StateName targetState) = 0;
    virtual StateName getCurrentStateName() const = 0;
};