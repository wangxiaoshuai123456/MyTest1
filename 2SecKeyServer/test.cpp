#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "ServerOperation.h"


using namespace std;

//创建一个守护进程
int createDaemon(void)
{
    //使当前进程成为守护进程
    //创建子进程 父进程退出 
    if (fork() > 0)
    {
        exit(0); 
    }

    //创建会话
    if (setsid() < 0)
    {
        perror("setsid"); 
        return 1;
    }

    //改变当前工作目录
    chdir("/");

    //设置权限掩码
    umask(0);

    return 0;
}

//信号捕捉函数实现
void fun(int signo)
{
    cout << "密钥协商服务端捕捉到信号: " << signo << endl;
    cout << "密钥协商服务器友好的退出" << endl;

    //设置退出标志为1
    ServerOperation::flag = 1;
    cout << "flag: " << ServerOperation::flag << endl;
}

//密钥协商服务端
int main(void)
{
    int ret = -1;

    struct sigaction act;

	//1. 从配置文件中获取服务端配置信息
	ServerInfo info;
	memset(&info, 0, sizeof(info));
	strcpy(info.serverID, "0001");
	strcpy(info.dbUse, "scott");
	strcpy(info.dbPasswd, "11");
	
	info.sPort = 10086;
	info.maxnode = 10;
	info.shmkey = 0x11;

    memset(&act, 0, sizeof(act));
    act.sa_handler = fun;
    act.sa_flags = 0;

#if 1
    //捕捉自定信号 编号为10的信号
    ret = sigaction(SIGUSR1, &act, NULL);
    if (-1 == ret)
    {
        perror("sigaction"); 
        return 1;
    }

#endif

	//2. 构造一个密钥协商服务器对象
	ServerOperation *server = new ServerOperation(&info);

    //使当前进程成为一个守护进程
    //createDaemon();

	//3. 启动密钥协商服务器
	server->startWork();

	cout << "hello itcast" << endl;

	//system("pause");

	return 0;
}
