#include "HtmlParser.h"
#include "../Http/HttpFileClient.h"
#include "../Http/HttpStreamClient.h"
#include <vector>
class HttpClient
{
private:
	const HtmlParser& html_parser;
	HttpStreamClient stream_client;
	HttpFileClient file_client;
public:
	HttpClient(boost::asio::io_service& IN_io, const std::string& directory, const HtmlParser& IN_parser) :stream_client(IN_io), file_client(IN_io, directory, 1), html_parser(IN_parser)
	{

	}
	std::vector<std::string> get_pic_head_link(const std::string& single_link)
	{
		
		std::vector<std::string> result_pic_link;
		std::string host_name = html_parser.get_hostname(single_link);
		std::stringbuf page_streambuf;
		std::stringbuf request;
		html_parser.generate_request(single_link, request);
		stream_client.tackle_single_request(request.str(), page_streambuf, host_name);
		std::string page_stream = page_streambuf.str();
		auto parse_result = html_parser.parse_html(page_stream, single_link);
		for (auto i : parse_result)
		{
			result_pic_link.push_back(i.first);
		}
		return result_pic_link;
		
	}
	std::vector<std::string> get_pic_file_link(const std::vector<std::string>& album_link)
	{
		if (album_link.empty())
		{
			return std::vector<std::string>();
		}
		std::vector<std::string> result_pic_file_link;
		std::vector<std::string> second_round;
		for (auto single_link : album_link)
		{
			std::string host_name = html_parser.get_hostname(single_link);
			std::stringbuf page_streambuf;
			std::stringbuf request;
			html_parser.generate_request(single_link, request);
			auto success=stream_client.tackle_single_request(request.str(), page_streambuf,host_name);
			if (!success)
			{
				std::cout << "link get failure : " << single_link << std::endl;
				continue;
			}
			std::string page_stream = page_streambuf.str();
			auto parse_result = html_parser.parse_html(page_stream,single_link);
			for (auto single_link_result : parse_result)
			{
				if (single_link_result.second)
				{
					result_pic_file_link.push_back(single_link_result.first);
				}
				else
				{
					second_round.push_back(single_link_result.first);
				}
			}
		}
		auto second_round_result = get_pic_file_link(second_round);
		for (auto single_link : second_round_result)
		{
			result_pic_file_link.push_back(single_link);
		}
		return result_pic_file_link;
	}
	std::vector<std::string> get_pic_file(const std::vector<std::string>& all_pic_file_links)
	{
		std::vector<std::string> success_links;
		for (auto single_file : all_pic_file_links)
		{
			std::string host_name;
			std::string file_name;
			std::stringbuf request_buf;
			std::string request_string;
			html_parser.generate_request(single_file,request_buf);
			host_name = html_parser.get_hostname(single_file);
			file_name = html_parser.get_file_name(single_file);
			request_string = request_buf.str();
			auto is_success=file_client.tackle_single_request(request_string, file_name, host_name);
			if (is_success)
			{
				success_links.push_back(single_file);
			}
			else
			{
				std::cout << "the file get failure : " << single_file << std::endl;
			}
		}
		return success_links;
	}
};