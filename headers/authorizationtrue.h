#ifndef AUTHORIZATIONTRUE_H
#define AUTHORIZATIONTRUE_H

#include <QMainWindow>
#include <./headers/orders.h>
#include <./headers/couriers.h>
#include <./headers/end.h>

namespace Ui {
class AuthorizationTrue;
}

class AuthorizationTrue : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuthorizationTrue(QWidget *parent = nullptr);
    ~AuthorizationTrue();

signals:
    void data_of_DB_couriers(QSqlDatabase a, QSqlQuery* b);
    void data_of_DB_orders(QSqlDatabase a, QSqlQuery* b);

private slots:
    void on_deliver_clicked();

    void on_couriers_clicked();

    void on_pushButton_clicked();

private:
    Ui::AuthorizationTrue *ui;
    Couriers *Cwindow = new Couriers();
    QSqlDatabase courierBD;
    QSqlQuery *courierQuery;
    orders *Owindow = new orders();
    QSqlDatabase orderBD;
    QSqlQuery *orderQuery;
    End *Ewindow = new End();
    std::atomic<bool>* thread_finished;
};

#endif // AUTHORIZATIONTRUE_H
