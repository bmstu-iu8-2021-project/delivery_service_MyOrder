#include "./headers/authorizationtrue.h"
#include "ui_authorizationtrue.h"
#include <QMessageBox>
#include "./headers/couriers.h"
#include <thread>

AuthorizationTrue::AuthorizationTrue(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AuthorizationTrue)
{
    ui->setupUi(this);
    QPixmap pix_name (":/images/name.png");
    ui->lab_name_pic->setPixmap(pix_name.scaled(100,100,Qt::KeepAspectRatio));
    QPixmap pix_del (":/images/курьер.png");
    ui->deliver_pic->setPixmap(pix_del.scaled(200,200,Qt::KeepAspectRatio));
    QPixmap pix_ord (":/images/заказ.png");
    ui->orders_pic->setPixmap(pix_ord.scaled(200,200,Qt::KeepAspectRatio));

    //Соединение для передачи бд в orders
    connect(this, &AuthorizationTrue::data_of_DB_orders, Owindow, &orders::make_BD);
    orderBD = QSqlDatabase::addDatabase("QSQLITE", "connection 2");
    orderBD.setDatabaseName("./orders");
    if (orderBD.open()) {
        qDebug("orderBD open");
    } else {
        qDebug("orderBD no open");
    }
    orderQuery = new QSqlQuery(orderBD);
    //orderQuery->exec("CREATE TABLE orders(ID INT, Откуда TEXT, "
      //               "Куда TEXT, Курьер INT, Дата TEXT, Статус TEXT, Сумма INT);");


    //Соединение для передачи бд в Couriers
    connect(this, &AuthorizationTrue::data_of_DB_couriers, Cwindow, &Couriers::make_BD);
    courierBD = QSqlDatabase::addDatabase("QSQLITE", "connection 3");
    courierBD.setDatabaseName("./couriers");
    if (courierBD.open()) {
        qDebug("courierBD open");
    } else {
        qDebug("courierBD no open");
    }
    courierQuery = new QSqlQuery(courierBD);
 //   courierQuery->exec("CREATE TABLE couriers(ID INT, Фамилия TEXT, "
   //                  "Имя TEXT, Статус TEXT, Выполненные INT, Сумма INT, Инциденты INT);");


}

AuthorizationTrue::~AuthorizationTrue()
{
    delete ui;
}

void AuthorizationTrue::on_deliver_clicked()
{
    emit data_of_DB_couriers(courierBD, courierQuery);
    Cwindow->resize(this->size());
    Cwindow->show();
}


void AuthorizationTrue::on_couriers_clicked()
{
    emit data_of_DB_orders(orderBD, orderQuery);
    Owindow->resize(this->size());
    Owindow->show();
}

void AuthorizationTrue::on_pushButton_clicked()
{
    Ewindow->show();
}
