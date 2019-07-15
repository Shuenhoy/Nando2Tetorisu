#ifndef DIALOGBOX_H
#define DIALOGBOX_H

#include <QDialog>
#include "observer.hpp"

using Nando2Tetorisu::common::Observer;

class QAction;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, QString title = QString(), QString content = QString());

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOGBOX_H
