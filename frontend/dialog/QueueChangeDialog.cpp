#include "QueueChangeDialog.h"
#include "ui_QueueChangeDialog.h"

QueueChangeDialog::QueueChangeDialog(QTreeWidgetItem *item, QWidget *parent) :
    QDialog(parent), _ui(new Ui::QueueChangeDialog), _item(item){
    _ui->setupUi(this);



}

QString QueueChangeDialog::getPeriod(){

    return _ui->cmb_period->currentText();
}

QueueChangeDialog::~QueueChangeDialog(){


    delete _ui;
}

void QueueChangeDialog::accept(){

//    qDebug()<<__FUNCTION__;

    if (_ui->cmb_period->currentIndex() == 0) {

         SSS_AND_RETURN(_ui->cmb_period,QComboBox,pink );

    } else {

        SSS(_ui->cmb_period,QComboBox,lime );
    }


    QString sqlQuery    = "";
    std::string error   = "";


    HelperDAssAdmin::Data result;


    sqlQuery = QString("UPDATE tObjects SET queue_id='%0' WHERE id='%1' ").arg( _ui->cmb_period->currentData().toString(),
                                                                                _item->data(0, HelperDAssAdmin::ID_ROLE).toString() );


    GetGPQConn::get()->execQuery(   MODEL,
                                    sqlQuery.toStdString(),
                                    result,
                                    error);

    if( ! error.empty()){


        throw std::string(error);
    }

    QDialog::accept();
    return;
}
