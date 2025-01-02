#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "CSession.h"
#include "MysqlMgr.h"
#include "RedisMgr.h"
ChatServiceImpl::ChatServiceImpl()
{
}

Status ChatServiceImpl::NotifyAddFriend(grpc::ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply)
{
  //�����û��Ƿ��ڱ�������
    auto touid = request->touid();
    auto session = UserMgr::GetInstance()->GetSession(touid);

    Defer defer([request, reply]() {
        reply->set_error(ErrorCodes::Success);
        reply->set_applyuid(request->applyuid());
        reply->set_touid(request->touid());
        });

    //�û��������ڴ���ֱ�ӷ���
    if (session == nullptr) {
        return Status::OK;
    }

    //���ڴ���ֱ�ӷ���֪ͨ�Է�
    Json::Value rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["applyuid"] = request->applyuid();
    rtvalue["name"] = request->name();
    rtvalue["desc"] = request->desc();
    rtvalue["icon"] = request->icon();
    rtvalue["sex"] = request->sex();
    rtvalue["nick"] = request->nick();

    std::string return_str = rtvalue.toStyledString();

    session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
    return Status::OK;
}

Status ChatServiceImpl::NotifyAuthFriend(grpc::ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* reply)
{
    //�����û��Ƿ��ڱ�������
    auto touid = request->touid();
    auto fromuid = request->fromuid();
    auto session = UserMgr::GetInstance()->GetSession(touid);
    Defer defer([request, reply]() {
        reply->set_error(ErrorCodes::Success);
        reply->set_fromuid(request->fromuid());
        reply->set_touid(request->touid());
        });
    //�û������ڴ�����ֱ�ӷ���
    if (session == nullptr) {
        return Status::OK;
    }
    //���ڴ�����ֱ�ӷ���֪ͨ�Է�
    Json::Value  rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["fromuid"] = request->fromuid();
    rtvalue["touid"] = request->touid();
    std::string base_key = USER_BASE_INFO + std::to_string(fromuid);
    auto user_info = std::make_shared<UserInfo>();
    bool b_info = GetBaseInfo(base_key, fromuid, user_info);
    if (b_info) {
        rtvalue["name"] = user_info->name;
        rtvalue["nick"] = user_info->nick;
        rtvalue["icon"] = user_info->icon;
        rtvalue["sex"] = user_info->sex;
    }
    else {
        rtvalue["error"] = ErrorCodes::UidInvalid;
    }
    std::string return_str = rtvalue.toStyledString();
    session->Send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(::grpc::ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* reply)
{
    //�����û��Ƿ��ڱ�������
    auto touid = request->touid();
    auto session = UserMgr::GetInstance()->GetSession(touid);
    reply->set_error(ErrorCodes::Success);

    //�û������ڴ�����ֱ�ӷ���
    if (session == nullptr) {
        return Status::OK;
    }

    //���ڴ�����ֱ�ӷ���֪ͨ�Է�
    Json::Value  rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["fromuid"] = request->fromuid();
    rtvalue["touid"] = request->touid();

    //������������֯Ϊ����
    Json::Value text_array;
    for (auto& msg : request->textmsgs()) {
        Json::Value element;
        element["content"] = msg.msgcontent();
        element["msgid"] = msg.msgid();
        text_array.append(element);
    }
    rtvalue["text_array"] = text_array;

    std::string return_str = rtvalue.toStyledString();

    session->Send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
    return Status::OK;
}

bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userInfo)
{
    //���Ȳ�redis�в�ѯ�û���Ϣ
    std::string info_str = "";
    bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userInfo->uid = root["uid"].asInt();
        userInfo->name = root["name"].asString();
        userInfo->pwd = root["pwd"].asString();
        userInfo->email = root["email"].asString();
        userInfo->nick = root["nick"].asString();
        userInfo->desc = root["desc"].asString();
        userInfo->sex = root["sex"].asInt();
    }
    else
    {
        //redis��û�����ѯmysql
        //��ѯ���ݿ�
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlMgr::GetInstance()->GetUser(uid);
        if (user_info == nullptr) {
            return false;
        }

        userInfo = user_info;

        //�����ݿ�����д��redis����
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["pwd"] = userInfo->pwd;
        redis_root["name"] = userInfo->name;
        redis_root["email"] = userInfo->email;
        redis_root["nick"] = userInfo->nick;
        redis_root["desc"] = userInfo->desc;
        redis_root["sex"] = userInfo->sex;
        redis_root["icon"] = userInfo->icon;
        RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
    }
    return true;
}
