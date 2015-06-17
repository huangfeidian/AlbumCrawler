#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#pragma once
class HtmlParser
{
public:
	HtmlParser()
	{

	}
	virtual std::vector<std::pair<std::string, bool>> parse_html(const std::string& webpage,const std::string& weblink)const
	{
		return std::vector<std::pair<std::string, bool>>();
	}
	std::string get_hostname(const std::string& IN_url) const
	{
		auto begin_of_delimitor = IN_url.find("//");
		auto end_of_delimitor = IN_url.find("/", begin_of_delimitor + 2);
		std::string hostname = IN_url.substr(begin_of_delimitor + 2, end_of_delimitor - begin_of_delimitor - 2);
		return hostname;
	}
	void generate_request(const std::string& IN_url, std::stringbuf& request)const
	{
		auto begin_of_delimitor = IN_url.find("//");
		auto end_of_delimitor = IN_url.find("/", begin_of_delimitor + 2);
		std::string hostname = IN_url.substr(begin_of_delimitor + 2, end_of_delimitor - begin_of_delimitor - 2);
		std::string content_url = IN_url.substr(end_of_delimitor);
		auto end_of_dot = IN_url.rfind(".");
		if (end_of_dot < end_of_delimitor || end_of_dot < (IN_url.size() - 5))//webpage request
		{
			std::ostream request_stream((&request));

			request_stream << "GET " << content_url << " HTTP/1.0\r\n";
			request_stream << "User-Agent: " << "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0) " << "\r\n";
			request_stream << "Host: " << hostname << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Connection: close\r\n\r\n";
		}
		else//get file
		{
			std::ostream request_stream((&request));
			request_stream << "GET " << content_url << " HTTP/1.0\r\n";
			request_stream << "Host: " << hostname << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Connection: close\r\n\r\n";
			auto file_name_delimitor = IN_url.rfind("/");
		}
	}
	std::string get_file_name(const std::string& IN_url)const
	{
		auto begin_of_delimitor = IN_url.find("//");
		auto end_of_delimitor = IN_url.find("/", begin_of_delimitor + 2);
		//std::string hostname = IN_url.substr(begin_of_delimitor + 2, end_of_delimitor - begin_of_delimitor - 2);
		std::string content_url = IN_url.substr(end_of_delimitor);
		auto end_of_dot = IN_url.rfind(".");
		auto file_name_delimitor = IN_url.rfind("/");
		std::string file_name = IN_url.substr(file_name_delimitor + 1);
		return  file_name;
	}
};