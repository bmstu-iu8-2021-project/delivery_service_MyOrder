#ifndef END_H
#define END_H

#include <QMainWindow>
#include <QSqlQuery>

namespace Ui {
class End;
}

class End : public QMainWindow
{
    Q_OBJECT

public:
    explicit End(QWidget *parent = nullptr);
    ~End();
    void ending();

private slots:
    void on_end_order_clicked();

private:
    Ui::End *ui;
    QSqlQuery* orders_query;
    QSqlQuery* couriers_query;
};

#endif // END_H
