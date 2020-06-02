#include "xmodem.h"
#include "string.h"

Xmodem::Xmodem()
{
}

unsigned short Xmodem::crc16_ccitt(const char *buf, int len)
{
    register int counter;
    register unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
    crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}

void Xmodem::run()
{

}
