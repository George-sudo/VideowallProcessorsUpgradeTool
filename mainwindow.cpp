#include "mainwindow.h"
#define ProgressBar 0

int MainWindow::flags = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inrow = 0;
    incol = 0;
    outrow = 0;
    outcol = 0;
    conrow = 0;
    concol = 0;
    bacrow = 0;
    baccol = 0;
    CardTypeFlag = 0;
    //20200508yu: 设置标题
    this->setWindowTitle("LED拼接处理器升级软件");
    GW_connection = new Connection(parent);
    xmodem = new Xmodem(Connection::tcpClient);
    connect(this,&MainWindow::FileSend,xmodem,&Xmodem::StartSendFile);
    connect(this,&MainWindow::SendRespond,xmodem,&Xmodem::ReceiveRespond);
    connect(Connection::tcpClient,&QTcpSocket::connected,this,&MainWindow::ScanCard);
    connect(Connection::tcpClient,&QTcpSocket::readyRead,this,&MainWindow::ReceiveData);
    timer = new QTimer(this);
    connect(MainWindow::timer, &QTimer::timeout, this, &MainWindow::ReceiveCardInfo);
    connect(xmodem,&Xmodem::SendFileFinished,this,&MainWindow::StartUpgrade);
    connect(xmodem,&Xmodem::reconnection,this,&MainWindow::reconnectionControlCard);
    connect(xmodem,&Xmodem::SendDataPacket,this,&MainWindow::TcpSendDataPacket);
    connect(xmodem,&Xmodem::PercentageProgress,this,&MainWindow::SetPercentageProgress);
    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    ui->textBrowser->setText("文件格式为.ufw");
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

    //留接口动态改变卡槽数量
    m_InOutCardCount = 9;
    m_ControlBackCardCount = 1;
    AddLayoutItem(m_InOutCardCount,m_ControlBackCardCount);

    //将板卡初始化为[空],并且将复选框设置为不能选择
    for(int i=0; i<m_InOutCardCount; ++i)
    {
        GI_inputChild[i][0]->setText("[空]");
        GI_outputChild[i][0]->setText("[空]");
        for(int j=1; j<7; j+=2)
        {
            GI_inputChild[i][j]->setCheckable(false);
            GI_outputChild[i][j]->setCheckable(false);
        }
    }
    for(int i=0; i<m_ControlBackCardCount; ++i)
    {
        GI_controlChild[i][0]->setText("[空]");
        GI_backboardtChild[i][0]->setText("[空]");
        for(int j=1; j<7; j+=2)
        {
            GI_controlChild[i][j]->setCheckable(false);
            GI_backboardtChild[i][j]->setCheckable(false);
        }
    }

#if ProgressBar
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
//    GM_TreeViewModel->setData(GM_TreeViewModel->index(0,0).child(2,2),80);
#endif

    //关联项目属性改变的信号和槽
    connect(GM_TreeViewModel, &QStandardItemModel::itemChanged, this, &MainWindow::treeItemChanged);

    ui->treeView->setModel(GM_TreeViewModel);
    ui->treeView->expandAll();//展开所有

    //网络连接窗口
    ConnectView();
}

