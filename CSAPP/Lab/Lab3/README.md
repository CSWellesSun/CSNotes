# 记录

`answer`文件夹中`inputX.txt`为十六进制文件，`inputX-raw.txt`为ASCII码文件即输入文件。

## Phase 1

首先运行程序需要`./ctarget -q`，否则会报错`FAILED: Initialization error: Running on an illegal host`。

这个phase比较简单，只需要输入的数据超过栈顶将return的地方覆盖即可。

## Phase 2

一开始尝试嵌入代码，想法是跳转到输入string开头的位置然后执行插入代码，最后retq回到touch2，但是发现会SEGV，最后发现跳转地址是**8位**！

然后发现又SEGV，经过测试发现cookie这个立即数前未加`$`！此时修改了代码最初会先压栈touch2的地址。

结果又SEGV（哭），搞了半天不知道原因，最后和知乎答案比较发现是touch2的地址又没加`$`！对立即数产生阴影了……

参考：https://zhuanlan.zhihu.com/p/140514922

## Phase 3

一开始对`sprintf`理解不清，和之前的`sscanf`搞混了。其实就是将字符串放到一个不会被破坏的位置，然后让寄存器`rdi`指向这个字符串的首地址即可。

我一开始将字符串放在了exploit string的开头，但是代码中会修改最初的8个地址，且各种push和call导致exploit string前0x28长度的字节都会被修改，于是最终只能将字符串放在exploit string最后。

## Phase 4

经过前面的铺垫这个phase花的时间非常少，甚至没用gdb一次过了（乐）。主要就是找到`popq rax`和`mov rax, rdi`两条指令即可。

## Phase 5

一直没想出来，发现了`mov rsp, rax`和`pop rax`以及`lea`等操作，但是还是没有构造出来。实际上的思路就是通过`mov rsp, rax`得到随机化栈
的某个地址，然后使用`pop rax`设置偏移量，用`lea`计算出字符串的首地址。

参考：https://blog.csdn.net/ch_fu/article/details/78915043


# 原版README

This file contains materials for one instance of the attacklab.

Files:

    ctarget

Linux binary with code-injection vulnerability.  To be used for phases
1-3 of the assignment.

    rtarget

Linux binary with return-oriented programming vulnerability.  To be
used for phases 4-5 of the assignment.

     cookie.txt

Text file containing 4-byte signature required for this lab instance.

     farm.c

Source code for gadget farm present in this instance of rtarget.  You
can compile (use flag -Og) and disassemble it to look for gadgets.

     hex2raw

Utility program to generate byte sequences.  See documentation in lab
handout.

