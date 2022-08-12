# 记录

本lab比较难，主要需要分析cache miss的次数。

## PartA

对于命令行参数的parse可以直接使用Unix库中的`getopt`函数，我一开始还自己手动写Parser。

另外注意`M`即Modify的时候会先`load`然后`store`，因此总会多一个`hit`出来，可以用`csim-ref`并开`-v`选项查看。

## PartB

参考大神：

- https://zhuanlan.zhihu.com/p/387662272

- https://zhuanlan.zhihu.com/p/79058089

首先要理解`S/E/B`这三个参数，本题设置的是`S=32, E=1, B=32byte`，因此cache能存储16*16个int(且每行为8个int)，但是src和dst两个矩阵
都需要存储，因此先尝试stride为8的效果，最终发现`32*32`确实是最低值343（稍大于300），`61*67`为2118稍大于2000，但是`64*64`为4723远超
2000并且和正常情况没有区别，`64*64`尝试stride设置为4的时候miss为1891稍小于2000，但效果仍然不理想。

### 32*32

```c
int i, j, m, n;
for (i = 0; i < N; i += 8)
    for (j = 0; j < M; j += 8)
        for (m = i; m < i + 8; ++m)
            for (n = j; n < j + 8; ++n)
            {
                B[n][m] = A[m][n];
            }
```

可以用较小的矩阵跑`csim`来看miss数。查看源代码可知两个矩阵首地址相差`256*256*8`个字节，因此src和dst矩阵的对角线上对应元素的index一定相同
(相同位置的元素在同一个cache line上，会发生conflict)，但tag不同，那么就一定会产生conflict，所以当先访问src对角线上某元素然后拷贝到dst对应位置的时候就发生替换，导致之后再访问的时候miss了。解决方法：直接用8个变量一次性读出src矩阵的一行，这样被替换掉也无所谓了，因为之后不会再访问src这一行了。
实际上`32*32`的情况下是非主对角线的block是不会产生冲突的，因此非主对角线block可以就用上面的代码。

```c
for (i = 0; i < N; i += 8) {
    for (j = 0; j < M; j += 8) {
        for (ii = i; ii < i + 8; ii++) {
            a0 = A[ii][j];
            a1 = A[ii][j+1];
            a2 = A[ii][j+2];
            a3 = A[ii][j+3];
            a4 = A[ii][j+4];
            a5 = A[ii][j+5];
            a6 = A[ii][j+6];
            a7 = A[ii][j+7];
            B[j][ii] =  a0;
            B[j+1][ii] = a1;
            B[j+2][ii] = a2;
            B[j+3][ii] = a3;
            B[j+4][ii] = a4;
            B[j+5][ii] = a5;
            B[j+6][ii] = a6;
            B[j+7][ii] = a7;
        }
    }
}
```

这样src矩阵就优化到最优解了，但是dst矩阵没有，上面src完成一行的时候，dst完成一列此时将所有行都放到了cache里，但是接下来src开始第二行的时候会替换掉dst的一行cache，然后dst开始第二列的时候一到对角线（第二列第二行）就得重新读入这一行被替换掉的cache line。解决方案：src读完第i行后dst才读第i行，src读完的内容先都放在8个变量里面，然后放到dst的第i行对应位置并将已经能转置的位置转置好，于是src永远冷启动一次，然后dst冷启动一次并且以后一直在cache里面可以随意访问dst之前几行的内容。

### 64*64

`64*64`的难度大大增加，原因在于每个`8*8`的block上半部分和下半部分也是处于同一个cache line中，因此导致miss大量增加。具体来说：非对角block中src和dst**自身**上半部分和下半部分是conflict的，而对角block除了自身conflict之外，src和dst对应的行也是占用相同cache line的。

解法参考上面知乎第一个解答(非常详细)。

为了尽量降低miss，关键在于：同一个cache line，src先访问读取所有数据之后就全部交给dst来访问(可以将src的内容先读取到dst中，然后dst可以随意地从之前的cache line读取数据)。

非对角block采用第一种解答中方法，对角block采用`32*32`的方法已经可以通过。

### 61*67

尝试`17*17`发现已经达到标准。

# 原版README

This is the handout directory for the CS:APP Cache Lab. 

************************
Running the autograders:
************************

Before running the autograders, compile your code:
    linux> make

Check the correctness of your simulator:
    linux> ./test-csim

Check the correctness and performance of your transpose functions:
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67

Check everything at once (this is the program that your instructor runs):
    linux> ./driver.py    

******
Files:
******

# You will modifying and handing in these two files
csim.c       Your cache simulator
trans.c      Your transpose function

# Tools for evaluating your simulator and transpose function
Makefile     Builds the simulator and tools
README       This file
driver.py*   The driver program, runs test-csim and test-trans
cachelab.c   Required helper functions
cachelab.h   Required header file
csim-ref*    The executable reference cache simulator
test-csim*   Tests your cache simulator
test-trans.c Tests your transpose function
tracegen.c   Helper program used by test-trans
traces/      Trace files used by test-csim.c
