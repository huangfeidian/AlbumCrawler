#include "HttpFileClient.h"
#include "HttpStreamClient.h"
#include "DoubanParser.h"
#include "GenerateRequest.h"
#include <boost/filesystem.hpp>
#include "ChatMessage.h"
#include "ChatInstruction.h"
#include "RawFileServer.h"
class SimpleServer :std::enable_shared_from_this < SimpleServer >
{
private:
	DoubanParser douban_extract;
	HttpStreamClient html_client;
	HttpFileClient pic_client;
	uri current_uri_generator;
	tcp_file_server file_server;
	boost::filesystem::path root_path;
public:
	boost::asio::io_service& current_io;
public:
	void run();
	SimpleServer(boost::asio::io_service& IN_io, int chat_port, int file_port) :current_io(IN_io), html_client(IN_io, 1), pic_client(IN_io, 1), file_server(IN_io,file_port)
	{
		run();
	}
};