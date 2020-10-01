#include "DialogAddingValuationObject.h"
#include "ui_DialogAddingValuationObject.h"

namespace DAVO {

    void getObjects(const QString & idRef, std::vector<std::vector<Cell> > &result)throw(std::string){

        QString sqlQuery    = "";
        std::string error   = "";



        sqlQuery = QString("SELECT id, name, assess_flag  FROM tObjects WHERE id_ref='%0' ORDER BY id ").arg(idRef);

        //qDebug()<< "sqlQuery = "<< sqlQuery;
        GetGPQConn::get()->execQuery(   MODEL,
                                        sqlQuery.toStdString(),
                                        result,
                                        error);

        if( ! error.empty()){


            throw std::string(error);
        }

           return;
    }


    void addChildren(QTreeWidgetItem *parent ){

        std::vector<std::vector<Cell> > result;
        getObjects(parent->data(0, HelperDAssAdmin::ID_ROLE).toString(), result);


        QTreeWidgetItem *item = NULL;


        for (std::vector<std::vector<Cell> >::iterator  it  = result.begin();
                                                        it != result.end(); ++it) {

            item = new QTreeWidgetItem(parent);

            item->setText(0, QString::fromStdString((*it).at(1).value ));
            item->setData(0, HelperDAssAdmin::ID_ROLE, QString::fromStdString((*it).at(0).value ) );

            if((*it).at(2).value == "f"){

                item->setData(0, Qt::CheckStateRole, Qt::Unchecked );
            }

            addChildren(item);


        }
        return;
    }



    void saveChange(QTreeWidgetItem *parent){

        long long int size = parent->childCount();

        QTreeWidgetItem *item = NULL;
        for (long long int i = 0; i < size; ++i) {

            item  = parent->child(i);

            if( item->data(0, Qt::CheckStateRole).isValid() ){

                if (item->data(0, Qt::CheckStateRole).toInt() == Qt::Checked ) {

//                    HelperDAssAdmin::setAssessFlag( item->data(0, HelperDAssAdmin::ID_ROLE).toString(),
//                                                    true);
                }
            }

            DAVO::saveChange(item);
        }

    }

}

DialogAddingValuationObject::DialogAddingValuationObject(QWidget *parent) :
    QDialog(parent), _ui(new Ui::DialogAddingValuationObject){

    _ui->setupUi(this);


    std::vector<std::vector<Cell> > result;
    DAVO::getObjects("-2", result);


    QTreeWidgetItem *item = NULL;

    for (std::vector<std::vector<Cell> >::iterator  it  = result.begin();
                                                    it != result.end(); ++it) {

        item = new QTreeWidgetItem(_ui->treeWidget->invisibleRootItem());


        item->setText(0, QString::fromStdString((*it).at(1).value ));
        item->setData(0, HelperDAssAdmin::ID_ROLE, QString::fromStdString((*it).at(0).value ) );

        if( (*it).at(2).value == "f" ){

            item->setData(0, Qt::CheckStateRole, Qt::Unchecked );
        }

        DAVO::addChildren(item);
    }

}



DialogAddingValuationObject::~DialogAddingValuationObject(){

    delete _ui;
}

void DialogAddingValuationObject::saveChange(){

    DAVO::saveChange(_ui->treeWidget->invisibleRootItem());
    return;
}




