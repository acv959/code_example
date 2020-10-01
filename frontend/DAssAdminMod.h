#ifndef DAssAdminModMOD_H
#define DAssAdminModMOD_H

#include <FuncMod.h>

#include <QTranslator>
#include <QApplication>


#include "DAssAdmin.h"

/*!
    \brief Модуль оценки данных
    \author acv
    \version 0.1
    \date Январь 2020
*/

/**
  \defgroup mod Модуль oбеспечивает настройку оценки данных
  @{
*/


namespace UAPP_DSSSD {
class DAssAdminMod : public FuncMod
{

    Q_OBJECT

public:
    /*!
     * \param name имя модуля
     * \param descr описание модуля
     * \param version версия модуля
     * \param author  автор модуля
    */
    DAssAdminMod(const std::string &id,
              const std::string &name,
              const std::string &descr,
              const std::string &version,
              const std::string &author);

    /**
     * @brief init                  - инициализация модуля
     * @return                      - 0 в случае успешной инициализации
     */
    int init();

    /**
     * @brief initPermission        - инициализация прав модуля
     */
    void initPermission();

    /**
     * @brief getNavigateWidget     - возвращает указатель на объект навигационного меню (панель слева)
     * @return                      - указатель на объект навигационного меню
     */
    QWidget *getNavigateWidget();

    /**
     * @brief getMainWidget         - возвращает указатель на объект главной рабочей области (в центре)
     * @return                      - указатель на объект главной рабочей области
     */
    QWidget *getMainWidget();

    /**
     * @brief getTKSWidget          - возвращает указатель на объект панели ТКС
     * @return                      - указатель на объект панели ТКС
     */
    QWidget *getTKSWidget(){return NULL;}

    /**
     * @brief getMenuList           - возвращает список меню модуля
     *
     * Меню будет встраиватся в главное меню унифицированного приложения
     * @return                      - список меню модуля
     */
    QList<QMenu *> getMenuList();

    /**
     * @brief getToolBarList        - возвращает список панелей инструментов модуля
     *
     * Меню будет встраиватся в главное меню унифицированного приложения
     * @return                      - список панелей инструментов
     */
    QList<QToolBar *> getToolBarList();

    /**
     * @brief getModuleIcon         - возвращает указатель на иконку модуля
     * @return                      - указатель на иконку модуля
     */
    QIcon *getModuleIcon();


    ~DAssAdminMod();

private:

    QTranslator *_translator;       ///< объект для поиска переводов в файле перевода
    DAssAdmin   *_dassadmin;        ///< объект администрирование  оценки данных

};
}
///@}
#endif // DAssAdminModMOD_H
