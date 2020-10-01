#ifndef DASSSERVER_H
#define DASSSERVER_H



#include "./DASSSRV_BSUOTU_SERVER.h"

extern std::string APP_NAME;

/**
 * @brief DASSServer             - сервер оценки данных
 */
class DASSServer
{
public:

    /**
     * @brief DASSServer         - конструктор
     * @param host               - IP-адрес на котором сервер принимает подключения
     * @param port               - номер порта на котором сервер принимает подключения
     */
    DASSServer(const std::string &host,
               int port);

    /**
     * @brief init               - инициализация сервера
     * @return                   - true, в случае успеха
     */
    bool init();


    /**
     * @brief start              - старт сервера
     * @return                   - true, в случае успеха
     */
    bool start();


    /**
     * @brief stop               - остановить
     * @return                   - true, в случае успеха
     */
    bool stop();

    /**
     * @brief ~DASSServer        - DASSServer
     */
    ~DASSServer();


private:


    std::string                          _host;                 ///< хост сервера
    int                                  _port;                 ///< порт сервера

    pthread_t                           *_thread;               ///< поток для сервера
    DASSSRV_BSUOTU_SERVER               *_bsp_server;           ///< сервер для приема запросов по BSP протоколу

    /**
     * @brief  запустить сервер BSP для принятия запрсов
     *
     * @param param параметр - указатель на собственный класс
     */
    static void         *startBSPServer(void *param);
};

#endif // DASSSERVER_H
