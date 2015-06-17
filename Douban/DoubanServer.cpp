#include "DoubanParser.h"
#include "../src/Album/AlbumServer.h"
int main()
{
	boost::asio::io_service file_io;
	boost::asio::io_service msg_io;
	boost::asio::io_service http_io;
	std::string file_port, msg_port,download_path;
	std::cout << "the file port is " << std::endl;
	cin >> file_port;
	std::cout << "the msg port is " << std::endl;
	cin >> msg_port;
	DoubanParser cur_parser;
	AlbumServer cur_server(msg_io, file_io, http_io,msg_port, file_port, "./douban_files", cur_parser);
	cur_server.run();
	std::thread msg_thread([&]()
	{
		msg_io.run();
	});
	std::thread file_thread([&]()
	{
		file_io.run();
	});
	msg_thread.join();
	file_thread.join();
}