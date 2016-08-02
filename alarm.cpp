#include "alarm.h"
#include <QSettings>

extern Alarms *alarms;

//=================================================================
bool Alarm::IsActive()
{
return active;
}
//=================================================================
bool Alarm::IsConfirmed()
{
return confirmed;
}
//=================================================================
void Alarm::SetActive(bool newactive)
{
active=newactive;
}
//=================================================================
void Alarm::SetConfirmed(bool newconfirmed)
{
confirmed=newconfirmed;
}
//=================================================================
QString Alarm::GetTextOfAlarm()
{
return text;
}
//=================================================================
QString Alarm::GetDateTimeOfAlarm()
{
QString tmp;
tmp.sprintf("%.2u.%.2u.%.4u %.2u:%.2u:%.2u",
            dt.date().day(),dt.date().month(),dt.date().year(),
            dt.time().hour(),dt.time().minute(),dt.time().second());
return tmp;
}
//=================================================================
void Alarm::SetDateTimeOfAlarm()
{
dt=QDateTime::currentDateTime();
}
//=================================================================
Alarm::Alarm(AlarmLevel alarmlevel,AlarmType alarmtype,FloatTag *tag,AlarmCondition alarmcondition,
             float alarmvalue,QString alarmtext)
{
active=false;
confirmed=false;
alarmlevel=alarmlevel;
alarmCondition=alarmcondition;
alarmValue=alarmvalue;
text=alarmtext;

if (alarmlevel==Critical)
{
actColor.setRgb(255,71,26);
nonactColor.setRgb(233,75,80);
act2Color.setRgb(213,0,0);
}
if (alarmlevel==Warning)
{
actColor.setRgb(233,233,7);
nonactColor.setRgb(145,200,255);
act2Color.setRgb(248,248,20);
}

if (alarmlevel==Information)
{
actColor.setRgb(255,255,21);
nonactColor.setRgb(208,208,21);
act2Color.setRgb(220,220,0);
}

if (alarmtype==OnValueChanged)
connect(tag,SIGNAL(ValueChanged(float)),this,SLOT(TagValueChanged(float)));
if (alarmtype==OnQualityChanged)
    connect(tag,SIGNAL(QualityChanged(bool)),this,SLOT(TagQualityChanged(bool)));

connect(this,SIGNAL(AlarmActivating(Alarm*)),alarms,SLOT(AddEnabledAlarm(Alarm*)));
connect(this,SIGNAL(AlarmDeactivating(Alarm*)),alarms,SLOT(DeleteEnabledAlarm(Alarm*)));
}
//=================================================================
void Alarm::TagValueChanged(float newvalue)
{

    if ((active==false && alarmCondition==Bigger && newvalue>alarmValue  )   ||
        (active==false && alarmCondition==Less   && newvalue<alarmValue  )   ||
        (active==false && alarmCondition==Equal  && newvalue==alarmValue ))
    {
        SetDateTimeOfAlarm();
        active=true;
  //      if (confirmed==true)
  //      {

        confirmed=false;

   //     oldalarms.AddAlarm(this);
        emit AlarmActivating(this);
  //      }

    }

    if ((active==true && alarmCondition==Bigger && newvalue<=alarmValue) ||
        (active==true && alarmCondition==Less   && newvalue>=alarmValue) ||
        (active==true && alarmCondition==Equal  && newvalue!=alarmValue))

    {
        active=false;
        confirmed=false;
        emit AlarmDeactivating(this);
    }

}
//=================================================================
void Alarm::TagQualityChanged(bool newquality)
{
float fltquality;
    if (newquality) fltquality=1.0;
    else fltquality=0.0;

    if ((active==false && alarmCondition==Bigger && fltquality>alarmValue  )   ||
        (active==false && alarmCondition==Less   && fltquality<alarmValue  )   ||
        (active==false && alarmCondition==Equal  && fltquality==alarmValue ))
    {
        SetDateTimeOfAlarm();
        active=true;
  //      if (confirmed==true)
  //      {

        confirmed=false;

   //     oldalarms.AddAlarm(this);
        emit AlarmActivating(this);
  //      }

    }

    if ((active==true && alarmCondition==Bigger && fltquality<=alarmValue) ||
        (active==true && alarmCondition==Less   && fltquality>=alarmValue) ||
        (active==true && alarmCondition==Equal  && fltquality!=alarmValue))

    {
        active=false;
        confirmed=false;
        emit AlarmDeactivating(this);
    }

}
//=================================================================
Alarms::Alarms()
{
logger=Logger::Instance();
enabledAlarmList.clear();
logger->AddLog("АЛАРМ: Старт подсистемы...",Qt::darkCyan);
}
//=================================================================
void Alarms::AddAlarm(AlarmLevel alarmlevel,AlarmType alarmType,FloatTag *tag,AlarmCondition alarmcondition,
              float alarmvalue,QString alarmtext)
{
    Alarm *newAlarm=new Alarm(alarmlevel,alarmType,tag,alarmcondition, alarmvalue,alarmtext);
    allAlarmsList.push_back(newAlarm);
    logger->AddLog("АЛАРМ: Добавлен: "+alarmtext,Qt::darkCyan);

}

