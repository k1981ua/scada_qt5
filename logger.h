#ifndef MESSAGESWINDOW_H
#define MESSAGESWINDOW_H

#include <QDialog>
#include <QColor>
#include <QMutex>
//#include <list>

//using std::list; -// расскомментировать messageList в .h и .cpp
                    // если понадобиться  вести лог досоздания
                    // окна - будет писать в лист, а потом при
                    // создании окна добавлять в окно
//===============================================================
namespace Ui {
    class LoggerWindow;
}
//===============================================================
class LoggerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoggerWindow(QWidget *parent = 0);
    ~LoggerWindow();
public slots:
    void AddMessage(QString what, QColor color);
private:
    Ui::LoggerWindow *ui;
};
//===============================================================
/*
struct Message
{
QString what;
QColor color;
};
*/
//=============================================================
class Logger: public QObject
{
    Q_OBJECT
private:
    Logger(){}
    ~Logger();
private:
    static Logger* theSingleInstanceLogger;
    static LoggerWindow *theSingleInstanceLoggerWindow;
//    static list<Message> messageList;
    static QMutex mutex;
public:
        static Logger* Instance();

        static LoggerWindow* InstanceWindow();

        void AddLog(QString what, QColor color);

signals:
    void signalAddMessage(QString what, QColor color);

};

//=============================================================
#endif // MESSAGESWINDOW_H
