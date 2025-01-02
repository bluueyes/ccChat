#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "Singleton.h"
#include "userdata.h"
#define CHAT_COUNT_PER_PAGE 13
class UserMgr:public QObject,public Singleton<UserMgr>
    ,public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend Singleton<UserMgr> ;
    ~UserMgr();
    void SetToken(QString token);
    QString GetName();
    int GetUid();
    QString GetIcon();
    bool AlreadyApply(int uid);
    void AddApplyList(std::shared_ptr<ApplyInfo> apply);
    void SetUserInfo(std::shared_ptr<UserInfo>);
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);
    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    std::shared_ptr<UserInfo> GetUserInfo();
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);
signals:
    //void sig_auth_rsp(std::shared_ptr<AuthRsp>);
private:
    UserMgr();
    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::shared_ptr<UserInfo> user_info;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    int _chat_loaded;
    int _contact_loaded;
};

#endif // USERMGR_H
