#include "ServerOperation.h"
#include "SecKeyShm.h"
#include "TcpServer.h"
#include "RequestCodec.h"
#include "RequestFactory.h"
#include "RespondCodec.h"
#include "RespondFactory.h"

#include <iostream>
#include <pthread.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

using namespace std;

//线程处理函数
void *callback(void *arg);

//线程传递参数
struct ThreadArg
{
	TcpSocket *pSocket;
	ServerOperation *pServer;
};

//静态数据成员类外初始化
int ServerOperation::flag = 0;

//构造函数
ServerOperation::ServerOperation(ServerInfo * info)
{
	//1. 容错判断
	if (NULL == info)
	{
		//写日志
		cout << "Info is NULL" << endl;
		return;
	}

	//2. 创建ServerInfo对象  ServerInfo *m_info;
	m_info = new ServerInfo;
	if (NULL == m_info)
	{
		//写日志
		cout << "new m_info failed..." << endl;
		return;
	}

	//拷贝数据
	memcpy(m_info, info, sizeof(ServerInfo));

	//3. 创建共享内存的对象	SecKeyShm	*m_shm;
	m_shm = new SecKeyShm(info->shmkey, info->maxnode);
	if (NULL == m_shm)
	{
		//写日志
		cout << "new SecKeyShm failed..." << endl;
		return;
	}
	//4. 创建TCPServer对象   TcpServer *m_server;
	m_server = new TcpServer;
	if (NULL == m_server)
	{
		//写日志
		cout << "new TcpServer failed..." << endl;
		return;
	}
}


//析构函数
ServerOperation::~ServerOperation()
{
	//释放一些对象
	delete m_info;
	delete m_shm;
	delete m_server;
}

//启动服务器 开始监听 接受客户端的连接
void ServerOperation::startWork()
{
	int ret = -1;
	pthread_t tid = -1;

	ThreadArg *pArg = NULL;

	//设置监听
	ret = m_server->setListen(m_info->sPort);
	if (0 != ret)
	{
		//写日志
		cout << "m_server setListen failed..." << endl;
		return;
	}

	//循环接受客户端的连接
	while (1)
	{
        //循环结束的标志 
        if (1 == flag)
        {
            cout << "密钥协商服务端startWork退出" << endl;
            break;        
        }

		//接受客户端的连接
		m_client = m_server->acceptConn(1);  //超时时间为1秒
		if (NULL == m_client)
		{
			cout << "m_server acceptConn failed..." << endl;
			continue;
		}

		//分配空间 创建一个对象
		pArg = new ThreadArg;
		if (NULL == pArg)
		{
			//写日志
			cout << "new ThreadArg" << endl;
			break;
		}

		//赋值
		pArg->pSocket = m_client;
		pArg->pServer = this; //this指向该对象本身


		//创建一个线程 与客户端进行数据交互
		pthread_create(&tid, NULL, callback, (void*)pArg);

		//设置线程分离
		pthread_detach(tid);
	}	
}


