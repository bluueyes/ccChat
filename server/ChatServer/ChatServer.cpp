﻿#include "LogicSystem.h"
#include <csignal>
#include <thread>
#include <mutex>
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include <iostream>
#include "RedisMgr.h"
#include "const.h"
#include "ChatServiceImpl.h"

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;
int main()
{

	auto& cfg = ConfigMgr::Inst();
	auto server_name = cfg["SelfServer"]["Name"];
	try {
		auto& cfg = ConfigMgr::Inst();
		auto pool = AsioIOServicePool::GetInstance();

		//将登录数设置为0并同步到redis
		RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");
		//定义一个GrpcServer
		std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
		ChatServiceImpl service;
		grpc::ServerBuilder builder;
		//监听端口和添加服务
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);

		//构建和启动grpc服务器
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		std::cout << "RPC Server listening on " << server_address << std::endl;
		
		//单独启动一个线程处理grpc服务
		std::thread  grpc_server_thread([&server]() {
			server->Wait();
			});



		boost::asio::io_context io_context;
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		//接受系统信号后优雅推出
		signals.async_wait([&io_context, pool,&server](auto, auto) {
			io_context.stop();
			pool->Stop();
			server->Shutdown();
			});

		auto port_str = cfg["SelfServer"]["Port"];
		CServer s(io_context, atoi(port_str.c_str()));
		io_context.run();

		RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
		RedisMgr::GetInstance()->Close();
		grpc_server_thread.join();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}