#ifndef DASSSRV_BSUOTU_SERVER_H
#define DASSSRV_BSUOTU_SERVER_H


#include "./utils/Sloth.h"
#include "./ObjectsEstimation.h"

class DASSSRV_BSUOTU_SERVER : public BSUOTUServer<DASSSRV_BSUOTU_SERVER>
{
public:
    DASSSRV_BSUOTU_SERVER(const string &host,
                          int port,
                          const std::string &serv_id,
                          const std::string& serv_name,
                          const std::string& serv_descr,
                          LibAAAC::SERV_TYPE  type,
                          int threads_num = 4,
                          const std::string ping_method="p");

private:

    JLC_Compiler                         _jlc_c;     ///< компилятор языка 'JLC'
    JLC_VirtualMachine                   _vm;        ///< виртуальная машина языка 'JLC'

    /**
     * @brief зарегистрировать API методоы
     *
     */
    void registateAPI(const std::string & name_method,
                      const std::string & desc_method,
                      const bsp_api_method &method);

/*
 * PROCCESS API
*/

    /**
     * @brief вызов произвольной функции модуля
     *
     * @param req
     * @return BSPObject
     */

    /// получить знания по uri значение : uri значения
    BSPObject* proccessGetValuesByURI(s_CLIENT_REQUEST *cr);

    ///сохранить скрипт, значение : uri скрипта и тело скрипта
    BSPObject* proccessSaveScript(s_CLIENT_REQUEST *cr);

    ///удалть скрипт, значение : uri скрипта для удаления
    BSPObject* proccessRmScript(s_CLIENT_REQUEST *cr);

    ///выполнть скрипт, значение : тело скрипта для выполнения и список параметров для возврата
    BSPObject* proccessExecScript(s_CLIENT_REQUEST *cr);
};

#endif // DASSSRV_BSUOTU_SERVER_H
