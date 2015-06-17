#include <iostream>
#include <cstdio>
#include "RawFileInfo.h"
#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include <filesystem>
class FileSession : public std::enable_shared_from_this<FileSession>
{
public:
	typedef boost::asio::ip::tcp TCP;
	typedef boost::system::error_code Error;
	typedef std::shared_ptr<FileSession> Session_ptr;

	static void print_asio_error(const Error& error)
	{
		std::cerr << error.message() << "\n";
	}

	static Session_ptr create(boost::asio::io_service& io, const std::tr2::sys::path& in_path)
	{
		return Session_ptr(new FileSession(io, in_path));
	}

	TCP::socket& socket()
	{
		return socket_;
	}

	~FileSession()
	{
		if (fp_) fclose(fp_);
		/*clock_ = clock() - clock_;
		uint32_t bytes_writen = total_bytes_writen_;
		if (clock_ == 0) clock_ = 1;
		double speed = bytes_writen * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / clock_;
		std::cout << "cost time: " << clock_ / (double) CLOCKS_PER_SEC << " s  "
		<< "bytes_writen: " << bytes_writen << " bytes\n"
		<< "speed: " << speed << " MB/s\n\n";*/
	}

	void start()
	{
		clock_ = clock();
		std::cout << "client: " << socket_.remote_endpoint().address() << "\n";
		socket_.async_receive(
			boost::asio::buffer(reinterpret_cast<char*>(&file_info_), sizeof(file_info_)),
			std::bind(&FileSession::handle_header, shared_from_this(), std::placeholders::_1));
	}

private:
	FileSession(boost::asio::io_service& io, const std::tr2::sys::path& in_path) : socket_(io), fp_(NULL), total_bytes_writen_(0), recieve_path(in_path)
	{
	}

	void handle_header(const Error& error)
	{
		if (error)
		{
			return print_asio_error(error);
		}
		size_t filename_size = file_info_.filename_size;
		if (filename_size > k_buffer_size)
		{
			std::cerr << "Path name is too long!\n";
			return;
		}
		//得用async_read, 不能用async_read_some，防止路径名超长时，一次接收不完
		boost::asio::async_read(socket_, boost::asio::buffer(buffer_, file_info_.filename_size),
			std::bind(&FileSession::handle_file, shared_from_this(), std::placeholders::_1));
	}

	void handle_file(const Error& error)
	{
		if (error)
		{
			return print_asio_error(error);
		}
		const char *basename = buffer_ + file_info_.filename_size - 1;
		while (basename >= buffer_ && (*basename != '\\' && *basename != '/'))
		{
			--basename;
		}
		++basename;

		std::cout << "Open file: " << basename << " (" << buffer_ << ")\n";
		std::string file_path = recieve_path.string()+"/" + basename;
		fp_ = fopen(file_path.c_str(), "wb");
		if (fp_ == NULL)
		{
			std::cerr << "Failed to open file to write\n";
			return;
		}
		receive_file_content();
	}

	void receive_file_content()
	{
		socket_.async_receive(boost::asio::buffer(buffer_, k_buffer_size),
			std::bind(&FileSession::handle_write, shared_from_this(), std::placeholders::_1,
			std::placeholders::_2));
	}

	void handle_write(const Error& error, size_t bytes_transferred)
	{
		if (error)
		{
			if (error != boost::asio::error::eof) return print_asio_error(error);
			uint32_t filesize = file_info_.filesize;
			if (total_bytes_writen_ != filesize)
				std::cerr << "Filesize not matched! " << total_bytes_writen_
				<< "/" << filesize << "\n";
			return;
		}
		total_bytes_writen_ += fwrite(buffer_, 1, bytes_transferred, fp_);
		receive_file_content();
	}

	clock_t clock_;
	TCP::socket socket_;
	FILE *fp_;
	std::tr2::sys::path recieve_path;
	File_info file_info_;
	uint32_t total_bytes_writen_;
	static const unsigned k_buffer_size = 1024 * 32;
	char buffer_[k_buffer_size];
};
