#include "trendlogger.h"
#include "logger.h"


//======================================================================
TrendLogger::TrendLogger()
{
logger=Logger::Instance();
    strncpy(trendPath,"d:\\mnu_scada\\trends\\",256);
    for(int i=0;i<17280;i++)
    {
    empty_file[i]=min_float;
    }
}
//======================================================================
TrendLogger::TrendLogger(const char *trend_path)
{
logger=Logger::Instance();
    strncpy(trendPath,trend_path,256);
    for(int i=0;i<17280;i++)
    {
    empty_file[i]=min_float;
    }
}
//======================================================================

void TrendLogger::AddTrendTag(Tag* trendtag)
{
//TrendTag tmpTrendTag;
//tmpTrendTag.trendTag=trendtag;
//strncpy(tmpTrendTag.trendName,trendname,32);
VectTrendTag.push_back(trendtag);
logger->AddLog(QString("TREND: Добавлен тэг: ")+trendtag->GetTrendName(),Qt::darkMagenta);
}
//======================================================================
void TrendLogger::run()
{
    logger->AddLog(QString("TREND: Старт подсистемы, путь:")+trendPath,Qt::darkMagenta);
    wchar_t filenamew[320];
    //char filenamec[256];
  //  QString filenameQ;
    SYSTEMTIME st;
    HANDLE hFile=INVALID_HANDLE_VALUE;
    DWORD NumberOfBytesWritten;

    int itime,iprevtime=100;
    float value;

    for (;;)
    {
    ///////////////////////////////////////////////////////


    GetLocalTime(&st);
    itime=st.wSecond / 5;

    if (itime!=iprevtime)
    {
    iprevtime=itime;

    int time22=(st.wHour*60*60+st.wMinute*60+st.wSecond)/5;


        foreach(Tag *tag, VectTrendTag)
        {//foreach
            if (tag->GetQuality())
        {

    wsprintf(filenamew,L"%S%S_%.2u_%.2u_%.4u.trn",trendPath,tag->GetTrendName(),st.wDay,st.wMonth,st.wYear);
  //  filenameQ.sprintf("%s%s_%.2u_%.2u_%.4u.trn",trendPath,tag->GetTrendName(),st.wDay,st.wMonth,st.wYear);
   // sprintf(filenamec,"%s%s_%.2u_%.2u_%.4u.trn",trendPath,tag.trendname,st.wDay,st.wMonth,st.wYear);

    hFile=CreateFileW(filenamew,GENERIC_WRITE	,FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile==INVALID_HANDLE_VALUE)
    {
    hFile=CreateFileW(filenamew,GENERIC_WRITE	,FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    WriteFile(hFile,empty_file,69120,&NumberOfBytesWritten,NULL);
    }
    OVERLAPPED ovplp;
    ovplp.Offset=time22*4;
    ovplp.OffsetHigh=NULL;
    ovplp.hEvent=NULL;
    value=tag->GetValueForTrend();
    WriteFile(hFile,&value,4,&NumberOfBytesWritten,&ovplp);
    CloseHandle(hFile);
    }//if ..........
    }//foreach
    }

    Sleep(1000);

    if (CheckThreadStop()) return;

    }//for
   // return;

}
//=====================================================================
