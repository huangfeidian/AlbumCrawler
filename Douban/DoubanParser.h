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
		//http://www.douban.com/people/lowdive/photos 这个是个人的所有相册
		//http://www.douban.com/photos/album/148563068/ 这个是单个相册
		//http://www.douban.com/photos/photo/2208957478/ 这个是单个图片的页面
		//http://www.douban.com/photos/album/148563068/?start=18 这个是单个相册里面不同的子页面
		//http://www.douban.com/photos/photo/2213829470/large 这个是单独的一个照片的大图页面
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
				if (second_part_str == "photo")//对应的是单张图片的地址
				{
					if (weblink.find("large", second_delimiter) == std::string::npos)//如果不是大图地址
					{
						auto single_pic_link = extract_single_pic(webpage);
						result.push_back(single_pic_link);
					}
					else// 如果是大图地址
					{
						result.push_back(std::make_pair(extract_large(webpage), true));
					}
				}
				else
				{
					if (second_part_str == "album")
					{
						if (weblink.find("?") != std::string::npos)//如果是专辑中某一页的地址
						{
							auto pages = extract_album_head(webpage, weblink);
							for (auto single_page : pages)
							{
								result.push_back(std::make_pair(single_page, false));
							}
						}
						else//如果是专辑首页的地址
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
		if (block_of_page != std::string::npos)//对应个人相册不止一页的情况
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
		else//否则直接返回当前页
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
		//通过处理当前图片的网页得到图片的链接，如果没有查看大图选项，则返回图片链接地址和true；如果有查看大图选项，则返回大图的网页链接和false
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
		//通过处理个人相册页面来得到当前页面的所有图片的网页的链接 注意这个并不仅仅是个人相册的第一个页面
		//同时并不是直接返回当前相册的所有页面
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
		auto begin_of_page = album_head_webpage.find("data-total-page=");//有些相册可能只有一页
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