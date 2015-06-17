//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdint>
enum class chat_instructions :uint32_t
{
	request_register = 1,//客户端请求注册
	admit_register,//服务器端返回注册允许
	reject_register,//服务器端拒绝注册
	page_request,//请求解析此页面 我们单次只发送一个页面
	page_request_end,//请求结束
	file_send_end//文件发送完毕

};
class chat_message
{
public:
	enum
	{
		header_length = 8
	};
	enum
	{
		max_body_length = 512
	};

	chat_message()
		: body_length_(0)
	{
	}

	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	std::uint32_t length() const
	{
		return header_length + body_length_;
	}

	const char* body() const
	{
		return data_ + header_length;
	}

	char* body()
	{
		return data_ + header_length;
	}
	std::uint32_t body_length() const
	{
		return body_length_;
	}


	bool decode_header(chat_instructions& lala)
	{
		char header[5] = "";
		char instruction[5] = "";
		std::strncat(header, data_, 4);
		body_length_ = std::atoi(header);
		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		std::strncat(instruction, data_+4, 4);
		lala=chat_instructions(atoi(instruction));
		return true;
	}
	void encode_header(chat_instructions lala)
	{
		int inst_int = (int) lala;
		char inst_byte[5] = "";
		std::sprintf(inst_byte, "%4d", inst_int);
		std::memcpy(data_ +4, inst_byte, 4);
		char header[5] = "";
		std::sprintf(header, "%4d", 0);
		std::memcpy(data_, header, 4);
		body_length_ = 0;

	}

	bool encode_uri(const std::string& IN_uri)
	{
		std::uint32_t uri_length = IN_uri.size();
		if (uri_length >= max_body_length - 1)
		{
			return false;
		}
		body_length_ = uri_length + 1;
		std::memcpy(data_ + header_length , IN_uri.data(), uri_length);
		data_[header_length + body_length_ - 1] = '\0';
		char header[5] = "";
		std::sprintf(header, "%4d", body_length_);
		std::memcpy(data_, header, header_length);
		int inst_int = (int) chat_instructions::page_request;
		char inst_byte[5] = "";
		std::sprintf(inst_byte, "%4d", inst_int);
		std::memcpy(data_ + 4, inst_byte, 4);
		return true;
	}
	void decode_uri(std::string& OUT_uri)
	{
		OUT_uri = data_ + 8;
	}
private:
	char data_[header_length + max_body_length];
	std::uint32_t body_length_;
};

#endif // CHAT_MESSAGE_HPP
