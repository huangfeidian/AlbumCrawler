#include "../Src/File/FolderSender.h"
#include <iostream>
int main(int args, char* argc [])
{
	if (args != 4)
	{
		std::cerr << "Usage: " << argc[0] << " ip_address  port path \n";
		return 1;
	}

	boost::asio::io_service io;
	FolderSender folder_sender(io, argc[1], argc[2], argc[3]);
	folder_sender.send();
}