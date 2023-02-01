#ifndef QDIALOGADICIONAREMPRESTIMO_H
#define QDIALOGADICIONAREMPRESTIMO_H

#include <QDialog>

namespace Ui {
class QDialogAdicionarEmprestimo;
}

class QDialogAdicionarEmprestimo : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogAdicionarEmprestimo(QWidget *parent = 0);
    ~QDialogAdicionarEmprestimo();

private slots:
    void on_CancelButton_clicked();

private:
    Ui::QDialogAdicionarEmprestimo *ui;
};

#endif // ADICIONAREMPRESTIMO_H
