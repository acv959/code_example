#include "DAssAdmin.h"
#include "ui_DAssAdmin.h"

#define ENAB_T(obj) obj->setEnabled(true);
#define ENAB_F(obj) obj->setEnabled(false);

#define HIDE(obj) obj->hide();
#define SHOW(obj) obj->show();

#define FORMS_CLEANING                         _script_bac     = "";\
\
_ui->le_last_data->setText("");\
_ui->le_condition->setText("");\
\
_ui->script_edit->setText("");\
\
\
ENAB_F(_ui->le_condition);\
ENAB_F(_ui->le_last_data);\
\
ENAB_F(_ui->l_condition);\
ENAB_F(_ui->l_last_data);\
\
\
ENAB_F(_ui->l_oe);\
ENAB_F(_ui->ckb_oe);\
_ui->ckb_oe->setChecked(false);\
\
\
ENAB_F(_ui->pbtn_apply_script);\
ENAB_F(_ui->pbtn_change_queue);\
ENAB_F(_ui->pbtn_edit_script);\
ENAB_F(_ui->pbtn_save_rule);\



#define  SMR     if(_script_bac != _ui->script_edit->toPlainText()  ){\
\
if(_current_eo.isValid() && QAlarmMsg::msgQuest(tr("Save modified rule?"), this) ){\
\
\
    try {\
\
\
        HelperDAssAdmin::saveScript(_ui->cmp_ontology->currentData().toString() ,\
                                    _current_eo.uri(),\
                                    _ui->script_edit->toPlainText() );\
\
\
    } catch (std::string error) {\
\
        QAlarmMsg::msgErr(QString::fromStdString(error), this);\
\
    }\
\
\
\
}\
\
}\





DAssAdmin::DAssAdmin(QWidget *parent) :
    QWidget(parent),_script_bac(""), _ui(new Ui::DAssAdmin),
_objects_tree_menu(new QMenu(NULL)), _da_conn(nullptr), _timer(nullptr), _current_item(nullptr),
  _cur_modules_tree(nullptr){

    _ui->setupUi(this);



    _ui->script_edit->setReadOnly(true);

    HIDE(_ui->pbtn_save_rule)

    ENAB_F(_ui->le_condition);
    ENAB_F(_ui->le_last_data);

    ENAB_F(_ui->l_condition);
    ENAB_F(_ui->l_last_data);

    ENAB_F(_ui->l_oe);
    ENAB_F(_ui->ckb_oe);


    ENAB_F(_ui->pbtn_apply_script);
    ENAB_F(_ui->pbtn_change_queue);
    ENAB_F(_ui->pbtn_edit_script);
    ENAB_F(_ui->pbtn_save_rule);

    HIDE(_ui->pbtn_apply_script);
    HIDE(_ui->pbtn_change_queue);

    HIDE(_ui->w_navi_not_work);

    _timer = new TimerINSPERT(&_mutex);

    connect(_timer, SIGNAL( timeout() ),
            this, SLOT(onTimeout() ) );



    connect(_ui->ckb_oe, SIGNAL( clicked(bool) ),
            this, SLOT(onClicked(bool)  ) );

    _timer->start(15000);
}

