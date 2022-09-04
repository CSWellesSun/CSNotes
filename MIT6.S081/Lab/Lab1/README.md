# 记录

## sleep & pingpong

略

## primes

构造递归函数，最初版本能够顺利打印出所有prime，但是出现如下报错：

```
usertrap(): unexpected scause 0x000000000000000d pid=3
            sepc=0x0000000000000158 stval=0x0000000000003f5c
```

最后发现是`main`里面没有写`exit(0)`...

## find

参考`ls`写即可，主要注意要去除`.`和`..`两个文件夹，另外我写了`fmtname(buf) == fmtname(filename)`这样的句子，但是`fmtname`这个函数返回的是一个静态字符数组地址，因此这个永远是成立的。我的解决方法就是将后者`strcpy`到另一段区域中再进行比较。

## xargs

需要注意的是每一行作为一个argument，而不是根据空格来定。另外一开始设置buffer过大，`char buf[32][1024]`超出了范围。