#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QObject>
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>


class ChatUserList:public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget* parent=nullptr);

protected:
    bool eventFilter(QObject* watched,QEvent* event) override;

signals:
    void sig_loading_chat_user();

private:
    int _load_pending;
};

#endif // CHATUSERLIST_H
