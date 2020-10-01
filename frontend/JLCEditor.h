#ifndef JLCEDITOR_H
#define JLCEDITOR_H

#include <time.h>
#include <unistd.h>

#include <QTextEdit>
#include <QCompleter>
#include <QTimer>
#include <QDateTime>

#include <QKeyEvent>

#include <QStringListModel>
#include <QListWidget>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QTreeView>
#include <QScrollBar>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QToolTip>
#include <QEventLoop>
#include <QTimer>


#include "./JLCSyntaxHighlighter.h"


/**
 * @brief JLCEditor                 - класс редактора 'JLC'
 */
class JLCEditor : public QTextEdit
{

    Q_OBJECT
public:

    /**
     * @brief JLCEditor             - конструктор
     * @param parent                - указатель родительского виджета
     */
    explicit JLCEditor(QWidget *parent = 0);

    /**
     * @brief JLCEditor             - конструктор
     * @param text                  - строка содержащая текст
     * @param parent                - указатель родительского виджета
     */
    explicit JLCEditor(const QString &text, QWidget *parent = 0);

    virtual ~JLCEditor();

private:

    JLCSyntaxHighlighter    *_jlcsh;                ///< подсветка синтаксиса для языка 'JLC'

    QCompleter          *_completer;                ///< автозавершения
    QListView           *_completer_list_view;      ///< просмоторщик автозавершения
    QStandardItemModel  *_model;                    ///< модель автозавершения
    QString              _text;                     ///< для хранения временого текста
    QTimer               _timer;                    ///< таймер

    QModelIndex         _current;                   ///< текущий индекс модели



protected:

    /**
     * @brief keyPressEvent         -  событие по нжиму клавиши
     * @param e                     -  указатель на обеъкт 'QKeyEvent'
     */
    void keyPressEvent(QKeyEvent *e);



private slots:

    /**
     * @brief insertCompletion      - вставка  автозавершения
     * @param index                 - индекс
     */
    void insertCompletion(const QModelIndex &index);

    /**
     * @brief onCurrentRowChanged   - на изменения текущий строки
     * @param current               - текущий индекс
     * @param previous              - предыдущие индекс
     */
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    /**
     * @brief onTimeout             - на завершения таймера
     */
    void onTimeout();

private:
    QString textUnderCursor() const;


};

#endif // JLCEDITOR_H
