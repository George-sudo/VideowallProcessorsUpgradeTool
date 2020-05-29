#include "unzip_ufw.h"

//20200511yu: 直接拷贝自upgradeTool工程
//20200425yu: 测试了下解压缩,ok
//JlCompress::extractDir("D:/Application/Qt/gujiambao.ufw", "D:/Application/Qt/baojieya1");
/*
 * 需要两个参数，一个是 压缩包所在的路径 由用户选择 存储在主程序
 * 一个是 解压到的路径 软件运行所在路径。 由软件读取 最好可以在升级完成和退出软件时清除掉
 */
//返回状态 0 解压成功  1 文件未找到  2 文件不是升级固件包  3 资源占用
int unzipFile(QString file_path) {
    QDir work_directy = QDir::currentPath();
    qDebug()<<work_directy.path()<<file_path<<endl;
    //JlCompress::extractDir("E:/Application/Qt/gujiambao.ufw", "E:/Application/Qt/baojieya1");
    return 0;
}
