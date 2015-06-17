#include "../Chat/ChatClient.h"
#include "../Http/HttpClient.h"
#include "../File/FolderSender.h"
class AlbumClient :public ChatClient
{
private:
	HttpClient http_client;
	FolderSender folder_sender;
public:
	AlbumClient(boost::asio::io_service& msg_io, boost::asio::io_service& file_io, const std::string& server,
		const std::string& file_port, const std::string& chat_port, 
		const std::string& download_path, int time_gap,
		const HtmlParser& IN_parser) :ChatClient(msg_io, server, chat_port), http_client(file_io, download_path, IN_parser), folder_sender(file_io,download_path,server,file_port)
	{
		
	}
	void analyze_uri()
	{
		auto all_file_links=http_client.get_pic_file_link(all_uri);
		http_client.get_pic_file(all_file_links);
		
	}
	void send_files()
	{
		folder_sender.send();
	}
};