/***连接界面***/
void MainWindow::ConnectView()
{
    //弹出子窗口
    GW_connection->setWindowTitle(tr("连接方式"));
    GW_connection->setWindowFlags(GW_connection->windowFlags()&~Qt::WindowContextHelpButtonHint);
    GW_connection->setWindowModality(Qt::ApplicationModal);
    GW_connection->resize(640, 480);
    GW_connection->show();
}

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
//    qDebug()<<item->accessibleDescription();
    if(item->accessibleDescription().size() > 0)
    {
        //全选与全不选
        char c = item->accessibleDescription().at(0).toLatin1();
        switch(c){
        case '0':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_inputChild[i][1]->isCheckable())
                    GI_inputChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '1':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_inputChild[i][3]->isCheckable())
                    GI_inputChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '2':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_inputChild[i][5]->isCheckable())
                    GI_inputChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '3':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_outputChild[i][1]->isCheckable())
                    GI_outputChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '4':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_outputChild[i][3]->isCheckable())
                    GI_outputChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '5':
            for(int i=0; i<m_InOutCardCount; ++i)
            {
                if(GI_outputChild[i][5]->isCheckable())
                    GI_outputChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '6':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_controlChild[i][1]->isCheckable())
                    GI_controlChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case '7':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_controlChild[i][3]->isCheckable())
                    GI_controlChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case '8':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_controlChild[i][5]->isCheckable())
                    GI_controlChild[i][5]->setCheckState(item->checkState());
            }
            break;
        case '9':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_backboardtChild[i][1]->isCheckable())
                    GI_backboardtChild[i][1]->setCheckState(item->checkState());
            }
            break;
        case 'a':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_backboardtChild[i][3]->isCheckable())
                    GI_backboardtChild[i][3]->setCheckState(item->checkState());
            }
            break;
        case 'b':
            for(int i=0; i<m_ControlBackCardCount; ++i)
            {
                if(GI_backboardtChild[i][5]->isCheckable())
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

void MainWindow::on_SelectFileBt_clicked()
{
    //获取文件路径（包括文件名）
    QString ufwFilePath = QFileDialog::getOpenFileName(this, tr("选择文件"),tr("*.ufw"));
    //获取文件名
    QStringList str= ufwFilePath.split("/");
    QString ufwFileName = str[str.count()-1];
    if(ufwFileName.size()>0)
    {
        ui->textBrowser->setText(ufwFileName);

        //获取压缩包所在目录
        QString ufwDir = ufwFilePath.mid(0,ufwFilePath.size()-ufwFileName.size());
        //解压ufw压缩文件
        unzipFile(ufwDir,ufwFilePath);
        //保存解压目录
        m_BinFileDirPath = ufwDir;
        //保存.bin文件名
        GetBinFileNameList(m_BinFileDirPath,file_list);

        //升级状态栏清空
        for(uint i=0; i<GI_inputChild.size(); ++i)
        {
            GI_inputChild[i][2]->setText(" ");
            GI_inputChild[i][4]->setText(" ");
            GI_inputChild[i][6]->setText(" ");
        }
        for(uint i=0; i<GI_outputChild.size(); ++i)
        {
            GI_outputChild[i][2]->setText(" ");
            GI_outputChild[i][4]->setText(" ");
            GI_outputChild[i][6]->setText(" ");
        }
        for(uint i=0; i<GI_controlChild.size(); ++i)
        {
            GI_controlChild[i][2]->setText(" ");
            GI_controlChild[i][4]->setText(" ");
            GI_controlChild[i][6]->setText(" ");
        }
        for(uint i=0; i<GI_backboardtChild.size(); ++i)
        {
            GI_backboardtChild[i][2]->setText(" ");
            GI_backboardtChild[i][4]->setText(" ");
            GI_backboardtChild[i][6]->setText(" ");
        }
    }
}

/***扫描板卡信息***/
void MainWindow::ScanCard()
{
    QByteArray m_outBlock;
    QDataStream out(&m_outBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);

    m_outBlock.resize(0);
    uchar ScanCardOrder[9] = {0x42, 0x4c, 0x13 ,0x89, 0x00, 0x01, 0x00, 0x08, 0xfe};
    out.writeRawData((char*)ScanCardOrder,9);
//    for(int i=0; i<9; ++i)
//    {
//        qDebug("%#x",m_outBlock.at(i));
//        qDebug("%#x",ScanCardOrder[i]);
//    }

    //发送扫描板卡命令
    flags = -1;//为了不让MainWindow::ReceiveData处理板卡信息
    Connection::tcpClient->write(m_outBlock);
    timer->start(1000);//等待若干秒，让控制卡把所有板卡信息返回来，再通过ReceiveCardInfo读取板卡信息

    //显示控制卡信息（暂不支持升级）
    GI_controlChild[0][0]->setText("卡槽1");
    GI_controlChild[0][3]->setText("[空]");
    GI_controlChild[0][4]->setText("暂不支持升级");
    GI_controlChild[0][5]->setText("[空]");
    GI_controlChild[0][6]->setText("暂不支持升级");
}

/***升级固件***/
void MainWindow::on_upgradeBt_clicked()
{
#if 0
    //读取Json数据
    QFile file("E:/git/update/ts-94xx_update_firmware/_metadata/packing.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    QJsonObject jsonObject = document.object();

    if(jsonObject.contains(QStringLiteral("update_firmware")))
    {
        QJsonValue Value = jsonObject.value(QStringLiteral("update_firmware"));
        QJsonObject a = Value.toObject();
        qDebug()<<a["single_board"].toObject()["TS-9436HM-PJ"].toObject()["FPGA1"].toObject()["md5"];
    }
#endif

#if 1
    QMessageBox::StandardButton MessageBox;
    MessageBox = QMessageBox::question(this, "升级提示",
        "升级后将覆盖系统当前配置，是否继续执行?",
        QMessageBox::Yes | QMessageBox::No);

    if (MessageBox == QMessageBox::Yes)
    {
        //升级状态栏清空
        for(uint i=0; i<GI_inputChild.size(); ++i)
        {
            GI_inputChild[i][2]->setText(" ");
            GI_inputChild[i][4]->setText(" ");
            GI_inputChild[i][6]->setText(" ");
        }

        //调试命令
        QByteArray m_outBlock;
        QDataStream out(&m_outBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);
        m_outBlock.resize(0);
        uchar UpgradeOrder[12] = {0x42, 0x4c, 0x13, 0x89, 0x00, 0x04, 0x00, 0x35, 0x01,0x01,0x01,0xfe};
        out.writeRawData((char*)UpgradeOrder,12);
        Connection::tcpClient->write(m_outBlock);
#if 1
        emit FileSend("E:/git/firmware/456.pdf","输入卡",0,1);
#else
        emit FileSend("E:/git/firmware/tv-9411ho-mcu-v1.0.hex","输入卡",0,1);
#endif
    }

#endif
    if(ui->textBrowser->toPlainText()=="文件格式为.ufw" || ui->textBrowser->toPlainText()=="")
    {
        QMessageBox::information(NULL,"提醒","请选择升级需要的正确固件!");
    }
    else{
        QMessageBox::StandardButton MessageBox;
        MessageBox = QMessageBox::question(this, "升级提示",
            "升级后将覆盖系统当前配置，是否继续执行?",
            QMessageBox::Yes | QMessageBox::No);

        if (MessageBox == QMessageBox::Yes)
        {
            //升级状态栏清空
            for(uint i=0; i<GI_inputChild.size(); ++i)
            {
                GI_inputChild[i][2]->setText(" ");
                GI_inputChild[i][4]->setText(" ");
                GI_inputChild[i][6]->setText(" ");
            }
            for(uint i=0; i<GI_outputChild.size(); ++i)
            {
                GI_outputChild[i][2]->setText(" ");
                GI_outputChild[i][4]->setText(" ");
                GI_outputChild[i][6]->setText(" ");
            }
            for(uint i=0; i<GI_controlChild.size(); ++i)
            {
                GI_controlChild[i][2]->setText(" ");
                GI_controlChild[i][4]->setText(" ");
                GI_controlChild[i][6]->setText(" ");
            }
            for(uint i=0; i<GI_backboardtChild.size(); ++i)
            {
                GI_backboardtChild[i][2]->setText(" ");
                GI_backboardtChild[i][4]->setText(" ");
                GI_backboardtChild[i][6]->setText(" ");
            }
            //重新赋值，
            inrow = 0;
            incol = 0;
            outrow = 0;
            outcol = 0;
            conrow = 0;
            concol = 0;
            bacrow = 0;
            baccol = 0;
            //开始升级
            StartUpgrade();
        }
    }
}

/***开始升级***/
void MainWindow::StartUpgrade(QString condition, QString CardType, int Row, int Col)
{
    QString str;

    //显示升级结果
    if(Row!=-1 && Col!=-1 )
    {
        if(CardType == "输入卡" && Row<9 && Col<6)
        {
            if(condition.mid(0,4).contains("v",Qt::CaseInsensitive))
            {
                GI_inputChild[Row][Col]->setText(condition.mid(0,4));//版本号
#if !ProgressBar
                GI_inputChild[Row][Col+1]->setText(condition.mid(4,condition.size()-4));//升级完成
                GI_inputChild[Row][Col+1]->setForeground(QBrush(QColor(0, 0, 255)));
#endif
            }
            else
            {

#if !ProgressBar
                GI_inputChild[Row][Col+1]->setText(condition);//升级失败
                GI_inputChild[Row][Col+1]->setForeground(QBrush(QColor(255, 0, 0)));
#endif
            }

            if(Col >= 5)
            {
                Col = 0;
                Row += 1;
            }
            else
                Col += 1;
            incol = Col;
            inrow = Row;
        }
        if(CardType == "输出卡" && Row<9 && Col<6)
        {
            if(condition.mid(0,4).contains("v",Qt::CaseInsensitive))
            {
                GI_outputChild[Row][Col]->setText(condition.mid(0,4));//版本号
#if !ProgressBar
                GI_outputChild[Row][Col+1]->setText(condition.mid(4,condition.size()-4));//升级完成
                GI_outputChild[Row][Col+1]->setForeground(QBrush(QColor(0, 0, 255)));
#endif
            }
            else
            {
#if !ProgressBar
                GI_outputChild[Row][Col+1]->setForeground(QBrush(QColor(255, 0, 0)));
                GI_outputChild[Row][Col+1]->setText(condition);//升级失败
#endif
            }

            if(Col >= 5)
            {
                Col = 0;
                Row += 1;
            }
            else
                Col += 1;
            outrow = Col;
            outcol = Row;
        }
        if(CardType == "控制卡" && Row<1 && Col<6)
        {
            if(condition.mid(0,4).contains("v",Qt::CaseInsensitive))
            {
                GI_controlChild[Row][Col]->setText(condition.mid(0,4));//版本号
#if !ProgressBar
                GI_controlChild[Row][Col+1]->setText(condition.mid(4,condition.size()-4));//升级完成
                GI_controlChild[Row][Col+1]->setForeground(QBrush(QColor(0, 0, 255)));
#endif
            }
            else
            {
#if !ProgressBar
                GI_controlChild[Row][Col+1]->setText(condition);//升级失败
                GI_controlChild[Row][Col+1]->setForeground(QBrush(QColor(255, 0, 0)));
#endif
            }

            if(Col >= 5)
            {
                Col = 0;
                Row += 1;
            }
            else
                Col += 1;
            conrow = Col;
            concol = Row;
        }
        if(CardType == "背板" && Row<1 && Col<6)
        {
            if(condition.mid(0,4).contains("v",Qt::CaseInsensitive))
            {
                GI_backboardtChild[Row][Col]->setText(condition.mid(0,4));//版本号
#if !ProgressBar
                GI_backboardtChild[Row][Col+1]->setText(condition.mid(4,condition.size()-4));//升级完成
                GI_backboardtChild[Row][Col+1]->setForeground(QBrush(QColor(0, 0, 255)));
#endif
            }
            else
            {
#if !ProgressBar
                GI_backboardtChild[Row][Col+1]->setText(condition);//升级失败
                GI_backboardtChild[Row][Col+1]->setForeground(QBrush(QColor(255, 0, 0)));
#endif
            }

            if(Col >= 5)
            {
                Col = 0;
                Row += 1;
            }
            else
                Col += 1;
            bacrow = Col;
            baccol = Row;
        }
    }

    //查找输入卡需要升级的固件
    for(; inrow<GI_inputChild.size(); ++inrow)
    {
        for(; incol<GI_inputChild[0].size(); ++incol)
        {
           if(GI_inputChild[inrow][incol]->checkState() == Qt::Checked)
           {
               QByteArray m_outBlock;
               QDataStream out(&m_outBlock, QIODevice::WriteOnly);
               out.setVersion(QDataStream::Qt_5_6);
               //升级固件
               m_outBlock.resize(0);
               uchar UpgradeOrder[12] = {0x42, 0x4c, 0x13, 0x89, 0x00, 0x04, 0x00, 0x35, 0x00};
               //判断卡槽
               switch (inrow+1) {
               case 1:
                   UpgradeOrder[9] = 0x01;
                   break;
               case 2:
                   UpgradeOrder[9] = 0x05;
                   break;
               case 3:
                   UpgradeOrder[9] = 0x09;
                   break;
               case 4:
                   UpgradeOrder[9] = 0x13;
                   break;
               case 5:
                   UpgradeOrder[9] = 0x17;
                   break;
               case 6:
                   UpgradeOrder[9] = 0x21;
                   break;
               case 7:
                   UpgradeOrder[9] = 0x25;
                   break;
               case 8:
                   UpgradeOrder[9] = 0x29;
                   break;
               case 9:
                   UpgradeOrder[9] = 0x33;
                   break;
               default:
                   break;
               }
               //判断固件升级类型
               switch (incol){
               case 1://mcu
                   UpgradeOrder[10] = 0x01;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("hi","mcu");
                   break;
               case 3://fpga1
                   UpgradeOrder[10] = 0x02;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("hi","fpga1");
                   break;
               case 5://fpga2
                   UpgradeOrder[10] = 0x03;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("hi","fpga2");
                   break;
               default:
                   break;
               }
               UpgradeOrder[11] = 0xfe;
               out.writeRawData((char*)UpgradeOrder,12);
               if(str != "not exist")
               {
                   //发送板卡升级命令
                   Connection::tcpClient->write(m_outBlock);
                   emit FileSend(str,"输入卡",inrow,incol);
                   GI_inputChild[inrow][incol+1]->setText("正在升级...");
                   return;
               }
               else
                   qDebug()<<"没有此固件类型";
           }
           if(incol == 6)
           {
               incol = 0;
               break;
           }
        }
    }

#if 1
    //查找输出卡需要升级的固件
    for(; outrow<GI_outputChild.size(); ++outrow)
    {
        for(; outcol<GI_outputChild[0].size(); ++outcol)
        {
           if(GI_outputChild[outrow][outcol]->checkState() == Qt::Checked)
           {
               QByteArray m_outBlock;
               QDataStream out(&m_outBlock, QIODevice::WriteOnly);
               out.setVersion(QDataStream::Qt_5_6);
               //升级固件
               m_outBlock.resize(0);
               uchar UpgradeOrder[12] = {0x42, 0x4c, 0x13, 0x89, 0x00, 0x04, 0x00, 0x35, 0x01};
               //判断卡槽
               switch (outrow+1) {
               case 1:
                   UpgradeOrder[9] = 0x01;
                   break;
               case 2:
                   UpgradeOrder[9] = 0x05;
                   break;
               case 3:
                   UpgradeOrder[9] = 0x09;
                   break;
               case 4:
                   UpgradeOrder[9] = 0x13;
                   break;
               case 5:
                   UpgradeOrder[9] = 0x17;
                   break;
               case 6:
                   UpgradeOrder[9] = 0x21;
                   break;
               case 7:
                   UpgradeOrder[9] = 0x25;
                   break;
               case 8:
                   UpgradeOrder[9] = 0x29;
                   break;
               case 9:
                   UpgradeOrder[9] = 0x33;
                   break;
               default:
                   break;
               }
               //判断固件升级类型
               switch (outcol){
               case 1://mcu
                   UpgradeOrder[10] = 0x01;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("ho","mcu");
                   break;
               case 3://fpga1
                   UpgradeOrder[10] = 0x02;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("ho","fpga1");
                   break;
               case 5://fpga2
                   UpgradeOrder[10] = 0x03;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("ho","fpga2");
                   break;
               default:
                   break;
               }
               UpgradeOrder[11] = 0xfe;
               out.writeRawData((char*)UpgradeOrder,12);
               if(str != "not exist")
               {
                   //发送板卡升级命令
                   Connection::tcpClient->write(m_outBlock);
                   emit FileSend(str,"输出卡",outrow,outcol);
                   GI_outputChild[outrow][outcol+1]->setText("正在升级...");
                   return;
               }
               else
                   qDebug()<<"没有此固件类型";
           }
           if(outcol == 6)
           {
               outcol = 0;
               break;
           }
        }
    }

    //查找控制卡需要升级的固件
    for(; conrow<GI_controlChild.size(); ++conrow)
    {
        for(; concol<GI_controlChild[0].size(); ++concol)
        {
           if(GI_controlChild[conrow][concol]->checkState() == Qt::Checked)
           {
               QByteArray m_outBlock;
               QDataStream out(&m_outBlock, QIODevice::WriteOnly);
               out.setVersion(QDataStream::Qt_5_6);
               //升级固件
               m_outBlock.resize(0);
               uchar UpgradeOrder[12] = {0x42, 0x4c, 0x13, 0x89, 0x00, 0x04, 0x00, 0x35, 0x01, 0x00};

               //判断固件升级类型
               switch (concol){
               case 1://mcu
                   UpgradeOrder[10] = 0x01;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("con","mcu");
                   break;
               case 3://fpga1
                   UpgradeOrder[10] = 0x02;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("con","fpga1");
                   break;
               case 5://fpga2
                   UpgradeOrder[10] = 0x03;
                   //查找升级的固件路径（带固件名）
                   str = FindBinPath("con","fpga2");
                   break;
               default:
                   break;
               }
               UpgradeOrder[11] = 0xfe;
               out.writeRawData((char*)UpgradeOrder,12);
               if(str != "not exist")
               {
                   //发送板卡升级命令
                   Connection::tcpClient->write(m_outBlock);
                   emit FileSend(str,"控制卡",conrow,concol);
                   GI_controlChild[conrow][concol+1]->setText("正在升级...");
                   return;
               }
               else
                   qDebug()<<"没有此固件类型";
           }
           if(concol == 6)
           {
               concol = 0;
               break;
           }
        }
    }
#endif
    //查找背卡需要升级的固件（暂时没有）

    //升级完成
    QMessageBox::information(NULL, "提醒", "固件升级完成，重启设备后生效");
}

/***查找升级的固件路径（带固件名）***/
QString MainWindow::FindBinPath(QString CardType, QString  FirmwareTpye)
{
    QString BinPath;
    for(int i=0; i<file_list.size(); ++i)
    {
        if(file_list[i].contains(CardType,Qt::CaseInsensitive) && file_list[i].contains(FirmwareTpye,Qt::CaseInsensitive))
        {
            BinPath = m_BinFileDirPath + file_list[i];
            return BinPath;
        }
    }

    BinPath =  "not exist";
    return BinPath;
}

/***获取.bin固件名***/
int MainWindow::GetBinFileNameList(QString BinFileDirPath, QStringList &file_list)
{
    QDir dir(BinFileDirPath);
    QStringList nameFilters;

    if (dir.exists())   //判断目录是否存在
    {
        nameFilters << "*.bin";
        file_list = dir.entryList(nameFilters,QDir::Files | QDir::NoDotAndDotDot);
    }
    else{
        file_list.clear();
        qDebug() << "该目录不存在！！！";
        return -1;
    }
    return 0;
}

/***处理数据***/
void MainWindow::ReceiveData()
{
//    qDebug()<<"$$$$$$$$$$$$$";
//    qDebug()<<flags;

#if 0
    //连接成功后第一次返回"Connect Success!\r\n"字符串，读取数据后发送扫描板卡命令
    if(flags == 0)
    {
        QByteArray FirstData = Connection::tcpClient->readAll();
        qDebug()<<"FirstData:"<<FirstData;
        if(FirstData.toStdString()=="Connect Success!\r\n")
        {
            //显示控制卡信息（暂不支持升级）
            GI_controlChild[0][0]->setText("卡槽1");
            GI_controlChild[0][3]->setText("[空]");
            GI_controlChild[0][4]->setText("暂不支持升级");
            GI_controlChild[0][5]->setText("[空]");
            GI_controlChild[0][6]->setText("暂不支持升级");
            //扫描板卡
            ScanCard();
            timer->start(1000);//等待控制卡把所有板卡信息返回来，再通过ReceiveCardInfo读取板卡信息
            flags = -1;//过滤完成，设置标志位为-1
        }
    }

    //板卡重启之后，再次连接时，过滤"Connect Success!\r\n"字符串，此时不扫描板卡信息
    if(flags == 1)
    {
        QByteArray FirstLaterData = Connection::tcpClient->readAll();
        if(FirstLaterData.toStdString()=="Connect Success!\r\n")
        {
            flags = -1;//过滤完成，设置标志位为-1
        }
    }
#endif

    if(flags == 2)//接收回应
    {
        QByteArray tem = Connection::tcpClient->readAll();
        qDebug()<<"ReceiveDataSize:"<<tem.size();
        for(int i=0; i<tem.size();++i)
            qDebug("ReceiveData[%d]:%#x",i,tem.at(i));
        if(tem.size() == 1)
            emit SendRespond(tem.at(0));
        else
            emit SendRespond(tem.at(tem.size()-1));
    }
}

/***接收板卡信息***/
void MainWindow::ReceiveCardInfo()
{
    timer->stop();//停止定时器
    m_CardInfo.resize(0);
    m_CardInfo = Connection::tcpClient->readAll();
//    for(int i = 0; i<m_CardInfo.size(); ++i)
//    {
//        qDebug("%#x",m_CardInfo.at(i));
//    }
    SetCardInfoToItem(m_CardInfo);
}

/***重新连接控制卡***/
void MainWindow::reconnectionControlCard()
{
    Connection::tcpClient->disconnectFromHost();
    Connection::tcpClient->close();
    Connection::tcpClient->connectToHost(Connection::ip,Connection::port);

    if(!Connection::tcpClient->waitForConnected(1000))
    {
        QMessageBox::information(NULL, "提醒", "连接失败");
    }
    else
    {
        QMessageBox::information(NULL, "提醒", "连接成功");
    }
}

void MainWindow::TcpSendDataPacket(QByteArray outBlock)
{
    Connection::tcpClient->write(outBlock);
}

void MainWindow::SetPercentageProgress(QString value, QString CardType, int Row, int Col)
{
#if ProgressBar
    //进度条显示
    if(CardType == "输入卡")
    {
        if(value == "100")
        {
            GM_TreeViewModel->setData(GM_TreeViewModel->index(0,0).child(Row,Col+1),value.toInt());
        }
        else
        {
            GM_TreeViewModel->setData(GM_TreeViewModel->index(0,0).child(Row,Col+1),value.mid(0,value.indexOf('.')).toInt());
        }
    }
    if(CardType == "输出卡")
    {
        if(value == "100")
        {
            GM_TreeViewModel->setData(GM_TreeViewModel->index(1,0).child(Row,Col+1),value.toInt());
        }
        else
        {
            GM_TreeViewModel->setData(GM_TreeViewModel->index(1,0).child(Row,Col+1),value.mid(0,value.indexOf('.')).toInt());
        }
    }
    if(CardType == "控制卡")
    {
        if(value == "100")
        {
           GM_TreeViewModel->setData(GM_TreeViewModel->index(2,0).child(Row,Col+1),value.toInt());
        }
        else
        {
            GM_TreeViewModel->setData(GM_TreeViewModel->index(2,0).child(Row,Col+1),value.mid(0,value.indexOf('.')).toInt());
        }
    }
    //背板暂时没有
#else
    //文字和数字显示进度
    if(CardType == "输入卡")
    {
        if(value == "100")
        {
            GI_inputChild[Row][Col+1]->setText("升级完成");
        }
        else
        {
            GI_inputChild[Row][Col+1]->setText("正在升级...\n"+value.mid(0,value.indexOf('.'))+"%");
            GI_inputChild[Row][Col+1]->setForeground(QBrush(QColor(0, 220, 0)));
        }
    }
    if(CardType == "输出卡")
    {
        if(value == "100")
        {
            GI_outputChild[Row][Col+1]->setText("升级完成");
        }
        else
        {
            GI_outputChild[Row][Col+1]->setText("正在升级...\n"+value.mid(0,value.indexOf('.'))+"%");
            GI_outputChild[Row][Col+1]->setForeground(QBrush(QColor(0, 220, 0)));
        }
    }
    if(CardType == "控制卡")
    {
        if(value == "100")
        {
            GI_controlChild[Row][Col+1]->setText("升级完成");
        }
        else
        {
            GI_controlChild[Row][Col+1]->setText("正在升级...\n"+value.mid(0,value.indexOf('.'))+"%");
            GI_controlChild[Row][Col+1]->setForeground(QBrush(QColor(0, 220, 0)));
        }
    }
    //背板暂时没有
#endif
}

/***设置界面板卡信息***/
void MainWindow::SetCardInfoToItem(QByteArray &CardInfo)
{
    QByteArray ary;
    for(int i=0; i<CardInfo.size()/12; ++i)
    {
//        qDebug()<<"oneCardData";
        ary.resize(0);
        ary = CardInfo.mid(i*12,12);
//        for(int j = 0; j<ary.size(); ++j)
//        {
//            qDebug("%#x",ary.at(j));
//        }

        //背板卡（暂时没有）

        //控制板卡
        if(ary.at(1) == 0)
        {
            GI_controlChild[0][0]->setText("卡槽1");
            GI_controlChild[0][1]->setCheckable(true);
            GI_controlChild[0][3]->setCheckable(true);
            GI_controlChild[0][5]->setCheckable(true);
            //版本号
            GI_controlChild[0][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
            GI_controlChild[0][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
            GI_controlChild[0][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
        }
        //输入卡
        if(ary.at(0) == 0x00)
        {
            switch (ary.at(1)) {
            case 1://卡槽1
                GI_inputChild[0][0]->setText("卡槽1");
                GI_inputChild[0][1]->setCheckable(true);
                GI_inputChild[0][3]->setCheckable(true);
                GI_inputChild[0][5]->setCheckable(true);
                //版本号
                GI_inputChild[0][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[0][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[0][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 5://卡槽2
                GI_inputChild[1][0]->setText("卡槽2");
                GI_inputChild[1][1]->setCheckable(true);
                GI_inputChild[1][3]->setCheckable(true);
                GI_inputChild[1][5]->setCheckable(true);
                //版本号
                GI_inputChild[1][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[1][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[1][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 9://卡槽3
                GI_inputChild[2][0]->setText("卡槽3");
                GI_inputChild[2][1]->setCheckable(true);
                GI_inputChild[2][3]->setCheckable(true);
                GI_inputChild[2][5]->setCheckable(true);
                //版本号
                GI_inputChild[2][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[2][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[2][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 13://卡槽4
                GI_inputChild[3][0]->setText("卡槽4");
                GI_inputChild[3][1]->setCheckable(true);
                GI_inputChild[3][3]->setCheckable(true);
                GI_inputChild[3][5]->setCheckable(true);
                //版本号
                GI_inputChild[3][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[3][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[3][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 17://卡槽5
                GI_inputChild[4][0]->setText("卡槽5");
                GI_inputChild[4][1]->setCheckable(true);
                GI_inputChild[4][3]->setCheckable(true);
                GI_inputChild[4][5]->setCheckable(true);
                //版本号
                GI_inputChild[4][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[4][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[4][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 21://卡槽6
                GI_inputChild[5][0]->setText("卡槽6");
                GI_inputChild[5][1]->setCheckable(true);
                GI_inputChild[5][3]->setCheckable(true);
                GI_inputChild[5][5]->setCheckable(true);
                //版本号
                GI_inputChild[5][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[5][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[5][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 25://卡槽7
                GI_inputChild[6][0]->setText("卡槽7");
                GI_inputChild[6][1]->setCheckable(true);
                GI_inputChild[6][3]->setCheckable(true);
                GI_inputChild[6][5]->setCheckable(true);
                //版本号
                GI_inputChild[6][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[6][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[6][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 29://卡槽8
                GI_inputChild[7][0]->setText("卡槽8");
                GI_inputChild[7][1]->setCheckable(true);
                GI_inputChild[7][3]->setCheckable(true);
                GI_inputChild[7][5]->setCheckable(true);
                //版本号
                GI_inputChild[7][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[7][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[7][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 33://卡槽9
                GI_inputChild[8][0]->setText("卡槽9");
                GI_inputChild[8][1]->setCheckable(true);
                GI_inputChild[8][3]->setCheckable(true);
                GI_inputChild[8][5]->setCheckable(true);
                //版本号
                GI_inputChild[8][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_inputChild[8][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_inputChild[8][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            default:
                break;
            }
        }
        //输出卡
        if(ary.at(0) == 0x01)
        {
            switch (ary.at(1)) {
            case 0x01://卡槽1
                GI_outputChild[0][0]->setText("卡槽1");
                GI_outputChild[0][1]->setCheckable(true);
                GI_outputChild[0][3]->setCheckable(true);
                GI_outputChild[0][5]->setCheckable(true);
                //版本号
                GI_outputChild[0][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[0][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[0][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x05://卡槽2
                GI_outputChild[1][0]->setText("卡槽2");
                GI_outputChild[1][1]->setCheckable(true);
                GI_outputChild[1][3]->setCheckable(true);
                GI_outputChild[1][5]->setCheckable(true);
                //版本号
                GI_outputChild[1][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[1][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[1][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x09://卡槽3
                GI_outputChild[2][0]->setText("卡槽3");
                GI_outputChild[2][1]->setCheckable(true);
                GI_outputChild[2][3]->setCheckable(true);
                GI_outputChild[2][5]->setCheckable(true);
                //版本号
                GI_outputChild[2][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[2][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[2][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x13://卡槽4
                GI_outputChild[3][0]->setText("卡槽4");
                GI_outputChild[3][1]->setCheckable(true);
                GI_outputChild[3][3]->setCheckable(true);
                GI_outputChild[3][5]->setCheckable(true);
                //版本号
                GI_outputChild[3][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[3][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[3][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x17://卡槽5
                GI_outputChild[4][0]->setText("卡槽5");
                GI_outputChild[4][1]->setCheckable(true);
                GI_outputChild[4][3]->setCheckable(true);
                GI_outputChild[4][5]->setCheckable(true);
                //版本号
                GI_outputChild[4][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[4][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[4][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x21://卡槽6
                GI_outputChild[5][0]->setText("卡槽6");
                GI_outputChild[5][1]->setCheckable(true);
                GI_outputChild[5][3]->setCheckable(true);
                GI_outputChild[5][5]->setCheckable(true);
                //版本号
                GI_outputChild[5][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[5][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[5][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x25://卡槽7
                GI_outputChild[6][0]->setText("卡槽7");
                GI_outputChild[6][1]->setCheckable(true);
                GI_outputChild[6][3]->setCheckable(true);
                GI_outputChild[6][5]->setCheckable(true);
                //版本号
                GI_outputChild[6][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[6][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[6][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x29://卡槽8
                GI_outputChild[7][0]->setText("卡槽8");
                GI_outputChild[7][1]->setCheckable(true);
                GI_outputChild[7][3]->setCheckable(true);
                GI_outputChild[7][5]->setCheckable(true);
                //版本号
                GI_outputChild[7][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[7][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[7][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            case 0x33://卡槽9
                GI_outputChild[8][0]->setText("卡槽9");
                GI_outputChild[8][1]->setCheckable(true);
                GI_outputChild[8][3]->setCheckable(true);
                GI_outputChild[8][5]->setCheckable(true);
                //版本号
                GI_outputChild[8][1]->setText(QString("V%1.%2").arg(ary.at(9)>>4).arg(ary.at(9)&0x0f));
                GI_outputChild[8][3]->setText(QString("V%1.%2").arg(ary.at(10)>>4).arg(ary.at(10)&0x0f));
                GI_outputChild[8][5]->setText(QString("V%1.%2").arg(ary.at(11)>>4).arg(ary.at(11)&0x0f));
                break;
            default:
                break;
            }
        }
    }
}