bool DAssAdmin::init(){

    // init server checker
    _ui->w_main->setCheckByServ("dasssrv");

    connect(_ui->w_main, SIGNAL(setConnStatus(bool)),
            this, SLOT(onSetConnStatus(bool)));

    _ui->w_main->onCheckConn(); // immidiatly check connection


    std::map<std::string, Model*> *modelMap = GetModel::getModelMap();

    Model *m = nullptr;
    for(auto &modelVal: *modelMap){

        m = modelVal.second;
        if ( !  m->getObject(HelperDAssAdmin::T_SCRIPTS_SYS)  ) {

            continue;
        }

        QString model_id = QString::fromStdString( m->getId() );
        QString model_name = QString::fromStdString( m->getName() );

        _ui->cmp_ontology->addItem( model_name, model_id);

        ModulesTree *mt = new ModulesTree(this);
        _modules_tree_map.insert(model_id, mt);

        mt->setHeaderHidden(true);
        mt->setDragDropMode(QAbstractItemView::DragOnly);

        mt->onLoad();

        Hierarchy *h_model = new Hierarchy(*(m->getHierarchy()));

        for (std::vector<Hierarchy*>::iterator it = h_model->getChilds()->begin();
             it!=h_model->getChilds()->end();++it) {
            (*it)->setLink(true);
        }
        mt->addHierarchleElem(h_model);

//        mt->setIconSize(QSize( 48,48) );

        QList<QAction*> actions = mt->actions();
        for (int i = 0; i < actions.size(); ++i) {

            mt->removeAction(actions.at(i));
        }

        _ui->vl_credentials->addWidget(mt);
        mt->setVisible(false);

        connect(mt, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
                this, SLOT(onItemPressed(QTreeWidgetItem*,int) ) );
    }

    if (_modules_tree_map.empty()) {
        SHOW(_ui->w_navi_not_work);
        HIDE(_ui->w_navi_work);
    }


    connect(_ui->cmp_ontology, SIGNAL(activated(int)),
            this, SLOT(onSelectCurOntology(int)));


    if (_ui->cmp_ontology->count())
        _ui->cmp_ontology->emitSigmalActivated( _ui->cmp_ontology->currentIndex() );

    return true;
}


DAssAdmin::~DAssAdmin(){

    delete _ui;
    delete _objects_tree_menu;
    delete _da_conn;
}



void DAssAdmin::onSelectCurOntology(int index){


    QString modelId = _ui->cmp_ontology->itemData(index).toString();

    _ui->w_ont_search->setSearchModel(GetModel::getModel(modelId.toStdString()));

    if(_modules_tree_map.count(modelId) ){

        if(_cur_modules_tree){

            _cur_modules_tree->hide();
        }

        _cur_modules_tree = _modules_tree_map.value(modelId);
        _cur_modules_tree->show();
    }

    return;
}

void DAssAdmin::onCustomContextMenuRequested(QPoint pos){



}

void DAssAdmin::onClicked(bool checked){

    Q_UNUSED(checked)

    QObject *sender = this->sender();

    if(sender == _ui->pbtn_edit_script){

        qDebug()<<"_ui->pbtn_edit_script";
        if (! _ui->script_edit->isReadOnly()  ) {

            if(_current_eo.isValid()  ){

                HelperDAssAdmin::saveScript(_ui->cmp_ontology->currentData().toString() ,
                                            _current_eo.uri(),
                                            _ui->script_edit->toPlainText() );
            }

            _ui->groupBox->setTitle(tr("Evaluation Rules(view mode)"));
            _ui->script_edit->setReadOnly(true);
            _ui->pbtn_edit_script->setText(tr("Edit rule"));
            _ui->pbtn_edit_script->setDown(false);
            _ui->pbtn_edit_script->setIcon(QIcon (QStringLiteral(":/img/edit.png") ) );

            _script_bac = _ui->script_edit->toPlainText();
            _ui->script_edit->setTextColor( QColor( Qt::gray ) );
//            _ui->pbtn_save_rule->hide();

        }else{

            _ui->groupBox->setTitle(tr("Evaluation Rules(edit mode)"));
            _ui->script_edit->setReadOnly(false);
            _ui->pbtn_edit_script->setText(tr("Save rule"));
            _ui->pbtn_edit_script->setDown(true);
            _ui->pbtn_edit_script->setIcon(QIcon (QStringLiteral(":/img/save.png") ) );

            _script_bac = _ui->script_edit->toPlainText();
            _ui->script_edit->setTextColor( QColor( ) );

//            _ui->pbtn_save_rule->show();
        }

        _ui->script_edit->clear();
        _ui->script_edit->append(_script_bac);


    }else if(sender == _ui->pbtn_change_queue){

        qDebug()<<"_ui->pbtn_change_queue";

        QMutexLocker ml(&_mutex);




    }else if(sender == _ui->pbtn_apply_script){

        qDebug()<<"_ui->pbtn_apply_script";
        QAlarmMsg::msgWarn("Under construction", this);

    }else if(sender == _ui->pbtn_save_rule){

        qDebug()<<"_ui->pbtn_save_rule";
        QAlarmMsg::msgWarn("Under construction", this);

    }else if(sender == _ui->ckb_oe && _current_eo.isValid()){

        qDebug()<<"_ui->ckb_oe"<<" checked = " << checked;

        try {

            HelperDAssAdmin::setAssessFlag(_ui->cmp_ontology->currentData().toString(),
                                           _current_eo.uri(),
                                           checked);
        } catch (std::string error) {

            FORMS_CLEANING
            QAlarmMsg::msgErr(QString::fromStdString(error), this);
            return;
        }



        if ( ! checked) {

            FORMS_CLEANING

            ENAB_T(_ui->l_oe);
            ENAB_T(_ui->ckb_oe);

        } else if(_current_item){

            this->onItemPressed(_current_item, 0);
        }
    }


    return;
}


