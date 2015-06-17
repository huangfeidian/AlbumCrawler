#include "DoubanParser.h"
#include "UriClient.h"
int main()
{
	try
	{
		DoubanParser cur_douban_parser;
		boost::asio::io_service cur_io;
		UriClient cur_douban_uri_client(cur_io, "./", cur_douban_parser);
		std::cout << "please enter the douban url" << std::endl;
		std::string  cur_url;
		std::cin >> cur_url;
		std::vector<std::string> temp_url_set;
		temp_url_set.push_back(cur_url);
		auto links = cur_douban_uri_client.get_pic_file_link(temp_url_set);
		auto files = cur_douban_uri_client.get_pic_file(links);
		for (auto i : files)
		{
			std::cout << "file get: " << i << std::endl;
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	
}