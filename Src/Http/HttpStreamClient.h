#include <iostream>
#include "../Http/NameResolver.h"
#include <boost/asio.hpp>
#include <queue>
#include <tuple>
#include <thread>
#include <chrono>
#include <random>
using namespace boost::asio;
using boost::asio::ip::tcp;
class HttpStreamClient
{
private:
	EndPointResolver current_resolver;
	tcp::socket current_socket;
	std::default_random_engine dre;
	std::uniform_int_distribution<int> time_snap;
private:
	bool get_response(const std::string& request,std::stringbuf& out_buf,const std::string& hostname)
	{

		auto endpoints = current_resolver.resolve(hostname);
		auto end = endpoints.cend();
		auto endpoint_iterator = endpoints.cbegin();
		// Try each endpoint until we successfully establish a connection.
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			current_socket.close();
			current_socket.connect(*endpoint_iterator++, error);
		}


		if (error)
		{
			return false;
		}
		// Send the request.
		boost::asio::write(current_socket, boost::asio::buffer(request));

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(current_socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		if (status_code != 200)
		{
			return false;
		}
		std::string status_message;
		std::getline(response_stream, status_message);


		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(current_socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		{
		}

		// Write whatever content we already have to output.
		ostream outfile(&out_buf);
		if (response.size() > 0)
		{
			outfile << &response;
		}
		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(current_socket, response, boost::asio::transfer_at_least(1), error))
		{
			outfile << &response;
		}
		outfile.flush();
		return true;
	}

public:
	HttpStreamClient() = delete;
	HttpStreamClient(boost::asio::io_service& IN_io_service, int time_interval=1) : current_resolver(IN_io_service), current_socket(IN_io_service), time_snap(1, time_interval)
	{

	}
	
	bool tackle_single_request(const std::string& IN_request, std::stringbuf& OUT_file, const std::string& IN_hostname)
		//注意此时必须保证之前的submit已经处理完，否则错了不要怪我
	{
		return get_response(IN_request,OUT_file,IN_hostname);
	}

};

