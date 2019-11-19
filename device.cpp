#include <device.h>

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <ping-message-common.h>
#include <ping-message-bluebps.h>
#include <ping-message-openesc.h>
#include <register-model.h>


Device::Device(QSerialPortInfo info)
{

    handle = new ComHandle(info);
    connect(handle->serialPort, &QSerialPort::readyRead, this, &Device::consumeData);

    connect(handle, &ComHandle::closed, this, &Device::closed);
}

bool Device::open()
{
    return handle->open();
}

void Device::writeMessage(ping_message message)
{
    write(message.msgData, message.msgDataLength());
}

void Device::requestProtocolVersion()
{
    requestMessage(CommonId::PROTOCOL_VERSION);
}

void Device::requestDeviceInformation()
{
    requestMessage(CommonId::DEVICE_INFORMATION);
}

void Device::requestMessage(uint16_t messageId)
{
    common_general_request msg;
    msg.set_requested_id(messageId);
    msg.updateChecksum();
    writeMessage(msg);
}

void Device::consumeData()
{
    auto data = handle->serialPort->readAll();
    for (auto b : data) {
        if (parser.parseByte(b) == ComParser::NEW_MESSAGE) {
            handleMessage(&parser.parser.rxMessage);
            switch (parser.parser.rxMessage.message_id()) {
            case CommonId::DEVICE_INFORMATION:

                 device_type = ((common_device_information)parser.parser.rxMessage).device_type();
                break;
            default:
                break;
            }

            device_id = parser.parser.rxMessage.source_device_id();
            emit newData();
        }
    }
}

void Device::close()
{
    handle->close();
}

void Device::write(uint8_t *data, uint16_t length)
{
    if (handle) {
        handle->write(data, length);
    }
}

void Device::handleMessage(ping_message* message)
{
    switch (message->message_id()) {
    case CommonId::DEVICE_INFORMATION:
        device_id = message->source_device_id();
        device_type = ((common_device_information*)message)->device_type();
        break;
    case BluebpsId::STATE:
    {

        bluebps_state* msg = (bluebps_state*)message;
        state.battery_voltage = msg->battery_voltage();
        state.battery_current = msg->battery_current();
        state.battery_temperature = msg->battery_temperature();
        state.cpu_temperature = msg->cpu_temperature();
        for (uint8_t i = 0; i < msg->cell_voltages_length(); i++) {
            state.cell_voltages[i] = msg->cell_voltages()[i];
        }
        emit newData();
    }
        break;
    default:
        break;

    }
}
