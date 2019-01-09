#pragma once

#include "Codec.h"

class FactoryCodec
{
public:
	FactoryCodec();
	~FactoryCodec();

	//负责创建对应类的对象
	virtual Codec *createCodec(void *arg = NULL);
};

