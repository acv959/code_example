#include "InitApp.h"



#define INIT_PAIR(pair) pair.first ="-1"; \
    pair.second ="0";

#define ASSINGN_PAIR_VALUES(pair, db, table)  pair.first = IA::getXMIN(db, table); \
    pair.second = IA::getRowsCount(db, table);

namespace IA {

    std::pair<std::string, std::string> chek_pair;

    std::string getXMIN(const std::string &db,
                                       const std::string &table)throw(std::string){

        std::string sqlQuery = table;
        std::vector<std::string> column;
        std::vector<std::string> where_column;
        std::vector<std::string> where_values;
        std::vector<std::vector<Cell> > result;
        std::string                     error = "";


        sqlQuery.append(" order by xmin::text::bigint LIMIT 1 OFFSET  ");
        sqlQuery.append("CASE");
        sqlQuery.append(" WHEN (SELECT COUNT (*) FROM ");
        sqlQuery.append(table);
        sqlQuery.append(" ) > 0");
        sqlQuery.append(" THEN (SELECT COUNT (*) FROM ");
        sqlQuery.append(table);
        sqlQuery.append(" )-1 ");
        sqlQuery.append(" ELSE 0");
        sqlQuery.append(" END");
        column.push_back("xmin");




        GetGPQConn::get()->getData(db,
                                   sqlQuery ,
                                   column,
                                   where_column,
                                   where_values,
                                   result,
                                   error );


        if(! error.empty()){

            throw std::string(error);

        }

        if(result.empty()){

            return "-1";
        }

        if((*result.begin()).empty()){

            return "-1";
        }

        return (*(*result.begin()).begin()).value;

    }


    std::string getRowsCount(const std::string &db, const std::string &table)throw(std::string){

        //SELECT COUNT(*) FROM fooTable;
        std::string sqlQuery = ArgStr("SELECT COUNT(*) FROM %s;").arg(table);

        std::vector<std::vector<Cell> > result;
        std::string error = "";


        GetGPQConn::get()->execQuery(db,
                                     sqlQuery,
                                     result,
                                     error);

        if( ! error.empty() ){

            throw std::string (error);

        }

        if(result.empty()){

            return "0";
        }

        if((*result.begin()).empty()){

            return "0";
        }

        return (*(*result.begin()).begin()).value;
    }




}






InitApp::InitApp():_period(0),_server(nullptr),_chchit_timer(nullptr){

    _chchit_timer = new Timer<InitApp*, void(InitApp::*)()>(this, &InitApp::checkingChangesInTables);
}

bool InitApp::initApp(const std::string &url){

    try {

        if( ! this->readConfigFile(url) ){  return false;}
        if( ! _server->init()           ){  return false;}
        if( ! _server->start()          ){  return false;}
        if( ! this->initTCConn()        ){  return false;}

    } catch (std::string &err) {

        InstanceLogger *log = InstanceLogger::instance();
        log->error(_("InitApp:: %s"),
                   err.c_str());

        return false;
    }



    GetModel::init(GetGPQConn::get()->getModels(),GetGPQConn::get());

    if( ! OSE->init() )              {  return false;}

    OSE->start();

    std::map<std::string, Model*> *models_map = GetModel::getModelMap();
    Model   *model  = nullptr;
    Object  *object = nullptr;


    for(auto &&pair: *models_map){

        model = pair.second;

        if ( !  (object = model->getObject(HelperDASSSRV::T_SCRIPTS_SYS) ) ) {

            continue;
        }

        ASSINGN_PAIR_VALUES(IA::chek_pair,model->getId(), HelperDASSSRV::T_SCRIPTS_SYS);

        _chchit_map.insert(std::pair<std::string, std::pair<std::string, std::string> >(model->getId(),
                                                                                        IA::chek_pair));

    }


    _chchit_timer->start(_period);
    return true;
}

InitApp::~InitApp(){

    delete GetTCConn::instance();

    if(_chchit_timer){

        _chchit_timer->stop();
        delete _chchit_timer;
    }

    if(_server){

        _server->stop();
        delete _server;
    }

    OSE->stop();
}

bool InitApp::readConfigFile(std::string url){

    try{

        XmlConfigPtr xml = new XmlConfig(url);
        if( ! xml->isLoaded() ){ return false;}

        ConfigList config = xml ->getConfigs("//config");

        if (config.size()==0) {

                 LOGGER->error("No tag <config>");
                 return false;
        }

        ConfigList server = config.at(0)->getConfigs("server");

        if (server.size()==0) {

            LOGGER->error("No tag <server>");
            return false;
        }


        if (server.at(0)->get("host").empty()) {

            LOGGER->error("server host is empty");
            return false;
        }


        if (server.at(0)->get("port").empty()) {

           LOGGER->error("server port is empty");
           return false;
        }



       if (server.at(0)->get("period").empty()) {

           LOGGER->error("period val is empty");
           return false;
       }

       _server = new DASSServer(server.at(0)->get("host"),
                                s2i(server.at(0)->get("port") ) );

       _period = atoi(server.at(0)->get("period").c_str() );


//       cDebug()<<server.at(0)->get("host") << " "
//              << server.at(0)->get("port") << " "
//              << _period;

    } catch (const std::exception& e) {

        LOGGER->error(e.what());
        return false;

    }

    return true;
}



bool InitApp::initTCConn(){

    std::string host = "";
     int port = 0;


     try {



         GetLibAAA::get()->getActiveService("dcd",
                                            host,
                                            port);

         if(GetTCConn::init(host,port) != 0){

             return false;
         }

     } catch (std::string error) {

         error = _("Unable to connect to the 'Data collection' server -\n")
                 +error;

         InstanceLogger::instance()->error(error.c_str());
         return false;
     }


     return true;
}


void InitApp::checkingChangesInTables(){

    cDebug()<<__FUNCTION__;

    std::map<std::string, Model*> *models_map = GetModel::getModelMap();
    Model   *model  = nullptr;
    Object  *object = nullptr;


    try {

        for(auto &&pair: *models_map){

            model = pair.second;

            if ( !  (object = model->getObject(HelperDASSSRV::T_SCRIPTS_SYS) ) ) {

                continue;
            }

            ASSINGN_PAIR_VALUES(IA::chek_pair,model->getId(), HelperDASSSRV::T_SCRIPTS_SYS);

            if(_chchit_map.count(model->getId() ) ){

                auto it = _chchit_map.find(model->getId());

                if(it->second ==  IA::chek_pair){

                    continue;
                }

                _chchit_map.erase(it);
            }

            _chchit_map.insert(std::pair<std::string, std::pair<std::string, std::string> >(model->getId(),
                                                                                            IA::chek_pair));



            if (OSE->reinit()) {

                LOGGER->info(_("Reinit for '%s'"), model->getId().c_str() );

            } else {

                LOGGER->info(_("Reinit error for '%s'"), model->getId().c_str() );
            }
        }

    } catch (std::string error) {

        LOGGER->error(error.c_str() );
    }


    return ;
}


