#include "../Src/File/RawFileClient.h"
#include <iostream>
int main(int args, char* argc [])
{
	if (args < 3)
	{
		std::cerr << "Usage: " << argc[0] << " ip_address  filename1 filename2 ...\n";
		return 1;
	}

	boost::asio::io_service io;
	for (int i = 2; i < args; ++i)
	{
		try
		{
			sender(io, argc[1], 1345, argc[i]);
		}
		catch (std::exception& err)
		{
			std::cerr << err.what() << "\n";
		}
	}
}