# 记录

## RISC-V assembly

1. 13放在a2中，f(8)+3的结果放在a1中

2. 编译器优化掉了`f`和`g`(内联函数)，直接在26行计算出了结果12

3. 0x628

4. 一开始以为是ra = 48 = 0x30，即`auipc ra, 0x0`这句话的pc值，但是参考https://zhuanlan.zhihu.com/p/347945926，发现`jalr`会将`ra`改成下一条指令的地址，所以应该是0x38。

5. 在小端存储中，57616=0xE110，所以"H%x"就是"He110"，而0x00646c72按照从小到大的顺序72 6c 64 00，而小端接近栈顶，也是指针指向的位置，所以会打印"world"。在大端存储中，应该是`0x726c6400`，但是57616不需要改。

6. a2

## Backtrace

主要要看清楚是打印出返回地址！

## Alarm

我一开始以为test0中一直没能获得到sigalarm第二个参数的地址，因为一直显示为0，但是实际上获得了地址，但是这个地址是user page table里的0x0，所以需要将kernel page table转换成user page table。

下一个重要的问题就是如何调用在user space的函数。最终发现只要设置SEPC即可，因为这是内核回到user space时下一条执行的命令。

完成test0之后，test1和test2主要是处理`sys_sigreturn`，即恢复中断，回到中断前的状态。

参考：https://zhuanlan.zhihu.com/p/347945926，我还是对目前的寄存器保护存疑，因为`usertrap`函数前一部分就会修改寄存器。