#pragma once
#include "const.h"

class CServer:public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, unsigned short& port);
	void Start();

private:
	tcp::acceptor _acceptor;	//用于接受传入的tcp连接
	net::io_context& _ioc;	//boost::asio 库中管理 I/O 操作的核心类
	tcp::socket _socket;	//用于管理连接事件
};

