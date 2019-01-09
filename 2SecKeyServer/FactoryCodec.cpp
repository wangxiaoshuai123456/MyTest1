#include "FactoryCodec.h"


//构造函数
FactoryCodec::FactoryCodec()
{
}

//析构函数
FactoryCodec::~FactoryCodec()
{
}

//创建具体对象   工厂模式  一个工厂类只负责创建一个对象
Codec * FactoryCodec::createCodec(void * arg)
{

	return nullptr;
}
