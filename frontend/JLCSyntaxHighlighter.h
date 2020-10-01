#ifndef JLCSYNTAXHIGHLIGHTER_H
#define JLCSYNTAXHIGHLIGHTER_H


#include <QDebug>
#include <QSyntaxHighlighter>

class JLCEditor;


/**
 * @brief JLCSyntaxHighlighter              - класс подсветки синтаксиса для языка 'JLC'
 */
class JLCSyntaxHighlighter : public QSyntaxHighlighter
{

     Q_OBJECT
public:

    /**
     * @brief JLCSyntaxHighlighter          - конструктор
     * @param editor                        - указатель на объект 'JLCEditor'
     * @param parent                        - указатель родительского документа
     */
    JLCSyntaxHighlighter(JLCEditor *editor, QTextDocument *parent = 0);


    /**
     * @brief JLCSyntaxHighlighter          - деструктор
     */
    ~JLCSyntaxHighlighter();

protected:

    /**
     * @brief highlightBlock                - подсветка текстового блока
     * @param text                          - строка содержащая текст блока
     */
    void highlightBlock(const QString &text);



private:

    /**
     * @brief HighlightingRule           - структура правил подсветки синтаксиса
     */
    struct HighlightingRule
    {
        QRegExp         _pattern;   ///< шаблон выражение
        QTextCharFormat _format;    ///< формат шаблона
    };
    QVector<HighlightingRule> _rules;           ///< вектор правил подсветки синтаксиса

    QTextCharFormat _comment_format;            ///< формат для комментариев
    QRegExp         _cse;                       ///< стартовое выражение для многострочный комментариев '/*'
    QRegExp         _cee;                       ///< конечное выражение для многострочный комментариев '*/'

    JLCEditor      *_editor;                    ///< редактор

};

#endif // JLCSYNTAXHIGHLIGHTER_H
