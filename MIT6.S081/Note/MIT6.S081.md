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