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

void Device::ping()
{
    pingchirp_transmit m;
    m.set_nsamples(nsamples);
    m.set_pulse_duration(pulse_duration);
    m.set_opamp1(opamp1_gain);
    m.set_opamp2(opamp2_gain);
    m.set_f0(f0);
    m.set_f1(f1);
    m.updateChecksum();
    writeMessage(m);
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
        float Dn = 343*Ts/2;
        QVector<double> data(msg->data_length());
        for (uint16_t i = 0; i < msg->data_length(); i++) {
            keys[i] = i*Dn;
            data[i] = msg->data()[i];
        }
        nsamples = msg->nsamples();
        f0 = msg->f0();
        f1 = msg->f1();
        pulse_duration = msg->pulse_duration();


        emit newData(keys, data);
        break;
    }
    default:
        break;

    }
    //emit newData();

}
