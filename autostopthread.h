
/*
Клас для наследования функциональности
автозавершения потоков при закрытии программы
(при вызове деструкторов объектов)
ИСПОЛЬЗОВАНИЕ:
1. Наследуемся от него
2. В переопределенном методе run() в безопасном месте цикла вставляем проверку
if (CheckThreadStop()) return;

РАБОТАЕТ ЭТО ТАК:
1. При вызове деструктора взводится флаг необходимости остановки потока и
вызывается wait() - функция ожидания завершения потока.
2. Поток доходит до безопасного места проверки CheckThreadStop() и
завершается return-ом.
3. Происходит выход из wait() в деструкторе и он завершает работу.
*/

#ifndef AUTOSTOPTHREAD_H
#define AUTOSTOPTHREAD_H

#include <QThread>

class AutoStopThread: public QThread
{
    Q_OBJECT
 private:
      volatile bool stopping;
 public:
    AutoStopThread(){stopping=false;}
    ~AutoStopThread(){stopping=true;wait();}
    bool CheckThreadStop(){return stopping;}
};
#endif // AUTOSTOPTHREAD_H
