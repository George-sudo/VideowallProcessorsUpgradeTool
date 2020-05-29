#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QCloseEvent>
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

private:
    Ui::Connection *ui;
};

#endif // CONNECTION_H
