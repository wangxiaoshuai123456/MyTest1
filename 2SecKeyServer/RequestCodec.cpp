#define _CRT_SECURE_NO_WARNINGS
#include "RequestCodec.h"
#include <iostream>

using namespace std;

//用于解码的构造函数
RequestCodec::RequestCodec():Codec()
{
}

//用于编码构造函数
RequestCodec::RequestCodec(RequestMsg * msg): Codec()
{
	m_msg.cmdType = msg->cmdType;
	strcpy(m_msg.clientId, msg->clientId);
	strcpy(m_msg.authCode, msg->authCode);
	strcpy(m_msg.serverId, msg->serverId);
	strcpy(m_msg.r1, msg->r1);
}



RequestCodec::~RequestCodec()
{
}

//请求报文编码
int RequestCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//1. 编码cmdType
	ret = writeHeadNode(m_msg.cmdType);
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//2. 编码clientID
	ret = writeNextNode(m_msg.clientId, strlen(m_msg.clientId));
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//3. 编码authCode
	ret = writeNextNode(m_msg.authCode, strlen(m_msg.authCode));
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//4. 编码serverId
	ret = writeNextNode(m_msg.serverId, strlen(m_msg.serverId));
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//5. 编码r1
	ret = writeNextNode(m_msg.r1, strlen(m_msg.r1));
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//6. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "packSequence failed..." << endl;
		return ret;
	}

	//7. 传出

	//8. 释放链表内存
	freeSequence(NULL);

	return 0;
}


//请求报文解码
void * RequestCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	//1. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed..." << endl;
		return NULL;
	}

	memset(&m_msg, 0, sizeof(m_msg));
	//2. 解码cmdType
	ret = readHeadNode(m_msg.cmdType);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//3. 解码clientId
	ret = readNextNode(m_msg.clientId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//4. 解码authCode
	ret = readNextNode(m_msg.authCode);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//5. 解码serverId
	ret = readNextNode(m_msg.serverId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//6. 解码r1
	ret = readNextNode(m_msg.r1);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//8. 释放链表内存
	freeSequence(NULL);

	//7. 传出
	return &m_msg;
}

//请求报文内存释放
int RequestCodec::msgMemFree(void ** data)
{
	if (NULL == data || NULL == *data)
	{
		cout << "some parameters are NULL" << endl;
		return 1;
	}

	delete (RequestMsg*)*data;

	*data = NULL;

	return 0;
}
