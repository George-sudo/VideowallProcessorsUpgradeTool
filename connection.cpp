#include "connection.h"
#include "ui_connection.h"
#include <QMessageBox>

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    ui->setupUi(this);
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
