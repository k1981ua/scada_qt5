#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QColor>
#include <list>
#include "tag.h"
#include "logger.h"
#include <QColor>
#include "autostopthread.h"


using std::list;

enum AlarmLevel
{
   Critical,
   Warning,
   Information
};

enum AlarmCondition
{
    Bigger,
    Less,
    Equal
};

enum AlarmType
{
    OnValueChanged,
    OnQualityChanged
};


class Alarm : public QObject
{
    Q_OBJECT

private:

    bool active;
    bool confirmed;
    QString text;
    AlarmLevel alarmLevel;
    AlarmCondition alarmCondition;
    AlarmType alarmType;
    float alarmValue;

public:
        QDateTime dt;
    //Alarm();
    Alarm(AlarmLevel alarmlevel,AlarmType alarmtype,FloatTag *tag,
                 AlarmCondition alarmcondition,float alarmvalue,QString alarmtext);
    void SetDateTimeOfAlarm();
    QString GetDateTimeOfAlarm();
    QString GetTextOfAlarm();
    bool IsActive();
    bool IsConfirmed();
    void SetActive(bool newactive);
    void SetConfirmed(bool newconfirmed);
    QColor actColor;
    QColor act2Color;
    QColor nonactColor;
signals:
    void AlarmActivating(Alarm *alarm);
    void AlarmDeactivating(Alarm *alarm);
public slots:
    void TagValueChanged(float newvalue);
    void TagQualityChanged(bool newquality);

};
//=================================================================================


class Alarms : public AutoStopThread
{
        Q_OBJECT
public:
    list < Alarm* > enabledAlarmList;
    list < Alarm* > allAlarmsList;
    static const unsigned int maxEnabledAlarmCount=50;

    Alarms();
    void AddAlarm(AlarmLevel alarmlevel,AlarmType alarmType,FloatTag *tag,
                  AlarmCondition alarmcondition,float alarmvalue,QString alarmtext);
    void run();
    Logger *logger;
signals:
    void EnabledAlarmsChanged(list < Alarm* > *pEnabledAlarmList, bool onlyColorChange);

public slots:
    void AddEnabledAlarm(Alarm *alarm);
    void DeleteEnabledAlarm(Alarm *alarm);
    void Kvitirovat(int alarmscount);
    void Kvitirovat2();
};

//=================================================================================
#endif // ALARM_H
