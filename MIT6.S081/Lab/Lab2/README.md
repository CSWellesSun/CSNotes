# 记录

## trace

主要是看懂作业的hints，其中比较重要的是：在`proc`结构中加一个变量保存trace掩码。

另外第一次实现完之后发现还是提升找不到trace的syscall，最后发现还需要在`syscall.c`中加上`sys_trace`。

但是发现似乎掩码出现了一点问题，例如我尝试`trace 32`， 结果打印出来的都是`kill`的trace而不是`read`。最后发现确实执行的是`read`，但是我打印成`kill`，原因在于数据第一个`syscallname`这个数组0号位应该为空，因为syscall的编号是从1开始的！

## sysinfo

主要需要理解copyout这个函数，它从kernel中拷贝数据到user里的数据结构中。利用`argaddr`来获得这个user mode中的数据结构地址。

另外在`kalloc.c`和`proc.c`文件中添加函数之后需要在`defs.h`中添加头文件。

注意在计算freemem的时候，如果`freelist`首地址为0，那么说明freemem为0，需要特殊考虑。另外`freelist`是一个链表，应该遍历链表来计算freemem。

计算nproc的时候犯了很粗心的错误，题目要求是统计**not UNUSED**，我看反了...