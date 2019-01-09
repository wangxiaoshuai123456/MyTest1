#include "SecKeyShm.h"
#include <cstring>
#include <unistd.h>

#include <iostream>

using namespace std;

//打开共享内存
SecKeyShm::SecKeyShm(int key):ShareMemory(key)
{
	m_maxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(int key, int maxNode):ShareMemory(key, maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{     

}

//打开共享内存
SecKeyShm::SecKeyShm(char * pathName):ShareMemory(pathName)
{
	m_maxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(char * pathName, int maxNode):ShareMemory(pathName, maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{

}

SecKeyShm::~SecKeyShm()
{
}

//将网点密钥信息写入共享内存
//1. 如果共享内存中存在对应的网点密钥信息, 就应该覆盖
//2. 如果共享内存中不存在对应的网点密钥信息, 就应该找一个空白的区域写进去
int SecKeyShm::shmWrite(NodeSHMInfo * pNodeInfo)
{
	int i = 0;

	void *addr = NULL;

	NodeSHMInfo *tmp = NULL;
	NodeSHMInfo tmpNode;

	//1. 关联共享内存
	addr = mapShm();


	//2. 判断共享内存中是否存在对应的网点的信息
	for (i = 0; i < m_maxNode; i++)
	{
		//cout << "for i: " << i << endl;
		tmp = static_cast<NodeSHMInfo*>(addr) + i;
		if ((strcmp(tmp->clientID, pNodeInfo->clientID) == 0) && (strcmp(tmp->serverID, pNodeInfo->serverID) == 0))
		{
			cout << "共享内存中存在对应的客户端密钥信息, 现在就覆盖" << endl;
			memcpy(tmp, pNodeInfo, sizeof(NodeSHMInfo));
			break;
		}
	}

	//3. 在共享内存中找一个空白区域填写密钥信息
	if (i == m_maxNode)
	{
		//清零
		memset(&tmpNode, 0, sizeof(tmpNode));
		for (i = 0; i < m_maxNode; i++)
		{
			tmp = static_cast<NodeSHMInfo*>(addr) + i;
			if (memcmp(&tmpNode, tmp, sizeof(NodeSHMInfo)) == 0)
			{
				cout << "找到空白的区域, 现在将网点密钥信息写入共享内存" << endl;
				//cout << "find empty space...." << endl;
				memcpy(tmp, pNodeInfo, sizeof(NodeSHMInfo));
				break;
			}
		}
	}

	//没有空白区域写网点密钥信息
	if (i == m_maxNode)
	{
		cout << "没有空白区域, 写网点密钥信息, 请联系管理员" << endl;
	}

	//4. 解除关联
	unMapShm();

	return 0;
}

int SecKeyShm::shmRead(const char * clientID, const char * serverID, NodeSHMInfo * pNodeInfo)
{
	int i = 0;
	void *addr = NULL;
	NodeSHMInfo *tmp = NULL;

	//1. 关联共享内存
	addr = mapShm();

	//2. 循环遍历共享内存
	for (i = 0; i < m_maxNode; i++)
	{
		tmp = static_cast<NodeSHMInfo*>(addr) + i;
		if ((strcmp(tmp->serverID, serverID) == 0) && (strcmp(tmp->clientID, clientID) == 0))
		{
			cout << "共享内存中有对应的网点密钥信息..." << endl;
			memcpy(pNodeInfo, tmp, sizeof(NodeSHMInfo));
			break;
		}
	}

	if (i == m_maxNode)
	{
		cout << "对应网点密钥信息不存在..." << endl;
	}

	//3. 取消关联共享内存
	unMapShm();

	return 0;
}
