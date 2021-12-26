#include "./headers/orders.h"
#include "ui_orders.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>

orders::orders(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::orders)
{
    ui->setupUi(this);

    model = new QSqlTableModel(this, orderBD);
    model->setTable("orders");
    model->select();

    ui->OrdersTable->setModel(model);
    ui->OrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QRegularExpression reg_exp("[0-9]{1,20}");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(reg_exp, this);
    ui->idline->setValidator(validator);
    ui->courierline->setValidator(validator);
    ui->sumline->setValidator(validator);
    QRegularExpression reg_exp2("[0-9а-яА-Я ]{1,20}");
    QRegularExpressionValidator* validator2 = new QRegularExpressionValidator(reg_exp2, this);
    ui->fromline->setValidator(validator2);
    ui->whereline->setValidator(validator2);

    QSqlDatabase couriers_db = QSqlDatabase::addDatabase("QSQLITE", "connection 9");
    couriers_db.setDatabaseName("couriers");
    couriers_db.open();
    if (couriers_db.open()) {
        qDebug() << "БД курьеров открылась";
    }
    couriers_query = new QSqlQuery(couriers_db);
    *thread_is_free = true;
}

void orders::make_BD(QSqlDatabase a, QSqlQuery* b) {
    orderBD = a;
    orderQuery = b;
}

//Переделка функции для открытия её в другом потоке
//Сделала только одну функцию, потому что боюсь делать остальные, у меня
//ноутбук страшные звуки начинает издавать и греться сильно, когда работаю с несколькими потоками
//седьмая лаба этого семестра чуть не отправила мой ноутбук в нокаут, тут я рисковать не хочу
void adding_in_thread(QSqlQuery* couriers_query, QSqlQuery* orderQuery, QString courierline, QString idline,
                      QString fromline, QString whereline, QString dateline, QString sumline, std::atomic<bool>* thread_is_free) {
    couriers_query->exec("select Статус from couriers where ID = '"+courierline+"'");
    couriers_query->first();
    if (couriers_query->value(0).toString() != "Занят") {
        orderQuery->prepare("INSERT INTO orders(ID, Откуда, Куда, Курьер, Дата, Статус, Сумма) "
                                "VALUES('"+idline+"', '"+fromline+"', '"+whereline+"', '"+courierline+"', '"+dateline+"', 'В процессе', '"+sumline+"')");
        if(orderQuery->exec()){
        couriers_query->exec("update couriers set Статус='Занят' where ID='"+courierline+"'");
            }
         }
     *thread_is_free = true;
}

//Использую обычный thread a не QThread, потому что цитата разработчика QT в переводе:
// "Класс QThread создан в качестве интерфейса к потокам операционной системы, но не для того, чтобы помещать
// в него код, предназначенный для выполнения в отдельном потоке"
void orders::adding() {
    if (*thread_is_free == true) {
        *thread_is_free = false;
        QString courierline = ui->courierline->text();
        QString idline = ui->idline->text();
        QString fromline = ui->fromline->text();
        QString whereline = ui->whereline->text();
        QString dateline = ui->dateline->text();
        QString sumline = ui->sumline->text();
        std::thread my_thread(adding_in_thread, std::cref(couriers_query), std::cref(orderQuery), std::cref(courierline), std::cref(idline),
                                   std::cref(fromline), std::cref(whereline), std::cref(dateline), std::cref(sumline), thread_is_free);
        my_thread.join();
    }
   // } else {
     //   QMessageBox::information(this, "Wait", "wait");
    //}
}

//Вот так эта функция до исполнения в другом потоке выглядела
/*void orders::adding() {
    couriers_query->exec("select Статус from couriers where ID = '"+ui->courierline->text()+"'");
    couriers_query->first();
    if (couriers_query->value(0).toString() == "Занят") {
        QMessageBox::critical(this, "Ошибка", "Курьер занят");
    } else {
    orderQuery->prepare("INSERT INTO orders(ID, Откуда, Куда, Курьер, Дата, Статус, Сумма) "
                            "VALUES('"+ui->idline->text()+"', '"+ui->fromline->text()+"', '"+ui->whereline->text()+"', '"+ui->courierline->text()+"', '"+ui->dateline->text()+"', 'В процессе', '"+ui->sumline->text()+"')");
    if(orderQuery->exec()){
    couriers_query->exec("update couriers set Статус='Занят' where ID='"+ui->courierline->text()+"'");
    QMessageBox::information(this, "Заказ добавлен", "Данные успешно сохранены");
    }
  }
}
*/

