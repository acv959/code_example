#ifndef CHECKSRVCONNWIDGET_H
#define CHECKSRVCONNWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>


/**
 * @brief описывает панель которая проверяет наличие подключения к заданному сервису
 *
 */
class CheckSrvConnWidget : public QWidget{

    Q_OBJECT

public:

    /**
     * @brief
     *
     * @param parent
     */
    CheckSrvConnWidget(QWidget *parent=nullptr);

    void setCheckByServ(const QString &serv);

    void setCheckByHostPort(const QString &host,
                            int port);

public slots:

    /// переодическая проверка новых значений табло
    /**
     * @brief
     *
     */
    void onCheckConn();

protected:

    /**
     * @brief
     *
     * @param visible
     */
    void setVisible(bool visible);

signals:

    /**
     * @brief
     *
     * @param bool
     */
    void setConnStatus(bool);

private:

    /// если serv не пусто будет проверятся по сервису. хост порт будет браться из службы ААА
    /// если не установлено, будет братся от сюда хост, порт
    QString host = "";           ///< какой хост проверять
    int port = -1;               ///< какой порт проверять

    QString serv = "";           ///< какую службу проверять

    QTimer *check_conn_timer; /**< TODO */
};

#endif // CHECKSRVCONNWIDGET_H
