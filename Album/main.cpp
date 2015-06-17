#include "Extract.h"
#include <iostream>
#include <fstream>
#include "HttpFileClient.h"
#include "GenerateRequest.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include "HttpStreamClient.h"
using namespace std;
using namespace boost::asio;
int main()
{
	
	//Extract hehe{};
	//io_service current_service;
	//uri current_uri_generator{};
	//EndPointResolver current_resolver(current_service);
	//string current_url = "http://www.douban.com/doulist/2944989/";
	//stringbuf current_stringbuf{};
	//auto current_request = current_uri_generator.get_request(current_url, current_stringbuf);
	//string host_name = current_request.first;
	//ofstream file("album_example.htm", ios::out | ios::binary);
	//auto& begin_of_endpoint = current_resolver.resolve(host_name);
	//string current_request_string = current_stringbuf.str();
	//if (current_request.second.compare("null")==0)
	//{
	//	get_response(current_request_string, file, begin_of_endpoint, current_service);
	//}
	//file.close();
	boost::asio::io_service current_io;
	Extract douban_extract;
	HttpStreamClient html_client(current_io, 3);
	HttpFileClient pic_client(current_io, 3);
	uri current_uri_generator{};
	const string file_root = "F:\\douban";
	const string douban_pic_src = "http://www.douban.com/photos/album/149940508/?start=36";
	boost::filesystem::path root_path(file_root);
	if (!boost::filesystem::is_directory(root_path))
	{
		boost::filesystem::create_directory(root_path);
	}
	stringbuf htmlbuf;//存储所有的网页 注意当前是同步执行的，所以一次只需要一个buf就够了
	stringbuf requestbuf;//存储当前url的请求
	auto current_request = current_uri_generator.get_request(douban_pic_src, requestbuf);//根据相册的url生成request，填充到requestbuf中
	string request_string = requestbuf.str();//将requestbuf中的string抽出
	html_client.tackle_single_request(request_string, htmlbuf, current_request.first);//开始get操作 将结果存入htmlbuf中
	auto imgsrc_set = douban_extract.extract_album_page(htmlbuf.str());//然后再对所得到的网页进行parse，提取出所有的图片网页链接
	for (auto single_imgsrc : imgsrc_set)//对于每一个图片网页链接都提交获得该图片的请求
	{
		string final_imgsrc;
		requestbuf.swap(stringbuf());//首先清空requestbuf
		current_request = current_uri_generator.get_request(single_imgsrc, requestbuf);//根据当前图片网页的url，生成requestbuf
		request_string = requestbuf.str();//然后再提取出之中的string
		htmlbuf.swap(stringbuf());//清空之前的htmlbuf
		html_client.tackle_single_request(request_string,htmlbuf,current_request.first);//进行一个GET操作，返回的网页结果存入htmlbuf中
		auto single_request_parse_result = douban_extract.extract_single_pic(htmlbuf.str());//对该网页中的图片地址进行分析
		final_imgsrc = single_request_parse_result.first;
		if (single_request_parse_result.second == false)//如果有大图
		{
			requestbuf.swap(stringbuf());//再次清空requestbuf
			current_request = current_uri_generator.get_request(single_request_parse_result.first, requestbuf);//对大图的地址生成一个request存入requestbuf中
			string request_string = requestbuf.str();
			htmlbuf.swap(stringbuf());//再次清空htmlbuf
			html_client.tackle_single_request(request_string, htmlbuf, current_request.first);//然后以新的requestbuf进行GET操作
			final_imgsrc = douban_extract.extract_large(htmlbuf.str());//然后把最终的大图地址填入single_request_parse_result中
		}
		requestbuf.swap(stringbuf());//再次清空requestbuf
		current_request = current_uri_generator.get_request(final_imgsrc, requestbuf);//根据图片地址生成requestbuf
		auto file_name = current_request.second;
		auto file_path = file_root + "\\" + file_name;//获得最终的文件路径
		pic_client.submit_request(requestbuf.str(),file_path, current_request.first);//最后再把这个文件下载的请求提交
	}
	pic_client.tackle_all_request();//最后 再开始下载图片

	
}