
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QMessageBox>
#include <QTime>
#include <QProcess>
#include <QPixmap>
#include <QBitmap>
#include <QProgressBar>
//#include <QMotifStyle>
#include <QTextCodec>
#include "mymodbus.h"
#include <stdio.h>
#include "tag.h"
#include "trendlogger.h"
#include "alarm.h"
#include <QTimer>
#include "odbcdb.h"
#include <QDoubleValidator>
#include <logger.h>


extern Alarms *alarms;
extern FloatTag LT1,LT2;
extern FloatTag Temp;
extern FloatTag DiskVar;

extern FloatTag VolFlow;
extern FloatTag MassFlow;
extern FloatTag FlowNORD;
extern FloatTag ObvodnOpos;
extern FloatTag Dens;
extern FloatTag Tisk;
extern FloatTag Temp;
extern FloatTag Obvodn;


ThreadTimeUpdate myTimeUpdater;
QTimer updateFormTimer;

//===================================================================

void ThreadTimeUpdate::run()
{
    QTime mtime;
    mtime.start();
    int sec_prev=-1;
    int sec;
    //int hour_prev=mtime.hour();

    while (1)
    {
        mtime.restart();
        sec=mtime.second();

        if (sec_prev!=sec) emit timechange(mtime.hour(),mtime.minute(),sec);
        sec_prev=sec;
        //if (hour_prev!=mtime.hour()) emit event1hSignal(mtime.hour());
        //hour_prev=mtime.hour();
        Sleep(50);
        if (CheckThreadStop()) return;
    }
}

//===================================================================
void MainWindow::updateForm()
{
   //Temp=LT1+50;

    ui->label_float_modbus->setText(LT1.ToString("%.3f"));

    ui->label_float_modbus2->setText(LT2.ToString("%.2f"));

    //ui->progressBar->setValue(LT1);
    //QPalette pal=ui->progressBar->palette();
    //pal.setColor(QPalette::Highlight,Qt::black);
    //ui->progressBar->setPalette(pal);

    DiskVar=int(DiskVar+1) % 10000;
    ui->lcdNumber->display(DiskVar);

QString tmp;

    ui->lineEdit_VolFlow->setText(tmp.sprintf("%.2f",VolFlow.GetValue())+" м3/г");
    ui->lineEdit_MassFlow->setText(tmp.sprintf("%.2f",MassFlow.GetValue())+" т/г");
    ui->lineEdit_FlowNORD->setText(tmp.sprintf("%.2f",FlowNORD.GetValue())+" м3/г");
    ui->lineEdit_Dens->setText(tmp.sprintf("%.2f",Dens.GetValue())+" кг/м3");
    ui->lineEdit_ObvodnOpos->setText(tmp.sprintf("%.2f",ObvodnOpos.GetValue())+" %");
    ui->lineEdit_Tisk->setText(tmp.sprintf("%.2f",Tisk.GetValue())+" кгс/см2");
    ui->lineEdit_Temp->setText(tmp.sprintf("%.2f",Temp.GetValue())+" гр.С");
    ui->lineEdit_Obvodn->setText(tmp.sprintf("%.2f",Obvodn.GetValue())+" %");


}
//===================================================================

//===================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logger=Logger::Instance();


    connect(&myTimeUpdater,         SIGNAL(timechange(int,int,int)),SLOT(timeChanged(int,int,int)));
    connect(ui->buttonTrendRVS1,    SIGNAL(clicked()),              SLOT(buttonTrendRVS1_clicked()));
    //connect(ui->buttonTrendRVS2,    SIGNAL(clicked()),              SLOT(buttonTrendRVS2_clicked()));
    connect(ui->buttonReports,      SIGNAL(clicked()),              SLOT(buttonReports_clicked()));

    connect(ui->buttonMessagesShow,SIGNAL(clicked()),SLOT(buttonMessagesShow_clicked()));

    connect(ui->buttonAlarmsConfirm, SIGNAL(clicked()),alarms,SLOT(Kvitirovat2()));

    connect(alarms,SIGNAL(EnabledAlarmsChanged(list<Alarm*>*,bool)),SLOT(alarmsChanged(list<Alarm*>*,bool)));

    connect(&updateFormTimer,SIGNAL(timeout()),SLOT(updateForm()));
    updateFormTimer.start(500);

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));



    myTimeUpdater.start(QThread::InheritPriority);

