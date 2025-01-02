#ifndef CLICKEDLAB_H
#define CLICKEDLAB_H

#include <QLabel>
#include "global.h"

class ClickedLab : public QLabel
{
    Q_OBJECT
public:
    ClickedLab(QWidget* parent);

    virtual void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标释放
    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    void SetState(QString normal="",QString hover="",QString press="",
                  QString select="",QString select_hover="",QString select_press="");

    ClickedLabState GetCurState();
    void SetCurState(ClickedLabState state);
    void ResetNormalState();
private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickedLabState _curstate;
signals:
    void clicked(QString,ClickedLabState);
};
#endif // CLICKEDLAB_H
