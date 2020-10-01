#include "DAssAdminMod.h"
#include "ui_DAssAdmin.h"

extern std::string APP_NAME;
extern std::string log_dir;
extern std::string errPath;
extern std::string funcPath;
extern std::string taskPath;
extern std::string logLevel;

#define MOD_ID      "libdassadmin-mod.so"
#define MOD_NAME    "Оценка данных"
#define MOD_DESCR   "Модуль oбеспечивает настройку оценки данных"
#define MOD_VER     "0.1"
#define MOD_AUTHOR  "Аверьянов Константин"

extern "C" {
FuncMod* attach() {

    FuncMod *mod = new UAPP_DSSSD::DAssAdminMod( MOD_ID,
                                                 MOD_NAME,
                                                 MOD_DESCR,
                                                 MOD_VER,
                                                 MOD_AUTHOR);

        return mod;
    }
}






UAPP_DSSSD::DAssAdminMod::DAssAdminMod(const std::string &id,
                                       const std::string &name,
                                       const std::string &descr,
                                       const std::string &version,
                                       const std::string &author):
    FuncMod(id, name, descr, version, author),_translator(NULL),_dassadmin(NULL){

    _translator = new QTranslator(this);
    _translator->load(":/dassadmin_mod_ru.qm");
    qApp->installTranslator(_translator);

    this->initPermission();
}

int UAPP_DSSSD::DAssAdminMod::init(){

    _dassadmin = new DAssAdmin();

    if( _dassadmin->init()){

        return 0;
    }

    return 1;
}

void UAPP_DSSSD::DAssAdminMod::initPermission(){


    return;
}

QWidget *UAPP_DSSSD::DAssAdminMod::getNavigateWidget(){

    return _dassadmin->_ui->w_navigate;
}


QWidget *UAPP_DSSSD::DAssAdminMod::getMainWidget(){

    return _dassadmin->_ui->w_main;
}


QList<QMenu *> UAPP_DSSSD::DAssAdminMod::getMenuList(){

    return QList<QMenu *> ();
}


QList<QToolBar *> UAPP_DSSSD::DAssAdminMod::getToolBarList(){


    return QList<QToolBar *>();
}


QIcon *UAPP_DSSSD::DAssAdminMod::getModuleIcon(){

    return new QIcon(":/img/data_estimation.png");
}


UAPP_DSSSD::DAssAdminMod::~DAssAdminMod(){

}

