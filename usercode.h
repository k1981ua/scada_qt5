#ifndef USERCODE_H
#define USERCODE_H
#include <QThread>
#include <QTextCodec>
#include "alarm.h"
#include "logger.h"
#include "mainwindow.h"
#include "mymodbus.h"
#include "odbcdb.h"
#include "tag.h"
#include "trendlogger.h"
#include "autostopthread.h"

class UserCode: public AutoStopThread
{
public:
    Logger *logger;
    UserCode();
    ~UserCode();
    void run();
};

#endif // USERCODE_H
