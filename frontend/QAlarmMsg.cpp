#include "QAlarmMsg.h"


void QAlarmMsg::msgInfo(QString text,QWidget *parent){

    QString title=tr("Information");
    QString text_button=tr("Ok");
    QMessageBox::information(parent,title,text,text_button,0,0);

}

void QAlarmMsg::msgWarn(QString text, QWidget *parent){

    QString title = tr("Warning");
    QString text_button=tr("Cancel");
    QMessageBox::warning(parent,title,text,text_button,0,0);

}

void QAlarmMsg::msgErr(QString text, QWidget *parent){

    QString title = tr("Error");
    QString text_button=tr("Cancel");
    QMessageBox::critical(parent,title,text,text_button,0,0);


}

bool QAlarmMsg::msgQuest   (QString text,QWidget *parent){

    QString title = tr("To make decisions");
    int reply = QMessageBox::question(parent,title,text,QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        return true;
    }
    return false;
}
