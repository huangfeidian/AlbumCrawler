#include "ChatClient.h"
#include "ChatServer.h"
#include "RawFileServer.h"
#include "HttpFileClient.h"
#include "DoubanExtract.h"
#include "GenerateRequest.h"
//本文件描述Server端的行为
//所有的操作都是在server端启动之后启动
//server端启动之后 首先打开一个端口 a 用来接收客户端的注册
//这里我们设置为启动2分钟之后客户端必须全都连进来 逾期不候
//这里的注册是利用ChatClient和ChatServer来交互的
//为了建立这个Chat 每一个客户端都需要打开一个端口 用来连接server端
//客户端注册之后 继续通过Chat来获得server的指令请求
//在服务器端启动2分钟之后，提示输入相册网址
//在输入网址之后，服务器端首先获得该相册的第一个页面 并分析其中的子页面
//提取出其中的子页面 并把子页面的请求（chat）发送给所有的客户端，用来负载均衡
//客户端解析子页面请求，然后分析出其中的页面中的图片链接
//在获得图片链接之后，有两个选项
//1 客户端自己进行图片的获取请求
//2 所有请求汇总到server端 然后server端把所有的图片链接重新再分配到各个客户端 然后客户端再去获得图片
//在获得所有的图片之后，利用文件传送功能将文件传送回server端
//所以总的来说 有三个主要部分
//1 聊天部分（设计几种命令格式）
//2 获得网页及图片部分
//3 文件传送部分


int main()
{

}