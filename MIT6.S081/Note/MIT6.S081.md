# MIT6.S081

[TOC]

## Lec01 Introduciton and Examples

### 1.7 Shell

当用C语言执行系统调用的时候，实际上函数内的指令是机器指令（汇编），在RISC-V被称为ECALL，这个指令将控制权转给内核，之后内核检查进程的内存和寄存器，并确定相应的参数。

### 1.9 exec, wait系统调用

- exec系统调用时候保留当前的文件描述符表单，在exec系统调用之前的文件描述符在新的程序中表示相同的东西
- `wait(&status)`，其中`status`等待子进程`exit`时候的返回值，而`wait`返回值是结束子进程的`pid`。如果没有子进程的话，`wait`返回-1。多个子进程就需要多个`wait`

### 1.10 I/O Redirect

```c
int main() {
    int pid;
    pid = fork();
    if (pid == 0) {
        close(1);
        open("output.txt", O_WRONLY | O_CREATE);
        char *argv[] = {"echo", "this", "is", "redirected", "echo", 0};
        exec("echo", argv);
        printf("exec failed!\n");
        exit(1);
    } else {
        wait((int *) 0);
    }
    exit(0);
}
```

注意上面第5行，子进程`close(1)`关闭了`stdout`，第6行返回值一定是1因为`open`返回当前进程**未使用的最小文件描述符序号**，于是1和`output.txt`关联，因此接下来的`echo`会输出到文件描述符1即`output.txt`中。

这体现`fork`和`exec`分离的好处，可以在执行之前修改文件描述符从而实现`redirect`。

### 管道

```c
int p[2];
pipe(p);
```

上述代码创建一个管道，并且将**读写**两个描述符记录在数组`p`中，这两个

```C
// 相当于 echo "hello world\n" | wc
int p[2];
char *argv[2];
argv[0] = "wc";
argv[1] = 0;
pipe(p);
if(fork() == 0) {
    close(0);
    dup(p[0]); // 拷贝读口并设置描述符为0取代stdin
    close(p[0]);
    close(p[1]); // 此时写口描述符都关闭了管道不再等待read
    exec("/bin/wc", argv);
} else {
    write(p[1], "hello world\n", 12);
    close(p[0]);
    close(p[1]);
}
```

注意管道会一直等待read，直到有数据（它再传递给写口）或者绑定在**写口**上的描述符都关闭了（此时read返回0即`EOF`），所以在`exec`之前必须把写口关闭，否则管道将一直等待read

## Lec03 OS Organization and System Calls

### 3.5 User/Kernel mode切换

当一个用户程序想要将程序执行的控制权交给内核，只需要执行ECALL指令并传入一个数字，这个数字代表应用程序想要调用的System Call

内核侧`syscall.c`中有一个函数`syscall`，它会检查`ECALL`参数

用户不能直接调用`fork`等函数，只能用封装好的系统调用函数执行`ECALL`指令，指令参数是代表了write系统调用的数字，控制权给了`syscall`之后，它实际调用`write`系统调用

### 3.6 宏内核 vs 微内核 （Monolithic Kernel vs Micro Kernel）

内核被称为TCB可被信任的计算空间（Trusted Computing Base）

操作系统服务都放在kernel mode中被称为Monolithic Kernel Design宏内核

- 容易出现漏洞
- 集成度高，性能好

Micro Kernel微内核，在Kernel Mode中运行尽可能少的代码，但是微内核中user/kernel mode切换通过消息性能损耗大，并且各个部分不易共享数据

### 3.7 编译运行kernel

代码分成3部分：

- kernel：所有文件被编译成一个叫做`kernel`的二进制文件，这个文件运行在kernel mode中
- user：运行在user mode的程序
- mkfs：空的文件镜像

传给QEMU的参数：

- -kernel：传递内核文件
- -m：内存数量
- -smp：可用的CPU核数
- -drive：磁盘驱动

## Lec04 Page Tables

### 4.3 页表

CPU通过MMU将VA转化成PA，页表保存在内存中，MMU从内存中读取，页表的地址保存在SATP的寄存器里。

VA分成两部分：index（27位）和offset（12位，对应4KB page中的某个字节），共39位，前25位空着。

PA：共56位，44位是物理page号（PPN），剩下12位是offset继承自VA最后12位

所以地址转换就是将27位VA index转换成44位PPN即可

实际上PageTable是多级结构，VA index实际上有3个9bit的index组成（L2, L1, L0）

