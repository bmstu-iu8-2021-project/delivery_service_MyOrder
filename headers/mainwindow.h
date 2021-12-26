#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <./headers/authorizationtrue.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_radioButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QSqlDatabase log_and_pass;
    QSqlQuery *query;
    AuthorizationTrue *authotrue;
};

#endif // MAINWINDOW_H
