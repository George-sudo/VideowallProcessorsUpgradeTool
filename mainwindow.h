#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initMainWindow(void);
    void initView(void);

    static QString binfile_path;
    static QString jsonfile_path;


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
