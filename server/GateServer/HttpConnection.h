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
	
	//ʮ����ת16����
	unsigned char ToHex(unsigned char x);

	//16����ת10����
	unsigned char FromHex(unsigned char x);

	//url ����
	std::string UrlEncode(const std::string& str);

	//url ����
	std::string UrlDecode(const std::string& str);

	//��������
	void PreParseGetParam();

private:
	tcp::socket _socket;

	//���ļ�׼����buffer
	beast::flat_buffer _buffer{ 8192 };

	//������Ϣ
	http::request<http::dynamic_body> _request;

	//�ظ���Ϣ
	http::response<http::dynamic_body> _response;

	//�������ӳ�ʱ��ʱ��
	net::steady_timer deadline_{ _socket.get_executor(),std::chrono::seconds(60) };

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;


};

