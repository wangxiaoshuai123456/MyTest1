#include "RequestFactory.h"
#include "RequestCodec.h"

//构造函数
RequestFactory::RequestFactory()
{
}

//析构函数
RequestFactory::~RequestFactory()
{
}

//创建请求报文编解码对象的方法
//工厂模式  请求报文类工厂
Codec * RequestFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	if (NULL == arg)
	{
		//创建请求报文解码对象
		codec = new RequestCodec();
	}
	else
	{
		//创建请求报文编码对象
		codec = new RequestCodec((RequestMsg*)arg);
	}

	return codec;
}
