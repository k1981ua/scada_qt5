#include "logger.h"
#include "ui_logger.h"
#include <QDateTime>


Logger* Logger::theSingleInstanceLogger=NULL;
LoggerWindow* Logger::theSingleInstanceLoggerWindow=NULL;
//list<Message> Logger2::messageList;
QMutex Logger::mutex;

//====================================================================
LoggerWindow::LoggerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoggerWindow)
{
    ui->setupUi(this);

    connect(ui->buttonClose,SIGNAL(clicked()),SLOT(close()));
}
//=====================================================================
LoggerWindow::~LoggerWindow()
{
    delete ui;
}
//=====================================================================
void LoggerWindow::AddMessage(QString what,QColor color)
{

    QString strDateTime;
    QDateTime dt;
    dt=QDateTime::currentDateTime();
    strDateTime.sprintf("%.2u.%.2u.%.4u %.2u:%.2u:%.2u.%.3u",
                dt.date().day(),dt.date().month(),dt.date().year(),
                dt.time().hour(),dt.time().minute(),dt.time().second(),dt.time().msec());

    ui->listWidget->addItem(strDateTime+"  "+what);
    if (ui->listWidget->count()>200)
    {
        ui->listWidget->removeItemWidget(ui->listWidget->takeItem(0));
    }
ui->listWidget->scrollToBottom();
ui->listWidget->item(ui->listWidget->count()-1)->setForeground(color);

}
//=====================================================================
//=====================================================================
Logger::~Logger(){delete theSingleInstanceLogger;
               delete theSingleInstanceLoggerWindow;}

//=====================================================================
Logger* Logger::Instance()
{
mutex.lock();
if(theSingleInstanceLogger==NULL) theSingleInstanceLogger=new Logger;
mutex.unlock();
return theSingleInstanceLogger;
}
//=====================================================================
LoggerWindow* Logger::InstanceWindow()
{
mutex.lock();
if(theSingleInstanceLoggerWindow==NULL)
      {
           theSingleInstanceLoggerWindow=new LoggerWindow;
           connect(theSingleInstanceLogger,SIGNAL(signalAddMessage(QString,QColor)),theSingleInstanceLoggerWindow,SLOT(AddMessage(QString,QColor)));

//                foreach(Message msg_tmp,messageList)
//                {
//                theSingleInstanceLoggerWindow->AddMessage(msg_tmp.what,msg_tmp.color);
//                }
//                messageList.clear();
       }
       mutex.unlock();
return theSingleInstanceLoggerWindow;
}
//=====================================================================
void Logger::AddLog(QString what, QColor color)
{
//            mutex.lock();
//            if (theSingleInstanceLoggerWindow==NULL)
//            {
//             Message tmp_msg;
//             tmp_msg.what=what;
//             tmp_msg.color=color;
//             messageList.push_back(tmp_msg);
//            }
//            else
//            {
emit signalAddMessage(what, color);
//            }
//            mutex.unlock();
}
//=====================================================================