void orders::deleting(QString id) {
    //Проверяем существование заказа
    orderQuery->exec("select Статус from orders where ID='"+id+"'");
    orderQuery->first();
    QString status = orderQuery->value(0).toString();
    if (status == "В процессе") {
        //Получаем курьера по заказу
        orderQuery->exec("select Курьер from orders where ID='"+id+"'");
        orderQuery->first();
        int cour_id = orderQuery->value(0).toInt();
        //Изменяем статус курьера на Свободен
        couriers_query->exec("update couriers set Статус='Свободен' where ID='"+QString::number(cour_id)+"'");
    }
}

orders::~orders()
{
    delete ui;
}



void orders::on_add_clicked()
{
    QString id = ui->idline->text();
    QString from = ui->fromline->text();
    QString where = ui->whereline->text();
    QString courier = ui->courierline->text();
    QString date = ui->dateline->text();
    QString sum = ui->sumline->text();

    bool flagID = true;

    if ((!id.isEmpty())&&(!from.isEmpty())&&(!where.isEmpty())
            &&(!courier.isEmpty())&&(!date.isEmpty())&&(!sum.isEmpty())) {
        //Проверка ID заказа
        orderQuery->prepare("select * from orders");
        orderQuery->exec();
        while (orderQuery->next()) {
            QString otherID = orderQuery->value(0).toString();
            if (id == otherID) {
                flagID = false;
            }
        }

        //Проверка существования такого курьера
        couriers_query->prepare("select * from couriers");
        couriers_query->exec();
        bool flag = false;
        while (couriers_query->next()) {
                     QString id_courier = couriers_query->value(0).toString();
                     if (id_courier == courier) {
                         flag = true;
                     }
             }
        if ((flagID)&&(flag)) {
                   adding();
        } else if ((!flagID)&&(flag)){
            QMessageBox::critical(this, "Невозможный ID", "Этот ID уже занят, попробуйте другой");
        } else {
            QMessageBox::critical(this, "Курьер не найден", "Такого курьера не существует");
        }
    }  else {
        QMessageBox::critical(this, "Не хватает данных", "Все поля должны быть заполнены");
    }
}



void orders::on_reload_clicked()
{
    QSqlQueryModel* modelTours = new QSqlQueryModel();
    QSqlQuery* selectQuery = orderQuery;

    selectQuery -> prepare("SELECT * from orders");
    selectQuery->exec();

    modelTours ->setQuery(*selectQuery);
    ui->OrdersTable->setModel(modelTours);
}


void orders::on_delete_2_clicked()
{
    QString id = ui->idline->text();

    bool flag = false;

    //Проверяем, введён ли ID корректно
    orderQuery->prepare("select * from orders");
    orderQuery->exec();
    while (orderQuery->next()) {
        QString otherID = orderQuery->value(0).toString();
        if (id == otherID) {
            flag = true;
        }
    }

    if (flag){ //если id введен корректно
        deleting(id);
        orderQuery->prepare("delete from orders where ID = '"+id+"'");
        if(orderQuery->exec()){
            QMessageBox::information(this, "Готово!", "Запись успешно удалена");
            ui->idline->clear();
            ui->fromline->clear();
            ui->whereline->clear();
            ui->courierline->clear();
            ui->dateline->clear();
            ui->sumline->clear();

        } else {
            QMessageBox::critical(this, "Ошибка!", "Запись не удалена");
        }

    } else{
        QMessageBox::critical(this, "Неверный ID", "Проверьте правильность написания ID");
    }


}


void orders::on_edit_clicked()
{
    //Получаем курьера  по заказу
    orderQuery->exec("select Курьер from orders where ID='"+ui->idline->text()+"'");
    orderQuery->first();
    QString old_cour_id = orderQuery->value(0).toString();
    qDebug() << old_cour_id;
    couriers_query->exec("select Статус from couriers where ID = '"+ui->courierline->text()+"'");
    couriers_query->first();
    if (couriers_query->value(0).toString() == "Занят") {
        QMessageBox::critical(this, "Ошибка", "Курьер занят");
    } else {
    orderQuery->exec("update orders set Откуда = '"+ui->fromline->text()+"', Куда = '"+ui->whereline->text()+"', Курьер = '"+ui->courierline->text()+"', Дата = '"+ui->dateline->text()+"', Сумма = '"+ui->sumline->text()+"' where ID='"+ui->idline->text()+"'");
    if (orderQuery->isActive()) {
        couriers_query->exec("update couriers set Статус ='Свободен' where ID = '"+old_cour_id+"'");
        couriers_query->exec("update couriers set Статус ='Занят' where ID = '"+ui->courierline->text()+"'");
        QMessageBox::information(this, "Заказ обнавлён", "Обновите таблицу");
    } else {
        QMessageBox::warning(this, "Упс...", "Что-то пошло не так");
    }
    }
}

