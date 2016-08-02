
#include "mymodbus.h"
#include "logger.h"

//==========================================================================
MyModbus::MyModbus(QString PortName,int Speed, int DataBits, int Parity, int StopBits)
{
logger=Logger::Instance();
hComPort=NULL;
ComPortName=PortName;
speed=Speed;
databits=DataBits;
parity=Parity;
stopbits=StopBits;

FloatInvertedBytes=0;
FloatInvertedWords=1;
IntInvertedBytes=0;
IntInvertedWords=1;
ShortInvertedBytes=1;
logger->AddLog("MODBUS: Старт драйвера...",Qt::blue);
}

//=========================================================================
unsigned short MyModbus::CRC16(unsigned char *Buffer, int Buffer_Length)
{   //
       // Algorithm - CRC16 Standard, Polynom - 8005 (X16 + X15 + X2 + 1)
       // Init const = ffff
       // Check: CRC16("123456789",9) = BB3D
       //

       const unsigned short CRC16_tbl[ 256 ] = {
       0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
       0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
       0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
       0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
       0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
       0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
       0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
       0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
       0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
       0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
       0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
       0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
       0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
       0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
       0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
       0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
       0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
       0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
       0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
       0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
       0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
       0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
       0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
       0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
       0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
       0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
       0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
       0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
       0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
       0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
       0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
       0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

       unsigned short CRC16 = 0xffff;

       for (int i=0; i < Buffer_Length; i++)
           {
               CRC16 = ( CRC16 >> 8 ) ^ CRC16_tbl[ ( CRC16 & 0xFF ) ^ Buffer[i] ];
           }
       return CRC16;
}
//-------------------------------------------------------------------------
bool MyModbus::OpenCommPort()
{
hComPort = CreateFile(ComPortName.toStdWString().data(),
    GENERIC_READ | GENERIC_WRITE,
    0,    /* comm devices must be opened w/exclusive-access */
    NULL, /* no security attrs */
    OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
    0,    /* not overlapped I/O */
    NULL  /* hTemplate must be NULL for comm devices */
    );

        if (hComPort!=INVALID_HANDLE_VALUE)
        {
            logger->AddLog("MODBUS: драйвер занял порт "+ComPortName,Qt::blue);
            return true;
        }
        else
        {
            logger->AddLog("MODBUS: драйвер не может открыть порт "+ComPortName,Qt::red);
            return false;
        }
}
//--------------------------------------------------------------------------
void MyModbus::SetComPortDcb()
{
    DCB dcb1;
    COMMTIMEOUTS comtimeouts;

    if (hComPort!=INVALID_HANDLE_VALUE)
    {
    GetCommState(hComPort, &dcb1);
    dcb1.BaudRate=speed;
    dcb1.fBinary=1;
    dcb1.fParity=1;
    dcb1.fDtrControl=0;
    dcb1.Parity=parity;
    dcb1.ByteSize=databits;
    dcb1.StopBits = stopbits;
    dcb1.fOutxCtsFlow=0;
    dcb1.fOutxDsrFlow=0;
    dcb1.fDsrSensitivity=0;
    dcb1.fOutX=0;
    dcb1.fInX=0;
    SetCommState(hComPort, &dcb1);

    comtimeouts.ReadIntervalTimeout=0;
    comtimeouts.ReadTotalTimeoutMultiplier=0;
    comtimeouts.WriteTotalTimeoutMultiplier=0;
    comtimeouts.ReadTotalTimeoutConstant=500;
    comtimeouts.WriteTotalTimeoutConstant=500;
    SetCommTimeouts(&hComPort,&comtimeouts);

    QString strparity;
    if (parity==EVENPARITY) strparity="even";  //EVENPARITY=2
    if (parity==MARKPARITY) strparity="mark";  //MARKPARITY=3
    if (parity==NOPARITY ) strparity="no";     //NOPARITY=0
    if (parity==ODDPARITY) strparity="odd";    //ODDPARITY=1
    if (parity==SPACEPARITY) strparity="space";//SPACEPARITY=4

    QString strstopbits;
     if (stopbits==ONESTOPBIT) strstopbits="1";         //ONESTOPBIT=0
     if (stopbits==ONE5STOPBITS) strstopbits="1.5";     //ONE5STOPBITS=1
     if (stopbits==TWOSTOPBITS) strstopbits="2";        //TWOSTOPBITS=2


        logger->AddLog("MODBUS: Установил параметры порта "+ComPortName+
                       " ("+QString::number(speed)+","+QString::number(databits)+
                       ","+strstopbits+","+strparity+")",Qt::blue);
        }
}
//-----------------------------------------------------------------
bool MyModbus::ReadDev(BYTE mbadr,BYTE addr_hi,BYTE addr_lo)
{
        union  crc_union{
        unsigned short integ;
        BYTE bytes[2];
        };

        struct {
        BYTE _1addr;
        BYTE _2cmd;
        BYTE _3data_start_addr_hi;
        BYTE _4data_start_addr_lo;
        BYTE _5data_length_hi;
        BYTE _6data_length_lo;
        BYTE _7crc1;
        BYTE _8crc2;
        } zapros;

        struct otvet4{
        BYTE _1from_addr;
        BYTE _2cmd;
        BYTE _3byte_count;
        BYTE _4data[4];
        BYTE _8crc1;
        BYTE _9crc2;
        };

    crc_union crc_long, crc_otv_lng;
    otvet4 otvet4l;


    DWORD lpNumberOfBytesWritten;	// pointer to number of bytes written

    COMSTAT statistic;

    crc_long.integ=0;

    zapros._1addr=mbadr;
    zapros._2cmd=0x03;
    zapros._3data_start_addr_hi=addr_hi;
    zapros._4data_start_addr_lo=addr_lo;
    zapros._5data_length_hi=0x00;
    zapros._6data_length_lo=0x02;

    crc_long.integ=CRC16(&zapros._1addr,6);

    zapros._7crc1=crc_long.bytes[0];
    zapros._8crc2=crc_long.bytes[1];

    ZeroMemory(&otvet4l,sizeof(otvet4l));

    PurgeComm(hComPort,PURGE_TXCLEAR);
    PurgeComm(hComPort,PURGE_RXCLEAR);

    if (WriteFile(hComPort,&zapros,sizeof(zapros),&lpNumberOfBytesWritten,NULL))
    {
    Sleep(1000);

       ClearCommError(hComPort,NULL,&statistic);

    /*
       if (statistic.cbInQue>=sizeof(otvet4l))
       {
        ReadFile(hComPort,&otvet4l,sizeof(otvet4l),&lpNumberOfBytesWritten,NULL);
       } else {return false;}

       crc_otv_lng.integ=CRC16(&otvet4l._1from_addr,7);

       if ((crc_otv_lng.bytes[0]==otvet4l._8crc1) && (crc_otv_lng.bytes[1]==otvet4l._9crc2))
       {
           return true;
       }
    */
       int len=statistic.cbInQue;
       unsigned char buff[256];

       if (len>=4)
       {
        ReadFile(hComPort,buff,len,&lpNumberOfBytesWritten,NULL);
       } else {return false;}

       crc_otv_lng.integ=CRC16(buff,len-2);

       if ((crc_otv_lng.bytes[0]==buff[len-2]) && (crc_otv_lng.bytes[1]==buff[len-1]))
       {
           return true;
       }

    } else {return false;} //write file
    return false;
}
//=======================================================================
bool MyModbus::ReadFloat(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,float *buf)
{
        union  crc_union{
        unsigned short integ;
        BYTE bytes[2];
        };

        struct {
        BYTE _1addr;
        BYTE _2cmd;
        BYTE _3data_start_addr_hi;
        BYTE _4data_start_addr_lo;
        BYTE _5data_length_hi;
        BYTE _6data_length_lo;
        BYTE _7crc1;
        BYTE _8crc2;
        } zapros;

        struct otvet4{
        BYTE _1from_addr;
        BYTE _2cmd;
        BYTE _3byte_count;
        BYTE _4data[4];
        BYTE _8crc1;
        BYTE _9crc2;
        } ;


        COMSTAT statistic;
        DWORD lpNumberOfBytesWritten;	// pointer to number of bytes written





    crc_union crc_float, crc_otv_flt;
    otvet4 otvet4f;
    DWORD p;


    crc_float.integ=0;

    zapros._1addr=mbadr;
    zapros._2cmd=0x03;
    zapros._3data_start_addr_hi=addr_hi;
    zapros._4data_start_addr_lo=addr_lo;
    zapros._5data_length_hi=0x00;
    zapros._6data_length_lo=0x02;

    crc_float.integ=CRC16(&zapros._1addr,6);

    zapros._7crc1=crc_float.bytes[0];
    zapros._8crc2=crc_float.bytes[1];

    //=========099-9-9-09-90=

    for(p=0;p<4;p++)
    {
        if (p==3)
        {
            *buf=0;
            return false;
        }
    ZeroMemory(&otvet4f,sizeof(otvet4f));

    PurgeComm(hComPort,PURGE_TXCLEAR);
    PurgeComm(hComPort,PURGE_RXCLEAR);

    if (WriteFile(
        hComPort,	// handle to file to write to
        &zapros,	// pointer to data to write to file
        sizeof(zapros),	// number of bytes to write
        &lpNumberOfBytesWritten,	// pointer to number of bytes written
        NULL 	// pointer to structure needed for overlapped I/O
       ))
    {


        Sleep(50);
        for(int i=0;i<20;++i)
        {
        ClearCommError(hComPort,NULL,&statistic);
        if (statistic.cbInQue>=sizeof(otvet4f)) break;
        Sleep(50);
        }

       ClearCommError(hComPort,NULL,&statistic);
    if (statistic.cbInQue>=sizeof(otvet4f))
    {
       ReadFile(
        hComPort,	// handle of file to read
        &otvet4f,	// address of buffer that receives data
        sizeof(otvet4f),	// number of bytes to read
        &lpNumberOfBytesWritten,	// address of number of bytes read
        NULL	// address of structure for data
       );


    crc_otv_flt.integ=CRC16(&otvet4f._1from_addr,7);
    if ((crc_otv_flt.bytes[0]==otvet4f._8crc1)&& (crc_otv_flt.bytes[1]==otvet4f._9crc2))
    {
    p=5;
    }
    else
    {
    ZeroMemory(&otvet4f,sizeof(otvet4f));
    *buf=0;
    return false;
    }
    }

    }
    else
    {
    *buf=0;
    return false;
    }
    //===============08908=989=
    }
      if (floatNotNumber(otvet4f._4data))
      {
          *buf=0;
          return false;

      }
          else
      {
      *buf=IEEE754float(otvet4f._4data);
      return true;
      }
    }
//====================================================================
float MyModbus::IEEE754float(BYTE src[4])
{
     BYTE aaa;
     float ffloat;
     if (FloatInvertedBytes)
     {
     aaa=src[0];
     src[0]=src[3];
     src[3]=aaa;
     aaa=src[1];
     src[1]=src[2];
     src[2]=aaa;
     }
     if (FloatInvertedWords)
     {
     aaa=src[0];
     src[0]=src[1];
     src[1]=aaa;
     aaa=src[3];
     src[3]=src[2];
     src[2]=aaa;
     }

     memcpy(&ffloat,src,4);
     return ffloat;
    }
    //--------------------------------------------------------------------------
    BOOL MyModbus::floatNotNumber(BYTE src[4])
    {
    return  (((src[0]==0x7F) || (src[0]==0xFF)) && (src[1]>0x7F)
     & ((src[1]>0x80) || (src[2]>0x00) || (src[3]>0x00)));
    }
    //--------------------------------------------------------------------------
    int MyModbus::IEEE754long(BYTE src[4])
    {
     BYTE aaa;
     int ilong;
     if (IntInvertedBytes)
     {
     aaa=src[0];
     src[0]=src[3];
     src[3]=aaa;
     aaa=src[1];
     src[1]=src[2];
     src[2]=aaa;
     }
     if (IntInvertedWords)
     {
     aaa=src[0];
     src[0]=src[1];
     src[1]=aaa;
     aaa=src[3];
     src[3]=src[2];
     src[2]=aaa;
     }
     memcpy(&ilong,src,4);
     return ilong;
    }
    //==========================================================================
bool MyModbus::ReadInt(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,int *buf)
{
        union  crc_union{
        unsigned short integ;
        BYTE bytes[2];
        };

        struct {
        BYTE _1addr;
        BYTE _2cmd;
        BYTE _3data_start_addr_hi;
        BYTE _4data_start_addr_lo;
        BYTE _5data_length_hi;
        BYTE _6data_length_lo;
        BYTE _7crc1;
        BYTE _8crc2;
        } zapros;

        struct otvet4{
        BYTE _1from_addr;
        BYTE _2cmd;
        BYTE _3byte_count;
        BYTE _4data[4];
        BYTE _8crc1;
        BYTE _9crc2;
        } ;

        COMSTAT statistic;
        DWORD lpNumberOfBytesWritten;	// pointer to number of bytes written

    crc_union crc_long, crc_otv_lng;
    otvet4 otvet4l;
    DWORD p;

    crc_long.integ=0;

    zapros._1addr=mbadr;
    zapros._2cmd=0x03;
    zapros._3data_start_addr_hi=addr_hi;
    zapros._4data_start_addr_lo=addr_lo;
    zapros._5data_length_hi=0x00;
    zapros._6data_length_lo=0x02;

    crc_long.integ=CRC16(&zapros._1addr,6);

    zapros._7crc1=crc_long.bytes[0];
    zapros._8crc2=crc_long.bytes[1];

    for(p=0;p<4;p++)
    {
        if (p==3)
        {
            *buf=0;
            return false;
        }

    ZeroMemory(&otvet4l,sizeof(otvet4l));

    PurgeComm(hComPort,PURGE_TXCLEAR);
    PurgeComm(hComPort,PURGE_RXCLEAR);

    if (WriteFile(
        hComPort,	// handle to file to write to
        &zapros,	// pointer to data to write to file
        sizeof(zapros),	// number of bytes to write
        &lpNumberOfBytesWritten,	// pointer to number of bytes written
        NULL 	// pointer to structure needed for overlapped I/O
       ))
    {


        Sleep(50);
        for(int i=0;i<20;++i)
        {
        ClearCommError(hComPort,NULL,&statistic);
        if (statistic.cbInQue>=sizeof(otvet4l)) break;
        Sleep(50);
        }

       ClearCommError(hComPort,NULL,&statistic);
    if (statistic.cbInQue>=sizeof(otvet4l))
    {
       ReadFile(
        hComPort,	// handle of file to read
        &otvet4l,	// address of buffer that receives data
        sizeof(otvet4l),	// number of bytes to read
        &lpNumberOfBytesWritten,	// address of number of bytes read
        NULL	// address of structure for data
       );

    crc_otv_lng.integ=CRC16(&otvet4l._1from_addr,7);
    if ((crc_otv_lng.bytes[0]==otvet4l._8crc1) && (crc_otv_lng.bytes[1]==otvet4l._9crc2))
    {
    p=5;
    }
    else
    {
    ZeroMemory(&otvet4l,sizeof(otvet4l));
    *buf=0;
    return false;
    }
    }
    }
    else
    {
    *buf=0;
    return false;
    }

    }

     *buf=IEEE754long(otvet4l._4data);
     return true;
    }

//==========================================================================
short MyModbus::IEEE754int(BYTE src[2])
    {
     BYTE aaa;
     short ilong;

     if (ShortInvertedBytes)
     {
     aaa=src[0];
     src[0]=src[1];
     src[1]=aaa;
     }
     memcpy(&ilong,src,2);
     return ilong;
}
//==========================================================================
bool MyModbus::ReadShort(BYTE mbadr,BYTE addr_hi,BYTE addr_lo,short *buf)
{
        union  crc_union{
        unsigned short integ;
        BYTE bytes[2];
        };

        struct {
        BYTE _1addr;
        BYTE _2cmd;
        BYTE _3data_start_addr_hi;
        BYTE _4data_start_addr_lo;
        BYTE _5data_length_hi;
        BYTE _6data_length_lo;
        BYTE _7crc1;
        BYTE _8crc2;
        } zapros;

        struct otvet2{
        BYTE _1from_addr;
        BYTE _2cmd;
        BYTE _3byte_count;
        BYTE _4data[2];
        BYTE _6crc1;
        BYTE _7crc2;
        } ;

        COMSTAT statistic;
        DWORD lpNumberOfBytesWritten;	// pointer to number of bytes written

    crc_union crc_long, crc_otv_lng;
    otvet2 otvet2i;
    DWORD p;

    crc_long.integ=0;

    zapros._1addr=mbadr;
    zapros._2cmd=0x03;
    zapros._3data_start_addr_hi=addr_hi;
    zapros._4data_start_addr_lo=addr_lo;
    zapros._5data_length_hi=0x00;
    zapros._6data_length_lo=0x01;

    crc_long.integ=CRC16(&zapros._1addr,6);

    zapros._7crc1=crc_long.bytes[0];
    zapros._8crc2=crc_long.bytes[1];

    for(p=0;p<4;p++)
    {
        if (p==3)
        {
            *buf=0;
            return false;
        }

    ZeroMemory(&otvet2i,sizeof(otvet2i));

    PurgeComm(hComPort,PURGE_TXCLEAR);
    PurgeComm(hComPort,PURGE_RXCLEAR);

    if (WriteFile(
        hComPort,	// handle to file to write to
        &zapros,	// pointer to data to write to file
        sizeof(zapros),	// number of bytes to write
        &lpNumberOfBytesWritten,	// pointer to number of bytes written
        NULL 	// pointer to structure needed for overlapped I/O
       ))
    {


    Sleep(50);
    for(int i=0;i<20;++i)
    {
    ClearCommError(hComPort,NULL,&statistic);
    if (statistic.cbInQue>=sizeof(otvet2i)) break;
    Sleep(50);
    }



       ClearCommError(hComPort,NULL,&statistic);
    if (statistic.cbInQue>=sizeof(otvet2i))
    {
       ReadFile(
        hComPort,	// handle of file to read
        &otvet2i,	// address of buffer that receives data
        sizeof(otvet2i),	// number of bytes to read
        &lpNumberOfBytesWritten,	// address of number of bytes read
        NULL	// address of structure for data
       );

    crc_otv_lng.integ=CRC16(&otvet2i._1from_addr,5);
    if ((crc_otv_lng.bytes[0]==otvet2i._6crc1) && (crc_otv_lng.bytes[1]==otvet2i._7crc2))
    {
    p=5;
    }
    else
    {
    ZeroMemory(&otvet2i,sizeof(otvet2i));
    *buf=0;
    return false;
    }
    }//if (statistic.cbInQue>=sizeof(otvet2i))

    }
    else
    {
    *buf=0;
    return false;
    }

    }

     *buf=IEEE754int(otvet2i._4data);
     return true;
    }

    //==========================================================================
void MyModbus::LinkFloat(FloatTag *tag,BYTE devaddr,unsigned short celladdr)
{
    ModbusLinkedFloatTag tmp;
    tmp.tag=tag;
    tmp.devaddr=devaddr;
    tmp.celladdr=celladdr;
    VectFloat.push_back(tmp);
    logger->AddLog(QString("MODBUS: Тэг ")+ tag->GetTrendName()+" связан с адресом "+QString::number(celladdr),Qt::blue);
}
//==========================================================================
void MyModbus::run()
{
        float flttmp;
        //short inttmp;

        if (OpenCommPort())
        {
                SetComPortDcb();
                while (1)
                {
                    foreach(ModbusLinkedFloatTag LinkedFloatTag,VectFloat)
                    {
                        if (ReadFloat(LinkedFloatTag.devaddr,LinkedFloatTag.celladdr / 256 ,(LinkedFloatTag.celladdr % 256) - 1,&flttmp))
                            {
                                    LinkedFloatTag.tag->SetValueQuality(flttmp,true);
                            }
                            else
                            {
                                    LinkedFloatTag.tag->SetValueQuality(0,false);
                            }
                        if (CheckThreadStop()) {return;}
                    }

                }
        }
        else
        {
            logger->AddLog("MODBUS: СОМ порт не открыт "+ComPortName,Qt::red);
        }


}
//==========================================================================
