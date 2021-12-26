#include "./headers/couriers.h"
#include "ui_couriers.h"
#include <QMessageBox>
#include "./headers/orders.h"
#include <QSqlQuery>
#include <QDebug>
#include <QTextStream>

Couriers::Couriers(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Couriers)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this, courierBD);
    model->setTable("couriers");
    model->select();
    ui->courierTable->setModel(model);
    ui->courierTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QRegularExpression reg_exp("[0-9]{1,20}");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(reg_exp, this);
    ui->idline->setValidator(validator);
    QRegularExpression reg_exp2("[а-я' 'А-Я]{1,20}");
    QRegularExpressionValidator* validator2 = new QRegularExpressionValidator(reg_exp2, this);
    ui->FIOline->setValidator(validator2);
}

void Couriers::make_BD(QSqlDatabase a, QSqlQuery* b) {
    courierBD = a;
    courierQuery = b;
}

void Couriers::deleting() {
    courierQuery->exec("select Статус from couriers where ID = '"+ui->idline->text()+"'");
    courierQuery->first();
    if (courierQuery->value(0).toString() == "Занят") {
        QMessageBox::critical(this, "Ошибка", "В данный момент курьер выполняет заказ");
    } else {
        courierQuery->prepare("delete from couriers where ID = '"+ui->idline->text()+"'");
        if(courierQuery->exec()){
            QMessageBox::information(this, "Готово!", "Запись успешно удалена");
            ui->idline->clear();
            ui->FIOline->clear();
            ui->idline->clear();
    }
}
}

Couriers::~Couriers()
{
    delete ui;
}

void Couriers::on_add_clicked()
{
    QString id = ui->idline->text();
    QString fio = ui->FIOline->text();
    bool flag = true;

    if ((!id.isEmpty())&&(!fio.isEmpty())) {
        if (fio.contains(" ")) {
            QStringList list = fio.split(QLatin1Char(' '));

            QString surname = list[0];
            QString name = list[1];
        //Проверка id
        courierQuery->prepare("select * from couriers");
        courierQuery->exec();
        while (courierQuery->next()) {
                     QString otherID = courierQuery->value(0).toString();
                     if (id == otherID) {
                         flag = false;
                         QMessageBox::critical(this, "Невозможный ID", "Этот ID уже занят, попробуйте другой");
                     }
        }
        if (flag) {
            courierQuery->prepare("INSERT INTO couriers(ID, Фамилия, Имя, Статус, Выполненные, Сумма, Инциденты) "
                                "VALUES('"+id+"', '"+surname+"', '"+name+"', 'Свободен', 0, 0, 0)");
            if(courierQuery->exec()){
                QMessageBox::information(this, "Курьер добавлен", "Данные успешно сохранены");
                ui->idline->clear();
                ui->FIOline->clear();
            } else {
                QMessageBox::critical(this, "Ошибка", "Данные не сохранены");
            }
        }
        } else {
            QMessageBox::critical(this, "Ошибка", "Вы должны ввести Фамилию и Имя");
        }
    } else {
        QMessageBox::critical(this, "Не хватает данных", "Все поля должны быть заполнены");
    }
}


void Couriers::on_refresh_clicked()
{
    QSqlQueryModel* modelCouriers = new QSqlQueryModel();
    QSqlQuery* selectQuery = courierQuery;

    selectQuery -> prepare("SELECT * from couriers");
    selectQuery->exec();

    modelCouriers ->setQuery(*selectQuery);
    ui->courierTable->setModel(modelCouriers);
}


void Couriers::on_delete_2_clicked()
{
    QString id = ui->idline->text();

    bool flag = false;

    //Проверяем, введён ли ID корректно
    courierQuery->prepare("select * from couriers");
    courierQuery->exec();
    while (courierQuery->next()) {
                 QString otherID = courierQuery->value(0).toString();
                 if (id == otherID) {
                     flag = true;
                 }
    }

    if (flag){ //если id введен корректно
        deleting();
    } else{
        QMessageBox::critical(this, "Неверный ID", "Проверьте правильность написания ID");
    }

}


void Couriers::on_report_clicked()
{
    courierQuery->prepare("select ID from couriers");
    courierQuery->exec();
    bool flag = false;
    while (courierQuery->next()) {
                QString needstatus = courierQuery->value(0).toString();
                 if (ui->idline->text() == needstatus) {
                     flag = true;
                 }
         }
    if (!ui->idline->text().isEmpty()) {
        if (flag) {

            courierQuery->prepare("select Фамилия from couriers where ID = '"+ui->idline->text()+"'");
            courierQuery->exec();
            courierQuery->first();
            QString surname = courierQuery->value(0).toString();

            courierQuery->prepare("select Имя from couriers where ID = '"+ui->idline->text()+"'");
            courierQuery->exec();
            courierQuery->first();
            QString name = courierQuery->value(0).toString();

            courierQuery->prepare("select Сумма from couriers where ID = '"+ui->idline->text()+"'");
            courierQuery->exec();
            courierQuery->first();
            QString sum = courierQuery->value(0).toString();

            courierQuery->prepare("select Выполненные from couriers where ID = '"+ui->idline->text()+"'");
            courierQuery->exec();
            courierQuery->first();
            QString made = courierQuery->value(0).toString();

            courierQuery->prepare("select Инциденты from couriers where ID = '"+ui->idline->text()+"'");
            courierQuery->exec();
            courierQuery->first();
            QString late = courierQuery->value(0).toString();

    QString filename = "Отчёт_по_курьеру.txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << "Отчёт по заработной плате курьера " << ui->idline->text() << '\n' ;
        stream << '\n' ;
        stream << "Фамилия: " << surname << "." << '\n' ;
        stream << "Имя: " << name << "." << '\n' ;
        stream << "За этот месяц курьер доставил " << made << " заказов" << '\n' ;
        stream << "на общую сумму " << sum << " рублей." << '\n' ;
        stream << "Из них " << late << " с опозданием." << '\n' ;
        stream << "В связи с этим заработная плата курьера составляет ____________ рублей" << '\n' ;
        stream << "Премия составляет ____________ рублей" << '\n' ;
        QMessageBox::information(this, "Отчёт составлен", "Заберите отчёт в папке");
    }
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверный ID курьера");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Укажите ID курьера");
    }
}

