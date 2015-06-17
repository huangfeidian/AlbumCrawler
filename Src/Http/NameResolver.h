#include <map>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <unordered_set>
#include <vector>
#pragma once
using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class EndPointResolver
{
private:
	tcp::resolver resolver;
	std::vector < std::vector<tcp::endpoint>> all_endpoints;
	std::map<std::string, int>  host_endpoint_index;
	int index_bound = 0;
public:
	EndPointResolver(boost::asio::io_service& IN_io_service) :resolver(IN_io_service)
	{
		//do nothing
	}
	std::vector<tcp::endpoint>& resolve(std::string IN_string)
	{
		
		auto map_find_result = host_endpoint_index.find(IN_string);
		if (map_find_result == host_endpoint_index.end())
		{
			tcp::resolver::query query(IN_string, "http");
			std::vector<tcp::endpoint> host_endpoint{};
			host_endpoint_index.insert(std::make_pair(IN_string, index_bound));

			auto endpoint_first = resolver.resolve(query);
			tcp::resolver::iterator end;
			while (endpoint_first != end)
			{
				host_endpoint.push_back(*endpoint_first);
				endpoint_first++;
			}
			all_endpoints.push_back(host_endpoint);
			index_bound++;
			return all_endpoints[index_bound - 1];
		}
		else
		{
			int current_host_index = (*(map_find_result)).second;
			return all_endpoints[current_host_index];
		}
	}
};

