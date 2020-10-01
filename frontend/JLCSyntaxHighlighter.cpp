#include "JLCSyntaxHighlighter.h"
#include "JLCEditor.h"

JLCSyntaxHighlighter::JLCSyntaxHighlighter(JLCEditor *editor, QTextDocument *parent):QSyntaxHighlighter(parent),
    _editor(editor){

    HighlightingRule rule;



    QTextCharFormat setFirstKeywordFormat;
    setFirstKeywordFormat.setForeground(Qt::darkBlue);
    setFirstKeywordFormat.setFontWeight(QFont::Bold);

    rule._pattern = QRegExp("\\bvar\\b");
    rule._format = setFirstKeywordFormat;
    _rules.append(rule);



    QTextCharFormat setSecondKeywordFormat;
    setSecondKeywordFormat.setForeground(Qt::darkRed);
    setSecondKeywordFormat.setFontWeight(QFont::Bold);

    rule._pattern = QRegExp("\\bfunction\\b");
    rule._format = setSecondKeywordFormat;
    _rules.append(rule);

    setSecondKeywordFormat.setForeground(Qt::blue);
    setSecondKeywordFormat.setFontWeight(QFont::Normal);
    rule._pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule._format = setSecondKeywordFormat;
    _rules.append(rule);


    QTextCharFormat setThirdKeywordFormat;
    setThirdKeywordFormat.setForeground(QBrush(QColor("#FF9900")));
    setThirdKeywordFormat.setFontWeight(QFont::Bold);
    QStringList setThirdKeywordFormatPatterns;
    setThirdKeywordFormatPatterns << "\\bif\\b" << "\\belse\\b"
                                  << "\\bwhile\\b"
                                  << "\\bfor\\b" << "\\bin\\b";

    foreach (const QString &pattern, setThirdKeywordFormatPatterns) {
      rule._pattern = QRegExp(pattern);
      rule._format = setThirdKeywordFormat;
      _rules.append(rule);
    }


    QTextCharFormat setFourthKeywordFormat;
    setFourthKeywordFormat.setForeground(QBrush(QColor("#FF6666")));
    setFourthKeywordFormat.setFontWeight(QFont::Bold);
    QStringList setFourthKeywordFormatPatterns;
    setFourthKeywordFormatPatterns << "\\breturn\\b"
                                  << "\\bbreak\\b"
                                  << "\\bcontinue\\b" ;

    foreach (const QString &pattern, setFourthKeywordFormatPatterns) {
      rule._pattern = QRegExp(pattern);
      rule._format = setFourthKeywordFormat;
      _rules.append(rule);
    }



    QTextCharFormat setFifthKeywordFormat;
    setFifthKeywordFormat.setForeground(QBrush(QColor(Qt::magenta)));
    setFifthKeywordFormat.setFontWeight(QFont::Bold);
    QStringList setFifthKeywordFormatPatterns;
    setFifthKeywordFormatPatterns << "\\bimport\\b"
                                  << "\\bas\\b";

    foreach (const QString &pattern, setFifthKeywordFormatPatterns) {
      rule._pattern = QRegExp(pattern);
      rule._format = setFifthKeywordFormat;
      _rules.append(rule);
    }


    QTextCharFormat setSixthKeywordFormat;
    setSixthKeywordFormat.setForeground(QBrush(QColor(Qt::magenta)));

    QStringList setSixthKeywordFormatPatterns;
    setSixthKeywordFormatPatterns << "\\btrue\\b"
                                  << "\\bfalse\\b";

    foreach (const QString &pattern, setSixthKeywordFormatPatterns) {
      rule._pattern = QRegExp(pattern);
      rule._format = setSixthKeywordFormat;
      _rules.append(rule);
    }


    QTextCharFormat setSeventhKeywordFormat;
    setSeventhKeywordFormat.setForeground(QBrush(QColor(Qt::darkGreen)));

    rule._pattern = QRegExp("(\"([^\"]|\"\")*\")");
    rule._format = setSeventhKeywordFormat;
    _rules.append(rule);



    _comment_format.setForeground(QBrush(QColor("#CC9900")));
    _comment_format.setFontWeight(QFont::Bold);

    rule._pattern = QRegExp("//.*");
    rule._format = _comment_format;
    _rules.append(rule);


    _cse = QRegExp("/\\*");
    _cee = QRegExp("\\*/");



}

void JLCSyntaxHighlighter::highlightBlock(const QString &text){

    if(_editor->isReadOnly() ){

        return;
    }
//    qDebug()<<"text = " << text;


    foreach (const HighlightingRule &rule, _rules) {
        QRegExp expression(rule._pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule._format);
            index = expression.indexIn(text, index + length);
        }
    }


    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = _cse.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = _cee.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + _cee.matchedLength();
        }

        setFormat(startIndex, commentLength, _comment_format);
        startIndex = _cse.indexIn(text, startIndex + commentLength);
    }

    return;
}

JLCSyntaxHighlighter::~JLCSyntaxHighlighter(){

}
