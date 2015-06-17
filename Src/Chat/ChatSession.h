#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "ChatMessage.h"
#include <vector>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>
#include <stack>
#include <atomic>
#include <thread>
using boost::asio::ip::tcp;
using chat_message_queue = std::queue < chat_message >;
class session
	: public std::enable_shared_from_this<session>
{
public:
	std::atomic_int& connection_count;
	std::atomic_bool&  is_expired;
	session(tcp::socket socket, boost::asio::io_service& IN_io, std::atomic_int& in_connection_count, std::atomic_bool&  in_is_expired)
		: socket_(std::move(socket)), io_service_(IN_io), connection_count(in_connection_count), is_expired(in_is_expired)
	{

	}

	void start()
	{
		std::cout << "connection from " << socket_.remote_endpoint().address() << std::endl;
		wait_for_register();
	}
	void close()
	{
		socket_.close();
		connection_count--;
		if (connection_count.load() == 0)
		{
			std::cout << " everything has finished" << std::endl;
			io_service_.stop();
		}
	}
	void push_uri(const std::string& uri)
	{
		uri_stack.push(uri);
	}
	void uri_end(void)
	{
		if (!uri_stack.empty())
		{
			chat_message temp_msg;
			std::cout << "sending message to remote " << std::endl;
			temp_msg.encode_uri(uri_stack.top());
			uri_stack.pop();
			boost::asio::async_write(socket_, boost::asio::buffer(temp_msg.data(), temp_msg.length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					uri_end();
				}
				else
				{
					std::cout << "error while sending uri" << std::endl;
					std::cout << ec << std::endl;
					close();
				}
			});
		}
		else
		{
			chat_message temp_msg;
			std::cout << "sending the end of request to remote" << std::endl;
			temp_msg.encode_header(chat_instructions::page_request_end);
			boost::asio::async_write(socket_, boost::asio::buffer(temp_msg.data(), temp_msg.length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
			{

				if (ec)
				{
					std::cout << "error while sending end of request" << std::endl;
					std::cout << ec << std::endl;
					close();
				}
			});
		}
	}

private:
	void wait_for_register()
	{
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](boost::system::error_code ec, std::uint32_t length)
		{
			chat_instructions cur_inst;
			if (!ec && read_msg_.decode_header(cur_inst))
			{
				std::cout << "the body length is " << read_msg_.body_length() << std::endl;
				std::cout << "the instruction is " << (int) cur_inst << std::endl;
				if (cur_inst == chat_instructions::request_register)
				{
					if (is_expired)
					{
						std::cout << "the window has expired, register declined" << std::endl;
						chat_message temp_msg;
						temp_msg.encode_header(chat_instructions::reject_register);
						boost::asio::async_write(socket_, boost::asio::buffer(temp_msg.data(), temp_msg.length()),
							[this](boost::system::error_code ec, std::size_t /*length*/)
						{
							close();
						});
					}
					else
					{
						std::cout << " register success" << std::endl;
						chat_message temp_msg;
						temp_msg.encode_header(chat_instructions::admit_register);
						boost::asio::async_write(socket_, boost::asio::buffer(temp_msg.data(), temp_msg.length()),
							[this](boost::system::error_code ec, std::size_t /*length*/)
						{
							if (!ec)
							{
								wait_for_eof();
							}
							else
							{
								close();
							}
						});
					}
				}
				else
				{
					std::cout << "unexpected message at the beginning" << std::endl;
					close();
				}
			}
			else
			{
				std::cout << " error while waiting for register head" << std::endl;
				std::cout << ec << std::endl;
				close();
			}
		});
	}
	void wait_for_eof()
	{
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](boost::system::error_code ec, std::uint32_t /*length*/)
		{
			chat_instructions cur_inst;
			if (!ec && read_msg_.decode_header(cur_inst))
			{
				if (cur_inst == chat_instructions::file_send_end)
				{
					std::cout << "file_end" << std::endl;
				}
				else
				{
					std::cout << "unexpected message while waiting for file file_send_end" << std::endl;
				}
				close();
			}
			else
			{
				close();
			}
		});
	}
	void wait_eof_body()
	{
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this](boost::system::error_code ec, std::uint32_t /*length*/)
		{
			if (!ec)
			{
				chat_instructions cur_inst;
				read_msg_.decode_header(cur_inst);
				if (cur_inst == chat_instructions::file_send_end)
				{
					std::cout << "file_end" << std::endl;
				}
				else
				{
					std::cout << "unexpected message while waiting for file file_send_end" << std::endl;
				}

			}
			else
			{
				std::cout << "error while waiting for file file_send_end" << std::endl;
				std::cout << ec << std::endl;
			}
			connection_count--;
			socket_.close();
			if (connection_count == 0)
			{
				std::cout << " everything has finished" << std::endl;
				io_service_.stop();
			}
		});
	}

	tcp::socket socket_;
	boost::asio::io_service& io_service_;
	enum
	{
		max_length = 512
	};
	char data_[max_length];
	chat_message read_msg_;
	std::stack<std::string> uri_stack;
};
