#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_actionSair_triggered()
{
    QApplication::quit();
}

void MainWindow::on_AddLoanButton_clicked()
{
    ptrAdicionarEmprestimo = new QDialogAdicionarEmprestimo(this);
    ptrAdicionarEmprestimo->show();
}
