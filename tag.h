#ifndef TAG_H
#define TAG_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <logger.h>




class Tag
{
public:
//    Tag() {logger=Logger2::Instance();}

//    Logger2 *logger;
    virtual float GetValueForTrend()=0;
    virtual bool GetQuality()=0;
    virtual char* GetTrendName()=0;
};
//=====================================================================
class FloatTag : public QObject, public Tag
{
  Q_OBJECT
private:
    float value;
    bool quality;
    bool tagFirstTimeSet;
    QMutex mutex;
    bool isDiskSavedVar;
public:
    FloatTag();
    FloatTag(const char *Trendname,bool isDiskSavedvar=false);
//    FloatTag(QString Trendname);
    char TrendName[64];
    float GetValue();
    operator float();
    float operator = (const float & other);
    QString ToString(const char *format);
    bool GetQuality();
    float GetValueForTrend();
    char* GetTrendName();
public slots:
    void SetValueQuality(float newValue, bool newQuality);

signals:
    void ValueChanged(float newValue);
    void QualityChanged(bool newQuality);

};

//======================================================================

#endif // TAG_H
