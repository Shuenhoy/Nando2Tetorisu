#include <QtWidgets>
#include "dialog.h"
#include "../ui/ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString title, QString content) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    this->ui->label->setText(content);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
}

void Dialog::on_pushButton_clicked()
{
    this->close();
}