//=================================================================
void Alarms::AddEnabledAlarm(Alarm *alarm)
{
    if (enabledAlarmList.size()<maxEnabledAlarmCount)
    {
        enabledAlarmList.push_front(alarm);
    }
    else
    {
        enabledAlarmList.pop_back();
        enabledAlarmList.push_front(alarm);
    }
    if (!isRunning()) start();
    emit EnabledAlarmsChanged(&enabledAlarmList, false);
    logger->AddLog("АЛАРМ: Сработал: "+alarm->GetTextOfAlarm(),Qt::darkCyan);
}
//==================================================================
void Alarms::DeleteEnabledAlarm(Alarm *alarm)
{
    enabledAlarmList.remove(alarm);
    emit EnabledAlarmsChanged(&enabledAlarmList, false);
    logger->AddLog("АЛАРМ: Пропал: "+alarm->GetTextOfAlarm(),Qt::darkCyan);
}
//==================================================================
void Alarms::Kvitirovat(int count)
{

    list<Alarm*>::iterator al_iter_begin;
    list<Alarm*>::iterator al_iter_end;

    if (enabledAlarmList.size()>0)
    {
    al_iter_begin=enabledAlarmList.begin();
    al_iter_end=enabledAlarmList.end();

    for(int i=0;i<count;++i)
    {
    if (al_iter_begin!=al_iter_end)
    {
        if (!(*al_iter_begin)->IsConfirmed())
        {
        (*al_iter_begin)->SetConfirmed(true);
        logger->AddLog("АЛАРМ: Квитирован: "+(*al_iter_begin)->GetTextOfAlarm(),Qt::darkCyan);
        }
        al_iter_begin++;
    }
    else break;
    }
    }
}

//==================================================================
bool compare_alarms(Alarm *first,Alarm *second)
{
if (!second->IsConfirmed()) return false;
if (!first->IsConfirmed()) return true;
return (first->dt > second->dt);

}
//==================================================================
void Alarms::Kvitirovat2()
{
    Kvitirovat(5);
    enabledAlarmList.sort(compare_alarms);
    emit EnabledAlarmsChanged(&enabledAlarmList, false);
}
//==================================================================
void Alarms::run()
{
    bool isBlinking=true;




    while (isBlinking)
    {
        isBlinking=false;
        msleep(500);
        if (CheckThreadStop()) return;
        foreach (Alarm *alarm, enabledAlarmList)
        {
            if (alarm->IsActive() && !alarm->IsConfirmed()) isBlinking=true;
        }
        if (isBlinking) emit EnabledAlarmsChanged(&enabledAlarmList, true);

    }

//isBlinking=false;


}
