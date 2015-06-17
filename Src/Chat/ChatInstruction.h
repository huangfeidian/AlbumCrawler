
enum class chat_instructions:int
{
	request_register=1,//客户端请求注册
	admit_register,//服务器端返回注册允许
	reject_register,//服务器端拒绝注册
	multi_page_,//服务器端要求客户端抓取这些页面里面的图片链接 这后面的会附带很多链接
	single_pic//服务器端要求客户端抓取图片 后面会附带很多链接

};
unsigned int magic_number = 0xBAADBAAD;//这个在所有消息的前面
//然后再跟着聊天类型
//然后才可能跟着超链接