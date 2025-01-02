#include "MysqlMgr.h"
#include "data.h"
MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
	return _dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
	return _dao.UpdatePwd(name, pwd);
}

bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo)
{
	return _dao.CheckPwd(email,pwd,userInfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(const std::string& name)
{
	return _dao.GetUser(name);
}

bool MysqlMgr::AddFriendApply(const int& from, const int& to)
{
	return _dao.AddFriendApply(from, to);
}

bool MysqlMgr::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit)
{
	return _dao.GetApplyList(touid, applyList, begin, limit);
}

bool MysqlMgr::AuthFriendApply(const int& uid, const int& touid)
{
	return _dao.AuthFriendApply(uid,touid);
}

bool MysqlMgr::AddFriend(const int& uid, const int& touid, const std::string& back_name)
{
	return _dao.AddFriend(uid, touid,back_name);
}

bool MysqlMgr::GetFriendList(const int& uid, std::vector<std::shared_ptr<UserInfo>>& user_info)
{
	return _dao.GetFriendList(uid, user_info);
}


MysqlMgr::MysqlMgr()
{
}
