#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
//#include "ui_connection.h"
#include "unzip_ufw.h"
#include <QStandardItemModel>
//#include <QMessageBox>
#include <QFlags>

//定义全局变量
QStandardItemModel* GM_TreeView;
QDialog* GW_connection;// = new QDialog(this);
//QFlags
    //文件路径
QString MainWindow::binfile_path = "";
QString MainWindow::jsonfile_path = "";
    //板卡类型
QStandardItem* GI_input;
QStandardItem* GI_output;
QStandardItem* GI_control;
QStandardItem* GI_backboard;
//QStandardItem* GI_view;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //20200508yu: 设置标题
    this->setWindowTitle("LED拼接处理器升级软件");
    //this->setStyleSheet("background-color: rgb(34, 153, 255);");
    GW_connection = new Connection(parent);
    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    //初始化全局变量
    GM_TreeView = new QStandardItemModel(ui->treeView);
    ui->treeView->setModel(GM_TreeView);
    GI_input = new QStandardItem("输入卡");
    GI_output = new QStandardItem("输出卡");
    GI_control = new QStandardItem("控制卡");
    GI_backboard = new QStandardItem("背板");
    //unzipFile("haha");
    //初始化视图
    initView();
}
void MainWindow::initView()
{
    qDebug()<<"initView"<<endl;
    GM_TreeView->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("板卡")<<QStringLiteral("\n版本")<<QStringLiteral("MCU固件\n升级状态")<<QStringLiteral("\n版本")<<QStringLiteral("FPGA1固件\n升级状态")<<QStringLiteral("\n版本")<<QStringLiteral("FPGA2固件\n升级状态"));
    ui->treeView->setColumnWidth(0, 240);//设置宽度
    //填充
    GM_TreeView->appendRow(GI_input);
    GM_TreeView->appendRow(GI_output);
    GM_TreeView->appendRow(GI_control);
    GM_TreeView->appendRow(GI_backboard);
    ui->treeView->expandAll();//展开所有
    //弹出子窗口
    GW_connection->setWindowTitle(tr("连接方式"));
    //Qt::WindowFlags flags= GW_connection->windowFlags();
    GW_connection->setWindowFlags(GW_connection->windowFlags()&~Qt::WindowContextHelpButtonHint);
    //topWindow->setWindowModality(Qt::ApplicationModal); //阻塞除当前窗体之外的所有的窗体
    GW_connection->setWindowModality(Qt::ApplicationModal);
    GW_connection->resize(640, 480);
    GW_connection->show();
}

