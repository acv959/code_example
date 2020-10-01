#ifndef TIMERINSPERT_H
#define TIMERINSPERT_H

#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QDialog>
#include <QThread>
#include <QEvent>
#include <QEventLoop>


#include <unistd.h>
#include <stdlib.h>

/**
 * @brief TimerINSPERT              - таймер в отдельном потоке
 *
 * Данный таймер работает в отдельном потоке.
 *
 */
class TimerINSPERT : public QTimer
{

       Q_OBJECT
public:

    /**
     * @brief TimerINSPERT          - конструктор
     * @param mutex                 - указатель на объект взаимного исключение
     */
    TimerINSPERT(QMutex *mutex);

    /**
      * @brief start                - pапускает или перезапускает таймер
      * @param msec                 - тайм-аут в мсек миллисекунд
      */
     void start(int msec);

    /**
     * @brief ~TimerINSPERT          - деструктор
     */
    ~TimerINSPERT();

private:

    QMutex      *_mutex;            ///< объект взаимного исключение
    QThread     _thread;            ///< поток
    int         _msec;              ///< тайм-аут в мсек миллисекунд

    /**
     * @brief event                 - предопределённый метод  базового класса
     */
    bool event(QEvent *event);

private slots:

    /**
     * @brief started               - запуск потока
     */
    void started();
};

#endif // TIMERINSPERT_H
