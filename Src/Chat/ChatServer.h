#include "chatSession.h"
class ChatServer
{
public:

	ChatServer(boost::asio::io_service& io_service, short port)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service), wait_windows(io_service, boost::posix_time::minutes(1)), io_service_(io_service)
	{
		is_expired.store(false);
		connection_count.store(0);
	}
	void run()
	{
		wait_windows.async_wait(std::bind(&ChatServer::get_album, this, std::placeholders::_1));
		listen();
	}
private:
	void listen()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				connection_count++;
				all_session.push_back(std::make_shared<session>(std::move(socket_), io_service_,connection_count,is_expired));
				all_session.back()->start();
			}
			listen();
		});
	}
	virtual void analyze_uri(const std::string& IN_uri)
	{
		all_uri.push_back(IN_uri);
	}
	void dispatch_uri()
	{
		int session_size = all_session.size();
		if (session_size == 0)
		{
			io_service_.stop();
			return;
		}
		for (int i = 0; i < all_uri.size(); i++)
		{
			all_session[i%session_size]->push_uri(all_uri[i]);
		}
		for (int i = 0; i < session_size; i++)
		{
			all_session[i]->uri_end();
		}
	}
	void get_album(const boost::system::error_code& error)
	{
		is_expired.store(true);
		std::cout << "please enter the douban album uri " << std::endl;
		std::string album_uri;
		std::cin >> album_uri;
		analyze_uri(album_uri);
		dispatch_uri();
	}
private:
	std::vector<std::shared_ptr<session>> all_session;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	boost::asio::deadline_timer wait_windows;
	std::atomic_bool is_expired;
	std::atomic_int connection_count;
	boost::asio::io_service& io_service_;
public:
	std::vector<std::string> all_uri;
};