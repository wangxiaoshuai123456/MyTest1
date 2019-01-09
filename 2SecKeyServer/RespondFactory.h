#pragma once
#include "FactoryCodec.h"
class RespondFactory : public FactoryCodec
{
public:
	RespondFactory();
	~RespondFactory();

	//创建响应报文编解码对象
	Codec *createCodec(void *arg = NULL);
};

