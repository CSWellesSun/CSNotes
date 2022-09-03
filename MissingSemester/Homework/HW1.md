题目链接：https://missing-semester-cn.github.io/2020/course-shell/

9. 使用`|`和`>`，将`semester`文件输出的最后更改日期信息，写入主目录下的`last-modified.txt`的文件中

首先理解`curl`的两个选项，`--silent`表示不打印错误信息，`--head`表示只下载网页header。其次理解`grep`，是在文件中找出符合pattern的字符串并打印该行。

答案：`./semester | grep --ignore-case last-modified > last-modified.txt`

10. 写一段命令来从`/sys`中获取笔记本的电量信息，或者台式机CPU的温度。注意macOS并没有`sysfs`，所以Mac用户可以跳过这一题。

根用户可以在`sysfs`文件中写入内容来改变系统参数。

答案：

```shell
find -L /sys/class/power_supply -maxdepth 2 -name capacity
cd /sys/class/power_supply/BAT1
cat capacity
```