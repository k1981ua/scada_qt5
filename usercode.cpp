#include "usercode.h"

//Тэги СКАДы
//Тэги могут быть связаны с драйверами протоколов,
//могут быть "дисковыми" и "трендовыми"
//1. Что бы тэг "привязался" к Модбас-протоколу -
//после создания Модбас драйвера с помощью
//    myModbus=new MyModbus("COM7",9600,8,NOPARITY,ONESTOPBIT);
//используйте
//    myModbus->LinkFloat(&LT1,1,100); 1-адрес устройства 100-адрес регистра  (40100)
//после привязывания всех тэгов не забываем
//    myModbus->start();
//2. Что бы тэг стал "трендовым" - при его объявлении нужно задать имя тренда
//    FloatTag LT1("LT11");
//создать объект тренд-логгера
//    myTrendLogger=new TrendLogger("d:\\mnu_scada\\trends\\");
//и "привязать" к нему тэг
//    myTrendLogger->AddTrendTag(&LT1);
//после привязывания всех тэгов делаем
//    myTrendLogger->start();
//3. Что бы переменная стала "дисковой" нужно при объявлении первым параметром задать
//имя, а вторым принак "дисковости" - true (если второй параметр не указать явно, он
//будет принят как false)
//    FloatTag DiskVar("DiskVar",true);
//
//Если нет необходимости в вышеуказанном функционале, объявляйте переменные просто как
//float или int


//АЛАРМы:
//Перед использованием нужно создать объект системы АЛАРМов
//alarms=new Alarms();
//После добавляем алармы:
//alarms->AddAlarm(Critical,&LT1,Bigger,90.0,"Рівень в РВС-1 критично високий");
//1-й параметр может быть Critical или Warning
//2-й параметр OnValueChanged - при изменении значения
//         или OnQualityChanged  - при изменении качества (для обработки событий сбоев связи)
//3-й - указатель на тэг
//4-й - Bigger(больше),Less(меньше) или Equal(равно) - условие срабатывания
//5-й - уставка аларма
//6-й - сообщение
//Таким образом аларм сработает по фронту наступления условия  LT1>90.0

//БД:
//Инициализируем перед использованием:
//    fireBird= new OdbcDb("fire_kacha1","sysdba","784523");
//Использование:
//    fireBird->ExecQuery("INSERT INTO PARAMS(DT,VALUE1) VALUES('26.09.2011 10:00:00',45.2)");
//
//
//
//
//




//Тэги
FloatTag LT1("LT11"),LT2("LT22");
FloatTag DiskVar("DiskVar",true);

FloatTag VolFlow("VolFlow",true);
FloatTag MassFlow("MassFlow",true);
FloatTag FlowNORD("FlowNORD",true);
FloatTag ObvodnOpos("ObvodnOpos",true);
FloatTag Dens("Dens",true);
FloatTag Tisk("Tisk",true);
FloatTag Temp("Temp",true);
FloatTag Obvodn("Obvodn",true);

//Подсистемы СКАДы
OdbcDb *fireBird;
MyModbus *myModbus;
TrendLogger *myTrendLogger;
Alarms *alarms;


//==================================================================
UserCode::UserCode()  //Инициализация
{

    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));  //Задаем шрифт системы

    logger=Logger::Instance();
    logger->InstanceWindow();
    logger->AddLog("Старт системы",Qt::black);

    myModbus=new MyModbus("COM1",9600,8,NOPARITY,ONESTOPBIT);

    myModbus->LinkFloat(&LT1,1,100);
    myModbus->LinkFloat(&LT2,1,102);
    myModbus->start();

    myTrendLogger=new TrendLogger("d:\\mnu_scada\\trends\\");

    myTrendLogger->AddTrendTag(&LT1);
    myTrendLogger->AddTrendTag(&LT2);

    myTrendLogger->AddTrendTag(&VolFlow);
    myTrendLogger->AddTrendTag(&MassFlow);
    myTrendLogger->AddTrendTag(&FlowNORD);
    myTrendLogger->AddTrendTag(&ObvodnOpos);
    myTrendLogger->AddTrendTag(&Dens);
    myTrendLogger->AddTrendTag(&Tisk);
    myTrendLogger->AddTrendTag(&Temp);
    myTrendLogger->AddTrendTag(&Obvodn);
    myTrendLogger->start();

    alarms=new Alarms();
    alarms->AddAlarm(Warning,OnValueChanged,&LT1,Bigger,10.0,"Рівень в РВС-1 високий");
    alarms->AddAlarm(Critical,OnValueChanged,&LT1,Bigger,20.0,"Рівень в РВС-1 критично високий");
    alarms->AddAlarm(Warning,OnValueChanged,&LT1,Bigger,30.0,"Рівень в РВС-1 високий");
    alarms->AddAlarm(Critical,OnValueChanged,&LT1,Bigger,40.0,"Рівень в РВС-1 критично високий");
    alarms->AddAlarm(Warning,OnValueChanged,&LT1,Bigger,50.0,"Рівень в РВС-1 високий");
    alarms->AddAlarm(Critical,OnValueChanged,&LT1,Bigger,60.0,"Рівень в РВС-1 критично високий");
    alarms->AddAlarm(Warning,OnValueChanged,&LT1,Bigger,70.0,"Рівень в РВС-1 високий");
    alarms->AddAlarm(Critical,OnValueChanged,&LT1,Bigger,80.0,"Рівень в РВС-1 критично високий");
    alarms->AddAlarm(Warning,OnValueChanged,&LT1,Bigger,85.0,"Рівень в РВС-1 високий");
    alarms->AddAlarm(Critical,OnValueChanged,&LT1,Bigger,90.0,"Рівень в РВС-1 критично високий");

    alarms->AddAlarm(Critical,OnQualityChanged,&LT1,Less,0.5,"Ошибка опроса параметра LT1");
    alarms->AddAlarm(Critical,OnQualityChanged,&LT2,Less,0.5,"Ошибка опроса параметра LT2");

    fireBird= new OdbcDb("fire_kacha1","sysdba","784523");








    this->start();  //начало выполнения потока в функции  void UserCode::run()

}
//==================================================================
UserCode::~UserCode()
{

    //usercode
    //myTrendLogger
    //myModbus
    //

    delete fireBird;
    delete myModbus;
    delete myTrendLogger;
    delete alarms;
}
//==================================================================

//==================================================================
void UserCode::run()  //Основной поток
{

    while(1)
    {


       VolFlow.SetValueQuality(50+5*(float)qrand()/RAND_MAX,true);
       MassFlow.SetValueQuality(40+5*(float)qrand()/RAND_MAX,true);
       FlowNORD.SetValueQuality(50+5*(float)qrand()/RAND_MAX,true);
       ObvodnOpos.SetValueQuality(20+2*(float)qrand()/RAND_MAX,true);
       Dens.SetValueQuality(855+10*(float)qrand()/RAND_MAX,true);
       Tisk.SetValueQuality(10+0.5*(float)qrand()/RAND_MAX,true);
       Temp.SetValueQuality(15+0.5*(float)qrand()/RAND_MAX,true);
       Obvodn.SetValueQuality(25+2*(float)qrand()/RAND_MAX,true);


        //kgfg



        if (CheckThreadStop()) {return;}
        Sleep(500);

    }








}
//==================================================================
/*
void UserCode::Event_1h()
{
QString tmp;
tmp.sprintf("INSERT INTO ICHANGES (TABLEKEY, TABLENAME, DST_ID) VALUES (200000, 'TABLENAME', %i)",hours);
fireBird.ExecQuery(tmp);
}
*/
