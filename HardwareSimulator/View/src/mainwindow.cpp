#include <QtWidgets>
#include <utility>
#include <any>
#include <string>
#include "mainwindow.h"
#include "dialog.h"
#include "../ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Observer* o) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer()
{
    if (o ==nullptr) observer = new Observer();
    else observer = o;
    ui->setupUi(this);
    ui->inputPinTable->setObserver(o);
    ui->internalPinTable->setObserver(o);
    ui->outputPinTable->setObserver(o);
    ui->hdlEdit->setLineWrapMode(QTextEdit::NoWrap);
    ui->simulationEdit->setLineWrapMode(QTextEdit::NoWrap);
    connect(ui->newHdlAction, SIGNAL(triggered()), this, SLOT(NewFile()));
    connect(ui->newSimAction, SIGNAL(triggered()), this, SLOT(NewSim()));
    connect(ui->openHdlAction, SIGNAL(triggered()), this, SLOT(LoadFile()));
    connect(ui->saveHdlAction, SIGNAL(triggered()), this, SLOT(SaveFile()));
    connect(ui->reloadAction, SIGNAL(triggered()), this, SLOT(Reload()));
    connect(ui->openSimAction, SIGNAL(triggered()), this, SLOT(LoadSim()));
    connect(ui->singleStepAction, SIGNAL(triggered()), this, SLOT(SingleStep()));
    connect(ui->pauseAction, SIGNAL(triggered()), this, SLOT(PauseSim()));
    connect(ui->execSimAction, SIGNAL(triggered()), this, SLOT(ExecSim()));
    connect(ui->resetSimAction, SIGNAL(triggered()), this, SLOT(ResetSim()));
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), ui->speedLabel, SLOT(setNum(int)));
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSpeed(int)));
    subscribe("set_input_pins", [this](std::any v){
        ui->inputPinTable->UpdateModel(std::any_cast<PinParcel>(v));
    });
    subscribe("set_internal_pins", [this](std::any v){
        ui->internalPinTable->UpdateModel(std::any_cast<PinParcel>(v));
    });
    subscribe("set_output_pins", [this](std::any v){
        ui->outputPinTable->UpdateModel(std::any_cast<PinParcel>(v));
    });
    subscribe("set_input_pins_value", [this](std::any v){
        ui->inputPinTable->UpdateModel(std::any_cast<PinValueParcel>(v));
    });
    subscribe("set_internal_pins_value", [this](std::any v){
        ui->internalPinTable->UpdateModel(std::any_cast<PinValueParcel>(v));
    });
    subscribe("set_output_pins_value", [this](std::any v){
         ui->outputPinTable->UpdateModel(std::any_cast<PinValueParcel>(v));
    });
    subscribe("set_hdl_source", [this](std::any v){
         ui->hdlEdit->setPlainText(QString::fromStdString(std::any_cast<std::string>(v)));
         ui->hdlEdit->setReadOnly(false);
         ui->hdlEdit->show();
    });
    subscribe("set_script_source", [this](std::any v){
         ui->simulationEdit->setPlainText(QString::fromStdString(std::any_cast<std::string>(v)));
         ui->simulationEdit->setReadOnly(false);
         ui->simulationEdit->show();
    });
    subscribe("clear_script", [this](std::any v){
         ui->simulationEdit->clear();
         ui->simulationEdit->setReadOnly(true);
         ui->simulationEdit->show();
    });
    subscribe("set_error", [this](std::any v) {
        Dialog *dialog = new Dialog(this, QString("Error"), QString::fromStdString(std::any_cast<std::string>(v)));
        dialog->exec();
    });
    subscribe("set_message", [this](std::any v) {
        Dialog *dialog = new Dialog(this, QString("Message"), QString::fromStdString(std::any_cast<std::string>(v)));
        dialog->exec();
    });
    subscribe("set_current_line", [this](std::any v) {
        SetSimLine(std::any_cast<int>(v));
    });
    subscribe("set_top_name", [this](std::any v) {
       ui->nameEdit->setPlainText(QString::fromStdString(std::any_cast<std::string>(v)));
       ui->nameEdit->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::NewFile()
{
    ui->hdlEdit->clear();
    ui->hdlEdit->setReadOnly(false);
}

void MainWindow::LoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开HDL文件"), "", tr("HDL文件 (*.hdl);;所有文件 (*)"));
    if (fileName.isEmpty()) return;
    else {
        ClearAllTable();
        ui->inputPinTable->setModelIsEditable(true);
        observer->publish(std::string("load_chip"), std::string(fileName.toUtf8().constData()));
    }
}

void MainWindow::SaveFile()
{
    observer->publish("edit_chip", ui->hdlEdit->toPlainText().toStdString());
    observer->publish("save_chip", {});
}

void MainWindow::Reload() {
    observer->publish("edit_chip", ui->hdlEdit->toPlainText().toUtf8().constData());
    observer->publish(std::string("reload"), nullptr);
}


void MainWindow::ShowHDLCode(std::string code) {
    ui->hdlEdit->setPlainText(QString::fromStdString(code));
}

void MainWindow::NewSim()
{
    ui->simulationEdit->clear();
    ui->simulationEdit->setReadOnly(false);
}

void MainWindow::LoadSim() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开仿真文件"), "", tr("仿真文件 (*.tst);;所有文件 (*)"));
    if (fileName.isEmpty()) return;
    else {
        ClearAllTable();
        observer->publish(std::string("load_script"), std::string(fileName.toUtf8().constData()));
    }
}

void MainWindow::SingleStep() {
    ui->inputPinTable->setModelIsEditable(false);
    observer->publish("single_step", {});
}

void MainWindow::SetSimLine(int line) {
    if (line >= 0) {
        QList<QTextEdit::ExtraSelection> exselList;
        QTextEdit::ExtraSelection exsel;
        QTextCursor cursor(ui->simulationEdit->document()->findBlockByLineNumber(line-1));
        cursor.select(QTextCursor::LineUnderCursor);
        exsel.cursor = cursor;
        exsel.format.setBackground(QColor(Qt::white));
        exsel.format.setForeground(QColor(Qt::darkBlue));
        exsel.format.setProperty(QTextFormat::FullWidthSelection, true);
        exselList.append(exsel);
        ui->simulationEdit->setExtraSelections(exselList);
        ui->simulationEdit->show();
    }
}

void MainWindow::ClearAllTable() {
    ui->inputPinTable->ClearModel();
    ui->internalPinTable->ClearModel();
    ui->outputPinTable->ClearModel();
}

void MainWindow::ExecSim() {
    ui->inputPinTable->setModelIsEditable(false);
    observer->publish("run", {});
}

void MainWindow::PauseSim() {
    observer->publish("puase", {});
}

void MainWindow::SetSpeed(int speed) {
    observer->publish("change_speed", speed);
    qDebug() << "Speed set to " << speed;
}

void MainWindow::ResetSim() {
    observer->publish("reset", {});
}
