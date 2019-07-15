#include <QtWidgets>
#include <QStandardItemModel>
#include <QHeaderView>
#include "pinview.h"

PinView::PinView(QWidget *parent)
    :   QTableView(parent),
        model(new PinModel(this))
{
    setShowGrid(false);
    verticalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setModel(model);
    show();
}

PinView::~PinView() {
    delete model;
}

void PinView::UpdateModel(PinParcel parcel) {
    model->Update(parcel);
    setModel(model);
    show();

    qDebug()<<"UpdateModel Finished";
    qDebug()<<model->rowCount() << model->columnCount();
    qDebug()<<model->data(model->index(0,0));

    qDebug()<<"Show Finished";
}

void PinView::UpdateModel(PinValueParcel parcel) {
    model->Update(parcel);
    setModel(model);
    show();
    qDebug()<<"UpdateModel Finished";
    qDebug()<<model->rowCount() << model->columnCount();
    qDebug()<<model->data(model->index(0,0));

    qDebug()<<"Show Finished";
}

void PinView::setModelIsEditable(bool state) {
    model->setIsEditable(state);
}

void PinView::setObserver(Observer *o) {
    model->setObserver(o);
}

void PinView::ClearModel() {
    model->Clear();
    setModel(model);
    show();
}
