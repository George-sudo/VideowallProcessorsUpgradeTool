#include "mainwindow.h"

//文件路径
QString MainWindow::binfile_path = "";
QString MainWindow::jsonfile_path = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //20200508yu: 设置标题
    this->setWindowTitle("LED拼接处理器升级软件");
    GW_connection = new Connection(parent);

    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    GM_TreeViewModel = new QStandardItemModel(ui->treeView);
    GM_TreeViewModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("板卡")<<QStringLiteral("\n版本")<<QStringLiteral("MCU固件\n升级状态")<<\
                                            QStringLiteral("\n版本")<<QStringLiteral("FPGA1固件\n升级状态")<<QStringLiteral("\n版本")<<QStringLiteral("FPGA2固件\n升级状态")<<QStringLiteral(""));
    //设置版本列宽
    ui->treeView->setColumnWidth(1,50);
    ui->treeView->setColumnWidth(3,50);
    ui->treeView->setColumnWidth(5,50);

    GI_input = new QStandardItem("输入卡");
    GI_output = new QStandardItem("输出卡");
    GI_control = new QStandardItem("控制卡");
    GI_backboard = new QStandardItem("背板");

    GI_input->setCheckable(true);
    GI_output->setCheckable(true);
    GI_control->setCheckable(true);
    GI_backboard->setCheckable(true);
    GI_input->setTristate(true);
    GI_output->setTristate(true);
    GI_control->setTristate(true);
    GI_backboard->setTristate(true);

    GM_TreeViewModel->appendRow(GI_input);
    GM_TreeViewModel->appendRow(GI_output);
    GM_TreeViewModel->appendRow(GI_control);
    GM_TreeViewModel->appendRow(GI_backboard);

    /*****布局所有卡信息item控件*******/
    std::vector<QStandardItem*> inputItem;
    std::vector<QStandardItem*> outputItem;
    std::vector<QStandardItem*> controlItem;
    std::vector<QStandardItem*> backboardItem;
    //输入/输出卡信息控件布局
    for(int i=0; i<9; ++i)
    {
        inputItem.clear();
        outputItem.clear();
        for(int j=0; j<7; ++j)
        {
           inputItem.push_back(new QStandardItem());
           outputItem.push_back(new QStandardItem());
           if(1==j || 3==j || 5==j)
           {
               inputItem[j]->setCheckable(true);
               outputItem[j]->setCheckable(true);
           }
        }
        GI_inputChild.push_back(inputItem);
        GI_outputChild.push_back(outputItem);
    }
    for(int i=0; i<9; ++i)
    {
        for(int j=0; j<7; ++j)
        {
           GI_input->setChild(i,j,GI_inputChild[i][j]);
           GI_output->setChild(i,j,GI_outputChild[i][j]);
        }
    }
    //控制卡/背板信息控件布局
    for(int i=0; i<1; ++i)
    {
        controlItem.clear();
        backboardItem.clear();
        for(int j=0; j<7; ++j)
        {
           controlItem.push_back(new QStandardItem());
           backboardItem.push_back(new QStandardItem());
           if(1==j || 3==j || 5==j)
           {
               controlItem[j]->setCheckable(true);
               backboardItem[j]->setCheckable(true);
           }
        }
        GI_controlChild.push_back(controlItem);
        GI_backboardtChild.push_back(backboardItem);
    }
    for(int i=0; i<1; ++i)
    {
        for(int j=0; j<7; ++j)
        {
           GI_control->setChild(i,j,GI_controlChild[i][j]);
           GI_backboard->setChild(i,j,GI_backboardtChild[i][j]);
        }
    }

//    GI_inputSlot2McuVersion->setAccessibleDescription("hello");
////    GI_inputSlot2->setText("[空]");


/*此部分为进度条部分，不能单独控制某条控制进度条，暂时先关闭
    ProgressBarDelegate* progressBar = new ProgressBarDelegate(ui->treeView);
    ui->treeView->setItemDelegateForColumn(2, progressBar);
    ui->treeView->setItemDelegateForColumn(4, progressBar);
    ui->treeView->setItemDelegateForColumn(6, progressBar);

    static const QString ProgressBar_QssStr =  "QProgressBar{"
                                               "    border:none;"
                                               "    background:rgba(210, 225, 240,255);"
                                               "    border-radius:4px;"
                                               "    text-align:center;"
                                               "}"
                                               "QProgressBar::chunk {"
                                               "    background:rgba(60, 140, 220,255);"
                                               "    border-radius:4px;"
                                               "}";
    qApp->setStyleSheet(ProgressBar_QssStr);
    GM_TreeViewModel->setData(GM_TreeViewModel->index(0,0).child(2,1),80);
*/

    //关联项目属性改变的信号和槽
    connect(GM_TreeViewModel, &QStandardItemModel::itemChanged, this, &MainWindow::treeItemChanged);
    ui->treeView->setModel(GM_TreeViewModel);

