#pragma once
#include "FactoryCodec.h"

//请求报文工厂类  主要负责请求报文编码对象或者解码对象创建
class RequestFactory :public FactoryCodec
{
public:
	RequestFactory();
	~RequestFactory();
	Codec *createCodec(void *arg = NULL);
};

