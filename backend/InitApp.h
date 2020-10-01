#ifndef INITAPP_H
#define INITAPP_H



#include "./DASSServer.h"
#include "./utils/Sloth.h"
#include "./ObjectsEstimation.h"


/**
 * @brief InitApp                   - класс инициализации приложения
 *
 * В классе происходит чтения конфигурационного файла.
 * Создания и инициализация объектов, необходимых для работы приложения.
 */
class InitApp
{
public:

    /**
     * @brief InitApp                       - конструктор без параметров
     */
    InitApp();


    /**
     * @brief initApp                       - инициализация приложения
     * @param url                           - строка содержащая путь и наименование конфигурационного файла
     * @return                              - true в случае успешной инициализации
     */
    bool initApp(const std::string &url);


    /**
     * @brief ~InitApp                      - деструктор
     */
    ~InitApp();

private:


    int                              _period;    ///< период проверки конфигурации, сек.
    DASSServer                      *_server;    ///< сервер

    Timer<InitApp*, void(InitApp::*)()>    *_chchit_timer;                      ///< таймер проверки изменения в таблицах  (chchit - checking changes in tables)

    std::map<std::string, std::pair<std::string, std::string> >  _chchit_map;    ///<< словарь проверяемых таблиц на изменение, ключ - имя таблици, а значение - проверочная пара



    /**
    * @brief readConfigFile                 - чтение конфигурационного файла
    * @param pathToFile                     - строка содержащая путь и наименование конфигурационного файла
    * @return                               - true в случае успешной инициализации
    */
   bool readConfigFile(std::string url);


   /**
    * @brief initTCConn                     - инициализация объекта 'TCConn'
    * @return
    */
   bool initTCConn();

   /**
    * @brief checkingChangesInTables        - проверка изменений в таблицах
    */
   void checkingChangesInTables();
};

#endif // INITAPP_H
