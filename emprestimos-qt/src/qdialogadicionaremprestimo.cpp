#include "qdialogadicionaremprestimo.h"
#include "ui_adicionaremprestimo.h"

QDialogAdicionarEmprestimo::QDialogAdicionarEmprestimo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDialogAdicionarEmprestimo)
{
    ui->setupUi(this);
}

QDialogAdicionarEmprestimo::~QDialogAdicionarEmprestimo()
{
    delete ui;
}

void QDialogAdicionarEmprestimo::on_CancelButton_clicked()
{
    this->close();
}
