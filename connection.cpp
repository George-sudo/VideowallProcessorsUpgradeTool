#include "connection.h"
#include "ui_connection.h"
#include <QMessageBox>

QTcpSocket *Connection::tcpClient = nullptr;

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    ui->setupUi(this);
    tcpClient = new QTcpSocket(this);
    tcpClient->abort();
}

Connection::~Connection()
{
    delete ui;
}

void Connection::closeEvent(QCloseEvent *event)
{
    /* 调试先屏蔽
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("退出程序"),
        QString(tr("提示：设备还未连接，退出LED拼接器升级?")),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {
        event->ignore();  //忽略退出信号，程序继续运行
    }
    else if (button == QMessageBox::Yes) {
        event->accept();  //接受退出信号，程序退出
        //以下代码会导致出现：程序异常结束。
        //parentWidget()->close();
        //20200528yu: DEBUG调试暂时先将下面两个屏蔽
        //QApplication *qapp;
        //qapp->quit();
    }
    */
}

void Connection::on_ConectBt_clicked()
{
    QString ip = ui->ipLE1->text() + '.' + ui->ipLE2->text() + '.' + ui->ipLE3->text() + '.' + ui->ipLE4->text();
    tcpClient->connectToHost(ip,ui->PortLE->text().toInt());
    if(!tcpClient->waitForConnected(1000))
    {
        QMessageBox::information(NULL, "提醒", "连接失败");
    }
}

