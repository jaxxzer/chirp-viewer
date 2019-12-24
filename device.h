#ifndef DEVICE_H
#define DEVICE_H

#include <com-handle.h>
#include <com-parser.h>
#include <QSerialPort>
#include <QObject>
#include <QTimer>
#include <QMap>

#include <register-model.h>

#include <inttypes.h>

#define ADC_NUM_CHANNELS 8

class Device : public QObject
{
    Q_OBJECT
public:
    Device(QSerialPortInfo info);
    ComParser parser;
    ComHandle* handle;
    bool open();

    typedef struct {
      uint16_t adc_buffer[ADC_NUM_CHANNELS];
      uint16_t phaseA;
      uint16_t phaseB;
      uint16_t phaseC;
      uint16_t phaseNeutral;
      uint16_t throttle;
      uint8_t direction;
      uint8_t direction_mode;
      uint16_t startup_throttle;
      uint8_t commutationStep;
      uint8_t bridgeStep;
    } global_t;

    global_t deviceGlobal;

    QVariant getAdc0() { return deviceGlobal.adc_buffer[0]; }
    QVariant getAdc1() { return deviceGlobal.adc_buffer[1]; }
    QVariant getAdc2() { return deviceGlobal.adc_buffer[2]; }
    QVariant getAdc3() { return deviceGlobal.adc_buffer[3]; }
    QVariant getAdc4() { return deviceGlobal.adc_buffer[4]; }
    QVariant getAdc5() { return deviceGlobal.adc_buffer[5]; }
    QVariant getAdc6() { return deviceGlobal.adc_buffer[6]; }
    QVariant getAdc7() { return deviceGlobal.adc_buffer[7]; }
    QVariant getPhaseA() { return deviceGlobal.phaseA; }
    QVariant getPhaseB() { return deviceGlobal.phaseB; }
    QVariant getPhaseC() { return deviceGlobal.phaseC; }
    QVariant getPhaseNeutral() { return deviceGlobal.phaseNeutral; }
    QVariant getThrottle() { return deviceGlobal.throttle; }
    QVariant getDirection() { return deviceGlobal.direction; }
    QVariant getDirectionMode() { return deviceGlobal.direction_mode; }
    QVariant getStartupThrottle() { return deviceGlobal.startup_throttle; }
    QVariant getCommutationStep() { return deviceGlobal.commutationStep; }
    QVariant getBridgeStep() { return deviceGlobal.bridgeStep; }

    void requestDeviceInformation();
    void requestProtocolVersion();
    void requestMessage(uint16_t messageId);
    void setThrottle(uint16_t throttle);
    void sendThrottle();

    void consumeData();

    uint8_t device_type;
    uint8_t device_id;

    void handleMessage(ping_message* message);

    uint16_t phaseA, phaseB, phaseC, neutral, current, voltage, _throttle, commutationFrequency;

    void writeMessage(ping_message message);

    void close();
    RegisterModel registerModel;

private:
    void write(uint8_t* data, uint16_t length);
    QTimer updateTimer;

signals:
    void newData();
    void closed();
    void firmwareVersionChanged(QString& firmwareVersion);
    void protocolVersionChanged(QString& protocolVersion);
    void deviceIdChanged(QString& deviceId);
    void hardwareRevisionChanged(QString& hardwareRevision);
};

#endif // DEVICE_H
