#pragma once

class ShareMemory
{
public:
    //打开共享内存的构造函数
    ShareMemory(int key);
    //打开共享内存的构造函数
    ShareMemory(char *pathName);
    //创建共享内存
    ShareMemory(int key, int size);
    //创建共享内存
    ShareMemory(char *pathName, int size);
    ~ShareMemory();

    //关联共享内存
    void *mapShm();

    //解除关联共享内存
    int unMapShm();

    //删除共享内存
    int delShm();

private:
    //共享内存ID
    int m_shmid;
    //建立关联之后返回的地址
    void *m_addr;
};
