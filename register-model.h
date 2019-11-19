#ifndef REGISTERMODEL_H
#define REGISTERMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class Device;

class RegisterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    typedef enum {
        REG_MODE_READONLY,
        REG_MODE_READWRITE,
    } register_mode_e;

    typedef enum {
        REG_TYPE_BOOL,
        REG_TYPE_UINT8,
        REG_TYPE_UINT16,
        REG_TYPE_UINT32,
    } register_type_e;

    typedef struct
    {
        QString name;
        register_type_e type;
        register_mode_e mode;
        bool plotEnabled;
        QVariant (Device::*data)();
    } register_t;

    RegisterModel(QObject* parent = nullptr, Device* _device = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void refreshAll() { emit dataChanged(index(0,0), index(registerList.size(),3), QVector<int>({Qt::EditRole})); }

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    QVector<register_t> registerList;
    Device* device;

signals:
    void registerEdited(int index);
    void plotEnabledChanged(int index);
};

#endif // REGISTERMODEL_H
