# 记录

本实验非常容易出bug，调试比较困难。

下载时缺少`traces`文件夹，并将`config.h`文件夹中`TRACEDIR`改成`"traces/"`。Baseline中`free`不做任何事情，所以在testcases中
会出现out of memory的情况。

另外为了调试，需要在`Makefile`中加上`-g`选项。

首先根据课本实现隐式空闲列表（first fit），最终得分为59/100。然后采用best fit，分数基本没有提升。

接下来实现显示空闲列表，来提高块分配的速度。

注意：

- 分配之后的block就没有prev和succ了，所以其实和之前的情况类似。但是注意没有prev和succ之后空间就变大了，`mdriver.c`测试文件中会将内存填充，所以会导致问题。因此在`malloc`中需要返回`bp+2*DSIZE`，而`free`中要先减去`2*DSIZE`

- 原来一块大的free空间split和没有split的情况是不同的，需要讨论

- 合并也需要讨论情况，修改prev和succ指针，最难的是前后都是free的，然后需要合并三个，此时需要考虑前后的prev和succ指针，方便的做法是要求free list里面的free空间都按照地址排序。另外还要考虑的是extend的部分可能会和前一个block合并，prev和succ指针的修改也需要讨论。还有合并的时机，我这里都选择是先合并再修改prev和succ指针。

- `OVERHEAD`大小为24，如果split之后身下的一半没有24就会产生问题

- 注意`GET_SIZE`和`GET_ALLOC`需要先调用`HDRP`和`FTRP`

呜呜呜最终BUG过多调不出来……放在了`mm_fail.c`里。

查看攻略，发现采用next fit比较好。参考：https://zhuanlan.zhihu.com/p/150100073

# 原版README

#####################################################################
# CS:APP Malloc Lab
# Handout files for students
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

***********
Main Files:
***********

mm.{c,h}	
	Your solution malloc package. mm.c is the file that you
	will be handing in, and is the only file you should modify.

mdriver.c	
	The malloc driver that tests your mm.c file

short{1,2}-bal.rep
	Two tiny tracefiles to help you get started. 

Makefile	
	Builds the driver

**********************************
Other support files for the driver
**********************************

config.h	Configures the malloc lab driver
fsecs.{c,h}	Wrapper function for the different timer packages
clock.{c,h}	Routines for accessing the Pentium and Alpha cycle counters
fcyc.{c,h}	Timer functions based on cycle counters
ftimer.{c,h}	Timer functions based on interval timers and gettimeofday()
memlib.{c,h}	Models the heap and sbrk function

*******************************
Building and running the driver
*******************************
To build the driver, type "make" to the shell.

To run the driver on a tiny test trace:

	unix> mdriver -V -f short1-bal.rep

The -V option prints out helpful tracing and summary information.

To get a list of the driver flags:

	unix> mdriver -h

