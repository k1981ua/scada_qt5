#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QGraphicsScene>
#include "alarm.h"
#include <list>
#include <QString>
#include "logger.h"
#include <QDoubleValidator>
#include "autostopthread.h"

using std::list;

namespace Ui {
    class MainWindow;
}
//=================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void closeEvent(QCloseEvent *);
     Logger *logger;
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    QPixmap *pixmap;
public slots:
    void updateForm();
    void buttonReports_clicked();
    void buttonTrendRVS1_clicked();
    void buttonTrendRVS2_clicked();
    void buttonMessagesShow_clicked();
    void timeChanged(int hours,int minutes, int seconds);
    void alarmsChanged(list < Alarm* > *pEnabledAlarmList, bool onlyColorChange);
};

//=================================================================
class ThreadTimeUpdate : public AutoStopThread
{
   Q_OBJECT
public:
    void run();
signals:
    void timechange(int hours,int minutes,int seconds);
};
//=================================================================
class MyDoubleValidator : public QDoubleValidator
{
public:
    MyDoubleValidator( double bottom, double top, int decimals, QObject*
        parent = 0)
        : QDoubleValidator( bottom, top, decimals, parent)
    {}

    QValidator::State validate ( QString & input, int &pos ) const
    {
        if ((this->bottom() < 0 && (input.isEmpty() || input == "." || input == "-" || input == "-."))
                    || (this->bottom() >= 0 && (input.isEmpty() || input == ".")))
            return Intermediate;
        if ( QDoubleValidator::validate( input, pos ) != Acceptable ) {
            return Invalid;
        }
        return Acceptable;
    }
};
//================================================================






#endif // MAINWINDOW_H