scene = new QGraphicsScene(this);
ui->graphicsView->setScene(scene);

//ui->graphicsView->showFullScreen();
//ui->graphicsView_2->setScene(scene);
pixmap=new QPixmap("E:\\ALL_PROJECTS\\SCADA\\new_massomer\\koziiv.bmp");
//pixmap->setMask(pixmap->createMaskFromColor(Qt::black, Qt::MaskInColor));
pixmapItem=scene->addPixmap(*pixmap);
//ui->progressBar->setStyle(new QMotifStyle() );
showFullScreen();
ui->graphicsView->fitInView(0,0,pixmap->width(),pixmap->height());
MyDoubleValidator *doubleValidator= new MyDoubleValidator(0,100,2,this);
ui->lineEdit->setValidator(doubleValidator);


}
//===================================================================
void MainWindow::buttonMessagesShow_clicked()
{
    logger->InstanceWindow()->setGeometry(this->width()-logger->InstanceWindow()->width(),
                                //messagesWindow->geometry().y()- messagesWindow->pos().y(),
                                40,
                                logger->InstanceWindow()->width(),
                                logger->InstanceWindow()->height());

    logger->InstanceWindow()->setWindowFlags(Qt::WindowStaysOnTopHint);
    logger->InstanceWindow()->show();

}
//===================================================================
void MainWindow::closeEvent(QCloseEvent *)
{
logger->InstanceWindow()->close();

}
//===================================================================
void MainWindow::timeChanged(int hours, int minutes, int seconds)
{
    QString temp;
    temp.sprintf("%.2i:%.2i:%.2i",hours,minutes,seconds);
    ui->label->setText(temp);
}
//===================================================================

//===================================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//===================================================================
void MainWindow::buttonTrendRVS1_clicked()
{
    QProcess myProcess;
    myProcess.startDetached(QString("trend_view5.exe ")+QApplication::applicationDirPath()+"\\talal1.ini");
}
//===================================================================
void MainWindow::buttonTrendRVS2_clicked()
{
    QProcess myProcess;
    myProcess.startDetached(QString("trend_view5.exe ")+QApplication::applicationDirPath()+"\\talal1.ini");
}
//===================================================================
void MainWindow::buttonReports_clicked()
{
    QProcess myProcess;
    myProcess.startDetached(QString("cpp_reporter.exe bytkiv_rep.ini"));
}
//=====================================================================
void MainWindow::alarmsChanged(list < Alarm* > *pEnabledAlarmList, bool onlyColorChange)
{

    int i=0;

    if (!onlyColorChange)
    {
    ui->alarmWidget->clear();
        foreach(Alarm *alarm, *pEnabledAlarmList)
        {

            if (i<5)
            {
            ui->alarmWidget->addItem(alarm->GetDateTimeOfAlarm()+"   "+alarm->GetTextOfAlarm()+
                                ((alarm->IsActive())?"    (АКТИВН ,":"    (НЕАКТИВ ,")+
                                ((alarm->IsConfirmed())?" КВИТ)":" НЕКВИТ)")
                                );
            ui->alarmWidget->item(i)->setBackground(alarm->actColor);
            }
        ++i;
        }
    }
    else
    {
        foreach(Alarm *alarm, *pEnabledAlarmList)
        {

            if (i<5)
            {
                if (!alarm->IsConfirmed())
                {
                    if (ui->alarmWidget->item(i)->background()==alarm->actColor)
                    {
                        ui->alarmWidget->item(i)->setBackground(alarm->act2Color);
                    }
                    else
                    {
                        ui->alarmWidget->item(i)->setBackground(alarm->actColor);
                    }
                }
            }
        ++i;
        }

    }
}
//=====================================================================
