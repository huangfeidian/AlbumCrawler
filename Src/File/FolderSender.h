#include "RawFileInfo.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <boost/asio.hpp>
#include <filesystem>
class FolderSender
{
public:
	boost::asio::ip::tcp::socket socket_;
	std::tr2::sys::path root_path;
	boost::asio::ip::tcp::endpoint server_;
	FolderSender(boost::asio::io_service& IN_io_, const std::string& IN_root, const std::string& IN_server, const std::string& IN_port) :root_path(IN_root), socket_(IN_io_)
	{
		server_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(IN_server), std::stoi(IN_port));
	}
	void send()
	{
		
		auto begin_iterator = std::tr2::sys::directory_iterator(root_path);
		while (begin_iterator != std::tr2::sys::directory_iterator())
		{
			socket_.connect(server_);
			std::string filename = (*begin_iterator).path().string();
			begin_iterator++;
			FILE *fp = fopen(filename.c_str(), "rb");
			if (fp == NULL)
			{
				std::cerr << "cannot open file\n";
				return;
			}
			std::cout << "sending file" << filename << std::endl;
			//使用智能指针，防止程序出现异常时，fclose未被调用。
			std::shared_ptr<FILE> file_ptr(fp, fclose);

			clock_t cost_time = clock();
			File_info file_info;

			int filename_size = filename.size() + 1;
			uint32_t file_info_size = sizeof(file_info);
			uint32_t total_size = file_info_size + filename_size;
			if (total_size > k_buffer_size)
			{
				std::cerr << "File name is too long";
				return;
			}
			file_info.filename_size = filename_size;

			fseek(fp, 0, SEEK_END);
			file_info.filesize = ftell(fp);
			rewind(fp);

			memcpy(buffer, &file_info, file_info_size);
			memcpy(buffer + file_info_size, filename.c_str(), filename_size);
			uint32_t len = total_size;
			unsigned long long total_bytes_read = 0;
			while (true)
			{
				socket_.send(boost::asio::buffer(buffer, len), 0);
				if (feof(fp)) break;
				len = fread(buffer, 1, k_buffer_size, fp);
				total_bytes_read += len;
			}
			socket_.close();
		}
	}
private:
	const int k_buffer_size = 32 * 1024;
	char buffer[32 * 1024];
	void delete_folder()
	{
		std::tr2::sys::remove_all(root_path);
	}
public:
	~FolderSender()
	{
		delete_folder();
	}
};