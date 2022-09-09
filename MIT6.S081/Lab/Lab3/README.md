# 记录

## Speed up system calls

主要困惑的问题就是`USYSCALL`在哪里alloc，alloc之后user和kernel为何能够映射到同一个区域。我的理解是每个进程创建的时候，在`proc`中增加一个`usyscall`的结构指针，然后给他分配一个物理页，然后将`USYSCALL`这个VA映射到物理页地址。初步写完之后触发panic，关于panic的调试：在gdb中设置panic断点进行调试，但是这里是usertrap，且获得不了很多有用信息。最后发现是权限需要加上`PTE_U`，否则在非特权模式下不能访问。

## Print a page table

这个比较简单。

## Detecting which pages have been accessed

主要难点在于在哪里设置`PTE_A`的标志，最终发现是在`walk`中设置`PTE_A`的flag，但是也要注意`pgaccess`也会调用类似`walk`的函数，因此需要改一下放在`pgaccess`里面。