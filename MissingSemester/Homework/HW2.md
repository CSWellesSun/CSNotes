题目链接：https://missing-semester-cn.github.io/2020/shell-tools/

常用软件：

- 查看文档：`tldr(too long, don't read)`

- 查找文件：`fd-find`/`locate`

- 查找代码：`ripgrep`

- 查找之前的命令：`history`(配合`grep`)/直接`ctrl+R`

- 模糊搜索：`fzf`

1. 显示所有文件、打印以人类理解方式、以最近访问顺序排序、以彩色文本显示输出结果。

`ls -alh --color --time=access -t`

2. 

```shell
#! /bin/bash
marco() {
    pwd > /tmp/missing/last_address
}

polo() {
    cd $(cat /tmp/missing/last_address)
}
```

3. 

```shell
count=0
./2_3_test.sh 1>2_3_out 2>2_3_err
while [[ $? -eq 0 ]]
do
        let a++
        ./2_3_test.sh 1>>2_3_out 2>>2_3_err
done
echo $a
```

4. 

`fd 'html' --type f -0 | xargs -0 tar czf 2_4.tar`
或者`fd 'html' --type f | xargs -d '\n' tar czf 2_4.tar `

5. 

`fd | ls -alh --time=use -t`