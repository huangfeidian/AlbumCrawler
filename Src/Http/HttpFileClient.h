#include <iostream>
#include "../Http/NameResolver.h"
#include <boost/asio.hpp>
#include <filesystem>
#include <queue>
#include <fstream>
#include <tuple>
#include <thread>
#include <chrono>
#include <random>
using namespace boost::asio;
using boost::asio::ip::tcp;
class HttpFileClient
{
private:
	EndPointResolver current_resolver;
	tcp::socket current_socket;
	std::default_random_engine dre;
	std::queue < std::tuple<std::string,std::string  ,std::string>> request_queue;
	std::uniform_int_distribution<int> time_snap;
	std::tr2::sys::path download_dir;
private:
	bool get_response()
	{
		auto current_request = request_queue.front();
		request_queue.pop();
		std::string request=get<0>(current_request);
		std::string outfile_name=get<1>(current_request);
		std::string hostname=get<2>(current_request);

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
		std::cout << "getting " << outfile_name << std::endl;
		// Write whatever content we already have to output.
		std::string outfile_path = download_dir.string() +"/"+ outfile_name;
		auto outfile = ofstream(outfile_path, std::ios::out | std::ios::binary);
		if (response.size() > 0)
		{
			outfile << &response;
		}
		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(current_socket, response, boost::asio::transfer_at_least(1), error))
		{
			outfile << &response;
		}
		outfile.close();
		return true;
	}

public:
	HttpFileClient() = delete;
	HttpFileClient(boost::asio::io_service& IN_io_service, const std::string& IN_dir, int time_interval = 1) : current_resolver(IN_io_service), current_socket(IN_io_service), time_snap(1, time_interval), download_dir(IN_dir)
	{
		if (!std::tr2::sys::is_directory(download_dir))
		{
			std::tr2::sys::create_directory(download_dir);
		}
	}
	void submit_request(std::string IN_request, std::string OUT_file, std::string IN_hostname)
	{
		request_queue.push(std::make_tuple(IN_request, OUT_file, IN_hostname));
	}
	void tackle_all_request()
	{
		while (!request_queue.empty())
		{
			/*this_thread::sleep_for(chrono::seconds(time_snap(dre)));*/
			get_response();
		}
	}
	bool tackle_single_request(std::string IN_request, std::string OUT_file , std::string IN_hostname)
		//注意此时必须保证之前的submit已经处理完，否则错了不要怪我
	{
		request_queue.push(std::make_tuple(IN_request, OUT_file, IN_hostname));
		return get_response();
	}
};

