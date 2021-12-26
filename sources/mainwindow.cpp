#include "./headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "./headers/authorizationtrue.h"
#include <QDebug>
//#include <picosha2.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pass->setEchoMode(QLineEdit::EchoMode::Password);

    ui->log_lab ->setBuddy(ui->pass);
    ui->pass_lab->setBuddy(ui->login);

    QRegularExpression reg_exp("[0-9a-zA-Z]{1,20}");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(reg_exp, this);
    ui->login->setValidator(validator);
    ui->login->setFocus();
    ui->pass->setValidator(validator);

    QPixmap pix (":/images/авторизация.png");
    ui->pic_label->setPixmap(pix.scaled(150,150,Qt::KeepAspectRatio));
    log_and_pass = QSqlDatabase::addDatabase("QSQLITE", "connection 6");
    log_and_pass.setDatabaseName("./log_and_pass");
    log_and_pass.open();
    //Инициализируем окно успешной авторизации
    authotrue = new AuthorizationTrue();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
   query = new QSqlQuery(log_and_pass);
   query->prepare("select * from LoginsANDPasswords");
   query->exec();
   QString login = ui->login->text();
   QString password = ui->pass->text();
   //std::string hash = picosha2::hash256_hex_string(password);
   bool flag = false;
   while (query->next()) {
                QString needlog = query->value(0).toString();
                QString needpass = query->value(1).toString();
                if ((login == needlog)&&(password == needpass)) {
                    flag = true;
                }
        }
   if (flag) {
       authotrue->resize(this->size());
       authotrue->restoreGeometry(this->saveGeometry());
       authotrue->show();
       this->close(); // Закрываем основное окно
       log_and_pass.close();
   } else {
       QMessageBox::warning(this, "Авторизация не удалась", "Неверный логин или пароль");
       ui->login->clear();
       ui->pass->clear();
   }
}


void MainWindow::on_radioButton_toggled(bool checked)
{
    if (!checked) {
        ui->pass->setEchoMode(QLineEdit::EchoMode::Password);
    } else {
        ui->pass->setEchoMode(QLineEdit::EchoMode::Normal);
    }
}
