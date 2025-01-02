#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QObject>
#include <QPushButton>
#include <QTimer>

class TimerBtn:public QPushButton
{
    Q_OBJECT
public:
    TimerBtn(QWidget *parent = nullptr);
    ~TimerBtn();

    //重写mouseReleaseEvent
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    QTimer* _timer;
    int _counter;

};

#endif // TIMERBTN_H
