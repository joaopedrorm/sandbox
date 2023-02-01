#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qdialogadicionaremprestimo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSair_triggered();

    void on_AddLoanButton_clicked();

private:
    Ui::MainWindow *ui;
    QDialogAdicionarEmprestimo *ptrAdicionarEmprestimo;
};

#endif // MAINWINDOW_H
