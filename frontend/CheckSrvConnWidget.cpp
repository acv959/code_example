#include "CheckSrvConnWidget.h"


CheckSrvConnWidget::CheckSrvConnWidget(QWidget *parent) :
    QWidget(parent) {

    check_conn_timer = new QTimer(this);
    connect(check_conn_timer, SIGNAL(timeout()), this, SLOT(onCheckConn()));
}


void CheckSrvConnWidget::setCheckByServ(const QString& serv) {

    if (serv.trimmed().isEmpty() == false)
        this->serv = serv;
    else {
        qDebug() << "Can't set null string for serv";
    }
}


void CheckSrvConnWidget::setCheckByHostPort(const QString& host, int port) {

    if (host.trimmed().isEmpty() == false)
        this->host = host;
    else {
        qDebug() << "Can't set null string for host";
    }

    if (port >0 && port <65000)
        this->port = port;
    else {
        qDebug() << "Wrong port number!";
    }
}


void CheckSrvConnWidget::onCheckConn() {

    // get conn info
    if (serv.isEmpty() == false) {

        LibAAAC *libaaac = GetLibAAA::get();

        try {
            std::string srv_host = "127.0.0.1"; int srv_port = 9602;
#ifdef CONN_TO_DA
            libaaac->getActiveService(serv.toStdString(), srv_host, srv_port);
            this->host = srv_host.c_str(); this->port = srv_port;
#endif
        } catch (std::string &err) {
            qDebug() << "Can't get service from AAA " << err.c_str();
            emit setConnStatus(false);
            return;
        }
    }

    bool is_conn = false;
    DASSSRVConn *DA = GetDAConn::instance();

    // already have connected pool
    if (DA) {
         is_conn = DA->isConnected();
    }

    // pool is empty. check by simple client
    else {

        SSLBSPClient cli(host.toStdString(), port,
                         GLOBAL_USER_CERT, GLOBAL_USER_KEY, GLOBAL_CA_FILE);

        try {

            cli.conn();

            // init pool connection
            int res = GetDAConn::init(host.toStdString(), port);

            if (res == 0)
                is_conn = true;
        }

        // err in connect
        catch (std::string &err) {
            is_conn = false;
            qDebug() << "Error while check connection:"<<err.c_str();
        }

    }

    emit setConnStatus(is_conn);
}


void CheckSrvConnWidget::setVisible(bool visible) {

    if (visible) check_conn_timer->start(5000);
    else check_conn_timer->stop();

    QWidget::setVisible(visible);
}
