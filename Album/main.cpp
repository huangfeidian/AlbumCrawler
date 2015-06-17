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
	stringbuf htmlbuf;//�洢���е���ҳ ע�⵱ǰ��ͬ��ִ�еģ�����һ��ֻ��Ҫһ��buf�͹���
	stringbuf requestbuf;//�洢��ǰurl������
	auto current_request = current_uri_generator.get_request(douban_pic_src, requestbuf);//��������url����request����䵽requestbuf��
	string request_string = requestbuf.str();//��requestbuf�е�string���
	html_client.tackle_single_request(request_string, htmlbuf, current_request.first);//��ʼget���� ���������htmlbuf��
	auto imgsrc_set = douban_extract.extract_album_page(htmlbuf.str());//Ȼ���ٶ����õ�����ҳ����parse����ȡ�����е�ͼƬ��ҳ����
	for (auto single_imgsrc : imgsrc_set)//����ÿһ��ͼƬ��ҳ���Ӷ��ύ��ø�ͼƬ������
	{
		string final_imgsrc;
		requestbuf.swap(stringbuf());//�������requestbuf
		current_request = current_uri_generator.get_request(single_imgsrc, requestbuf);//���ݵ�ǰͼƬ��ҳ��url������requestbuf
		request_string = requestbuf.str();//Ȼ������ȡ��֮�е�string
		htmlbuf.swap(stringbuf());//���֮ǰ��htmlbuf
		html_client.tackle_single_request(request_string,htmlbuf,current_request.first);//����һ��GET���������ص���ҳ�������htmlbuf��
		auto single_request_parse_result = douban_extract.extract_single_pic(htmlbuf.str());//�Ը���ҳ�е�ͼƬ��ַ���з���
		final_imgsrc = single_request_parse_result.first;
		if (single_request_parse_result.second == false)//����д�ͼ
		{
			requestbuf.swap(stringbuf());//�ٴ����requestbuf
			current_request = current_uri_generator.get_request(single_request_parse_result.first, requestbuf);//�Դ�ͼ�ĵ�ַ����һ��request����requestbuf��
			string request_string = requestbuf.str();
			htmlbuf.swap(stringbuf());//�ٴ����htmlbuf
			html_client.tackle_single_request(request_string, htmlbuf, current_request.first);//Ȼ�����µ�requestbuf����GET����
			final_imgsrc = douban_extract.extract_large(htmlbuf.str());//Ȼ������յĴ�ͼ��ַ����single_request_parse_result��
		}
		requestbuf.swap(stringbuf());//�ٴ����requestbuf
		current_request = current_uri_generator.get_request(final_imgsrc, requestbuf);//����ͼƬ��ַ����requestbuf
		auto file_name = current_request.second;
		auto file_path = file_root + "\\" + file_name;//������յ��ļ�·��
		pic_client.submit_request(requestbuf.str(),file_path, current_request.first);//����ٰ�����ļ����ص������ύ
	}
	pic_client.tackle_all_request();//��� �ٿ�ʼ����ͼƬ

	
}