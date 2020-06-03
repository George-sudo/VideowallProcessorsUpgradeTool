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
    GM_TreeViewModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("板卡")<<QStringLiteral("版本")<<QStringLiteral("升级状态")<<\
                                            QStringLiteral("版本")<<QStringLiteral("升级状态")<<QStringLiteral("版本")<<QStringLiteral("升级状态")<<QStringLiteral(""));
    //设置版本列宽
    ui->treeView->setColumnWidth(1,50);
    ui->treeView->setColumnWidth(3,50);
    ui->treeView->setColumnWidth(5,50);
    std::vector<QStandardItem*> inputMultipleCheckbox;
    std::vector<QStandardItem*> outputMultipleCheckbox;
    std::vector<QStandardItem*> controlMultipleCheckbox;
    std::vector<QStandardItem*> backboardMultipleCheckbox;
    for(int i=0; i<4; ++i)
    {
        switch (i) {
        case 0:
            inputMultipleCheckbox.push_back(new QStandardItem("输入卡"));
            outputMultipleCheckbox.push_back(new QStandardItem("输出卡"));
            controlMultipleCheckbox.push_back(new QStandardItem("控制卡"));
            backboardMultipleCheckbox.push_back(new QStandardItem("背板"));
            GM_TreeViewModel->setItem(0,i,inputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(1,i,outputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(2,i,controlMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(3,i,backboardMultipleCheckbox[i]);
            break;
        case 1:
            inputMultipleCheckbox.push_back(new QStandardItem("MCU固件"));
            outputMultipleCheckbox.push_back(new QStandardItem("MCU固件"));
            controlMultipleCheckbox.push_back(new QStandardItem("MCU固件"));
            backboardMultipleCheckbox.push_back(new QStandardItem("MCU固件"));
            inputMultipleCheckbox[i]->setAccessibleDescription("0InputMCU固件Box");
            outputMultipleCheckbox[i]->setAccessibleDescription("3OutputMCU固件Box");
            controlMultipleCheckbox[i]->setAccessibleDescription("6ControlMCU固件Box");
            backboardMultipleCheckbox[i]->setAccessibleDescription("9BackboardMCU固件Box");
            GM_TreeViewModel->setItem(0,i,inputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(1,i,outputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(2,i,controlMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(3,i,backboardMultipleCheckbox[i]);
            break;
        case 2:
            inputMultipleCheckbox.push_back(new QStandardItem("FPGA1固件"));
            outputMultipleCheckbox.push_back(new QStandardItem("FPGA1固件"));
            controlMultipleCheckbox.push_back(new QStandardItem("FPGA1固件"));
            backboardMultipleCheckbox.push_back(new QStandardItem("FPGA1固件"));
            inputMultipleCheckbox[i]->setAccessibleDescription("1InputFPGA1固件Box");
            outputMultipleCheckbox[i]->setAccessibleDescription("4OutputFPGA1固件Box");
            controlMultipleCheckbox[i]->setAccessibleDescription("7ControlFPGA1固件Box");
            backboardMultipleCheckbox[i]->setAccessibleDescription("aBackboardFPGA1固件Box");
            GM_TreeViewModel->setItem(0,i+1,inputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(1,i+1,outputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(2,i+1,controlMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(3,i+1,backboardMultipleCheckbox[i]);
            break;
        case 3:
            inputMultipleCheckbox.push_back(new QStandardItem("FPGA2固件"));
            outputMultipleCheckbox.push_back(new QStandardItem("FPGA2固件"));
            controlMultipleCheckbox.push_back(new QStandardItem("FPGA2固件"));
            backboardMultipleCheckbox.push_back(new QStandardItem("FPGA2固件"));
            inputMultipleCheckbox[i]->setAccessibleDescription("2InputFPGA2固件Box");
            outputMultipleCheckbox[i]->setAccessibleDescription("5OutputFPGA2固件Box");
            controlMultipleCheckbox[i]->setAccessibleDescription("8ControlFPGA2固件Box");
            backboardMultipleCheckbox[i]->setAccessibleDescription("bBackboardFPGA2固件Box");
            GM_TreeViewModel->setItem(0,i+2,inputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(1,i+2,outputMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(2,i+2,controlMultipleCheckbox[i]);
            GM_TreeViewModel->setItem(3,i+2,backboardMultipleCheckbox[i]);
            break;
        default:
            break;
        }
        inputMultipleCheckbox[i]->setCheckable(true);
        outputMultipleCheckbox[i]->setCheckable(true);
        controlMultipleCheckbox[i]->setCheckable(true);
        backboardMultipleCheckbox[i]->setCheckable(true);
        inputMultipleCheckbox[i]->setTristate(true);
        outputMultipleCheckbox[i]->setTristate(true);
        controlMultipleCheckbox[i]->setTristate(true);
        backboardMultipleCheckbox[i]->setTristate(true);
    }
    MultipleCheckbox.push_back(inputMultipleCheckbox);
    MultipleCheckbox.push_back(outputMultipleCheckbox);
    MultipleCheckbox.push_back(controlMultipleCheckbox);
    MultipleCheckbox.push_back(backboardMultipleCheckbox);

    m_InOutCardCount = 9;
    m_ControlBackCardCount = 9;
    AddLayoutItem(m_InOutCardCount,m_ControlBackCardCount);

#if 0
//此部分为进度条部分，不能单独控制某条控制进度条，暂时先关闭
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
    GM_TreeViewModel->setData(GM_TreeViewModel->index(0,0).child(2,2),80);
#endif

    //关联项目属性改变的信号和槽
    connect(GM_TreeViewModel, &QStandardItemModel::itemChanged, this, &MainWindow::treeItemChanged);
    ui->treeView->setModel(GM_TreeViewModel);

//    //unzipFile("haha");
//    //初始化视图
//    initView();
}

#if 0
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
#endif

/***增加并布局所有板卡类型展开的item控件***/
void MainWindow::AddLayoutItem(int InOutCardCount, int ControlBackCardCount)
{
    std::vector<QStandardItem*> inputItem;
    std::vector<QStandardItem*> outputItem;
    std::vector<QStandardItem*> controlItem;
    std::vector<QStandardItem*> backboardItem;
    //输入/输出卡信息控件布局
    for(int i=0; i<InOutCardCount; ++i)
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
               switch (j) {
               case 1:
                   inputItem[j]->setAccessibleDescription("InputMcuFW"+QString("%1").arg(i));
                   outputItem[j]->setAccessibleDescription("OutputMcuFW"+QString("%1").arg(i));
                   break;
               case 3:
                   inputItem[j]->setAccessibleDescription("InputFpga1FW"+QString("%1").arg(i));
                   outputItem[j]->setAccessibleDescription("OutputFpga1FW"+QString("%1").arg(i));
                   break;
               case 5:
                   inputItem[j]->setAccessibleDescription("InputFpga2FW"+QString("%1").arg(i));
                   outputItem[j]->setAccessibleDescription("OutputFpga2FW"+QString("%1").arg(i));
                   break;
               default:
                   break;
               }
           }
        }
        GI_inputChild.push_back(inputItem);
        GI_outputChild.push_back(outputItem);
    }
    for(int i=0; i<InOutCardCount; ++i)
    {
        for(int j=0; j<7; ++j)
        {
           MultipleCheckbox[0][0]->setChild(i,j,GI_inputChild[i][j]);
           MultipleCheckbox[1][0]->setChild(i,j,GI_outputChild[i][j]);
        }
    }
    //控制卡/背板信息控件布局
    for(int i=0; i<ControlBackCardCount; ++i)
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
               switch (j) {
               case 1:
                   controlItem[j]->setAccessibleDescription("ControlMcuFW"+QString("%1").arg(i));
                   backboardItem[j]->setAccessibleDescription("BackboardMcuFW"+QString("%1").arg(i));
                   break;
               case 3:
                   controlItem[j]->setAccessibleDescription("ControlFpga1FW"+QString("%1").arg(i));
                   backboardItem[j]->setAccessibleDescription("BackboardFpga1FW"+QString("%1").arg(i));
                   break;
               case 5:
                   controlItem[j]->setAccessibleDescription("ControlFpga2FW"+QString("%1").arg(i));
                   backboardItem[j]->setAccessibleDescription("BackboardFpga2FW"+QString("%1").arg(i));
                   break;
               default:
                   break;
               }
           }
        }
        GI_controlChild.push_back(controlItem);
        GI_backboardtChild.push_back(backboardItem);
    }
    for(int i=0; i<ControlBackCardCount; ++i)
    {
        for(int j=0; j<7; ++j)
        {
           MultipleCheckbox[2][0]->setChild(i,j,GI_controlChild[i][j]);
           MultipleCheckbox[3][0]->setChild(i,j,GI_backboardtChild[i][j]);
        }
    }
}

/***复选框状态改变响应的槽函数***/
void MainWindow::treeItemChanged(QStandardItem *item)
{
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
        McuFpga1Fpga2FWeBox(item);
    }
}

/***MCU、FPGA1、FPGA2固件复选框功能实现***/
void MainWindow::McuFpga1Fpga2FWeBox(QStandardItem *item)
{
    qDebug()<<item->accessibleDescription();
    if(item->accessibleDescription().size() > 0)
    {
        //全选与全不选
        char c = item->accessibleDescription().at(0).toLatin1();
        switch(c){
        case '0':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_inputChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '1':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_inputChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '2':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_inputChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '3':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_outputChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '4':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_outputChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '5':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                GI_outputChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '6':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_controlChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '7':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_controlChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '8':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_controlChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '9':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_backboardtChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case 'a':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_backboardtChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case 'b':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                GI_backboardtChild[i][5]->setCheckState(item->checkState());
            }
            break;
        default:
            break;
        }
    }

}

/***检查子节点的情况并设置状态***/
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

/***根据子节点的改变，更改父节点的选择情况***/
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

/***测量兄弟节点的情况，如果都选中返回Qt::Checked，都不选中Qt::Unchecked,不完全选中返回Qt::PartiallyChecked***/
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
