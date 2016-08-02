#ifndef TRENDLOGGER_H
#define TRENDLOGGER_H


#include <vector>
#include <windows.h>
#include "tag.h"
#include "autostopthread.h"
#include "logger.h"

using std::vector;

//==================================================================
/*
struct TrendTag
{
Tag *trendTag;
char trendName[32];
};
*/
//===================================================================
class TrendLogger: public AutoStopThread
{
    Q_OBJECT
 private:
 //   vector<TrendTag> VectTrendTag;
      vector<Tag*> VectTrendTag;

 public:
      Logger *logger;
    TrendLogger();
    TrendLogger(const char *trend_path);
     void run();
     char trendPath[256];
     float empty_file[17280];
     static const float min_float=-3.4028234663852886e+38;        //минимальное число float, недостоверное значение
     void AddTrendTag(Tag* trendtag);
};

//==================================================================

#endif // TRENDLOGGER_H
