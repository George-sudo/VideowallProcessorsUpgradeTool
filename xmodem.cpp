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
    QFile file(FilePath);
    quint8 seq = 1;
    quint16 crc;
    quint32 bytesSent = 0;
    quint64 FlieSize;
    quint64 DataReadSize;
    QByteArray m_outBlock;
    QDataStream out(&m_outBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);

    while(0 == rxChar)
        GetChar(&rxChar);

    if(rxChar != XMDM_CRC)
    {
        qDebug()<<"Error!未接受到应答信号XMDM_CRC！";
        return false;
    }
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"Error!文件打开失败！";
        return false;
    }
    FlieSize = file.size();

    while(bytesSent < FlieSize)
    {
        memset(PacketBuf,0,PKTSIZE);
        m_outBlock.clear();

        DataReadSize = file.read(PacketBuf,PKTSIZE);
        if(DataReadSize == PKTSIZE)
        {
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            out << XMDM_STX << seq << (~seq) << PacketBuf << crc;
        }else if(DataReadSize < PKTSIZE){
            for(quint64 i=PKTSIZE-1; i>=DataReadSize; --i)
            {
                PacketBuf[i] = XMDM_CTRLZ;
            }
            crc = crc16_ccitt(PacketBuf,PKTSIZE);
            out << XMDM_STX << seq << (~seq) << PacketBuf << crc;
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
        GetChar(&rxChar,5000);
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
            return true;
            break;
        default:
            break;
        }
    }
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

void Xmodem::run()
{
    for(int i=0; i<8; ++i)
    {
        qDebug()<<i;
        sleep(1);
    }
}
