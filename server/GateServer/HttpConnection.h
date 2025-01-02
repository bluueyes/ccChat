#pragma once
#include "const.h"
class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(boost::asio::io_context&);
	void Start();
	tcp::socket& GetSocket();

private:
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	
	//十进制转16进制
	unsigned char ToHex(unsigned char x);

	//16进制转10进制
	unsigned char FromHex(unsigned char x);

	//url 编码
	std::string UrlEncode(const std::string& str);

	//url 解码
	std::string UrlDecode(const std::string& str);

	//参数解析
	void PreParseGetParam();

private:
	tcp::socket _socket;

	//读文件准备的buffer
	beast::flat_buffer _buffer{ 8192 };

	//请求信息
	http::request<http::dynamic_body> _request;

	//回复信息
	http::response<http::dynamic_body> _response;

	//处理连接超时定时器
	net::steady_timer deadline_{ _socket.get_executor(),std::chrono::seconds(60) };

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;


};

