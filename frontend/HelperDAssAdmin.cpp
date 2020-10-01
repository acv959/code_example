#include "HelperDAssAdmin.h"




HelperDAssAdmin::EstimationObject::EstimationObject():
    _uri(""), _script(""), _assess_flag(""), _is_valid(false){

}

HelperDAssAdmin::EstimationObject::EstimationObject(const QString &uri,
                                                    const QString &script,
                                                    const bool &assessFlag):
    _uri(uri), _script(script), _assess_flag(assessFlag), _is_valid(true){

}

bool HelperDAssAdmin::EstimationObject::isValid(){

    return _is_valid;
}

QString HelperDAssAdmin::EstimationObject::uri(){

    return _uri;
}

QString HelperDAssAdmin::EstimationObject::script(){

    return _script;

}

bool HelperDAssAdmin::EstimationObject::assessFlag(){

    return _assess_flag;
}

QMap<QString, QString> HelperDAssAdmin::EstimationObject::getValues(){

    QMap<QString, QString> map;


    try {

        std::map<std::string, std::string> vals = GetDAConn::instance()->getValuesByURI(_uri.toStdString() );


        for(auto &&pair :  vals){

            map.insert(QString::fromStdString(pair.first),
                       QString::fromStdString(pair.second) );
        }

    } catch (std::string error) {

        map.insert("state", "Error state");
        map.insert("date", "Error data");
        QAlarmMsg::msgErr(QString::fromStdString(error), nullptr);
    }




    return std::move(map);
}


inline bool strToBool(const std::string &str){

    if(str == "1" || str == "T" || str == "TRUE" || str == "True" ||
            str == "true" || str == "t"){

        return true ;
    }

    return false;
}

HelperDAssAdmin::EstimationObject HelperDAssAdmin::getEstimationObject(const QString &modelId,
                                                                       const QString &uri)throw(std::string){

    std::string error = "";
    std::vector<std::vector<Cell> > result;


    GetGPQConn::get()->getData(modelId.toStdString(),
                               HelperDAssAdmin::T_SCRIPTS_SYS,
                              {HelperDAssAdmin::C_SCRIPTS_SCRIPT, HelperDAssAdmin::C_SCRIPTS_ASSESS_FLAG},
                              {HelperDAssAdmin::C_SCRIPTS_URI},
                              {uri.toStdString()},
                               result,
                               error);


    if(! error.empty() ){

        throw error;
    }

    if(result.empty() || result.front().empty() ){

        if(QAlarmMsg::msgQuest(QObject::tr("There is no evaluation scenario for the selected item.\n"
                                           "Apply a standard evaluation scenario for the selected item?"), nullptr)){




            ObjectData *od = ObjectData::fromId(uri.toStdString());
            if(!od || !od->isValid() ){


                throw std::string(QObject::tr("Internal error. Unable to create an object 'ObjectData' from uri '%0'").arg(uri).toStdString() );
            }


            if ( ! UTILS::checkIfScriptForObject(od->getObject()->getModel()->getId(),
                                              od->getObject()->getId() )) {

                throw std::string(QObject::tr("For an object of '%0' is no regular scenario evaluation").arg(QString::fromStdString(od->getObject()->getName() ) ).toStdString() );
            }


            UTILS::saveScriptForObject(modelId.toStdString(),
                                       od->getObject()->getId(),
                                       false,
                                       uri.toStdString());

            return HelperDAssAdmin::getEstimationObject(modelId,
                                                        uri);

        }else{

            throw std::string(QObject::tr("Not data for object with uri '%0'").arg(uri).toStdString() );
        }


    }


    return std::move(HelperDAssAdmin::EstimationObject(uri,
                                                       QByteArray::fromBase64( QByteArray::fromHex( result.at(0).at(0).value.c_str() ) ),
                                                       strToBool(result.at(0).at(1).value.c_str() ) ) );
}



void HelperDAssAdmin::saveScript(const QString &modelId, const QString &uri,
                                 const QString &script)throw(std::string){


//    qDebug()<<"!!!!!ALARM!!!!!! HelperDAssAdmin::saveScript - NOT ACTIVE";
    std::string error   = "";
    GetGPQConn::get()->setData(modelId.toStdString(),
                               HelperDAssAdmin::T_SCRIPTS_SYS,
                              {HelperDAssAdmin::C_SCRIPTS_SCRIPT},
                              {TO_BASE64(script.toStdString() )},
                              {HelperDAssAdmin::C_SCRIPTS_URI},
                              {uri.toStdString()},
                               error);


    if( ! error.empty()){

        throw std::string(error);
    }


    return;
}


void HelperDAssAdmin::setAssessFlag(const QString &modelId, const QString &uri,
                                    bool b)throw(std::string){

    (void)modelId;
    (void)uri;
    (void)b;


    std::string error   = "";
    GetGPQConn::get()->setData(modelId.toStdString(),
                               HelperDAssAdmin::T_SCRIPTS_SYS,
                              {HelperDAssAdmin::C_SCRIPTS_ASSESS_FLAG},
                              {(b ? "t" : "f" )},
                              {HelperDAssAdmin::C_SCRIPTS_URI},
                              {uri.toStdString()},
                               error);


    if( ! error.empty()){

        throw std::string(error);
    }


    return;
}
