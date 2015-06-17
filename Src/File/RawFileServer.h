//server_receiver
#include "FileSession.h"
#include <filesystem>
class FileServer
{
public:
	typedef boost::asio::ip::tcp TCP;
	typedef boost::system::error_code Error;
	std::tr2::sys::path recieve_path;
	FileServer(boost::asio::io_service& io, unsigned port,std::string download_path="./download") :
		acceptor_(io, TCP::endpoint(TCP::v4(), port)), recieve_path(download_path)
	{
		
	}

	static void print_asio_error(const Error& error)
	{
		std::cerr << error.message() << "\n";
	}
	void run()
	{
		start_accept();
	}
private:
	void start_accept()
	{
		FileSession::Session_ptr session = FileSession::create(acceptor_.get_io_service(),recieve_path);
		acceptor_.async_accept(session->socket(),
			std::bind(&FileServer::handle_accept, this, session, std::placeholders::_1));
	}

	void handle_accept(FileSession::Session_ptr session, const Error& error)
	{
		if (error) return print_asio_error(error);
		session->start();
		start_accept();
	}

	TCP::acceptor acceptor_;
};