//    //unzipFile("haha");
//    //初始化视图
//    initView();
}

void MainWindow::initView()
{
    GM_TreeViewModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("板卡")<<QStringLiteral("\n版本")<<QStringLiteral("MCU固件\n升级状态")<<QStringLiteral("\n版本")<<QStringLiteral("FPGA1固件\n升级状态")<<QStringLiteral("\n版本")<<QStringLiteral("FPGA2固件\n升级状态"));
    ui->treeView->setColumnWidth(0, 240);//设置宽度
    //填充
    GM_TreeViewModel->appendRow(GI_input);
    GM_TreeViewModel->appendRow(GI_output);
    GM_TreeViewModel->appendRow(GI_control);
    GM_TreeViewModel->appendRow(GI_backboard);
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

//复选框状态改变响应的槽函数
void MainWindow::treeItemChanged(QStandardItem *item)
{
//    qDebug()<<"test";
//    GI_inputChild[0][0]->setText("[空]");
//    GI_inputChild[0][1]->setCheckable(false);

    if(item->accessibleDescription() == "hello")
        if(item->checkState())
            qDebug()<<"hahahah";
    if (item == nullptr)
        return;
    if (item->isCheckable())
    {
        //如果条目是存在复选框的，那么就进行下面的操作
        Qt::CheckState state = item->checkState(); //获取当前的选择状态
        if (item->isTristate())
        {
             //如果条目是三态的，说明可以对子目录进行全选和全不选的设置
            if (state != Qt::PartiallyChecked)
            {
                //当前是选中状态，需要对其子项目进行全选
                treeItem_checkAllChild(item, state==Qt::Checked ? true : false);
            }
        }
        else
        {
            //说明是两态的，两态会对父级的三态有影响
            //判断兄弟节点的情况
            treeItem_CheckChildChanged(item);
        }
    }
}

//检查子节点的情况并设置状态
void MainWindow::treeItem_checkAllChild(QStandardItem *item, bool check)
{
    if(item == nullptr)
        return;
    int rowCount = item->rowCount();
    int columnCount = item->columnCount();
    for(int i=0; i<rowCount; ++i)
    {
        for(int j=0;j<columnCount;++j)
        {
            QStandardItem* childItems = item->child(i,j);

            if(childItems->isCheckable())
                childItems->setCheckState(check ? Qt::Checked : Qt::Unchecked);
        }
    }
    if(item->isCheckable())
        item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

///根据子节点的改变，更改父节点的选择情况
void MainWindow::treeItem_CheckChildChanged(QStandardItem * item)
{
    if(nullptr == item)
        return;
    Qt::CheckState siblingState = checkSibling(item);
    QStandardItem * parentItem = item->parent();
    if(nullptr == parentItem)
        return;
    if(Qt::PartiallyChecked == siblingState)
    {
        if(parentItem->isCheckable() && parentItem->isTristate())
            parentItem->setCheckState(Qt::PartiallyChecked);
    }
    else if(Qt::Checked == siblingState)
    {
        if(parentItem->isCheckable())
            parentItem->setCheckState(Qt::Checked);
    }
    else
    {
        if(parentItem->isCheckable())
            parentItem->setCheckState(Qt::Unchecked);
    }
    treeItem_CheckChildChanged(parentItem);
}

//测量兄弟节点的情况，如果都选中返回Qt::Checked，都不选中Qt::Unchecked,不完全选中返回Qt::PartiallyChecked
Qt::CheckState MainWindow::checkSibling(QStandardItem * item)
{
    //先通过父节点获取兄弟节点
    QStandardItem * parent = item->parent();
    if(nullptr == parent)
        return item->checkState();
    int brotherRowCount = parent->rowCount();
    int brotherColumnCount = parent->columnCount();
    int checkedCount(0),unCheckedCount(0);
    Qt::CheckState state;
    for(int i=0;i<brotherRowCount;++i)
    {
        for(int j=0; j<brotherColumnCount; ++j)
        {
            QStandardItem* siblingItem = parent->child(i,j);
            if(siblingItem->isCheckable())
            {
                state = siblingItem->checkState();
                if(Qt::PartiallyChecked == state)
                    return Qt::PartiallyChecked;
                else if(Qt::Unchecked == state)
                    ++unCheckedCount;
                else
                    ++checkedCount;
                if(checkedCount>0 && unCheckedCount>0)
                    return Qt::PartiallyChecked;
            }

        }
    }
    if(unCheckedCount>0)
        return Qt::Unchecked;
    return Qt::Checked;
}
