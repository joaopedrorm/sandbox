#include "databasemanager.h"

DataBaseManager::DataBaseManager(const QString& path)
{
    QString dbPath = path.isEmpty() ? "db.sqlite3" : path;

}

QSqlError DataBaseManager::DbInit()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if(!db.open())
    {
        MessageBox::critical(0, "Falha no acesso ao banco de dados",
                    qApp->tr("Não foi possível acessar o banco de dados especificado:\n"
                             + dbPath +
                             "Esta aplicação é compatível apenas com banco de dados sqlite3\n"
                             "Reinicie a aplicação para tentar novamente."), QMessageBox::Ok);
        return db.lastError();
    }

    QStringList tables = db.tables();

    if (tables.contains("loan", Qt::CaseInsensitive) && tables.contains("person", Qt::CaseInsensitive))
    {
        return QSqlError();
    }


}