![img](MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MKKjB2an4WcuUmOlE__%2F-MKPyAM8VDBz7ktllFGN%2Fimage.png)

每一个directory是4KB，每个条目（PTE）64bit，共512个entry，所以是用9位。SATP指向最高一级page table，然后用L2来索引最高一级page table，由此得到PPN来索引第二级（这里索引的方法是：44bit的PPN加上12bit的0，也就是**page的起始地址**，因为每个page directory都使用一个完整的page，所以直接从page起始地址开始使用就行，除了PPN之外的位可以用来存flag），在第三级的时候得到真正的PA

较重要的低5位标志位（从低到高）：

- valid
- readable
- writable
- executable
- user：可被运行在用户空间

SATP和所有page directory都存的是**物理地址**

3级page table是由硬件实现的，MMU属于硬件，但是xv中模拟了page table

### 4.4 页表缓存（Translation Lookaside Buffer）

RISCV清空TLB指令：`sfence_vma`

硬件实现

### 4.5 Kernel Page Table

![img](MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MK_UbCc81Y4Idzn55t8%2F-MKaY9xY8MaH5XTiwuBm%2Fimage.png)

主板设计者决定：在完成了虚拟到物理地址的翻译之后，如果得到的物理地址大于0x80000000会走向DRAM芯片，如果得到的物理地址低于0x80000000会走向不同的I/O设备，参考主板手册

