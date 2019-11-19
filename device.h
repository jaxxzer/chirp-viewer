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

class Device : public QObject
{
    Q_OBJECT
public:
    Device(QSerialPortInfo info);
    ComParser parser;
    ComHandle* handle;
    bool open();

    typedef struct {
      uint16_t battery_voltage;
      uint16_t battery_current;
      uint16_t battery_temperature;
      uint16_t cpu_temperature;
      uint16_t board_temperature;
      uint16_t cell_voltages[6];
    } state_t;


    QVariant battery_voltage() { return state.battery_voltage; }
    QVariant battery_current() { return state.battery_current; }
    QVariant battery_temperature() { return state.battery_temperature; }
    QVariant cell0() { return state.cell_voltages[0]; }
    QVariant cell1() { return state.cell_voltages[1]; }
    QVariant cell2() { return state.cell_voltages[2]; }
    QVariant cell3() { return state.cell_voltages[3]; }
    QVariant cell4() { return state.cell_voltages[4]; }
    QVariant cell5() { return state.cell_voltages[5]; }

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
    RegisterModel registerModel;


private:
    void write(uint8_t* data, uint16_t length);

signals:
    void newData();
    void closed();
};

#endif // DEVICE_H
