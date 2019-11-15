#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_released()
{

    QString file = QFileDialog::getOpenFileName(this,tr("Ouvrir une image"),QDir::homePath(),tr("Images (*.png *.jpg *.bmp)"));
    ui->label->setText(file);
}

void MainWindow::on_pushButton_2_released()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Ouvrir une image"),QDir::homePath(),tr("Images (*.png *.jpg *.bmp)"));
    ui->label_4->setText(file);
}

void MainWindow::on_pushButton_3_released()
{
    if(ui->label_4->text()==""){
        QMessageBox::critical(this,tr("Erreur"),tr("Veuillez sélectionner vos images"));
    }
    else if(ui->label->text()==""){
        QMessageBox::critical(this,tr("Erreur"),tr("Veuillez sélectionner vos images"));
    }
    else{
        this->setHidden(true);
        Dialog d;
        d.setFile1(ui->label->text());
        d.setFile2(ui->label_4->text());
        d.plot();
        d.exec();

    }

}
