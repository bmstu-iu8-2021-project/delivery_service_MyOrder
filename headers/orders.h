#ifndef ORDERS_H
#define ORDERS_H

#include <QMainWindow>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>
#include <thread>

namespace Ui {
class orders;
}

class orders : public QMainWindow
{
    Q_OBJECT

public:
    explicit orders(QWidget *parent = nullptr);
    ~orders();
    void make_BD(QSqlDatabase a, QSqlQuery* b);

private slots:
    void on_add_clicked();

    void on_reload_clicked();

    void on_delete_2_clicked();

    void adding();

    void deleting(QString id);

    void on_edit_clicked();

private:
    Ui::orders *ui;
    QSqlDatabase orderBD;
    QSqlQuery *orderQuery;
    QSqlTableModel *model;
    QSqlQuery* couriers_query;
    //В этой переменной храним значение свободен ли поток
    std::atomic<bool>* thread_is_free;
};

#endif // ORDERS_H
