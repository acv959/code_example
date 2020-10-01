#include "DASSSRV_BSUOTU_SERVER.h"
#include "./ObjectsEstimation.h"

DASSSRV_BSUOTU_SERVER::DASSSRV_BSUOTU_SERVER(const std::string &host,
                                             int port,
                                             const std::string &serv_id,
                                             const std::string &serv_name,
                                             const std::string &serv_descr,
                                             LibAAAC::SERV_TYPE type,
                                             int threads_num,
                                             const std::string ping_method):
    BSUOTUServer(host, port,
                 serv_id, serv_name, serv_descr, type,
                 threads_num, ping_method) {

    registateAPI("getValuesByURI",
                 "Получить знания по uri значение : uri значения",
                 &DASSSRV_BSUOTU_SERVER::proccessGetValuesByURI);

    registateAPI("saveScript",
                 "Сохранить скрипт, значение : uri скрипта и тело скрипта",
                 &DASSSRV_BSUOTU_SERVER::proccessSaveScript);

    registateAPI("rmScript",
                 "Удалть скрипт, значение : uri скрипта для удаления",
                 &DASSSRV_BSUOTU_SERVER::proccessRmScript);

    registateAPI("execScript",
                 "Выполнть скрипт, значение : тело скрипта для выполнения",
                 &DASSSRV_BSUOTU_SERVER::proccessExecScript);


    try {

        model_data = LibrarySync::readFile("/var/lib/nii-rubin/dad/gpq_dad"
                                           ""
                                           ""
                                           ".xml");

    } catch (std::string &err) {


        err = _("SSDBSD_BSUOTU_SERVER: Can't read self ontology file: "
                "") + err;

        throw err;
    }

}

void DASSSRV_BSUOTU_SERVER::registateAPI(const std::string &name_method,
                                         const std::string &desc_method,
                                         const BaseBSPServer::bsp_api_method &method){

    API[name_method] = method;
    BSUOTUServer<DASSSRV_BSUOTU_SERVER>::addSharedAPI(name_method, desc_method);
    return;
}




BSPObject *DASSSRV_BSUOTU_SERVER::proccessGetValuesByURI(BaseBSPServer::s_CLIENT_REQUEST *cr){

    cDebug()<<__FUNCTION__;

    BSPObject *req = cr->bsp;

    std::string methodName ; req->pop(methodName);

    std::string uri; req->pop(uri);

    auto vals = OSE->getValuesByURI(uri);


    BSPObject *res = new BSPObject();
    res->push(int(vals.size() ) );


    for (auto &&pair: vals) {



        res->push(std::string (pair.first ) );
        res->push(std::string ( HelperDASSSRV::ValuePtrToStr(pair.second) ) );
    }


    return res;
}


BSPObject *DASSSRV_BSUOTU_SERVER::proccessSaveScript(BaseBSPServer::s_CLIENT_REQUEST *cr){

    UNUSED(cr);
    cDebug()<<__FUNCTION__;

    throw std::string( _("In the development") );

    BSPObject *res = new BSPObject();


    return res ;
}


BSPObject *DASSSRV_BSUOTU_SERVER::proccessRmScript(BaseBSPServer::s_CLIENT_REQUEST *cr){

    UNUSED(cr);
    cDebug()<<__FUNCTION__;

    throw std::string( _("In the development") );


    BSPObject *res = new BSPObject();


    return res ;
}


inline std::string ValueToStr(const ValuePtr &valPtr);

inline std::string ArrayValueToStr(const ValuePtr &valPtr){

    std::string val(0x01,1);


    for(auto &&item: *toArray(valPtr) ){

        val.append(TO_BASE64( ValueToStr(item) ) );
        val.append(";");
    }

    size_t pos = std::string::npos;
    pos = val.find_last_of(';');

    if(pos != std::string::npos){

        val.erase(pos,1);
    }

    return val;
}



BSPObject *DASSSRV_BSUOTU_SERVER::proccessExecScript(BaseBSPServer::s_CLIENT_REQUEST *cr){

    cDebug()<<__FUNCTION__;
    BSPObject *req = cr->bsp;
    std::string methodName ; req->pop(methodName);

    std::string script; req->pop(script);

    BSPObject *ps ; req->pop( &ps);


    if(!ps){

        throw std::string(_("'BSPObject' is NULL") );
    }


    int count = 0; ps->pop(count);

    VarDict inputVarDict(NULL);

    std::string var = "";
    std::vector<std::string> vars;

    for (int i=0; i<count; i++) {

        ps->pop(var);

        vars.push_back(var);
        inputVarDict.replace(var, ValuePtr ( new NoNEValue() ) );
        inputVarDict.init(var);
    }

    VarStack varStack;
    FunDict funDict(nullptr);

    _vm.exec(_jlc_c.compiler(Base64::decode(script) ),
             &varStack,
             &inputVarDict, &funDict);

    BSPObject *res = new BSPObject();

    std::map<std::string, std::string> vals;
    for(auto &&var : vars){


        ValuePtr valPtr = inputVarDict.find(var);

        if(valPtr->type() == Value::NONE){

            continue;
        }



        //cDebug()<<var << "  " << val;
        vals.insert(std::pair<std::string, std::string>(var,
                                                        HelperDASSSRV::ValuePtrToStr( valPtr) ) );
    }


    res->push(int(vals.size() ) );

    for (auto &&pair: vals) {

        res->push(std::string (pair.first ) );
        res->push(std::string (pair.second ) );
    }

    return res ;
}

