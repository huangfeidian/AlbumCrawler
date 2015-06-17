#include "../Http/HttpClient.h"
#include "../Chat/ChatServer.h"
#include "../File/RawFileServer.h"
class AlbumServer :public ChatServer
{
private:
	HttpClient http_client;
	FileServer file_server;
public:
	AlbumServer(boost::asio::io_service& msg_io, boost::asio::io_service& file_io, boost::asio::io_service& http_io,const std::string& msg_port, const std::string& file_port,
		const std::string& download_path, const HtmlParser& IN_parser) :ChatServer(msg_io, std::stoi(msg_port)), http_client(http_io, download_path, IN_parser), file_server(file_io, std::stoi(file_port),download_path)
	{
		file_server.run();
	}
	void analyze_uri(const std::string& IN_uri)
	{
		auto result_uri = http_client.get_pic_head_link(IN_uri);
		all_uri.swap(result_uri);
	}
};