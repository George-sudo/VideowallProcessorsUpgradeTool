#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QHostAddress>
#include "mainwindow.h"

namespace Ui {
class Connection;
}

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(QWidget *parent = 0);
    ~Connection();
    void closeEvent(QCloseEvent *event);

public:
    static QTcpSocket *tcpClient;

private slots:
    void on_ConectBt_clicked();


private:
    Ui::Connection *ui;
};

#endif // CONNECTION_H
