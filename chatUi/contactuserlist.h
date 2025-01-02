#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H
#include <QListWidget>
#include "conuseritem.h"
#include "userdata.h"
#include <QObject>
#include <QEvent>

class ContactUserList:public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent=nullptr);
    void ShowRedPoint(bool bshow=true);

protected:
    bool eventFilter(QObject* watched,QEvent* event)override;

private:
    void addContactUserList();

public slots:
    void slot_item_clicked(QListWidgetItem* item);
    void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo>);

private:
    ConUserItem* _add_friend_item;
    QListWidgetItem* _groupitem;
    int _load_pending;
};

#endif // CONTACTUSERLIST_H