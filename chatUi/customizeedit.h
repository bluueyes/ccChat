#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QObject>
#include <QLineEdit>

class CustomizeEdit:public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget* parent=nullptr);
    void SetMaxLength(int maxlen);

protected:
    void focusOutEvent(QFocusEvent* event)override{

        //执行失去焦点后的处理逻辑
        //调用基类的focusOutEvent(),保证事件能正常处理
        QLineEdit::focusInEvent(event);
        //自己定义的处理
        emit sig_foucus_out();
    }

private:
    void limitTextLength(QString text){
        if(_max_len<=0){
            return;
        }

        QByteArray byteArray=text.toUtf8();
        if(byteArray.size()>_max_len){
            byteArray=byteArray.left(_max_len);
            this->setText(QString::fromUtf8(byteArray));
        }
    }

    int _max_len;

signals:
    void sig_foucus_out();
};

#endif // CUSTOMIZEEDIT_H
