为什么 创建文件的权限默认是664，创建目录的默认权限是775

umask  002



ls -lh   易于阅读的

ls ../../   按tab键，可以显示所有的



硬链接

​	相当于拷贝

软链接

​	ls  -s  a.tar.gz  b.tar.gz       b.tar.gz->a.tar.gz       相当于建立一个快捷方式

​	通过软链接可以屏蔽a.tar.gz的变化带来的影响



### shell脚本

​	\#!/bin/bash

 

​	\#shell脚本

​	mytext="hello world"

​	echo $mytext;

注意初学者易犯错误：

1） 第一行#不是注释 其他#都是注释；

2） echo $mytextA;写错现象；

3） 写错现象#!bin/bash

4） 脚本如果没有执行权限 chmod 111 01hello.sh，如何办

chmod u+x  01hello.sh  chmod u-x 01hello.sh

chmod 755  01hello.sh



alias  	别名

单反号 ``，  表示里面是一个命令，要执行这个命令， 很常用， 重点

ls \`cat abc.c\`



nohup  command  &    把命令转换到后台执行

nohup tar zcvf day02.tar.gz  day02   &



重定向   <  输入    >  输出

模式匹配

​	ls f*.txt

​	ls f????.txt

正则表达式

管道