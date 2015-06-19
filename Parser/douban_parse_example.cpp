#include "../Douban/DoubanParser.h"
#include "../Src/Http/HttpStreamClient.h"
int main()
{
	boost::asio::io_service http_io;
	DoubanParser parser;
	HttpStreamClient douban_client(http_io);
	std::cout << "please type in the album url" << std::endl;
	std::string album_link;
	std::cin >> album_link;
	std::string host_name = parser.get_hostname(album_link);
	std::stringbuf page_streambuf;
	std::stringbuf request;
	parser.generate_request(album_link, request);
	douban_client.tackle_single_request(request.str(), page_streambuf, host_name);
	std::cout << "analyzing " << album_link << std::endl;
	auto all_album_pages = parser.extract_personalbum_all_page(page_streambuf.str(), album_link);
	for (auto i : all_album_pages)
	{
		std::cout << "contains page " << i << std::endl;
	}
	request.swap(std::stringbuf());
	page_streambuf.swap(std::stringbuf());
	album_link = all_album_pages[0];
	host_name = parser.get_hostname(album_link);
	parser.generate_request(album_link, request);
	douban_client.tackle_single_request(request.str(), page_streambuf, host_name);
	std::cout << "analyzing " << album_link << std::endl;
	all_album_pages.swap(std::vector<std::string>());
	all_album_pages = parser.extract_personalbum_signlepage(page_streambuf.str());
	for (auto i : all_album_pages)
	{
		std::cout << "contains page " << i << std::endl;
	}
	request.swap(std::stringbuf());
	page_streambuf.swap(std::stringbuf());
	album_link = all_album_pages[0];
	host_name = parser.get_hostname(album_link);
	parser.generate_request(album_link, request);
	douban_client.tackle_single_request(request.str(), page_streambuf, host_name);
	std::cout << "analyzing " << album_link << std::endl;
	all_album_pages.swap(std::vector<std::string>());
	all_album_pages = parser.extract_album_head(page_streambuf.str(),album_link);
	for (auto i : all_album_pages)
	{
		std::cout << "contains page " << i << std::endl;
	}
	request.swap(std::stringbuf());
	page_streambuf.swap(std::stringbuf());
	album_link = all_album_pages[0];
	host_name = parser.get_hostname(album_link);
	parser.generate_request(album_link, request);
	douban_client.tackle_single_request(request.str(), page_streambuf, host_name);
	std::cout << "analyzing " << album_link << std::endl;
	all_album_pages.swap(std::vector<std::string>());
	all_album_pages = parser.extract_album_single_page(page_streambuf.str());
	for (auto i : all_album_pages)
	{
		std::cout << "contains page " << i << std::endl;
	}
}
