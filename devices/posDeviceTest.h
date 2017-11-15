#pragma once

#include <QObject>
#include <QTimer>

#include "deviceInterfaces/posinterface.h"

// test status codes
const int OK = 0;
const int ERROR = 1;

class POSDeviceTest : public QObject
                    , public POSInterface
{
    Q_OBJECT
public:
    explicit POSDeviceTest(QObject *parent = 0);

public:
    void turnOn(DeviceCallback callback) override;
    void turnOff(DeviceCallback callback) override;
    void sendPrice(int price, DeviceCallback callback) override;
    void getPaymentResponce(PayCallback payCallback) override;
    void abortPayment(DeviceCallback callback) override;

private:
    int _price;
    int _enteredAmount;
    QTimer _timer;

    const int _denominations[2] = {50, 100};
};