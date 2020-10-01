#ifndef DASSADMIN_H
#define DASSADMIN_H

#include <QMenu>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QDateTime>
#include <QMutexLocker>

#include "./TimerINSPERT.h"


#include "./HelperDAssAdmin.h"
#include "./dialog/QueueChangeDialog.h"
#include "./dialog/DialogAddingValuationObject.h"


//--------------------------------------------------------------------------
// Support classes
//--------------------------------------------------------------------------
/*!
 * \brief Вспомогательный класс описывающий выпадающий список учетных моделей
*/
/**
  \defgroup main_helper Вспомогательные классы
  \ingroup diagramscene
  @{
*/
class OntologyComboBox : public QComboBox {

public:

    OntologyComboBox(QWidget *parent=0) : QComboBox(parent) {}
    /*!
     * сигнал при выборе элемента из списка
    */
    void emitSigmalActivated(int index) {emit activated(index);}

};

///@}



namespace UAPP_DSSSD {
    class DAssAdminMod;
}


namespace Ui {
class DAssAdmin;
}

/**
 * @brief DAssAdmin                 - класс oбеспечивает настройку оценки данных
 */
class DAssAdmin : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief DAssAdmin             - конструктор
     * @param parent                - указатель родительского виджета
     */
    explicit DAssAdmin(QWidget *parent = 0);

    /**
     * @brief init                  - инициализация объекта
     * @return                      - true в случае успешной инициализации
     */
    bool init();

    /**
     * @brief ~DAssAdmin            -  деструктор
     */
    ~DAssAdmin();

private:

    friend class UAPP_DSSSD::DAssAdminMod;



    QString          _script_bac;                   ///< сохраненная копия скрипта
    Ui::DAssAdmin   *_ui;                           ///< oбъект qt designer
    QMenu           *_objects_tree_menu;            ///< меню древа объектов
    DASSSRVConn     *_da_conn;                      ///< коннектор к серверу оценки
    TimerINSPERT    *_timer;                        ///< таймер обновления состояний объектов
    QMutex           _mutex;                        ///< для синхронизации одновременно выполняющихся потоков
    QTreeWidgetItem *_current_item;                 ///< текущий элемент древа

    HelperDAssAdmin::EstimationObject _current_eo;  ///< текущий объект оценки

    QMap<QString, ModulesTree *>     _modules_tree_map;           ///< словарь обеъктов 'ModulesTree' отвечающих за отображения учётных данных
    ModulesTree                     *_cur_modules_tree;           ///< текущий обеъкт 'ModulesTree'





private slots:


    /**
     * @brief onSelectCurOntology               - на вызов при смене онтологии из выпадающего списка
     * @param index                             - индекс онтологии в выпадающем списке
     */
    void onSelectCurOntology            (int index);

    /**
     * @brief onCustomContextMenuRequested      - на вызов контекстного меню
     * @param pos                               - позиция курсора
     */
    void onCustomContextMenuRequested(QPoint pos);


    /**
     * @brief onClicked                         - на клик по кнопке
     * @param checked                           - флажок имеет значение true, если кнопка отмечена
     */
    void onClicked(bool checked);


    /**
     * @brief onTextEdited                      - на отредактированный текст
     * @param str                               - строка содержащая отредактированный текст
     */
    void onTextEdited(const QString &str);


    /**
     * @brief onTrigger                         - на срабатывания триггера
     */
    void onTrigger();


    /**
     * @brief onItemPressed                      - на нажатие по элементу
     * @param item                               - указаеть на объект 'QTreeWidgetItem'
     * @param column                             - номер колонки объекта 'QTreeWidgetItem'
     */
    void onItemPressed(QTreeWidgetItem *item, int column);

    /**
     * @brief onTimeout                          - на завершения таймера
     */
    void onTimeout();

    /// найти и отобразить объект
    void onShowObject(QString);

    /// найти и отобразить данные
    void onShowObjectData(QString od_id);

    /*!
     * \brief установить виджет в зависимости от состояние сервера DA
     */
    void onSetConnStatus(bool);
};

#endif // DASSADMIN_H
