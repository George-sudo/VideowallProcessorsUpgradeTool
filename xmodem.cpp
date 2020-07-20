#include "xmodem.h"

Xmodem::Xmodem(QTcpSocket *tcpClient):m_tcpClient(tcpClient)
{

}

quint16 Xmodem::crc16_ccitt(const char *buf, int len)
{
    register int counter;
    register unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}

bool Xmodem::SendFile(QString FilePath, QString CardType)
{
    char rxChar = 0;
    char PacketBuf[PKTSIZE] = {0};
    uchar CompanyCheckCode[2] = {0x42, 0x4c};
    uchar ProjectCode[2] = {0x13, 0x89};
    uchar ExtensionHeadLen = 0x00;
    uchar code = 0x5c;
    uchar end = 0xfe;
    const char xmdm_stx = XMDM_STX;
    uchar seq = 1;
    char useq = ~seq;
    char crcL = 0;
    char crcH = 0;
    QFile file(FilePath);
    quint16 crc;
    quint32 bytesSent = 0;
    quint64 FlieSize;
    quint64 DataReadSize;
    quint16 SendSize = PKTSIZE+6;
    uchar SendSizeH = SendSize>>8;
    uchar SendSizeL = SendSize&0x00ff;
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"Error!文件打开失败！";
        return false;
    }
    FlieSize = file.size();

    MainWindow::flags = 2;

#if 1
    //如果是控制卡升级，则等待板卡重启后，重连再发送
    if("控制卡" == CardType)
    {
        sleep(10);
        qDebug()<<"等待控制重启";
        //发送信号给MainWindow对象重新连接控制卡
        emit reconnection();
        sleep(1);
    }
#endif

#if 0
    //获取0x35,往控制卡发送即将发送包的次数
    rxChar = 0;
    GetChar(0x35,&rxChar,3000);
    if(rxChar != 0x35)
    {
        qDebug()<<"Error!未接受到应答信号0x35！";
        return false;
    }
    else
    {
        int count=0;
        if(FlieSize%PKTSIZE>0)
            count = FlieSize/PKTSIZE+1;
        else
            count = FlieSize/PKTSIZE;
        uchar FileSizeBuf[2] = {0};
        FileSizeBuf[0] = count>>8;
        FileSizeBuf[1] = count&0x00ff;
        QByteArray outBlock;
        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);
        outBlock.resize(0);
        out.writeRawData((char*)FileSizeBuf,2);
        qDebug("SendTheNumber:");
        qDebug("%#x",FileSizeBuf[0]);
        qDebug("%#x",FileSizeBuf[1]);
        emit SendDataPacket(outBlock);
    }
#endif

    //获取“C”
    rxChar = 0;
    qDebug(".......m_rxchar.......:%#x",m_rxchar);
    GetChar(0x43,&rxChar,5000);
    qDebug("11111111111111m_rxchar.......:%#x",m_rxchar);
    qDebug("****rxChar*****:%#x",rxChar);
    m_rxchar = 0;
    if(rxChar != XMDM_CRC)
    {
        qDebug()<<"Error!未接受到应答信号XMDM_CRC！";
        return false;
    }

    while(bytesSent < FlieSize)
    {
        QByteArray outBlock;
        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);

        memset(PacketBuf,0,PKTSIZE);
        outBlock.resize(0);
        DataReadSize = file.read(PacketBuf,PKTSIZE);
        if(DataReadSize == PKTSIZE)
        {
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            crcH = crc>>8;
            crcL = crc&0x00ff;
            useq = ~seq;
            out.writeRawData((char *)CompanyCheckCode,2);
            out.writeRawData((char *)ProjectCode,2);
            out.writeRawData((char *)&SendSizeH,1);
            out.writeRawData((char *)&SendSizeL,1);
            out.writeRawData((char *)&ExtensionHeadLen,1);
            out.writeRawData((char *)&code,1);
            out.writeRawData(&xmdm_stx,1);
            out.writeRawData((char *)&seq,1);
            out.writeRawData(&useq,1);
            out.writeRawData(PacketBuf,PKTSIZE);
            out.writeRawData(&crcH,1);
            out.writeRawData(&crcL,1);
            out.writeRawData((char *)&end,1);

        }else if(DataReadSize < PKTSIZE){
            for(quint64 i=PKTSIZE-1; i>=DataReadSize; --i)
            {
                PacketBuf[i] = XMDM_CTRLZ;
            }
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            crcH = crc>>8;
            crcL = crc&0x00ff;
            useq = ~seq;
//            out.writeRawData((char *)CompanyCheckCode,2);
//            out.writeRawData((char *)ProjectCode,2);
//            out.writeRawData((char *)&SendSizeH,1);
//            out.writeRawData((char *)&SendSizeL,1);
//            out.writeRawData((char *)&ExtensionHeadLen,1);
//            out.writeRawData((char *)&code,1);
            out.writeRawData(&xmdm_stx,1);
            out.writeRawData((char *)&seq,1);
            out.writeRawData(&useq,1);
            out.writeRawData(PacketBuf,PKTSIZE);
            out.writeRawData(&crcH,1);
            out.writeRawData(&crcL,1);
//            out.writeRawData((char *)&end,1);
        }

