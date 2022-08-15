# 记录

首先该Lab的Eval方法是：先`make`生成可执行程序，然后`make testXX`和`make rtestXX`比较两个程序的运行输出，除了PID
有差异之外，别的应该保持相同。推荐的做法是从`01`开始慢慢比对。

部分baseline可以从书中获得，在其基础上做修改。另外我从`csapp.c`中扒来了所有信号相关的包装函数。下面是主要的修改：

1. 对于`builtin_cmd`，直接从`eval`中`return`。书中源代码会导致`builtin_cmd`继续`fg`和`bg`的处理。

2. 子程序在`execve`之前要加上`setpgid(0,0)`，前一个0表示子程序使用当前PID作为自己的GPID，因为`ctrl-c`发送的`SIGINT`会发送给所有前台进程组的进程，而所有新子进程的进程组都是继承着父进程的进程组，使得`ctrl-c`会让所有新子进程都收到。另外注意`kill`函数的参数`pid`要取负数（即`-pid`），这样才能发给进程组。

3. 要理解`tsh`是一个`shell`，`ctrl-c`不会关闭这个shell，只会关闭前台运行的程序，所以就从`jobs`列表中找到前台程序然后kill掉！注意：有`fgpid`函数可以得到当前正在运行的前台进程。

4. 在addjob的之前屏蔽所有会干扰的信号，因为新子进程可能在addjob之前就运行完了。

5. (重要)等待fg程序完成，需要写在sigchld的信号处理程序里(wait都写在这里)，如果main和sigchld都写wait的话会error(后一个找不到child，因为都被回收了)，所以必须选一个。(见课件)

6. 要理解`waitpid`中`status`的各种取值，当`WIFEXITED(status)`为真表示是正常结束(而不是因为**信号**)，其他的有`WIFSIGNALED/WIFSTOPPED`！

7. 要理解为什么给子进程发送`kill`可以结束或者暂停进程，原因是因为这些进程没有用`sigaction`替换信号的默认行为，因此发送之后可以暂停。

8. 注意`WIFSTOPPED`为真，必须是`waitpid`的`options`设置为`WUNTRACED`，因为这个选项是当有进程终止或**暂停**的时候返回，而如果为0的话必须是进程**终止**才返回，导致`ctrl-z`之后程序就卡在`waitpid`不动了。

9. `printf`和`scanf`等要输出`%`必须用`%%`

10. 通过参考手册`man 2 waitpid`可知，实际上`wait`等待的是子进程的状态变化，即终止、暂停和继续，所以需要给`waitpid`再加一个选项`WCONTINUED`才能响应resume的子进程，否则的话会提示`No child process`！

# 原版README

################
CS:APP Shell Lab
################

Files:

Makefile	# Compiles your shell program and runs the tests
README		# This file
tsh.c		# The shell program that you will write and hand in
tshref		# The reference shell binary.

# The remaining files are used to test your shell
sdriver.pl	# The trace-driven shell driver
trace*.txt	# The 15 trace files that control the shell driver
tshref.out 	# Example output of the reference shell on all 15 traces

# Little C programs that are called by the trace files
myspin.c	# Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
myint.c         # Spins for <n> seconds and sends SIGINT to itself

