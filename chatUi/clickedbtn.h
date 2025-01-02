#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QObject>
#include <QPushButton>
#include <QEvent>
#include <QMoveEvent>

class ClickedBtn:public QPushButton
{
    Q_OBJECT
public:
    ClickedBtn(QWidget * parent = nullptr);
    ~ClickedBtn();
    void SetState(QString nomal,QString hover,QString press);

protected:
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override; // 鼠标按
    virtual void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标释放
    virtual void leaveEvent(QEvent *event) override;

private:
    QString _normal;
    QString _hover;
    QString _press;
};

#endif // CLICKEDBTN_H
