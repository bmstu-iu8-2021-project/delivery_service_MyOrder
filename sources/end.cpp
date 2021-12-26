#include "./headers/end.h"
#include "ui_end.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

End::End(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::End)
{
    ui->setupUi(this);

    QRegularExpression reg_exp("[0-9]{1,20}");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(reg_exp, this);
    ui->order->setValidator(validator);

    QSqlDatabase orders_db = QSqlDatabase::addDatabase("QSQLITE", "connection 4");
    orders_db.setDatabaseName("orders");
    orders_db.open();
    orders_query = new QSqlQuery(orders_db);

    QSqlDatabase couriers_db = QSqlDatabase::addDatabase("QSQLITE", "connection 5");
    couriers_db.setDatabaseName("couriers");
    couriers_db.open();
    couriers_query = new QSqlQuery(couriers_db);
}

void End::ending() {
    QString id_order = ui->order->text();
    QString late_ = ui->late->text();
    if ((!id_order.isEmpty())&&(!late_.isEmpty())) {
        //Проверяем существование заказа
        orders_query->exec("select Статус from orders where ID='"+id_order+"'");
        orders_query->first();
        QString status = orders_query->value(0).toString();
        if (status == "Завершён") {
            QMessageBox::warning(this, "Ошибка", "Заказ уже удалён");
        } else {
            if ((late_ == "Да")||(late_ == "да")||(late_ == "Нет")||(late_ == "нет")) {
            //Получаем курьера и сумму по заказу
            orders_query->exec("select Курьер from orders where ID='"+id_order+"'");
            orders_query->first();
            int cour_id = orders_query->value(0).toInt();

            orders_query->exec("select Сумма from orders where ID='"+id_order+"'");
            orders_query->first();
            int sum = orders_query->value(0).toInt();

            //Изменяем статус заказа на Завершён
            orders_query->exec("update orders set Статус='Завершён' where ID='"+id_order+"'");

            //Получаем Выполненные, сумму и Инциденты у курьера
            couriers_query->exec("select Выполненные from couriers where ID='"+QString::number(cour_id)+"'");
            couriers_query->first();
            int made = couriers_query->value(0).toInt();

            couriers_query->exec("select Сумма from couriers where ID='"+QString::number(cour_id)+"'");
            couriers_query->first();
            int real_sum = couriers_query->value(0).toInt();

            couriers_query->exec("select Инциденты from couriers where ID='"+QString::number(cour_id)+"'");
            couriers_query->first();
            int late = couriers_query->value(0).toInt();

            //Изменяем Статус курьера на СВОБОДЕН, Выполнено увеличиваем на 1
            //Сумму увеличиваем на сумму заказа, инциденты увеличиваем на 0 или 1
            made += 1;
            real_sum += sum;
            if ((late_ == "Да")||(late_ == "да")) {
            late += 1;
            }
            couriers_query->exec("update couriers set Статус='Свободен', Выполненные = '"+QString::number(made)+"', Сумма = '"+QString::number(real_sum)+"', Инциденты = '"+QString::number(late)+"' where ID='"+QString::number(cour_id)+"'");
            if (couriers_query->isActive()) {

                QMessageBox::information(this, "Информация по заказу", "Заказ успешно завершён");
            }
            } else {
                QMessageBox::warning(this, "Ошибка", "Заполните второе поле словами да или нет");
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены");
    }
}

End::~End()
{
    delete ui;
}

void End::on_end_order_clicked()
{
 ending();
}
