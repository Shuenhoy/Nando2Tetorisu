#ifndef PINVIEW_H
#define PINVIEW_H
#include <QTableView>
#include "pinmodel.h"
#include "observer.hpp"

using Nando2Tetorisu::common::Observer;
class PinView: public QTableView {
    Q_OBJECT

public:
    PinView(QWidget *parent = nullptr);
    ~PinView();
    void UpdateModel(PinParcel parcel);
    void UpdateModel(PinValueParcel parcel);
    void setModelIsEditable(bool state);
    void setObserver(Observer* o);
    void ClearModel();

private:
    PinModel* model;
};

#endif // PINVIEW_H
