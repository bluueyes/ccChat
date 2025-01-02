#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

#include <QObject>
#include <QPaintEvent>
#include <QHBoxLayout>
#include "global.h"
#include <QFrame>

class BubbleFrame:public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role,QWidget* parent=nullptr);
    void setMargin(int margin);
    void setWidget(QWidget * w);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role;
    int m_margin;

};

#endif // BUBBLEFRAME_H
