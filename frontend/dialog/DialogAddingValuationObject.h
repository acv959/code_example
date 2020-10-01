#ifndef DIALOGADDINGVALUATIONOBJECT_H
#define DIALOGADDINGVALUATIONOBJECT_H

#include <QDialog>


#include "../HelperDAssAdmin.h"

namespace Ui {
class DialogAddingValuationObject;
}


/**
 * @brief DialogAddingValuationObject           - диалог добавления объекта оценки
 */

class DialogAddingValuationObject : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief DialogAddingValuationObject       - конструктор
     * @param parent                            - указатель родительского виджета
     */
    explicit DialogAddingValuationObject(QWidget *parent = 0);


    /**
     * @brief saveChange                        - сохранить изменения
     */
    void saveChange();

    /**
     * @brief ~DialogAddingValuationObject      - деструктор
     */
    ~DialogAddingValuationObject();

private:


    Ui::DialogAddingValuationObject *_ui;




};

#endif // DIALOGADDINGVALUATIONOBJECT_H
