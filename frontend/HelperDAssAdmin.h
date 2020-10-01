#ifndef HELPERDASSADMIN_H
#define HELPERDASSADMIN_H

#include <string>
#include <QDebug>
#include <QString>
#include <QWidget>
#include <QComboBox>
#include <QTreeWidgetItem>
#include <QStandardItemModel>


#include "./QAlarmMsg.h"

#define MODEL "dass_model"


#define SSS_AND_RETURN(qobject, QObject, color)                     \
    qobject->setStyleSheet(#QObject"{background: "#color";}");      \
    return;                                                         \

#define SSS(qobject, QObject, color)                                \
    qobject->setStyleSheet(#QObject"{background: "#color";}");      \




/**
 * @brief HelperDBSAdmin        - коллекция необходимых констант и функций, необходимых для работы модуля оценки данных
 */
namespace HelperDAssAdmin{


    static const std::string  T_SCHEMES_SYS = "tschemes_sys";

    static const std::string  T_SCRIPTS_SYS         = "tscripts_sys";
    static const std::string  C_SCRIPTS_PID         = "pid" ;
    static const std::string  C_SCRIPTS_ID          = "id" ;
    static const std::string  C_SCRIPTS_NAME        = "name" ;
    static const std::string  C_SCRIPTS_SCRIPT      = "script" ;
    static const std::string  C_SCRIPTS_ASSESS_FLAG = "assess_flag" ;
    static const std::string  C_SCRIPTS_URI         = "uri" ;

    static const std::string  T_TCMDS_SYS   = "ttcmds_sys";


    typedef std::vector<std::vector<Cell> > Data;

    const int ID_ROLE           = Qt::UserRole+1;                     ///< роль - идентификатор объекта
    const int ASSESS_FLAG_ROLE  = HelperDAssAdmin::ID_ROLE+1;         ///< роль - флаг оценки объекта

    /**
     * @brief EstimationObject              - объект оценки
     */
    class EstimationObject
    {
        public:

            EstimationObject();

            EstimationObject(const QString &uri,
                             const QString &script,
                             const bool &assessFlag);

            bool    isValid();


            QString uri();
            QString script();
            bool    assessFlag();

            QMap<QString,QString> getValues();

        private:


            QString _uri;
            QString _script;
            bool    _assess_flag;
            bool    _is_valid;
    };


    /**
     * @brief getEstimationObject           - получить объект оценки
     * @throw std::string                   - строка с ошибкой
     * @param modelId                       - строка содержащая идентификатор модели
     * @param uri                           - содержащая идентификатор объекта
     * @return                              - объект оценки
     */
    EstimationObject getEstimationObject(const QString &modelId,
                                         const QString &uri)throw(std::string);



    /**
     * @brief saveScript                    - сохранить срипт оценки объекта
     * @throw std::string                   - строка с ошибкой
     * @param modelId                       - строка содержащая идентификатор модели
     * @param uri                           - строка содержащая идентификатор объекта
     * @param script                        - строка содержащая срипт оценки объекта
     */
    void saveScript(const QString &modelId, const QString &uri,
                    const QString &script)throw(std::string);


    /**
     * @brief setAssessFlag                 - установить значения флага оценки
     * @throw std::string                   - строка с ошибкой
     * @param modelId                       - строка содержащая идентификатор модели
     * @param uri                           - строка содержащая идентификатор объекта
     * @param b                             - true если для объекта надо вычислять значения оценки
     */
    void setAssessFlag(const QString &modelId, const QString &uri, bool b)throw(std::string);





}

#endif // HELPERDASSADMIN_H
