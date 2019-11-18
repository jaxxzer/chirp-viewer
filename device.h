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

    typedef struct {
      uint16_t adc_buffer[ADC_NUM_CHANNELS];
      uint16_t battery_voltage;
      uint16_t battery_current;
      uint16_t battery_temperature;
      uint16_t cpu_temperature;
      uint16_t board_temperature;
      uint16_t cell_voltages[8];
    } state_t;

    state_t state;
    void requestState();

    void requestDeviceInformation();
    void requestProtocolVersion();
    void requestMessage(uint16_t messageId);

    void consumeData();

    uint8_t device_type;
    uint8_t device_id;

    void handleMessage(ping_message* message);

    void writeMessage(ping_message message);

    void close();

private:
    void write(uint8_t* data, uint16_t length);

signals:
    void newData();
    void closed();
};

#endif // DEVICE_H
