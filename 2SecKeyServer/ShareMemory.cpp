#include "ShareMemory.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>

#include <iostream>

using namespace std;


//打开共享内存的构造函数
ShareMemory::ShareMemory(int key)
{
    //shmget
    //打开已经存在的共享内存
    m_shmid = shmget(key, 0, 0);  
    if (-1 == m_shmid)
    {
        //写日志 
    }
}


//打开共享内存的构造函数
ShareMemory::ShareMemory(char *pathName)
{
    key_t key = -1;
    //ftok获取key  'X'
    //shmget
    key = ftok(pathName, 'X');
    if (-1 == key)
    {
        //写日志 
    }

    //打开已经存在的共享内存
    m_shmid = shmget(key, 0, 0);
    if (-1 == m_shmid)
    {
        //写日志 
    }
}

//创建共享内存
ShareMemory::ShareMemory(int key, int size)
{
	void *addr = NULL;
	//如果共享内存不存在 创建之后就应该清零
	if (-1 == (m_shmid = shmget(key, 0, 0)))
	{
		//shmget
		m_shmid = shmget(key, size, IPC_CREAT | 0644);
		if (-1 == m_shmid)
		{
			//写日志 
			cout << "shmget failed..." << endl;
		}

		//关联共享内存
		addr = mapShm();

		//清零共享内存
		memset(addr, 0, size);

		//解除关联
		unMapShm();

	}

}

//创建共享内存
ShareMemory::ShareMemory(char *pathName, int size)
{
	void *addr = NULL;

    //ftok获取key  'X'
    //shmget

    key_t key = ftok(pathName, 'X');
    if (-1 == key)
    {
        //写日志 
    }

	//如果共享内存不存在 创建之后就应该清零
	if (-1 == (m_shmid = shmget(key, 0, 0)))
	{
		//shmget
		m_shmid = shmget(key, size, IPC_CREAT | 0644);
		if (-1 == m_shmid)
		{
			//写日志 
			cout << "shmget failed..." << endl;
		}

		//关联共享内存
		addr = mapShm();

		//清零共享内存
		memset(addr, 0, size);

		//解除关联
		unMapShm();

	}
}

//析构函数
ShareMemory::~ShareMemory()
{

}

//关联共享内存
void *ShareMemory::mapShm()
{
    
    //shmat 
    m_addr = shmat(m_shmid, NULL, 0);
    if ((void *)-1 == m_addr)
    {
        //写日志 
        m_addr = NULL;
    }


    return m_addr;
}


//解除关联共享内存
int ShareMemory::unMapShm()
{
    int ret = -1;
    //shmdt

    ret = shmdt(m_addr); 
    if (-1 == ret)
    {
        //写日志 
    }
    return ret;
}


//删除共享内存
int ShareMemory::delShm()
{
    int ret = -1;
    //shmctl
    ret = shmctl(m_shmid, IPC_RMID, NULL);
    if (-1 == ret)
    {
        //写日志 
    }

    return ret;
}