//密钥协商
int ServerOperation::secKeyAgree(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	int ret = -1;
	int i = 0;

	RespondMsg respondMsg;

	//1. 组织密钥协商响应报文
	memset(&respondMsg, 0, sizeof(RespondMsg));
	respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqmsg->clientId);
	strcpy(respondMsg.serverId, reqmsg->serverId);

	respondMsg.seckeyid = 1;

	//获取随机字符序列
	getRandString(sizeof(respondMsg.r2), respondMsg.r2);


	//2. 密钥协商响应报文编码
	//创建响应报文编解码工厂类对象
	FactoryCodec *factoryCodec = NULL;
	factoryCodec = new RespondFactory;
	if (NULL == factoryCodec)
	{
		cout << "RespondFactory failed..." << endl;
		return -1;
	}

	//创建响应报文编码对象
	Codec *codec = new RespondCodec(&respondMsg);
	if (NULL == codec)
	{
		cout << "RespondCodec failed..." << endl;
		return -1;
	}

	//编码
	char *msgData = NULL;
	int msgDataLen = -1;
	ret = codec->msgEncode(&msgData, msgDataLen);
	if (ret < 0)
	{
		cout << "msgEncode failed... " << endl;
		return -1;
	}

	//传出
	*outData = msgData;
	outLen = msgDataLen;

	//3. 协商密钥
	//请求报文r1  响应报文r2  密钥可以通过随机序列r1和随机序列r2 通过哈希函数生成
	SHA512_CTX ctx512;

	//初始化
	ret = SHA512_Init(&ctx512);
	if (0 == ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return 1;
	}

	//第一次添加数据
	ret = SHA512_Update(&ctx512, reqmsg->r1, strlen(reqmsg->r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return 1;
	}

	//第二次添加数据
	ret = SHA512_Update(&ctx512, respondMsg.r2, strlen(respondMsg.r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return 1;
	}

	unsigned char md512[64];

	//获取结果
	ret = SHA512_Final(md512, &ctx512);
	if (0 == ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return 1;
	}

	NodeSHMInfo nodeSHMInfo;
	memset(&nodeSHMInfo, 0, sizeof(nodeSHMInfo));
	nodeSHMInfo.status = 0;
	strcpy(nodeSHMInfo.clientID, reqmsg->clientId);
	strcpy(nodeSHMInfo.serverID, reqmsg->serverId);
	nodeSHMInfo.seckeyID = respondMsg.seckeyid;

	//生成密钥字符串
	for (i = 0; i < 64; i++)
	{
		sprintf(&nodeSHMInfo.secKey[i * 2], "%02x", md512[i]);
	}

	//4. 协商好的密钥写入共享内存
	m_shm->shmWrite(&nodeSHMInfo);

	//5. 将密钥信息写入数据库

	//6. 释放内存
	delete codec;
	delete factoryCodec;

	return 0;
}

//密钥校验
int ServerOperation::secKeyCheck(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	return 0;
}

//密钥注销
int ServerOperation::secKeyRevoke(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	return 0;
}

//密钥查看
int ServerOperation::secKeyView(void)
{
	return 0;
}

//获取一个随机序列
void ServerOperation::getRandString(int len, char * randBuf)
{
	int i = 0;

	int tag = 0;

	//设置随机种子
	srandom(time(NULL));

	//清零
	memset(randBuf, 0, len);

	//循环获取字符
	for (i = 0; i < len; i++)
	{
		//随机字符分成四类:  大写字母  小写字母  数字  特殊字符
		//随机字符种类
		tag = random() % 4;

		switch (tag)
		{
			//大写字母
		case 0:
			randBuf[i] = 'A' + random() % 26;
			break;

			//小写字母
		case 1:
			randBuf[i] = 'a' + random() % 26;
			break;

			//数字
		case 2:
			randBuf[i] = '0' + random() % 10;
			break;

			//特殊字符
		case 3:
			randBuf[i] = "~!#$%^&*()-="[random() % 12];
			break;

		}
	}

}

//线程回调函数
void *callback(void *arg)
{
	int i = 0;
	int ret = -1;

	char *recvData = NULL;
	int recvDataLen = -1;

	ThreadArg *pInfo = (ThreadArg*)arg;

	//1. 接收客户端请求报文
	ret = pInfo->pSocket->recvMsg(&recvData, recvDataLen); //1.free
	if (ret < 0)
	{
		//写日志
		cout << "failed..." << endl;
		return (void*)-1;
	}

	//2. 解码客户端请求报文
	FactoryCodec *factoryCodec = NULL;
	factoryCodec = new RequestFactory;
	if (NULL == factoryCodec)
	{
		//写日志
		cout << "factoryCodec failed..." << endl;
		return (void*)-1;
	}

	//通过工厂创建一个请求报文解码对象
	Codec *codec = factoryCodec->createCodec();
	if (NULL == codec)
	{
		//写日志
		cout << "codec failed..." << endl;
		return (void*)-1;
	}

	//解码
	RequestMsg *pMsg = (RequestMsg*)codec->msgDecode(recvData, recvDataLen); //2.free
	if (NULL == pMsg)
	{
		//写日志
		cout << "msgDecode failed..." << endl;
		return (void*)-1;
	}


	//3. 验证消息认证码(通过接收到消息认证码和自己计算出的消息认证码比较 如果一致表示ok)
	//unsigned char *HMAC(const EVP_MD *evp_md, const void *key, int key_len, 
	//const unsigned char *d, size_t n, unsigned char *md,
	//unsigned int *md_len);
	char key[64];
	unsigned char md[32];
	unsigned int mdLen = -1;

	unsigned char mdBuf[65];

	memset(md, 0, 32);
	memset(key, 0, sizeof(key));
	sprintf(key, "@%s+%s@", pMsg->serverId, pMsg->clientId);
	//得到消息认证码
	HMAC(EVP_sha256(), (void*)key, strlen(key), (unsigned char *)pMsg->r1, strlen(pMsg->r1), md, &mdLen);

	//转化为字符串
	memset(mdBuf, 0, 65);
	for (i = 0; i < mdLen; i++)
	{
		sprintf((char *)&mdBuf[i * 2], "%02X", md[i]);
	}

	cout << mdLen << endl;
	cout << pMsg->authCode << endl;
	cout << mdBuf << endl;

	if (strcmp((char *)mdBuf, pMsg->authCode) == 0)
	{
		cout << "消息认证码一致" << endl;
	}
	else
	{
		cout << "消息认证码验证不通过" << endl;
		return (void*)-1;

	}

	//4. 根据请求的类型 给出响应 (秘钥协商  密钥校验 密钥注销)
	char *outData = NULL;
	int outDataLen = -1;

	switch (pMsg->cmdType)
	{
		//密钥协商
	case RequestCodec::NewOrUpdate:
		cout << "密钥协商" << endl;
		ret = pInfo->pServer->secKeyAgree(pMsg, &outData, outDataLen); //3.free
		break;

		//密钥校验
	case RequestCodec::Check:
		cout << "密钥校验" << endl;
		ret = pInfo->pServer->secKeyCheck(pMsg, &outData, outDataLen);
		break;

		//密钥注销
	case RequestCodec::Revoke:
		cout << "密钥注销" << endl;
		ret = pInfo->pServer->secKeyRevoke(pMsg, &outData, outDataLen);
		break;

	default:
		cout << "非法请求类型" << endl;
	}

	//结果判断
	if (0 == ret)
	{
		cout << "做对应的业务ok" << endl;
	}
	else
	{
		cout << "做对应的业务失败.. " << endl;
		return (void*)-1;
	}

	//5. 发送响应报文
	ret = pInfo->pSocket->sendMsg(outData, outDataLen);
	if (ret < 0)
	{
		cout << "sendMsg failed..." << endl;

	}

	//6. 断开连接
	pInfo->pSocket->disConnect();

	//7. 释放内存
	if (NULL == recvData)
	{
		delete recvData;
	}

	if (NULL == pMsg)
	{
		codec->msgMemFree((void **)&pMsg);
	}

	if (NULL == outData)
	{
		delete outData;
	}

	delete codec;
	delete factoryCodec;


	return NULL;
}
