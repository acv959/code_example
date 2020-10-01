#ifndef HELPERDASSSRV_H
#define HELPERDASSSRV_H

#include <string>

namespace HelperDASSSRV{

    static const std::string T_SCHEMES_SYS = "tschemes_sys";
    static const std::string T_SCRIPTS_SYS = "tscripts_sys";
    static const std::string T_TCMDS_SYS   = "ttcmds_sys";



    static const std::string  M_GPQ_DAD             = "gpq_dad"; ///< модель - сервер оценки данных

    static const std::string  T_VALUES              = "tValues";
    static const std::string  C_VALUES_ID           = "id" ;
    static const std::string  C_VALUES_OBJECT_NAME  = "object_name";
    static const std::string  C_VALUES_VALUE_NAME   = "value_name";
    static const std::string  C_VALUES_VALUE        = "value";
    static const std::string  C_VALUES_URI          = "uri";


    /**
     * @brief saveVal       - сохранить вычисленные значения
     * @param valueName     - строка содержащая наименование значение
     * @param value         - строка содержащая  значение
     * @param od            - указатель на объект
     */
    void saveVal(const std::string &valueName, const std::string &value,
                 ObjectData *od);

    std::string ValuePtrToStr(const ValuePtr &valuePtr);

}

#endif // HELPERDASSSRV_H
