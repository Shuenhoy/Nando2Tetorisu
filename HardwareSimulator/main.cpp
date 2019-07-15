#include <QApplication>
#include <memory>
#include "mainwindow.h"
#include "view_model.hpp"
#include "../common/include/observer.hpp"

using Nando2Tetorisu::common::Observer;
using namespace  Nando2Tetorisu::HardwareSimulator;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto o = std::make_unique<Observer>();
    auto viewmodel=std::make_unique<ViewModel>(o.get());
    MainWindow w(nullptr, o.get());
    w.show();

    return a.exec();
}
