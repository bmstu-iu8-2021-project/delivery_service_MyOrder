#ifndef COURIERS_H
#define COURIERS_H

#include <QMainWindow>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

namespace Ui {
class Couriers;
}

class Couriers : public QMainWindow
{
    Q_OBJECT

public:
    explicit Couriers(QWidget *parent = nullptr);
    ~Couriers();
    void make_BD(QSqlDatabase a, QSqlQuery* b);

public slots:
    void on_add_clicked();

    void on_refresh_clicked();

    void on_delete_2_clicked();

    void deleting();

private slots:
    void on_report_clicked();

private:
    Ui::Couriers *ui;
    QSqlDatabase courierBD;
    QSqlQuery *courierQuery;
    QSqlTableModel *model;
};

#endif // COURIERS_H
