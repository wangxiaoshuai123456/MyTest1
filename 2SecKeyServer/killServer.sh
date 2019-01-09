#!/bin/bash

#获取当前用户的用户名
USER=$(whoami)

#获取密钥协商服务端进程号
pid=$(ps -u ${USER} | grep server | awk '{print $1}')
#echo "pid = $pid"

#如果pid的值为空值 条件为真
if [ -z $pid ]
then
    echo "密钥协商服务器没有启动"
else
    #向指定的进程发送编号为10的信号
    kill -10 $pid
    echo "密钥协商服务端友好退出"
fi

exit 0
