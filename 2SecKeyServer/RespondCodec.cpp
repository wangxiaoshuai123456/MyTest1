#define _CRT_SECURE_NO_WARNINGS
#include "RespondCodec.h"

#include <iostream>

using namespace std;

//构造响应报文解码对象
RespondCodec::RespondCodec():Codec()
{
	m_pmsg = new RespondMsg;
}



//构造响应报文编码对象
RespondCodec::RespondCodec(RespondMsg * msg):Codec()
{
	m_pmsg = new RespondMsg;

	//赋值
	m_pmsg->rv = msg->rv;
	strcpy(m_pmsg->clientId, msg->clientId);
	strcpy(m_pmsg->serverId, msg->serverId);
	strcpy(m_pmsg->r2, msg->r2);
	m_pmsg->seckeyid = msg->seckeyid;

}

RespondCodec::~RespondCodec()
{
	delete m_pmsg;
}


//响应报文编码
int RespondCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//1. 编码rv
	ret = writeHeadNode(m_pmsg->rv);
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return ret;
	}

	//2. 编码clientId
	ret = writeNextNode(m_pmsg->clientId, strlen(m_pmsg->clientId));
	if (0 != ret)
	{
		cout << "writeNextNode failed..." << endl;
		return ret;
	}

	//3. 编码serverId
	ret = writeNextNode(m_pmsg->serverId, strlen(m_pmsg->serverId));
	if (0 != ret)
	{
		cout << "writeNextNode failed..." << endl;
		return ret;
	}

	//4. 编码r2
	ret = writeNextNode(m_pmsg->r2, strlen(m_pmsg->r2));
	if (0 != ret)
	{
		cout << "writeNextNode failed..." << endl;
		return ret;
	}

	//5. 编码seckeyid
	ret = writeNextNode(m_pmsg->seckeyid);
	if (0 != ret)
	{
		cout << "writeNextNode failed..." << endl;
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

	//8. 释放内存
	freeSequence(NULL);

	return 0;
}


void * RespondCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	//清零存储空间
	memset(m_pmsg, 0, sizeof(*m_pmsg));

	//1. 解码结构体  得到链表
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed..." << endl;
		return NULL;
	}

	//2. 解码rv
	ret = readHeadNode(m_pmsg->rv);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//3. 解码clientId
	ret = readNextNode(m_pmsg->clientId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//4. 解码serverId
	ret = readNextNode(m_pmsg->serverId);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//5. 解码r2
	ret = readNextNode(m_pmsg->r2);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//6. 解码seckeyid
	ret = readNextNode(m_pmsg->seckeyid);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//7. 释放内存  链表
	freeSequence(NULL);

	//8. 传出
	return (void*)m_pmsg;
}

//响应报文结构体内存释放
int RespondCodec::msgMemFree(void ** data)
{
	//容错判断
	if (NULL == data || NULL == *data)
	{
		cout << "some values are NULL" << endl;
		return 1;
	}

	delete (RespondMsg*)*data;
	*data = NULL;

	return 0;
}