void DAssAdmin::onTextEdited(const QString &str){



    return;
}

void searchDisclosed(QMap<QString, bool> &map, QTreeWidgetItem *root ){

    int size = root->childCount();
    QTreeWidgetItem *item = NULL;
    for (int i = 0; i < size; ++i) {

        item = root->child(i);
        if(item->isExpanded() ){

            map.insert(item->data(0, HelperDAssAdmin::ID_ROLE).toString(),
                       true);

            searchDisclosed(map, item);
        }
    }

    return;
}

void setExpanded(QMap<QString, bool> &map, QTreeWidgetItem *root){

    int size = root->childCount();
    QTreeWidgetItem *item = NULL;
    for (int i = 0; i < size; ++i) {

        item = root->child(i);
        if(map.count(item->data(0, HelperDAssAdmin::ID_ROLE).toString() ) ){

            if(map.value(item->data(0, HelperDAssAdmin::ID_ROLE).toString() ) ){

                item->setExpanded(true);
                setExpanded(map, item);
            }
        }
    }
    return;
}


/**
 * @brief findItem          - поиск элемента среди дочерних элементов корневого элемента
 * @param root              - корневой элемент
 * @param item              - элемент поиска
 * @return                  - true в случае если элемент найден среди элементов корневого элемента
 */
bool  findItem(QTreeWidgetItem *root, QTreeWidgetItem *item){


    int childCount = root->childCount();

    QTreeWidgetItem *child = NULL;
    for (int i = 0; i < childCount; ++i) {

        child = root->child(i);
        if(child->data(0, HelperDAssAdmin::ID_ROLE) ==
           item->data(0, HelperDAssAdmin::ID_ROLE) ){

            return true;

        }else if (findItem(child, item) ) {

            return true;
        }
    }

    return false;
}



/**
 * @brief findItem      - поиск элемента по индетифакоту среди дочерних элементов корневого элемента
 * @param root          - корневой элемент
 * @param id            - строка содержащая идентификатор элемента
 * @return              - NULL в случае если элемент не найден
 */
QTreeWidgetItem *findItem(QTreeWidgetItem *root, const QString &id){

    int childCount = root->childCount();

    QTreeWidgetItem *child = NULL;
    for (int i = 0; i < childCount; ++i) {

        child = root->child(i);
        if(child->data(0, HelperDAssAdmin::ID_ROLE) ==  id ){

            return child;

        }else  {

            child = findItem(child, id) ;

            if(child){

                return child;
            }
        }
    }

    return NULL;
}

/**
 * @brief removeParent          - удалить родителя если у него нет потомков
 * @param root                  - корневой элемент
 * @param parent                - родитель
 */
void removeParent(QTreeWidgetItem *root, QTreeWidgetItem *parent){

    if(parent->childCount() == 0 && ! parent->data(0, HelperDAssAdmin::ASSESS_FLAG_ROLE).toBool()){

        if(QTreeWidgetItem *pp =  parent->parent() ){

            pp->removeChild(parent);
            removeParent(root, pp);

        }else{

            root->removeChild(parent);
        }

        delete parent;
    }


    return;
}




void DAssAdmin::onTrigger(){


    return;
}


inline QString getTextState(const QMap<QString,QString> & valsMap){

    QString text = "";
    switch (valsMap.value("state").toInt()) {
    case -1:

            text = QObject::tr("Evaluation error - %0").arg(valsMap.value("error"));
        break;

    case 0:

            text = QObject::tr("Crash");

        break;


    case 1:

            text = QObject::tr("Alarm");

        break;

    case 2:

            text = QObject::tr("Norm");

        break;
    case 10:

            text = QObject::tr("Indefinitely");

        break;
    default:

        text = QObject::tr("Unknown status code '%0'").arg(valsMap.value("state"));
        break;
    }

    return text;
}




