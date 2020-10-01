#include "JLCEditor.h"
#include <QLineEdit>

JLCEditor::JLCEditor(QWidget *parent):QTextEdit(parent),_jlcsh(NULL){

    _jlcsh = new  JLCSyntaxHighlighter(this, this->document() );


    const int tabStop = 8;  // 8 characters
    QFontMetrics metrics(this->font());
    this->setTabStopWidth(tabStop * metrics.width(' '));

    _model = new QStandardItemModel(this);
    QStandardItem *item = NULL;

    //var
    item = new QStandardItem("var");
    _model->invisibleRootItem()->appendRow(item);


    //if else
    item = new QStandardItem("if");
    item->setToolTip("if");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("if");
    item->setToolTip("if (condition) {\n}");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("if and else");
    item->setToolTip("if (condition) {\n}else{\n}");
    _model->invisibleRootItem()->appendRow(item);


    //for
    item = new QStandardItem("for");
    item->setToolTip("for");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("for");
    item->setToolTip("for (var = 0; var < total; ++var) {\n}");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("for");
    item->setToolTip("for (var item in items) {\n}");
    _model->invisibleRootItem()->appendRow(item);


    //while
    item = new QStandardItem("while");
    item->setToolTip("while");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("while");
    item->setToolTip("while (condition) {\n}");
    _model->invisibleRootItem()->appendRow(item);


    //function
    item = new QStandardItem("function");
    item->setToolTip("function");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("function");
    item->setToolTip("function function_name (vars) {\n}");
    _model->invisibleRootItem()->appendRow(item);


    //import
    item = new QStandardItem("import");
    item->setToolTip("import OF WHAT");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("import as");
    item->setToolTip("import OF WHAT as WHAT ");
    _model->invisibleRootItem()->appendRow(item);


    item = new QStandardItem("return");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("break");
    _model->invisibleRootItem()->appendRow(item);

    item = new QStandardItem("continue");
    _model->invisibleRootItem()->appendRow(item);



    _completer = new QCompleter( this );
    _completer->setWidget(this);
    _completer->setModel( _model );
    _completer->setCaseSensitivity( Qt::CaseInsensitive );
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);



    QObject::connect(_completer, SIGNAL(activated(QModelIndex)),
                  this, SLOT(insertCompletion(QModelIndex)));

    _completer_list_view= (QListView *)_completer->popup();


    connect(_completer_list_view->selectionModel(),
         SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
         this, SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));

    connect(&_timer, SIGNAL(timeout()),
            this, SLOT(onTimeout() ) );
}

JLCEditor::JLCEditor(const QString &text, QWidget *parent):
    QTextEdit(text, parent),_jlcsh(NULL){

    _jlcsh = new  JLCSyntaxHighlighter(this, this->document());

    const int tabStop = 8;  // 8 characters
    QFontMetrics metrics(this->font());
    this->setTabStopWidth(tabStop * metrics.width(' '));
}

JLCEditor::~JLCEditor(){

}

void JLCEditor::keyPressEvent(QKeyEvent *e){



    if (_completer && _completer->popup()->isVisible()) {
        // Следующие коды клавиш перенаправляются дополнителем виджету
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // позволим дополнителю поведение по умолчанию
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!_completer || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                      || eow.contains(e->text().right(1)))) {
        _completer->popup()->hide();
        return;
    }

    _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
    _completer->setCompletionPrefix("");


    if (completionPrefix != _completer->completionPrefix()) {
        _completer->setCompletionPrefix(completionPrefix);
        _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
    }


    QRect cr = cursorRect();
    cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                + _completer->popup()->verticalScrollBar()->sizeHint().width());
    _completer->complete(cr); // выталкиваем его вверх!


    return;
}







void JLCEditor::insertCompletion(const QModelIndex &index){

    if (_completer->widget() != this){

        return;
    }


    if(index.data(Qt::ToolTipRole).isValid()){

        _text = index.data(Qt::ToolTipRole).toString();
    }else{

        _text = index.data(Qt::DisplayRole).toString();
    }


    QTextCursor tc = textCursor();
    int extra = _text.length() - _completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(_text.right(extra));
    setTextCursor(tc);


    return;
}

void JLCEditor::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous){


    Q_UNUSED(previous);


    QToolTip::hideText();

    if( ! current.isValid()){

        return;
    }

    if( ! current.data(Qt::ToolTipRole).isValid()){

        return;
    }

    if( ! _completer->popup()->isVisible()){

        _current = current;
        _timer.start(50);
        return;

    }else{

        _timer.stop();
    }




//    qDebug()<<"_completer->popup()->isVisible() = " << ;
//    if ( ! _completer->popup()->isVisible()) {

//        _completer->popup()->setCurrentIndex(current);
//        return;
//    }



    QToolTip::showText( QPoint(_completer->popup()->pos().x()+_completer->popup()->width(),
                               _completer->popup()->pos().y()+( _completer_list_view->sizeHintForRow(current.row() ) * (current.row()-1) )),
                        current.data(Qt::ToolTipRole).toString(),
                        _completer_list_view);


}

void JLCEditor::onTimeout(){

    _timer.stop();

    this->onCurrentRowChanged(_current, _current);
    return;
}






QString JLCEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}
