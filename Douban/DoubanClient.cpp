#include "../src/Album/AlbumClient.h"
#include "DoubanParser.h"
int main()
{
	boost::asio::io_service file_io;
	boost::asio::io_service msg_io;
	boost::asio::io_service http_io;
	DoubanParser cur_parser;
	std::string server_ip, msg_port, file_port;
	std::cout << "please type the server ip" << std::endl;
	cin >> server_ip;
	std::cout << "please type the file_port " << std::endl;
	cin >> file_port;
	std::cout << "please type the msg_port " << std::endl;
	cin >> msg_port;
	std::string path = "./temp";
	AlbumClient hehe(msg_io, file_io, server_ip, file_port, msg_port, path, 0, cur_parser);
	hehe.run();
	msg_io.run();
}