void DAssAdmin::onItemPressed(QTreeWidgetItem *item, int column){

    Q_UNUSED(column)

    QMutexLocker ml(&_mutex);

    if( !item->data(column,ModulesTree::TREE_ITEM_IS_OBJECT_DATA).toBool() ){

        SMR

        FORMS_CLEANING
        _current_item = nullptr;
        _current_eo   = HelperDAssAdmin::EstimationObject();
        return;
    }


    if(item  != _current_item){

        SMR
    }

    ObjectData od = item->data(column,ModulesTree::TREE_ITEM_OBJECT_DATA).value<ObjectData>();

    Model *model  = od.getObject()->getModel();

    try {

        _current_eo =  HelperDAssAdmin::getEstimationObject(QString::fromStdString(model->getId() ),
                                                            QString::fromStdString(od.getId() ) );
        _current_item = item;


    } catch (std::string error) {

        FORMS_CLEANING
        _current_item = nullptr;
        _current_eo   = HelperDAssAdmin::EstimationObject();
        QAlarmMsg::msgErr(QString::fromStdString(error), this);

    }



//    qDebug()<<"_current_eo.assessFlag() = " << _current_eo.assessFlag();
//    qDebug()<<"_script_bac = " << _script_bac;
//    qDebug()<<"_ui->script_edit->toPlainText() = " << _ui->script_edit->toPlainText();



    if (! _ui->script_edit->isReadOnly()  ) {

        _ui->groupBox->setTitle(tr("Evaluation Rules(view mode)"));
        _ui->script_edit->setReadOnly(true);
        _ui->pbtn_edit_script->setText(tr("Edit rule"));
        _ui->pbtn_edit_script->setDown(false);
        _ui->pbtn_edit_script->setIcon(QIcon (QStringLiteral(":/img/edit.png") ) );

        _script_bac = _ui->script_edit->toPlainText();
        _ui->script_edit->setTextColor( QColor( Qt::gray ) );
    }

    if( ! _current_eo.isValid() || ! _current_eo.assessFlag() ){

        FORMS_CLEANING

        ENAB_T(_ui->l_oe);
        ENAB_T(_ui->ckb_oe);


    }else{

        ENAB_T(_ui->le_condition);
        ENAB_T(_ui->le_last_data);

        ENAB_T(_ui->l_condition);
        ENAB_T(_ui->l_last_data);

        ENAB_T(_ui->l_oe);
        ENAB_T(_ui->ckb_oe);

        ENAB_T(_ui->pbtn_apply_script);
        ENAB_T(_ui->pbtn_change_queue);
        ENAB_T(_ui->pbtn_edit_script);
        ENAB_T(_ui->pbtn_save_rule);


        try {


            QMap<QString, QString> valsMap  = _current_eo.getValues();



            _ui->le_last_data->setText(valsMap.value("date") );
            _ui->le_condition->setText(getTextState(valsMap) );
            _ui->ckb_oe->setChecked(_current_eo.assessFlag() );


            _ui->script_edit->setTextColor(Qt::darkGray);


            if (_ui->script_edit->isReadOnly()  ) {

                _ui->script_edit->setTextColor( QColor( Qt::gray ) );

            }else {

                _ui->script_edit->setTextColor( QColor( ) );

            }

            _ui->script_edit->clear();
            _ui->script_edit->append(_current_eo.script()  );
            _script_bac = _ui->script_edit->toPlainText();

        } catch (std::string error) {

            InstanceLogger::instance()->error("DAssAdmin::onItemPressed: %s", error.c_str() );

            QAlarmMsg::msgErr(QString::fromStdString(error), this);


            _ui->le_last_data->setText("");
            _ui->le_condition->setText("");

            _ui->script_edit->setText("");

            _script_bac = "";

            _current_item = nullptr;
            _current_eo   = HelperDAssAdmin::EstimationObject();
        }

    }
    //onItemPressed(QTreeWidgetItem,int)

    return;
}



