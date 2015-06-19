#include <string>
#include <vector>
#include "../Src/Http/HtmlParser.h"
#include <iostream>
class DoubanParser:public HtmlParser
{
public:
	DoubanParser()
	{

	}
	//std::vector<std::string> extract_pages(const std::string& webpage)
	//{

	//}
	std::vector<std::pair<std::string, bool>> parse_html(const std::string& webpage,const std::string& weblink)const
	{
		//http://www.douban.com/people/lowdive/photos ����Ǹ��˵��������
		//http://www.douban.com/photos/album/148563068/ ����ǵ������
		//http://www.douban.com/photos/photo/2208957478/ ����ǵ���ͼƬ��ҳ��
		//http://www.douban.com/photos/album/148563068/?start=18 ����ǵ���������治ͬ����ҳ��
		//http://www.douban.com/photos/photo/2213829470/large ����ǵ�����һ����Ƭ�Ĵ�ͼҳ��
		std::string douban_home{ "http://www.douban.com/" };
		std::vector<std::pair<std::string, bool>> result;
		auto is_douban = weblink.find(douban_home);
		if (is_douban >=weblink.size())
		{
			std::cout << " this is not a douban link" << std::endl;

			std::cout << weblink << std::endl;
			return std::vector < std::pair < std::string, bool>> ();
		}
		is_douban += douban_home.size();
		auto first_delimiter = weblink.find("/", is_douban);
		std::string first_part_str = weblink.substr(is_douban, first_delimiter-is_douban);
		if (first_part_str == "people")//this is a personal album pivotal link
		{
			if (weblink.find("?") == std::string::npos)
			{
				auto all_album_pages = extract_personalbum_all_page(webpage, weblink);
				for (auto i : all_album_pages)
				{
					std::cout << "pushing " << i << " to queue" << std::endl;
					result.push_back(std::make_pair(i, false));
				}

			}
			else
			{
				auto all_single_albums = extract_personalbum_signlepage(webpage);
				for (auto i : all_single_albums)
				{
					result.push_back(std::make_pair(i, false));
				}
			}
		}
		else
		{
			if (first_part_str == "photos")
			{
				auto second_delimiter = weblink.find("/", first_delimiter + 1);
				std::string second_part_str = weblink.substr(first_delimiter + 1, second_delimiter-first_delimiter-1);
				if (second_part_str == "photo")//��Ӧ���ǵ���ͼƬ�ĵ�ַ
				{
					if (weblink.find("large", second_delimiter) == std::string::npos)//������Ǵ�ͼ��ַ
					{
						auto single_pic_link = extract_single_pic(webpage);
						result.push_back(single_pic_link);
					}
					else// ����Ǵ�ͼ��ַ
					{
						result.push_back(std::make_pair(extract_large(webpage), true));
					}
				}
				else
				{
					if (second_part_str == "album")
					{
						if (weblink.find("?") != std::string::npos)//�����ר����ĳһҳ�ĵ�ַ
						{
							auto pages = extract_album_head(webpage, weblink);
							for (auto single_page : pages)
							{
								result.push_back(std::make_pair(single_page, false));
							}
						}
						else//�����ר����ҳ�ĵ�ַ
						{
							auto pages = extract_album_single_page(webpage);
							for (auto single_page : pages)
							{
								result.push_back(std::make_pair(single_page, false));
							}
						}
					}
				}
			}
			else
			{
				std::cout << "it is not a valid douban pic url" << std::endl;
				std::cout << first_part_str << std::endl;
			}
		}
		return result;

	}
	std::vector<std::string> extract_personalbum_signlepage(const std::string& webpage)const 
	{
		std::vector<std::string> result;
		auto block_of_album = webpage.find("class=\"album_photo\" href=");
		decltype(block_of_album) album_begin,album_end;
		while (block_of_album != std::string::npos)
		{
			album_begin = webpage.find("http",block_of_album);
			album_end = webpage.find('\"', album_begin);
			std::string album_link = webpage.substr(album_begin, album_end - album_begin);
			result.push_back(album_link);
			block_of_album = webpage.find("class=\"album_photo\" href=", album_end);
		}
		return result;
	}
	std::vector<std::string> extract_personalbum_all_page(const std::string& webpage,const std::string& weblink)const 
	{
		std::vector<std::string> result;
		auto block_of_page = webpage.find("data-total-page");
		if (block_of_page != std::string::npos)//��Ӧ������᲻ֹһҳ�����
		{
			block_of_page = webpage.find('\"', block_of_page);
			block_of_page++;
			int total_albums = 0;
			while (webpage[block_of_page] != '\"')
			{
				total_albums = total_albums * 10 + webpage[block_of_page] - '0';
				block_of_page++;
			}
			char inst_byte[5] = "";
			for (int i = 0; i < total_albums; i ++)
			{
				std::sprintf(inst_byte, "%d", i*16);
				result.push_back(weblink + "?start=" + inst_byte);
			}
		}
		else//����ֱ�ӷ��ص�ǰҳ
		{
			result=extract_album_single_page(webpage);
		}
		return result;
	}
	std::string extract_large(const std::string& webpage)const
	{
		auto begin_of_indicator = webpage.find("view-zoom view-zoom-out");
		auto begin_of_imgblock = webpage.find("src=", begin_of_indicator);
		auto begin_of_imgsrc = webpage.find("\"", begin_of_imgblock + 3);
		auto end_of_imgsrc = webpage.find("\"", begin_of_imgsrc + 2);
		int imgsrc_length = end_of_imgsrc - begin_of_imgsrc - 1;
		std::string result = webpage.substr(begin_of_imgsrc + 1, imgsrc_length);
		return result;
	}
	std::pair<std::string,bool> extract_single_pic(const std::string& webpage)const
		//ͨ������ǰͼƬ����ҳ�õ�ͼƬ�����ӣ����û�в鿴��ͼѡ��򷵻�ͼƬ���ӵ�ַ��true������в鿴��ͼѡ��򷵻ش�ͼ����ҳ���Ӻ�false
	{
		auto begin_of_indicator = webpage.find("image-show-inner");
		auto begin_of_imgblock = webpage.find("src=", begin_of_indicator);
		auto begin_of_imgsrc = webpage.find("\"", begin_of_imgblock + 3);
		auto end_of_imgsrc = webpage.find("\"", begin_of_imgsrc + 2);
		int imgsrc_length = end_of_imgsrc - begin_of_imgsrc - 1;
		std::string result = webpage.substr(begin_of_imgsrc + 1, imgsrc_length);
		auto begin_of_large = webpage.find("/large", end_of_imgsrc);
		
		if (begin_of_large == std::string::npos)
		{
			return std::make_pair(result,true);
		}
		else
		{
			auto begin_of_large_href = webpage.rfind("href", begin_of_large);
			auto begin_of_large_imgsrc = webpage.find("\"", begin_of_large_href);
			auto end_of_large_imgsrc = webpage.find("\"", begin_of_large_imgsrc + 2);
			auto large_imgsrc_length = end_of_large_imgsrc - begin_of_large_imgsrc - 1;
			std::string large_result = webpage.substr(begin_of_large_imgsrc + 1, large_imgsrc_length);
			return std::make_pair(large_result, false);
		}
	}
	std::vector<std::string> extract_album_single_page(const std::string& album_webpage)const
		//ͨ������������ҳ�����õ���ǰҳ�������ͼƬ����ҳ������ ע��������������Ǹ������ĵ�һ��ҳ��
		//ͬʱ������ֱ�ӷ��ص�ǰ��������ҳ��
	{
		std::vector<std::string> imgsrc_set{};
		auto begin_of_photo_wrap = album_webpage.find("<div class=\"photo_wrap\">");
		decltype(begin_of_photo_wrap) begin_of_imgsrc;
		decltype(begin_of_photo_wrap) end_of_imgsrc;
		int  imgsrc_length;

		while (begin_of_photo_wrap != std::string::npos)
		{
			begin_of_imgsrc = album_webpage.find("<a href=", begin_of_photo_wrap);
			end_of_imgsrc = album_webpage.find("\"", begin_of_imgsrc+10);
			begin_of_imgsrc = album_webpage.find("\"", begin_of_imgsrc);
			imgsrc_length = end_of_imgsrc - begin_of_imgsrc - 1;
			auto current_imgsrc = album_webpage.substr(begin_of_imgsrc + 1, imgsrc_length);
			imgsrc_set.push_back(current_imgsrc);
			begin_of_photo_wrap = album_webpage.find("<div class=\"photo_wrap\">", end_of_imgsrc);
		}
		return imgsrc_set;
	}
	std::vector<std::string> extract_album_head(const std::string& album_head_webpage,const std::string& album_head_link)const
	{
		const int pic_per_page = 18;
		std::vector<std::string> result_page;
		
		int total_pages = 0;
		auto begin_of_page = album_head_webpage.find("data-total-page=");//��Щ������ֻ��һҳ
		if (begin_of_page != std::string::npos)
		{
			auto end_of_page = album_head_webpage.find("=", begin_of_page);
			end_of_page+=2;
			while (album_head_webpage[end_of_page] >= '0'&&album_head_webpage[end_of_page] <= '9')
			{
				total_pages = total_pages * 10 + album_head_webpage[end_of_page] - '0';
				end_of_page++;
			}
		}
		for (int i = 0; i < total_pages; i++)
		{
			char inst_byte[5] = "";
			std::sprintf(inst_byte, "%d", i*pic_per_page);
			result_page.push_back(album_head_link + "?start=" + inst_byte);
		}
		return result_page;
	}
};