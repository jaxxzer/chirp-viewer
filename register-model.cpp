#include <register-model.h>
#include <device.h>

RegisterModel::RegisterModel(QObject* parent, Device* _device)
    : QAbstractTableModel(parent)
    , device(_device)
{
    registerList.append({"Battery Temperature", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::battery_temperature });
    registerList.append({"Battery Current", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::battery_current });
    registerList.append({"Battery Voltage", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::battery_voltage });
    registerList.append({"Cell0", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell0 });
    registerList.append({"Cell1", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell1 });
    registerList.append({"Cell2", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell2 });
    registerList.append({"Cell3", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell3 });
    registerList.append({"Cell4", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell4 });
    registerList.append({"Cell5", RegisterModel::REG_TYPE_UINT16, RegisterModel::REG_MODE_READONLY, false, &Device::cell5 });
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
    return 4;
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
    if (index.column() == 3) {
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
    } else if ((role == Qt::CheckStateRole) && (index.column() == 3)) {
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
