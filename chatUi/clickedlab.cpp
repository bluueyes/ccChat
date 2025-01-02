#include "clickedlab.h"
#include <QMouseEvent>
#include "global.h"
ClickedLab::ClickedLab(QWidget* parent):QLabel(parent),
    _curstate(ClickedLabState::Normal){
    setCursor(Qt::PointingHandCursor);
}

void ClickedLab::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickedLabState::Normal){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(),_curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

//处理鼠标点击
void ClickedLab::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton){
        if(_curstate==ClickedLabState::Normal){
            qDebug()<<"clicked,change to selected hover:"<<_selected_hover;
            _curstate=ClickedLabState::Selected;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }else{
            qDebug()<<"clicked , change to normal hover: "<< _normal_hover;
            _curstate = ClickedLabState::Normal;
            setProperty("state",_normal_press);
            repolish(this);
            update();
        }

        return;
    }
    //调用基类的mousePressEvent以保证正确的事件处理
    QLabel::mousePressEvent(ev);
}

//处理事件悬浮进入事件
void ClickedLab::enterEvent(QEnterEvent *event)
{
    if(_curstate==ClickedLabState::Normal){
        qDebug()<<"enter,change to normal horver:"<<_normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();
    }else{
        qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);

}

void ClickedLab::leaveEvent(QEvent *event)
{
    if(_curstate==ClickedLabState::Normal){
        qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();
    }else{
        qDebug()<<"leave , change to normal hover: "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLab::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal=normal;
    _normal_hover=hover;
    _normal_press=press;

    _selected=select;
    _selected_hover = select_hover;
    _selected_press = select_press;
    setProperty("state",normal);
    repolish(this);
}

ClickedLabState ClickedLab::GetCurState()
{
    return _curstate;

}

void ClickedLab::SetCurState(ClickedLabState state)
{
    _curstate=state;
}

void ClickedLab::ResetNormalState()
{
    _curstate=ClickedLabState::Normal;
}
