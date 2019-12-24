#include <register-model.h>
#include <device.h>

RegisterModel::RegisterModel(QObject* parent, Device* _device)
    : QAbstractTableModel(parent)
    , device(_device)
{
    registerList.append({"adc0", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc0 });
    registerList.append({"adc1", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc1 });
    registerList.append({"adc2", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc2 });
    registerList.append({"adc3", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc3 });
    registerList.append({"adc4", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc4 });
    registerList.append({"adc5", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc5 });
    registerList.append({"adc6", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc6 });
    registerList.append({"adc7", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getAdc7 });
    registerList.append({"phaseA", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getPhaseA });
    registerList.append({"phaseB", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getPhaseB });
    registerList.append({"phaseC", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getPhaseC });
    registerList.append({"PhaseNeutral", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getPhaseNeutral });
    registerList.append({"throttle", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getThrottle });
    registerList.append({"startupThrottle", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getStartupThrottle });
    registerList.append({"directionMode", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getDirectionMode });
    registerList.append({"direction", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::getDirection });
}

int RegisterModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return registerList.size();
    return 0;
}

int RegisterModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant RegisterModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 1:
            return (device->*registerList[index.row()].data)();
        case 0:
            return registerList[index.row()].name;
        default:
            break;
        }
    } else if (role == Qt::CheckStateRole) {
        if (index.column() == 2) {
            if (registerList[index.row()].plotEnabled) {
                return Qt::Checked;
            } else {
                return Qt::Unchecked;
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags RegisterModel::flags(const QModelIndex &index) const
{
    if (index.column() == 2) {
        return Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
    }
    if (index.column() == 1 && registerList[index.row()].mode == REG_MODE_READWRITE) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    }

    return QAbstractItemModel::flags(index);
}

bool RegisterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((role == Qt::EditRole) && (index.column() == 1)) {
        if (registerList[index.row()].mode == REG_MODE_READONLY) {
            return false;
        }
    } else if ((role == Qt::CheckStateRole) && (index.column() == 2)) {
        registerList[index.row()].plotEnabled = value.toBool();
        emit plotEnabledChanged(index.row());
        return true;
    }
    return false;
}

bool RegisterModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return false;
}

bool RegisterModel::insertColumns(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return false;
}

bool RegisterModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return false;
}

bool RegisterModel::removeColumns(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return false;
}
