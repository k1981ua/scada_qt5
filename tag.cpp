#include "tag.h"
#include <QSettings>
#include <QTextCodec>

//================================================================================
FloatTag::FloatTag()
{
    tagFirstTimeSet=true;
    quality=false;
    value=0;
    strcpy(TrendName,"");
    isDiskSavedVar=false;
}
//===============================================================================
FloatTag::FloatTag(const char *Trendname,bool isDiskSavedvar)
{
    tagFirstTimeSet=true;
    quality=false;
    value=0;
    strncpy(TrendName,Trendname,64);
    isDiskSavedVar=isDiskSavedvar;

    if (isDiskSavedVar && strcmp(TrendName,""))
    {
    QString tmp;
    tmp.sprintf("%s",TrendName);
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope, "QtSCADA", "DiskVars");
    SetValueQuality(settings.value(tmp,0).toFloat(),true);
//    logger->AddLog(QString("TAG: Переменная ")+Trendname+" дисковая",Qt::darkCyan);
    }

}
//==================================================================================
/*
FloatTag::FloatTag(QString Trendname)
{
    quality=false;
    value=0;
    TrendName=Trendname;
}
*/
//=================================================================================
FloatTag::operator float()
{
return GetValue();
}

//=================================================================================
float FloatTag::operator = (const float & other)
{
//float tmp;
SetValueQuality(other,true);
return GetValue();
//return tmp;
}
//=================================================================================
QString FloatTag::ToString(const char *format)
{
QString tmp;
tmp.sprintf(format,GetValue());
return tmp;
}
//=================================================================================
void FloatTag::SetValueQuality(float newValue, bool newQuality)
{
    float tmpValue;
    bool tmpQuality;
    mutex.lock();
    tmpValue=value;
    tmpQuality=quality;
    value=newValue;
    quality=newQuality;
    mutex.unlock();

    if (isDiskSavedVar && strcmp(TrendName,""))
    {
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope, "QtSCADA", "DiskVars");
    QString tmp;
    tmp.sprintf("%s",TrendName);
    settings.setValue(tmp,GetValue());
    }

    if (tagFirstTimeSet)
    {
        if (newQuality) emit ValueChanged(value);
        emit QualityChanged(quality);
        tagFirstTimeSet=false;
    }
    else
    {
    if (tmpValue!=newValue && newQuality) emit ValueChanged(value);
    if (tmpQuality!=newQuality) emit QualityChanged(quality);
    }
}
//==================================================================================
float FloatTag::GetValue()
{
    float tmpValue;
    mutex.lock();
    tmpValue=value;
    mutex.unlock();
    return tmpValue;
}
//===================================================================================
bool FloatTag::GetQuality()
{
    bool tmpQuality;
    mutex.lock();
    tmpQuality=quality;
    mutex.unlock();
    return tmpQuality;
}
//===================================================================================
float FloatTag::GetValueForTrend()
{
    float tmpValue;
    mutex.lock();
    tmpValue=value;
    mutex.unlock();
    return tmpValue;
}
//===================================================================================
char* FloatTag::GetTrendName()
{
return TrendName;
}
//===================================================================================
