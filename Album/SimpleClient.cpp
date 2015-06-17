#include "ChatClient.h"
#include "douban_uri_client.cpp"
class douban_client:public ChatClient
{
public:
	douban_uri_client douban_uri;
	douban_client(boost::asio::io_service& IN_io, const std::string& host, const std::string& msg_port, const std::string& file_port) :ChatClient(IN_io, host, msg_port),
		douban_uri(IN_io, host, file_port)
	{

	}
	void do_uri()
	{
		douban_uri.get_pic(all_uri);
		douban_uri.send_files();
		
	}
};