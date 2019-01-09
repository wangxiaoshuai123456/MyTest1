#pragma once
#include "ShareMemory.h"

//网点密钥信息类
class NodeSHMInfo
{
public:
	int status;     //密钥状态
	int seckeyID;   //密钥编号
	char clientID[12];  //服务端ID
	char serverID[12];  //客户端ID
	char secKey[128];   //密钥信息
};

//共享内存业务流类
class SecKeyShm : public ShareMemory
{
public:
	//打开共享内存
	SecKeyShm(int key);
	//创建共享内存
	SecKeyShm(int key, int maxNode);
	//打开共享内存
	SecKeyShm(char* pathName);
	//创建共享内存
	SecKeyShm(char* pathName, int maxNode);
	//析构函数
	~SecKeyShm();

	//将网点密钥信息写入共享内存
	int shmWrite(NodeSHMInfo* pNodeInfo);
	//从共享内存中读取网点密钥信息
	int shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo);

private:
	int m_maxNode;      //最大网点数
};