void DAssAdmin::onTimeout(){

    //qDebug()<<"onTimeout";

    QMutexLocker ml(&_mutex);


    if(_current_eo.isValid()  /*&& !_ui->le_condition->text().isEmpty()*/ ){

        try {

            qDebug()<<"updata";
            //QString id = _current_item->data(0, HelperDAssAdmin::ID_ROLE).toString();

            QMap<QString, QString> valsMap  = _current_eo.getValues();





            _ui->le_last_data->setText(valsMap.value("date") );
            _ui->le_condition->setText(getTextState(valsMap) );



        } catch (std::string error) {

            InstanceLogger::instance()->error("DAssAdmin::onItemPressed: %s", error.c_str() );

            QAlarmMsg::msgErr(QString::fromStdString(error), this);
        }
    }

    return;
}

void DAssAdmin::onShowObject(QString){

    return;
}

void DAssAdmin::onShowObjectData(QString od_id){

    ObjectData *od = ObjectData::fromId(od_id.toStdString());

    if (od->isValid() == false) {

        QMessageBox::critical(this, tr("Error"),
                              tr("Can't construct object data from id '%1' ").arg(od_id));
        return;
    }

    Object *o = od->getObject();
    if (o == nullptr) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Can't get object pointer '%1' from object data"));
        return;
    }

    Model *m = o->getModel();
    if (m == nullptr) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Can't get model from object '%1'").arg(o->getId().c_str()));
        return;
    }

    Hierarchy *h_root = m->getHierarchy();

    Hierarchy *h_find = nullptr;
    try {
        DataHierarchy::getHierarchly(h_root, Hierarchy::HIERARHLY_TYPE_OBJECT,
                                     m->getId(), o->getId(), "", "");
    } catch (Hierarchy *h) {h_find =h;}

    if (h_find == nullptr) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Can't find hierarchy object for '%1'").arg(o->getId().c_str()));
        return;
    }

    // store object hierarchy here
    // calculate object path
    QList<Hierarchy*> parent_obj_path;
    while (h_find->getParent()) {

        if (h_find->getType() == Hierarchy::HIERARHLY_TYPE_OBJECT) {

            parent_obj_path.push_front(h_find);
        }

        h_find = h_find->getParent();
    }

    // now calculate full path
    std::list<Hierarchy> item_path;

    // first init
    // add last element in path (obj data)
    item_path.push_front(Hierarchy(Hierarchy::HIERARHLY_TYPE_OBJECT_DATA,
                                  "", "", od->getId(), ""));

    // add obj
    item_path.push_front(Hierarchy(Hierarchy::HIERARHLY_TYPE_OBJECT,
                                  "", od->getObject()->getId(), "", ""));

    // remove already added
    Hierarchy *cur_h = parent_obj_path.back();
    parent_obj_path.pop_back();

    ObjectData *cur_od = od;

    while(parent_obj_path.isEmpty() == false) {

        Hierarchy *parent_h = parent_obj_path.back();
        parent_obj_path.pop_back();

        Relation *r = Hierarchy::getRelation(parent_h, cur_h);


        if (r) {

            std::string from_attr = r->getFrom()->at(0).attr->getId();

            cur_od = cur_od->getRelationObjectData(from_attr);

            item_path.push_front(Hierarchy(Hierarchy::HIERARHLY_TYPE_OBJECT_DATA,
                                          "", "", cur_od->getId(), ""));

            item_path.push_front(Hierarchy(Hierarchy::HIERARHLY_TYPE_OBJECT,
                                          "", r->getToObject()->getId(), "", ""));
        }

        else {

            QMessageBox::critical(this, tr("Error"),
                                  tr("Can't find relation from object '%1' to '%2'").
                                  arg(cur_h->getObject().c_str()).arg(parent_h->getObject().c_str()));
            return;
        }

        // for next calcualte
        cur_h = parent_h;
    }

    // now in \a item_path contains full path to object
    if ( _modules_tree_map.contains(m->getId().c_str()) == false) {

        QMessageBox::critical(this, tr("Error"),
                              tr("Can't get ModulesTree from map!"));
        return;
    }

    ModulesTree *mt = _modules_tree_map[m->getId().c_str()];
    mt->selectPath(item_path);

    return;
}


void DAssAdmin::onSetConnStatus(bool is_conn) {

    _ui->w_main->setEnabled(is_conn);
    _ui->w_navi_work->setEnabled(is_conn);

    _ui->w_da_not_active->setVisible(!is_conn);
}
