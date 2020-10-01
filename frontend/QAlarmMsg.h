#ifndef QALARMMSG_H
#define QALARMMSG_H

#include <iostream>

#include <QDialog>
#include <QMessageBox>





class QAlarmMsg: public QObject {

     Q_OBJECT

    public:
     static void msgInfo    (QString text,QWidget *parent);
     static void msgWarn    (QString text,QWidget *parent);
     static void msgErr     (QString text,QWidget *parent);
     static bool msgQuest   (QString text,QWidget *parent);
};

#endif // QALARMMSG_H
