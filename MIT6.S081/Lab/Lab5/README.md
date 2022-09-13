# 记录

首先先修改`uvmcopy`和`usertrap`两个函数，但是却出现了不明所以的问题，逐项排查：

- 发现`PTE_FLAGS`传入参数是`*pte`而不是`pte`

- 发现COW真正分配空间的时候没有将父进程的实际内容拷贝过去

- page fault有三种情况，分别是load/store/代码造成，前两者适合COW情况

- 关于引用计数加减的位置需要考虑好，特别是`free`的时候，因为本来代码里就是用`kfree`来表示清除物理页，所以就统一在这里减少物理页的引用计数。

- 特别注意`mappages`函数的返回值，当为0的时候是success

- 每次COW的时候就对原来共同所指的pa引用计数减一，即调用`kfree`

- 参考：https://blog.csdn.net/laplacebh/article/details/118358462