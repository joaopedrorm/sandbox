#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QMessageBox>
#include <QtSql>

class DataBaseManager
{
public:
    DataBaseManager(const QString& path);
    QSqlError DbInit();

private:
    QSqlDatabase db;
    QString dbPath;
};

#endif // DATABASEMANAGER_H
