#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include "ui_mainwindow.h"
#include "connection.h"
#include "unzip_ufw.h"
#include "xmodem.h"
#include <QStandardItemModel>
//#include <QMessageBox>
#include <QFlags>
#include <QItemDelegate>
#include <QProgressBar>
#include <vector>
#include <QTcpSocket>
#include <QFileDialog>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTimer>

namespace Ui {
class MainWindow;
}
namespace PBD {
class ProgressBarDelegate;
}

//声明类，避免系统在编译的时候，先加载mainwindow.h文件后加载xmodem.h，导致找不到类Xmodem的情况
class Xmodem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initMainWindow(void);
    void ConnectView(void);
    //复选框
    void treeItem_checkAllChild(QStandardItem *item, bool check);
    void treeItem_CheckChildChanged(QStandardItem *item);
    Qt::CheckState checkSibling(QStandardItem *item);
    //增加并布局所有板卡类型展开的item控件
    void AddLayoutItem(int InOutCardCount, int ControlBackCardCount);
    //MCU、FPGA1、FPGA2固件复选框
    void McuFpga1Fpga2FWeBox(QStandardItem *item);
    void SetCardInfoToItem(QByteArray &CardInfo);
    int GetBinFileNameList(QString BinFileDirPath, QStringList &file_list);
    QString FindBinPath(QString CardType, QString  FirmwareTpye);
    void StartUpgrade(QString condition="",QString CardType="", int Row=-1, int Col=-1);
    void ScanCard();

signals:
    void FileSend(QString BinFilePath, QString CardType, int Row, int Col);
    void SendRespond(char respond);

private slots :
    void treeItemChanged( QStandardItem * item );
    void on_SelectFileBt_clicked();
    void on_upgradeBt_clicked();
    void ReceiveData();
    void ReceiveCardInfo();
    void reconnectionControlCard();
    void TcpSendDataPacket(QByteArray outBlock);
    void SetPercentageProgress(QString value,QString CardType,int Row, int Col);

public:
    QString m_BinFileDirPath;
    QString m_BinFilePath;
    QString m_JsonFilePath;
    QStringList file_list;
    uint CardTypeFlag;
    uint inrow;
    uint incol;
    uint outrow;
    uint outcol;
    uint conrow;
    uint concol;
    uint bacrow;
    uint baccol;
    static int flags;

private:
    Ui::MainWindow *ui;
    QDialog* GW_connection;// = new QDialog(this);
    Xmodem *xmodem;
    QStandardItemModel* GM_TreeViewModel;
    std::vector<std::vector<QStandardItem*>> MultipleCheckbox;
    std::vector<std::vector<QStandardItem*>> GI_inputChild;
    std::vector<std::vector<QStandardItem*>> GI_outputChild;
    std::vector<std::vector<QStandardItem*>> GI_controlChild;
    std::vector<std::vector<QStandardItem*>> GI_backboardtChild;
    int m_InOutCardCount;
    int m_ControlBackCardCount;
    QByteArray m_CardInfo;
    QTimer *timer;
};

//进度条
class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ProgressBarDelegate(QObject *parent = nullptr)
        : QItemDelegate(parent) { }

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        //计算bar在表格中的位置
        int radio = 1;
        int top = option.rect.top() + radio;
        int left = option.rect.left() + radio;
        int width = option.rect.width() - 2 * radio;
        int height = option.rect.height() - 2 * radio;

        // 创建bar
        QStyleOptionProgressBar bar;
        bar.rect.setRect(left, top, width, height); // 设置bar位置
        bar.state = QStyle::State_Enabled;
        bar.progress = index.data(Qt::EditRole).toInt(); // 设置对应model列的值
        bar.minimum = 0;
        bar.maximum = 100;
        bar.textVisible = true;
        bar.text = QString("%1%").arg(bar.progress);
        bar.textAlignment = Qt::AlignCenter;

        QProgressBar pbar;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &bar, painter, &pbar);
    }

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override
    {
        if(event->type() == QEvent::MouseButtonDblClick) // 禁止双击编辑
            return true;
        return QItemDelegate::editorEvent(event,model,option,index);
    }
};

#endif // MAINWINDOW_H
