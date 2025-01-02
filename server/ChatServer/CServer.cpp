#include "CServer.h"
#include "AsioIOServicePool.h"
#include <iostream>
#include "UserMgr.h"

CServer::CServer(boost::asio::io_context& io_context, short port)
	:_io_context(io_context),_port(port),_acceptor(_io_context,tcp::endpoint(tcp::v4(),port))
{
	std::cout << "Server start success,listen on port: " << _port << std::endl;
	StartAccept();
}

CServer::~CServer()
{
	cout << "Server destruct listen on port : " << _port << endl;
}

void CServer::ClearSession(std::string session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) {
		//移除用户和session的关联
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}
	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error)
{
	if (!error) {
		new_session->Start();
		lock_guard<std::mutex> lock(_mutex);
		_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed failed ,err is " << error.what() << std::endl;
	}
	StartAccept();
}

void CServer::StartAccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1));

	
}
