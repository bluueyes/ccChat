#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include "Singleton.h"
#include "global.h"
#include <QMap>
#include <functional>
#include <QByteArray>
#include "userdata.h"
class TcpMgr:public QObject,public Singleton<TcpMgr>,
            public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    TcpMgr();
    friend class Singleton<TcpMgr>;

private:
    QTcpSocket socket_;
    QString host_;
    uint16_t port_;
    uint16_t message_length_;
    uint16_t message_id;
    bool b_recv_pending;
    QByteArray buffer_;
    QMap<ReqId,std::function<void(ReqId,int,QByteArray)>> handlers_;


    void initHandlers();
    void handlerMsg(ReqId id,int len,QByteArray data);
public slots:
    void slot_tcp_connect(ServerInfo si);
    void slot_send_data(ReqId,QByteArray);

signals:
    void sig_con_success(bool success);
    void sig_login_failed(int errid);
    void sig_send_data(ReqId,QByteArray bytesData);
    void sig_switch_chatdlg();

    void sig_user_search(std::shared_ptr<SearchInfo> si);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);

    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);

    void sig_text_chat_msg(std::shared_ptr<TextChatMsg>);
};

#endif // TCPMGR_H
