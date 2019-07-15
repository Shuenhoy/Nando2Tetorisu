#ifndef PINMODEL_H
#define PINMODEL_H
#include <QAbstractTableModel>
#include <QRegExpValidator>
#include "observer.hpp"

using Nando2Tetorisu::common::Observer;
using pin = std::pair<std::string, int>;
typedef std::pair<std::vector<pin>::const_iterator, std::vector<pin>::const_iterator> PinParcel;
typedef std::pair<std::vector<uint32_t>::const_iterator, std::vector<uint32_t>::const_iterator> PinValueParcel;
class PinModel: public QAbstractTableModel {
    Q_OBJECT

public:
    PinModel(QObject *parent = nullptr);
    ~PinModel()override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void Update(PinParcel parcel);
    void Update(PinValueParcel parcel);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setIsEditable(bool state);
    void setObserver(Observer* o);
    void Clear();

private:
    int _colCount;
    int _rowCount;
    bool IsEditable;
    QStringList header;
    QVector<QString> Name;
    QVector<quint32> Val;
    QVector<bool> IsShown;
    Observer* observer;
    QRegExpValidator* regval;
    //void RefreshValue();
};

#endif // PINMODEL_H
