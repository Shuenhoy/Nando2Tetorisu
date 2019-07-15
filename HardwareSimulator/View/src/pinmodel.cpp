#include <QString>
#include <QDebug>
#include <utility>
#include "pinmodel.h"

PinModel::PinModel(QObject *parent)
    : QAbstractTableModel(parent), _colCount(2), _rowCount(0), Name(), Val(), IsShown(), IsEditable(false), observer(nullptr) {
    header << "信号名" << "逻辑值";
    QRegExp rx("[0-9]{0,32}");
    regval = new QRegExpValidator(rx, this);
}

PinModel::~PinModel() {
    delete regval;
}

int PinModel::columnCount(const QModelIndex &parent) const {
    return _colCount;
}

int PinModel::rowCount(const QModelIndex &parent) const {
    return _rowCount;
}

QVariant PinModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0:
            if (index.row()<Name.size()) return Name[index.row()];
            else return QVariant();
        case 1:
            if (index.row()<Val.size() && IsShown[index.row()]) return Val[index.row()];
            else return QVariant();
        default:
            return QVariant();
        }
    }
    if(role==Qt::TextAlignmentRole) {
        switch(index.column()) {
        case 0:
            return Qt::AlignCenter;
        case 1:
            return Qt::AlignCenter;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant PinModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < _colCount) return header[section];
    }
    return QAbstractTableModel::headerData(section,orientation,role);;
}

void PinModel::Update(PinParcel parcel) {
    beginResetModel();
    Name.clear();
    IsShown.clear();
    int cnt = 0;
    for (auto i = parcel.first; i != parcel.second; i++, cnt++) {
        if ((*i).second > 1) {
            Name.push_back(QString::fromStdString((*i).first+"["+std::to_string((*i).second)+"]"));
        } else {
            Name.push_back(QString::fromStdString(((*i).first)));
        }
    }
    this->_rowCount = cnt;
    this->IsShown.resize(cnt);
    this->Val.resize(cnt);
    endResetModel();
}

void PinModel::Update(PinValueParcel parcel) {
    beginResetModel();
    Val.clear();
    IsShown.clear();
    int cnt = 0;
    for (auto i = parcel.first; i != parcel.second; i++, cnt++) {
        Val.push_back(*i);
        IsShown.push_back(true);
    }
    this->_rowCount = cnt;
    this->IsShown.resize(cnt);
    this->Name.resize(cnt);
    for (auto i = 0; i < cnt; i++) this->IsShown[i] = true;
    endResetModel();
}

void PinModel::setIsEditable(bool state) {
    IsEditable = state;
}

Qt::ItemFlags PinModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::ItemIsEnabled;
    if (IsEditable) {
        switch (index.column()) {
        case 0:
            return QAbstractItemModel::flags(index);
        case 1:
            if (IsEditable) return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
            else return QAbstractItemModel::flags(index);
        }
        return QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index);
}

bool PinModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        QString tmp_str = value.toString();
        int pos;
        switch(index.column()) {
        case 0:
            this->Name[index.row()] = value.toString();
            emit dataChanged(index, index);
            return true;
        case 1:
            if (regval->validate(tmp_str, pos)==QValidator::State::Acceptable) {
                this->Val[index.row()] = value.toUInt();
                this->IsShown[index.row()] = true;
                qDebug()<< "Data set:"<<index.row() <<value.toInt();
                observer->publish("set_pin", std::make_pair(index.row(), uint32_t(value.toInt())));
                qDebug()<<"set_pin";
                observer->publish("eval", {});
                emit dataChanged(index, index);
                return true;
            } else return false;
        default:
            return false;
        }
    }
    return false;
}

void PinModel::setObserver(Observer *o) {
    observer = o;
}

void PinModel::Clear() {
    _rowCount = 0;
}
/*
void PinModel::RefreshValue() {
    for (auto i = 0; i < _rowCount; i++) {
        IsShown[i] = false;
    }
}
*/