#if 1
NextStep:
        if(bytesSent < FlieSize)
        {
            emit SendDataPacket(outBlock);
        }
        else{
            QByteArray outBlock;
            QDataStream out(&outBlock, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_6);
            outBlock.resize(0);
            char eot = XMDM_EOT;
            out.writeRawData(&eot,1);
            emit SendDataPacket(outBlock);
            qDebug()<<"send XMDM_EOT";
        }

        qDebug("seq:%#x",seq);
        qDebug()<<"bytesSent"<<bytesSent<<"FlieSize"<<FlieSize;
        rxChar = 0;
        //获取XMDM_ACK、XMDM_NCK、XMDM_CAN
        GetChar(0x00,&rxChar,5000);
        m_rxchar = 0;
        qDebug("respond:%#x",rxChar);
        qDebug("*****************************");

        if(!(rxChar==XMDM_ACK || rxChar==XMDM_NCK || rxChar==XMDM_CAN))
        {
            MainWindow::flags = -1;
            return false;
        }

        switch (rxChar) {
        case XMDM_ACK:
            if(bytesSent >= FlieSize)
            {
                break;
            }
            ++seq;
            if(seq >= 255)
            {
               seq = 1;
            }
            bytesSent += DataReadSize;
            //发送升级进度
            emit PercentageProgress(QString("%1").arg(bytesSent/(FlieSize*1.0)*100),m_CardType,m_Row,m_Col);

            if(bytesSent >= FlieSize)
            {
                goto NextStep;
            }
            break;
        case XMDM_NCK:
            goto NextStep;
            break;
        case XMDM_CAN:
            MainWindow::flags = -1;
            return false;
            break;
        default:
            break;
        }
    }
#endif
    MainWindow::flags = -1;
    return true;
}

/***c：0x43获取XMDM_CRC  c：0x00获取XMDM_ACK、XMDM_NCK、XMDM_CAN***/
void Xmodem::GetChar(char c, char *rxchar,int msWaitTime)
{
    do
    {
        MainWindow::flags = 2;

        if(c == 0x43)
        {
            if(m_rxchar==XMDM_CRC)
            {
                *rxchar = m_rxchar;
                break;
            }
        }
        if(c == 0x00)
        {
            if( m_rxchar==XMDM_ACK || m_rxchar==XMDM_NCK || m_rxchar==XMDM_CAN)
            {

                *rxchar = m_rxchar;
                break;
            }
        }
        if(c == 0x35)
        {
            if( m_rxchar == 0x35)
            {
                *rxchar = m_rxchar;
                break;
            }
        }
        msleep(1);
    }while(msWaitTime--);
}

/***接收应答***/
void Xmodem::ReceiveRespond(char respond)
{
    m_rxchar = respond;
    qDebug("****m_rxchar*****:%#x",m_rxchar);
}

void Xmodem::StartSendFile(QString BinFilePath, QString CardType, int Row, int Col)
{
    m_BinFilePath = BinFilePath;
    //保存升级固件的信息
    m_CardType = CardType;
    m_Row = Row;
    m_Col = Col;
    this->start();
}

void Xmodem::run()
{
    if(SendFile(m_BinFilePath, m_CardType))
    {
        qDebug()<<"升级成功";
        QString version = m_BinFilePath.mid(m_BinFilePath.size()-8,4);
        emit SendFileFinished(version+"升级完成",m_CardType,m_Row,m_Col);
    }
    else
    {
        qDebug()<<"升级失败";
        //msleep(100);
        //emit reconnection();//发起重新连接socket信号
        emit SendFileFinished("升级失败",m_CardType,m_Row,m_Col);
    }
}
