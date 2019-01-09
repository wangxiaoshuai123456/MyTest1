#include "RespondFactory.h"
#include "RespondCodec.h"


//构造函数
RespondFactory::RespondFactory()
{
}

//析构函数
RespondFactory::~RespondFactory()
{
}

//工厂模式成员方法   只负责创建响应报文的编解码对象
Codec * RespondFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	if (NULL == arg)
	{
		//创建一个响应报文解码对象
		codec = new RespondCodec;
	}
	else
	{
		//创建一个响应报文编码对象
		codec = new RespondCodec((RespondMsg*)arg);
	}

	return codec;
}
