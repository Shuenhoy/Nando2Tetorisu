#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "pinmodel.h"
#include "pinview.h"
#include "observer.hpp"

using Nando2Tetorisu::common::Observer;

class QAction;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Observer *o = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Observer* observer;
    QTimer* timer;
    void ShowHDLCode(std::string code);
    void SetSimLine(int line);
    void ClearAllTable();
    template<typename U>
    void subscribe(std::string message, U u){
        observer->subscribe(message, [=](std::any v){
            qDebug()<< QString::fromStdString(message);
            QMetaObject::invokeMethod(this, [=](){u(v);});
        });
    }
private slots:
    void NewFile();
    void LoadFile();
    void SaveFile();
    void Reload();
    void NewSim();
    void LoadSim();
    void SingleStep();
    void ExecSim();
    void PauseSim();
    void SetSpeed(int speed);
    void ResetSim();
};

#endif // MAINWINDOW_H
