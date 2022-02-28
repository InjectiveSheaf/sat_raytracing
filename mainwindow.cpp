#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QFileDialog>
#include <QTableWidget>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer* timer = new QTimer();
    timer->setInterval(1);

    connect(timer, &QTimer::timeout, this, [=](){
        t += timer->interval();
        double dx = ui->x_spinBox->text().toDouble();
        double dy = ui->y_spinBox->text().toDouble();
        double dz = ui->z_spinBox->text().toDouble();
        double dphi = ui->phi_spinBox->value();
        double dpsi = ui->psi_spinBox->value();
        double dtheta = ui->theta_spinBox->value();

        ui->widget->recalc(t, *(ui->textBrowser), dx, dy, dz, dphi, dpsi, dtheta);
        ui->widget->repaint();
    });

    timer->start();
}

//*************************************************************************


void MainWindow::on_addClusterButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open A File", "C://");
    ui->widget->addTLEObjects(fileName.toStdString(), t);
}

//*************************************************************************

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_clearButton_clicked()
{
    ui->widget->restart();
}
