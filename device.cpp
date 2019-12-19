#include <device.h>

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <ping-message-common.h>
#include <ping-message-bluebps.h>
#include <ping-message-openesc.h>
#include <register-model.h>


Device::Device(QSerialPortInfo info)
    : registerModel(this, this)
{

    handle = new ComHandle(info);
    connect(handle->serialPort, &QSerialPort::readyRead, this, &Device::consumeData);

    connect(handle, &ComHandle::closed, this, &Device::closed);

//    RegisterModel::register_t a = {"Battery Temperature", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::battery_temperature };
//    registerList.append(a);
//    registerList.append({"Battery Temperature", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::battery_temperature });
//  //  registerList.append({"Battery Current", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false});
//    //registerList.append({"Battery Voltage", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false});
////    registerList.append({"Cell1", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false});
////    registerList.append({"Cell2", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, ([]() { return state.cell_voltages[0]; }) });
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
    case OpenescId::STATE:
    {

        openesc_state* msg = (openesc_state*)message;
        phaseA = msg->phaseA();
        phaseB = msg->phaseB();
        phaseC = msg->phaseC();
        neutral = msg->neutral();
        throttle = msg->throttle();
        voltage = msg->voltage();
        current = msg->current();
        commutationFrequency = msg->commutation_period();

    }
        break;
    default:
        break;

    }
    //emit newData();

}

void Device::setThrottle(uint16_t throttle) {
    if (throttle > 0xfff) {
        throttle = 0xfff;
    }
    _throttle = throttle;
    openesc_set_throttle m;
    m.set_throttle_signal(_throttle);
    m.updateChecksum();
    writeMessage(m);
}
