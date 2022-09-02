# 记录

首先是理解本实验的目的，即实现一个proxy，将terminal传来的HTTP request进行parse然后变成客户端，连接对应服务器，然后向服务器发送请求，并将结果返回给terminal。因此本体的关键在于parse，需要参考手册实现一个robust的parser。

一开始尝试运行`./driver.sh`发生问题，发现是WSL没有安装`netstat`，使用如下命令安装`sudo apt-get install net-tools`。之后还发现WSL开了代理，通过`echo http_proxy`查看代理，导致`curl`无法运行，使用`unset http_proxy`取消代理即可。

## Part1

理解意思之后，只需要将parse之后的执行换成`serve_as_client`，即proxy变成client去连接对应的服务器，并将服务器的结果发给最初的client。一开始发现能够获得文本文件，如`.html`和`.c`文件，但是`.jpg`文件无法获得，问题在于二进制文件不是以`\0`结束每一行，所以使用`readlineb`和在`writenb`中调用`strlen`都会产生问题。

解决方法：`readnb`会返回读取的字节数，将这个字节数设置成`writenb`的`n`即可。

## Part2

理解书上的生产和消费者模型即可，主线程每次将一个connfd放到线程池中，然后工作线程取其中一个工作即可（即调用`doit`）。

## Part3

本题中cache的LRU实现我使用的是链表，其中最主要的问题是读写锁的使用。本题一开始犯了一个很蠢的错误，`Cache *cache_init(int size)`没有返回创建的cache。

结果得分62/70，Part1中最后一个下载`tiny`程序发生错误，实际下载发现proxy得到的`tiny`实际上是`home.html`，说明cache有一定问题。最后发现cache中`data`和`uri`没有深拷贝，导致出现问题。

根据writeup，其中还有一些特殊情况，但这里就不再考虑了。

# 原版README

####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text

