#ifndef MYMODBUS_H
#define MYMODBUS_H
#include <QString>
#include <windows.h>
#include "tag.h"
#include <vector>
#include "autostopthread.h"
#include "logger.h"

using std::vector;

struct ModbusLinkedFloatTag
{
FloatTag *tag;
BYTE devaddr;
unsigned short celladdr;
};

class MyModbus : public AutoStopThread
{
private:
    HANDLE hComPort;
    bool FloatInvertedBytes;
    bool FloatInvertedWords;
    bool IntInvertedBytes;
    bool IntInvertedWords;
    bool ShortInvertedBytes;
    QString ComPortName;
    int speed;
    int databits;
    int parity;
    int stopbits;

    vector<ModbusLinkedFloatTag> VectFloat;

public:
    Logger *logger;
  //  MyModbus();
    MyModbus(QString PortName,int Speed, int DataBits, int Parity, int StopBits);
    bool OpenCommPort();
    void SetComPortDcb();
    unsigned short CRC16(unsigned char *Buffer, int Buffer_Length);
    bool ReadDev(BYTE mbadr,BYTE addr_hi,BYTE addr_lo); //проверка наличия девайса
    bool ReadFloat(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,float *buf);
    float IEEE754float(BYTE src[4]);
    BOOL floatNotNumber(BYTE src[4]);
    bool ReadInt(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,int *buf);        //Long = 4 byte
    int IEEE754long(BYTE src[4]);
    bool ReadShort(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,short *buf);       //Int=2 byte
    short IEEE754int(BYTE src[2]);
    void LinkFloat(FloatTag *tag,BYTE devaddr,unsigned short celladdr);
    void run();
};

#endif // MYMODBUS_H
