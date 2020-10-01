#ifndef QUEUECHANGEDIALOG_H
#define QUEUECHANGEDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

#include "./HelperDAssAdmin.h"

namespace Ui {
class QueueChangeDialog;
}

class QueueChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueueChangeDialog(QTreeWidgetItem *item,QWidget *parent = 0);

    QString getPeriod();

    ~QueueChangeDialog();

private:
    Ui::QueueChangeDialog   *_ui;
    QTreeWidgetItem         *_item;

private :

    void accept();

};

#endif // QUEUECHANGEDIALOG_H
