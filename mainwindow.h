#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include "ui_mainwindow.h"
#include "connection.h"
#include "unzip_ufw.h"
#include <QStandardItemModel>
//#include <QMessageBox>
#include <QFlags>
#include <QItemDelegate>
#include <QProgressBar>
#include <vector>

namespace Ui {
class MainWindow;
}
namespace PBD {
class ProgressBarDelegate;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initMainWindow(void);
    void initView(void);
    //复选框
    void treeItem_checkAllChild(QStandardItem *item, bool check);
    void treeItem_CheckChildChanged(QStandardItem *item);
    Qt::CheckState checkSibling(QStandardItem *item);
    //增加并布局所有板卡类型展开的item控件
    void AddLayoutItem(int InOutCardCount, int ControlBackCardCount);
    //MCU、FPGA1、FPGA2固件复选框
    void McuFpga1Fpga2FWeBox(QStandardItem *item);

private slots :
    void treeItemChanged( QStandardItem * item );

public:
    static QString binfile_path;
    static QString jsonfile_path;

private:
    Ui::MainWindow *ui;
    QDialog* GW_connection;// = new QDialog(this);
    QStandardItemModel* GM_TreeViewModel;
    std::vector<std::vector<QStandardItem*>> MultipleCheckbox;
    std::vector<std::vector<QStandardItem*>> GI_inputChild;
    std::vector<std::vector<QStandardItem*>> GI_outputChild;
    std::vector<std::vector<QStandardItem*>> GI_controlChild;
    std::vector<std::vector<QStandardItem*>> GI_backboardtChild;
    int m_InOutCardCount;
    int m_ControlBackCardCount;
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
