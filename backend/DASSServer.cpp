#include "DASSServer.h"

DASSServer::DASSServer(const std::string &host, int port):
    _host(host), _port(port), _thread(nullptr), _bsp_server(nullptr){

}


bool DASSServer::init(){




    _bsp_server = new DASSSRV_BSUOTU_SERVER(_host,
                                            _port,
                                            APP_NAME,
                                            "Сервер оценки данных",
                                            "Осуществляет оценку данных и выполнение скриптов на языке JLS",
                                            LibAAAC::SERV_TYPE_SINGLE);
    try {

        _bsp_server->init();
    }

    catch (std::string &err) {

        LOGGER->error(_("TCServer: Can't init BSP server. %s"), err.c_str());
        return false;
    }

    return true;
}


bool DASSServer::start(){


    _thread = new pthread_t;
    if((pthread_create(_thread, NULL, startBSPServer, this))<0){

    std::string error = _("Pthread create \'Server\':  " )+ std::string(strerror(errno));

        LOGGER->error(error.c_str() );
        return false;
    }


    return true;
}


bool DASSServer::stop(){


    if (_bsp_server) {

        LOGGER->info(_("DASSServer:start stoping"));

        _bsp_server->stop();
        

        LOGGER->info(_("DASSServer:stop OK"));
    }



/*

    TODO::XXX NOT WORK. FREEZY... need DEBUG libaaac 
    _bsp_server->stop(); not realy exit from thread ????!!!

    if (_thread) {

        if((pthread_join(*_thread,NULL))<0){

        std::string error = _("Pthread detach \'server\': ") + std::string(strerror(errno));

            LOGGER->error(error.c_str() );
        }
    }
*/

    return true;
}


DASSServer::~DASSServer(){

    delete _bsp_server;
    delete _thread;
}

void *DASSServer::startBSPServer(void *param){

    LOGGER->info(_("DASSServer:start service"));
    DASSServer *self = static_cast<DASSServer*>(param);

    // run TCP/BSP Server
    self->_bsp_server->startListen();


    return nullptr;
}
