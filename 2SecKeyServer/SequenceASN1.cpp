#include "SequenceASN1.h"

#include <iostream>
using namespace std;

SequenceASN1::SequenceASN1():BaseASN1()
{

}

//第一个节点是int类型
int SequenceASN1::writeHeadNode(int iValue)
{
	int ret = 0;

	ret = DER_ItAsn1_WriteInteger(iValue, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed...." << endl;
		return ret;
	}

	//m_next指向链表的最后一个节点
	m_next = m_header;

	return 0;
}

//编码字符串  链表的第一个节点
int SequenceASN1::writeHeadNode(char * sValue, int len)
{
	int ret = 0;

	ret = EncodeChar(sValue, len, &m_header);
	if (0 != ret)
	{
		cout << "EncodeChar failed... " << endl;
		return ret;
	}

	//m_next指向链表的最后一个节点
	m_next = m_header;

	return 0;
}

//编码后继节点
int SequenceASN1::writeNextNode(int iValue)
{
	int ret = 0;

	//编码整型数据
	ret = DER_ItAsn1_WriteInteger(iValue, &(m_next->next));
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed...." << endl;
		return ret;
	}

	//m_next永远指向链表的最后一个节点
	m_next = m_next->next;

	return 0;
}

//编码后继节点
int SequenceASN1::writeNextNode(char * sValue, int len)
{
	int ret = 0;

	ret = EncodeChar(sValue, len, &(m_next->next));
	if (0 != ret)
	{
		cout << "EncodeChar failed... " << endl;
		return ret;
	}

	//m_next永远指向链表的最后一个节点
	m_next = m_next->next;

	return 0;
}

//解码 整型
int SequenceASN1::readHeadNode(int & iValue)
{
	int num = -1;
	int ret = -1;

	ret = DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32*)&num);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;
		return ret;
	}

	iValue = num;

	//指向下一个节点
	m_next = m_header->next;

	return 0;
}

//解码 char*
int SequenceASN1::readHeadNode(char * sValue)
{
	int ret = -1;
	ret = DER_ItAsn1_ReadPrintableString(m_header, &m_temp); //1.free
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed..." << endl;
		return ret;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	//指向下一个节点
	m_next = m_header->next;

	//释放临时节点
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//解码不是第一个节点的节点 如果是整型
int SequenceASN1::readNextNode(int & iValue)
{
	int ret = -1;
	ret = DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32 *)&iValue);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;
		return ret;
	}

	//指向下一个节点
	m_next = m_next->next;

	return 0;
}

int SequenceASN1::readNextNode(char * sValue)
{
	int ret = -1;
	ret = DER_ItAsn1_ReadPrintableString(m_next, &m_temp); //1.free
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed..." << endl;
		return ret;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	//指向下一个节点
	m_next = m_next->next;

	//释放内存
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//编码结构体
int SequenceASN1::packSequence(char ** outData, int & outLen)
{
	int ret = -1;

	char *out = NULL;

	//编码结构体
	ret = DER_ItAsn1_WriteSequence(m_header, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteSequence failed..." << endl;
		return ret;
	}

	//分配空间
	out = new char[m_temp->dataLen + 1];
	if (NULL == out)
	{
		cout << "new out failed.." << endl;
		return 1;
	}

	//拷贝数据
	memcpy(out, m_temp->pData, m_temp->dataLen);

	//传出
	*outData = out;
	outLen = m_temp->dataLen;

	//释放临时内存
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//解码结构体
int SequenceASN1::unpackSequence(char * inData, int inLen)
{
	int ret = -1;

	//将char*类型转化为ANYBUF类型
	ret = DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char *)inData, inLen); //1. free
	if (0 != ret)
	{
		cout << "DER_ITCAST_String_To_AnyBuf failed..." << endl;
		return ret;
	}

	//解码结构体 得到链表
	ret = DER_ItAsn1_ReadSequence(m_temp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadSequence failed..." << endl;
		return ret;
	}

	//释放临时节点的内存
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

//释放内存
void SequenceASN1::freeSequence(ITCAST_ANYBUF * node)
{
	//node为NULL就释放 链表
	if (NULL == node)
	{
		DER_ITCAST_FreeQueue(m_header);
	}
	else
	{
		//node不为NULL 就释放节点
		DER_ITCAST_FreeQueue(node);
	}
}
