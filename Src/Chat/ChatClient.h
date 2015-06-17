//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include "ChatMessage.h"
#include <string>
#include <queue>
#include <cstring>
using boost::asio::ip::tcp;

typedef std::queue<chat_message> chat_message_queue;

class ChatClient
{
public:
	tcp::resolver::iterator endpoint_iterator;
	std::vector<std::string> all_uri;
	ChatClient(boost::asio::io_service& io_service,
		const std::string& host, const std::string& msg_port)
		: io_service_(io_service),
		socket_(io_service)
	{
		tcp::resolver resolver(io_service);
		endpoint_iterator = resolver.resolve({ host, msg_port });
		
	}
	void run()
	{
		do_connect(endpoint_iterator);
	}
	void close()
	{

		io_service_.post([this]()
		{
			socket_.close();
		});

	}

private:
	void do_connect(tcp::resolver::iterator endpoint_iterator)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
			[this](boost::system::error_code ec, tcp::resolver::iterator)
		{
			if (!ec)
			{
				std::cout << "sending the reg information" << std::endl;
				do_send_reg();
				//do_read_header();
			}
			else
			{
				std::cout << ec << std::endl;
				close();
			}
		});

	}
	void do_send_reg()
	{
		chat_message temp_msg;
		temp_msg.encode_header(chat_instructions::request_register);
		std::cout << "the length is " << temp_msg.length() << std::endl;
		boost::asio::async_write(socket_, boost::asio::buffer(temp_msg.data(), temp_msg.length()),
			[this](boost::system::error_code ec, std::size_t length)
		{
			wait_for_permit(ec, length);
		});

	}
	void wait_for_permit(boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.data(), chat_message::header_length),
				[this](boost::system::error_code ec, std::size_t length)
			{
				chat_instructions cur_inst;
				if (!ec&&read_msg_.decode_header(cur_inst))
				{
					if (cur_inst == chat_instructions::admit_register)
					{
						std::cout << "register accepted" << std::endl;
						wait_for_job();
					}
					else
					{
						if (cur_inst == chat_instructions::reject_register)
						{
							std::cout << "register declined" << std::endl;
						}
						else
						{
							std::cout << "unexpected register response" << std::endl;
						}
						close();
					}
				}
				else
				{
					std::cout << ec << std::endl;
					close();
				}
			});
		}
		else
		{
			std::cout << ec << std::endl;
			close();
		}
	}
	void wait_for_job()
	{
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](boost::system::error_code ec, std::size_t length)
		{
			chat_instructions cur_inst;
			std::string cur_uri;
			if (!ec&&read_msg_.decode_header(cur_inst))
			{
				if (cur_inst== chat_instructions::page_request)
				{
					decode_job_body();
				}
				else
				{
					if (cur_inst == chat_instructions::page_request_end)
					{
						std::cout << "the end of server page request" << std::endl;
						std::cout << "start to get the webpage " << std::endl;
						analyze_uri();
						std::cout << "start to send the files" << std::endl;
						send_files();
						std::cout << "sending eof" << std::endl;
						send_eof();
					}
					else
					{
						std::cout << "unexpected instruction " << std::endl;
						send_eof();
						close();
					}
				}
			}
			else
			{
				std::cout << ec << std::endl;
				send_eof();
				close();
			}
		});
	}
	void decode_job_body()
	{
		
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::string cur_uri;
				read_msg_.decode_uri(cur_uri);
				std::cout << "get uri from server :" << cur_uri << std::endl;
				all_uri.push_back(cur_uri);
				wait_for_job();
			}
			else
			{
				std::cout << ec << std::endl;
				close();
			}
		});
	}

	virtual void analyze_uri()
	{
		/*std::cout << "sending eof" << std::endl;
		send_eof();*/
	}

	virtual void send_files()
	{
		/*send_eof();*/
	}
	void send_eof()
	{
		chat_message end_of_file;
		end_of_file.encode_header(chat_instructions::file_send_end);
		boost::asio::async_write(socket_, boost::asio::buffer(end_of_file.data(), end_of_file.length()),
			[this](boost::system::error_code ec, std::size_t)
		{
			if (ec)
			{
				std::cout << ec << std::endl;
			}
			close();
		});
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	chat_message read_msg_;
};

