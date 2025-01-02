#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent=nullptr);
    void appendChatItem(QWidget* item);
    void prependChatItem(QWidget* item);
    void insertChatItem(QWidget* brfore,QWidget* item);
    void removeAllItem();
protected:
    bool eventFilter(QObject*o,QEvent* e)override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onVScrollBarMoved(int min,int max);
private:
    void initStyleSheet();
private:
    QVBoxLayout* m_pV1;
    QScrollArea* m_pScrollArea;
    bool isAppended;

};

#endif // CHATVIEW_H