![img](https://906337931-files.gitbook.io/~/files/v0/b/gitbook-legacy-files/o/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MK_UbCc81Y4Idzn55t8%2F-MKaeaT3eXOyG4jfKKU7%2Fimage.png?alt=media&token=a04af08d-3c8d-4c61-a63d-6376dec252ea)

PA中：最低下Reserved，0x100是boot，上电之后首先执行boot然后跳转到0x80000000执行。

这里还有一些其他的I/O设备：

- PLIC是中断控制器（Platform-Level Interrupt Controller）
- CLINT（Core Local Interruptor）也是中断的一部分。所以多个设备都能产生中断，需要中断控制器来将这些中断路由到合适的处理函数。
- UART0（Universal Asynchronous Receiver/Transmitter）负责与Console和显示器交互。
- VIRTIO disk，与磁盘进行交互。

为了使xv易于理解，左侧低于PHYSTOP的VA和右侧PA相同

![img](MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MK_UbCc81Y4Idzn55t8%2F-MKbZEkzbzbKYgRRedXU%2Fimage.png)

- VA顶部有个Guard Page，对应PTE的valie为0，如果kernel stack耗尽会溢出到guard page导致page fault，但是guard page不会映射到任何物理内存。kernel stack映射两次但是只用上面一部分。VA和PA可以一对一，一对多，多对一映射等。
- 权限，kernel text用来存代码，代码可以读，可以运行，但是不能篡改，kernel data用来存数据，数据可以读写，但是不能通过数据伪装代码在kernel中运行

- free memory中放user进程的page table，text和data

![img](https://906337931-files.gitbook.io/~/files/v0/b/gitbook-legacy-files/o/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MKlssQnZeSx7lgksqSn%2F-MKopGK-JjubGvX84-qy%2Fimage.png?alt=media&token=0084006f-eedf-44ac-b93e-a12c936e0cc0)

user进程的虚拟地址空间分布如上图。

## Lec05 Calling conventions and stack frames RISC-V

### 5.3 gdb和汇编代码执行

`.global <function_name>`表示后者是全局函数，其他文件中也可以调用这个函数

`.asm` 和`.s`的区别在于前者有大量额外的标注，通常编译之后是`.s`文件

gdb中输入`tui enable`可以打开源代码战士窗口

`layout asm`看到所有的汇编指令，`layout reg`看到所有寄存器信息，`layout  source`查看源代码，`layout split`同时查看源代码和汇编

`focus reg/asm`可以设置focus到某个窗口

### 5.5 Stack

![img](MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MM3Hk7Gv6ibvM2lxjCc%2F-MM4D2J3t3ajqkngxRPC%2Fimage.png)

sp指向Stack的底部，fp指向Stack的顶部

查看调用栈`info/i frame`

查看一个数组`p *<addr>@<len>`

watch一个变量：`watch <variable>`，然后用`i locals`可以看到，设置conditional break：`b <position> if <condition>`

## Lec06 Isolation & system call entry/exit

### 6.1 Trap机制

trap流程：

- 保护32个用户寄存器
- 保护PC
- mode改成supervisor mode
- SATP指向kernel page table
- 堆栈寄存器指向内核的一个地址
- 跳入内核代码

supervisor mode能做的事情：

- 读写控制寄存器
- 使用PTE_U标志位0的PTE

注意supervisor不能读写任意物理地址，也需要通过page table来访问内存，如果kernel page table中不包含该VA或者这个地址PTE_U=1那么也不能访问。

### 6.2 Trap代码执行流程

write系统调用的过程：

- write执行ECALL指令切换到内核
- 内核执行汇编函数`uservec`（`trampoline.s`）
- 跳转到C语言函数`usertrap`（`trap.c`）
- 在上面这个函数中执行`syscall`，根据传入的代表系统调用的数字实现对应的syscall，在这里是`sys_write`
- `sys_write`将数据显示到console上然后结束之后返回`syscall`
- `syscall`调用`usertrapret`（`trap.c`）完成部分返回给用户空间的工作
- 执行汇编函数`userret`（`trampoline.s`）调用机器指令返回用户空间，恢复ECALL之后的程序执行

### 6.3 ECALL指令之前的状态

QEMU中有一个方法可以打印当前的page table。从QEMU界面，输入`ctrl a + c`可以进入到QEMU的console，之后输入`info mem`，QEMU会打印完整的page table。

![img](MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-ML5UF6bH4CcXBo4EXjF%2F-MLAcP8ULSwHAM90b_AM%2Fimage.png)

这是个非常小的page table，它只包含了6条映射关系。这是用户程序Shell的page table，而Shell是一个非常小的程序，这6条映射关系是有关Shell的指令和数据，以及一个无效的page用来作为guard page，以防止Shell尝试使用过多的stack page。我们可以看出这个page是无效的，因为在attr这一列它并没有设置u标志位（第三行）。attr这一列是PTE的标志位，第三行的标志位是rwx表明这个page可以读，可以写，也可以执行指令。之后的是u标志位，它表明PTE_u标志位是否被设置，用户代码只能访问u标志位设置了的PTE。再下一个标志位我也不记得是什么了（注，从4.3可以看出，这个标志位是Global）。再下一个标志位是a（Accessed），表明这条PTE是不是被使用过。再下一个标志位d（Dirty）表明这条PTE是不是被写过。

最后两条PTE的虚拟地址非常大，非常接近虚拟地址的顶端，分别是trapframe page和trampoline page。你可以看到，它们都没有设置u标志，所以用户代码不能访问这两条PTE。一旦我们进入到了supervisor mode，我们就可以访问这两条PTE了。

对于这里page table，有一件事情需要注意：它并没有包含任何内核部分的地址映射，这里既没有对于kernel data的映射，也没有对于kernel指令的映射。除了最后两条PTE，这个page table几乎是完全为用户代码执行而创建，所以它对于在内核执行代码并没有直接特殊的作用。

### 6.4 ECALL指令之后的状态

ECALL执行三件事：

- user mode变成supervisor mode
- 将`PC`保存在`SEPC`中
- ecall跳转到`STVEC`指向的指令，`PC`设置为`STVEC`，在此即`trampoline page`

ECALL之后来到`trampoline page`（trampoline：**蹦床**），首先执行第一个函数`uservec`，`csrrw a0, sscratch, a0`将`a0`和`sscratch`交换，之后内核可以任意使用`a0`。

page table还没有变，依旧使用user page table，这意味着trap处理代码（`trampoline page`）必须存在于每一个user page table，内核将`trampoline page`映射给了所有user page table。

接下来：

- 我们需要保存32个用户寄存器的内容，这样当我们想要恢复用户代码执行时，我们才能恢复这些寄存器的内容。

- 因为现在我们还在user page table，我们需要切换到kernel page table。

- 我们需要创建或者找到一个kernel stack，并将Stack Pointer寄存器的内容指向那个kernel stack。这样才能给C代码提供栈。

- 我们还需要跳转到内核中C代码的某些合理的位置。

### 6.5 uservec函数

来到`trampoline page`直接执行第一个函数`uservec`，首先需要保护32个寄存器。supervisor mode不能直接访问物理内存，只能用user page table中的非user页（因为此时还没有更改`SATP`，因为我们此时还不知道kernel page table的位置，另外还因为我们还需要同一个空闲寄存器来暂存kernel page table的地址，而我们还没保护好寄存器），XV6给每个user page table中映射了一个非user页`trapframe page`（地址永远为`0x3ffffffe000`来存某些数据，包括32个寄存器）。

> 问题：为何不保存在user的stack里（用户内存），而放在trapframe page（内核内存）里？
>
> 答：因为有些编程语言没有栈或者stack pointer不指向任何地址或者栈格式奇怪等。

内核最早进入user space的时候将`trapframe page`地址放在了`sscratch`里，所以`trampoline.S`代码中可以看到将所有寄存器保存到了`trapframe page`里。

<img src="MIT6.S081.assets/assets%2F-MHZoT2b_bcLghjAOPsJ%2F-MLVt3rA2tFnm8lZdZC5%2F-MLWsVIkkZqnouG03j7c%2Fimage-16627352832414.png" alt="img" style="zoom:50%;" />

> 问题：内核在哪里设置`sscratch`？
>
> 答：第一次内核返回user space的时候（一开始就运行在supervisor，然后第一次回到user时），
>
> - 执行`trap.c`中的`fn`函数，该函数返回值为`TRAPFRAME`和user page table，而函数第一个返回值放在`a0`里
> - 执行`trampoline.S`的`userret`函数，将`a0`和`sscratch`交换，此时`sscratch`就是`TRAPFRAME`了

保护完寄存器，接下来：

- 将kernel stack放在sp中
- 将cpu核编号`hartid`放到tp中
- 将函数`usertrap`指针写入t0
- 将kernel page table写入t1（严格来说并不是kernel page table的地址，这是你需要向SATP寄存器写入的数据。它包含了kernel page table的地址，但是移位了），然后交换`SATP`和t1
  - 此时程序没有崩溃的原因：trampoline page在user page table和kernel page table都有相同的映射关系
- `jr t0`跳转到t0所在的函数即`usertrap`

### 6.6 usertrap函数

进入usertrap的情况：系统调用，运算时除以0，使用了一个未被映射的虚拟地址，或者是设备中断。

usertrap某种程度上存储并恢复硬件状态，但是它也需要检查触发trap的原因，以确定相应的处理方式。

流程：

- 设置`STVEC`为`kernelvec`，此为内核trap处理代码位置
- `myproc`函数从当前CPU核上的进程数组中获得当前进程，使用之前的tp寄存器（hartid）
- 保存user的PC到当前进程的trapframe中（因为接下来可能运行别的进程）

- 根据trap的原因RISC-V的SCAUSE寄存器会保存不同的值，8代表系统调用
  - 首先检查当前进程是否被其他进程杀掉
  - 调整sepc，使之+4，之后返回时跳过ecall
  - `intr_on`打开中断：XV6会在处理系统调用的时候使能中断，这样中断可以更快的服务，有些系统调用需要许多时间处理
  - 最后调用`syscall`函数（`syscall.c`），通过编号（放在a7中）查找对应syscall服务调用，注意：返回值重新放回trapframe的a0里
- 再次检查当前进程是否被kill
- 调用`usertrapret`

### 6.7 usertrapret函数

流程：

- 关闭中断。目的：将要更新`STVEC`来指向用户空间的trap处理代码，如果不关闭的话，出现中断就会跳到user trap process code发生错误
- 设置STVEC指向trampoline代码。注意`trampoline`最后`sret`会重新打开中断（利用SSTATUS寄存器）
- 填入trapframe
  - 存储kernel page table指针
  - 存储kernel stack
  - 存储`usertrap`函数地址（6.5中才能写入t0）
  - 从tp中读取CPU核编号放到trapframe中

- 设置SSTATUS寄存器，清除SPP位（即变为user mode），置中断位（允许中断）
- 设置sepc为之前保存的user pc值
- 设置SATP为user的page table
- `uint64 fn = TRAMPOLINE + (userret - trampoline)`用来计算跳转到汇编代码的位置（`trampoline.S`中`userret`函数的位置）
- 最后`((void(*)(uint64, uint64))fn)(TRAPFRAME, satp)`，将fn指针作为一个函数指针，执行相应的函数（也就是userret函数）并传入两个参数，两个参数存储在a0，a1寄存器中，即a0存trapframe，a1存satp。

### 6.8 userret函数

程序回到trampoline。流程：

- 切换SATP，`csrw satp, a1`
- `sfence.vma`，page table切换成user page table
- `ld t0, 112(a0); csrw sscratch, t0`，将系统调用的返回值放到`sscratch`中
- 恢复寄存器（除了a0，它还是trapframe的地址）
- 交换sscratch和a0，此时a0是系统调用返回值，sscratch是trapframe的地址
- sret
  - 程序切换成user mode
  - sepc拷贝到pc
  - 重新打开中断