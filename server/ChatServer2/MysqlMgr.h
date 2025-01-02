#pragma once
#include "const.h"
#include "MysqlDao.h"

class MysqlMgr:public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;

public:
	~MysqlMgr();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(const std::string& uid);
	bool AddFriendApply(const int& from, const int& to);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit);
	bool AuthFriendApply(const int& uid, const int& touid);
	bool AddFriend(const int& uid, const int& touid, const std::string& );
	bool GetFriendList(const int& uid, std::vector<std::shared_ptr<UserInfo>>& user_info);
	//bool TestProcedure(const std::string& email, int& uid, string& name);
private:
	MysqlMgr();
	MysqlDao _dao;
};

