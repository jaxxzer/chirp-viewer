#include <device.h>

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QVector>
#include <ping-message-common.h>
#include <ping-message-chirp.h>

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
    case PingchirpId::CHIRP_DATA:
    {
        pingchirp_chirp_data* msg = (pingchirp_chirp_data*)message;
        QVector<double> keys(msg->data_length());
        float Ts = 1/300000.0;
        QVector<double> data(msg->data_length());
        for (uint16_t i = 0; i < msg->data_length(); i++) {
            keys[i] = i*Ts;
            data[i] = msg->data()[i];
        }

        emit newData(keys, data);
        break;
    }
    default:
        break;

    }
    //emit newData();

}
