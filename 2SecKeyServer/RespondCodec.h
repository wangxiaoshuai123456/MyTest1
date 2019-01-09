#pragma once
#include "Codec.h"

//响应报文结构体
struct  RespondMsg
{
	int		rv;				// 返回值
	char	clientId[12];	// 客户端编号
	char	serverId[12];	// 服务器编号
	char	r2[64];			// 服务器端随机数
	int		seckeyid;		// 对称密钥编号    keysn
};

class RespondCodec : public Codec
{
public:
	RespondCodec();
	RespondCodec(RespondMsg *msg);
	~RespondCodec();

	// 函数重载
	int msgEncode(char** outData, int &len);
	void* msgDecode(char *inData, int inLen);
	//请求报文内存释放函数
	int msgMemFree(void **data);

private:
	RespondMsg *m_pmsg;
};

