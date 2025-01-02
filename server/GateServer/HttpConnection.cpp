#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc):_socket(ioc)
{
	
}

void HttpConnection::Start()
{
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [self](
		beast::error_code ec, std::size_t bytes) {

			try {
				if (ec) {
					std::cout << "http read err is" << ec.what() << std::endl;
					return;
				}
				//处理读到的数据
				boost::ignore_unused(bytes);
				self->HandleReq();		//处理请求
				self->CheckDeadline();	//查看是否超时

			}
			catch (std::exception& exp) {
				std::cout << "exception is" << exp.what() << std::endl;
			}
		});
}

void HttpConnection::CheckDeadline()
{
	auto self = shared_from_this();

	deadline_.async_wait(
		[self](beast::error_code er) {

			if (!er) {

				//超时断开连接.
				self->_socket.close();
			}
		}
	);

}

unsigned char HttpConnection::ToHex(unsigned char x)
{
	return x > 9 ? x + 55 : x + 48;
}

unsigned char HttpConnection::FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

//url编码
std::string HttpConnection::UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0;i < length;i++) {

		//判断是否仅有数字或字母构成
		if(isalnum((unsigned char)str[i])||
			(str[i]=='-')||
			(str[i]=='_')||
			(str[i]=='.')||
			(str[i]=='~'))
			strTemp+=str[i];
		else if (str[i] == ' ') {
			strTemp += '+';
		}
		else {
			//其他字符需要提前加%并且高四位和低四位分别为16进制

			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}

	return strTemp;
}

//url解码
std::string HttpConnection::UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();

	for (size_t i = 0;i < length;++i) {

		//还原空字符串
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%') {

			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;

		}
		else {
			strTemp += str[i];
		}
	}
	return strTemp;
}

void HttpConnection::PreParseGetParam()
{
	//提取url
	auto url = _request.target();

	//查找查询字符串开始的位置
	auto query_pos = url.find('?');
	if (query_pos == std::string::npos) {
		_get_url = url;
		return;
	}

	_get_url = url.substr(0, query_pos);
	std::string query_string = url.substr(query_pos + 1);
	std::string key;
	std::string value;

	size_t pos = 0;
	while ((pos = query_string.find('&')) != std::string::npos) {
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos));
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}

	//处理剩下的最后一个参数
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}

}



void HttpConnection::WriteResponse()
{
	//http是短链接，所以发送完数据后不需要再监听对方链接，直接断开发送端即可
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	http::async_write(_socket, _response, [self](beast::error_code ec, std::size_t) {
		//完成发送后断开连接
		self->_socket.shutdown(tcp::socket::shutdown_send, ec);
		self->deadline_.cancel();
		});
}




void HttpConnection::HandleReq()
{
	//设置版本
	_response.version(_request.version());

	//设置短链接
	_response.keep_alive(false);

	if (_request.method() == http::verb::get) {
		//处理请求，是否正确
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url
			, shared_from_this());

		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}

		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}

	if (_request.method() == http::verb::post){

		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(),
			shared_from_this());
		if (!success) {
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}


		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}

}

tcp::socket& HttpConnection::GetSocket()
{
	return _socket;
}
