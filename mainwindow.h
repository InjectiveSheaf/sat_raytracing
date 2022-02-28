#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showResult();

private slots:

    void on_addClusterButton_clicked();

    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    double t = 0;
};
#endif // MAINWINDOW_H
