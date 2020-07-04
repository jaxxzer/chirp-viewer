#ifndef DEVICE_H
#define DEVICE_H

#include <com-handle.h>
#include <com-parser.h>
#include <QSerialPort>
#include <QObject>
#include <QTimer>
#include <QMap>

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
    void requestDeviceInformation();
    void requestProtocolVersion();
    void requestMessage(uint16_t messageId);

    void consumeData();

    uint8_t device_type;
    uint8_t device_id;

    void handleMessage(ping_message* message);
    void writeMessage(ping_message message);

    void close();

    void ping();
    void setNSamples(int n);

    uint32_t cpu_frequency;
    uint32_t timer_frequency;
    uint16_t duty;

    uint8_t opamp1_gain;
    uint8_t opamp2_gain;
    uint8_t adc_resolution;
    uint8_t adc_sample_time;
    uint16_t pulse_duration;
    uint32_t sample_frequency;
    uint32_t f0;
    uint32_t f1;
    uint16_t digital_gain;
    uint16_t dc_bias;
    uint32_t nsamples;
    uint8_t profile_data[0xffff];

    void request_profile();


private:
    void write(uint8_t* data, uint16_t length);
    QTimer updateTimer;

signals:
    void closed();
    void firmwareVersionChanged(QString& firmwareVersion);
    void protocolVersionChanged(QString& protocolVersion);
    void deviceIdChanged(QString& deviceId);
    void hardwareRevisionChanged(QString& hardwareRevision);
    void newData(QVector<double>, QVector<double>);
};

#endif // DEVICE_H
