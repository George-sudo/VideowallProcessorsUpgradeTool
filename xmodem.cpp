#include "xmodem.h"
#include "string.h"

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

bool Xmodem::SendFile(QString FilePath)
{
    char rxChar = 0;
    char PacketBuf[PKTSIZE] = {0};
    const char xmdm_stx = XMDM_STX;
    char seq = 1;
    char useq = ~seq;
    char crcL = 0;
    char crcH = 0;
    QFile file(FilePath);
    quint16 crc;
    quint32 bytesSent = 0;
    quint64 FlieSize;
    quint64 DataReadSize;
    QByteArray m_outBlock;
    QDataStream out(&m_outBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);

    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"Error!文件打开失败！";
        return false;
    }
    FlieSize = file.size();

    //等待板卡重启
    sleep(10);
    //重新连接Socket
    m_tcpClient->connectToHost(Connection::ip,Connection::port);
    if(!m_tcpClient->waitForConnected(1000))
    {
        QMessageBox::information(NULL, "提醒", "连接失败");
    }
    //重启后获取“C”
    while(0 == rxChar)
        GetChar(&rxChar,3000);
    if(rxChar != XMDM_CRC)
    {
        qDebug()<<"Error!未接受到应答信号XMDM_CRC！";
        return false;
    }

    while(bytesSent < FlieSize)
    {
        memset(PacketBuf,0,PKTSIZE);
        m_outBlock.resize(0);

        DataReadSize = file.read(PacketBuf,PKTSIZE);

        if(DataReadSize == PKTSIZE)
        {
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            crcH = crc>>8;
            crcL = crc&0x00ff;
            useq = ~seq;
            out.writeRawData(&xmdm_stx,1);
            out.writeRawData(&seq,1);
            out.writeRawData(&useq,1);
            out.writeRawData(PacketBuf,PKTSIZE);
            out.writeRawData(&crcH,1);
            out.writeRawData(&crcL,1);
        }else if(DataReadSize < PKTSIZE){
            for(quint64 i=PKTSIZE-1; i>=DataReadSize; --i)
            {
                PacketBuf[i] = XMDM_CTRLZ;
            }
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            crcH = crc>>8;
            crcL = crc&0x00ff;
            useq = ~seq;
            out.writeRawData(&xmdm_stx,1);
            out.writeRawData(&seq,1);
            out.writeRawData(&useq,1);
            out.writeRawData(PacketBuf,PKTSIZE);
            out.writeRawData(&crcH,1);
            out.writeRawData(&crcL,1);
        }

NextStep:
        if(bytesSent < FlieSize)
        {
            m_tcpClient->write(m_outBlock);
        }
        else{
            m_outBlock.resize(0);
            out << XMDM_EOT;
            m_tcpClient->write(m_outBlock);
        }

        GetChar(&rxChar,3000);
        switch (rxChar) {
        case XMDM_ACK:
            if(bytesSent >= FlieSize)
            {
                break;
            }
            ++seq;
            bytesSent += DataReadSize;
            if(bytesSent >= FlieSize)
            {
                goto NextStep;
            }
            break;
        case XMDM_NCK:
            goto NextStep;
            break;
        case XMDM_CAN:
            return false;
            break;
        default:
            break;
        }
    }
    qDebug()<<"******************************rxChar"<<rxChar;
    return true;
}

void Xmodem::GetChar(char *rxchar,int msWaitTime)
{
    do
    {
        if(m_tcpClient->getChar(rxchar))
           break;
        msleep(msWaitTime);
    }while(msWaitTime--);
}

void Xmodem::StartSendFile(QString FilePath)
{
    m_FilePath = FilePath;
    this->start();
}

void Xmodem::run()
{
    if(SendFile(m_FilePath))
        emit SendFileFinished("升级完成");
    else
        emit SendFileFinished("升级失败");
}
