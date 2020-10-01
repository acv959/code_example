#include <string.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QApplication>
#include <QSplitter>

#include <libgpq/Globals.h>
#include <libnii-rubin/Config.h>
#include <libnii-rubin/Logable.h>
#include <QLibraryInfo>


#include "DAssAdminMod.cpp"

#define PACKAGE "libdassadmin-mod"
#define localedir_full "/usr/share/locale"

std::string APP_NAME = "libdassadmin-mod";


std::string log_dir="/var/log/nii-rubin/"+APP_NAME+"/";
std::string errPath  = log_dir + "error.log";
std::string funcPath = log_dir + "func.log";
std::string taskPath = log_dir + "task.log";
std::string logLevel = log_dir + "INFO";

int main(int argc, char *argv[]) {

    GLOBAL_USER_CERT = "/home/root/.uapp/certs/acv.crt";
    GLOBAL_USER_KEY = "/home/root/.uapp/certs/acv.key";

    QApplication a(argc, argv);
    setlocale(LC_ALL, "Russian");
    setlocale(LC_NUMERIC, "C");
    QLocale ru(QLocale::Russian,QLocale::RussianFederation);
#ifndef HAVE_QT5
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(),
               "qt", "_",
               QLibraryInfo::location(QLibraryInfo::TranslationsPath))){

       qDebug() << "qtTranslator ok";
       a.installTranslator(&qtTranslator);
    }

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(),
               QLibraryInfo::location(QLibraryInfo::TranslationsPath))){

       qDebug() << "qtBaseTranslator ok";
       a.installTranslator(&qtBaseTranslator);
    }



    if (argc <= 1) {


        try{

            GetLibAAA::initUserConn("root",
                                    "1");

        }catch (std::string error) {

            QMessageBox::critical(NULL, QObject::tr("Error"), QString::fromStdString( error) );
            return 1;

        } catch (...) {

            QMessageBox::critical(NULL, QObject::tr("Error"), QObject::tr("Unhandled exception. App will be close"));
            return 1;

        }

    } else {


        if(GetGPQConn::init(argv[1],atoi(argv[2])) != 0){

            return 2;
        }

        GetModel::init(GetGPQConn::get()->getModels(),GetGPQConn::get());

    }


    FuncMod *d= attach();

    if(d->init()){

        return 4;
    }

    QWidget w;
    w.setLayout(new QHBoxLayout());

    QSplitter *splitter = new QSplitter(&w);;

    splitter->addWidget(d->getNavigateWidget());
    splitter->addWidget(d->getMainWidget());

    splitter->setStretchFactor(1,1);
    w.layout()->addWidget(splitter);

    w.resize(1300, w.width());
    w.setWindowTitle(MOD_NAME);
    w.setWindowIcon(*d->getModuleIcon());

    w.show();

    int result =  a.exec();

    delete d;

    return result;
}
