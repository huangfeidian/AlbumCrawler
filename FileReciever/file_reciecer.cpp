#include "../Src/File/RawFileServer.h"
#include <iostream>
int main()
{
	std::cout << "Auto receive files and save then in current directory.\n";
	boost::asio::io_service io;
	FileServer receiver(io, 1345);
	io.run();
}