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



### 变量

set  显示所有的本地变量  

本地变量，只在当前shell下有用

环境变量，用于所有用户进程（经常称为子进程），.bash_profile, 每个用户都使用

声明环境变量 export myvar="fdfsd"

env,   查看环境变量

调环境的时候，心里要有linux kernel，要有shell

unset myvar,   清除变量



find   ./   -name  "*.c"

标准变量

在 /etc/profile  下定义



几个重要的配置文件

/etc/bashrc  /etc/profile  .bash_profile   

/etc下很多配置文件



特殊变量

$#    $*    $?



### 引号

“abc  $SHELL”   “”双引号中的 $会引用变量的值

   ‘abc  $SHELL’  ''单引号不做任何解析，直接输出

``单反号，里面的东西是执行命令

反斜杠



cat  显示文件内容

cat    file|more  分屏显示

cat  a.txt b.txt 一下显示多个文件内容



标准输入、标准输出和错误



### 文件重定向

​	>  把标准输出重定向到一个新文件中

​	>>  追加到现有文件

​	cat file1.txt file2.txt > out.txt 2>&1    项目用得多



### 控制流结构

### 条件测试

​	test  condition

​	test -d /etc/    测试目录存在不

​    echo  $?   为0， 目录存在

​	test -w tmp.txt  测试文件是否可写

​		[ -w tmp.txt]  检查文件



什么场合用什么技术

数字运算

​	expr 10  +  10   注意空格

if then else

使用格式

​	if 条件

​	then  命令

​	fi

注意：if语句必须以fi终止

if   then   else  fi

for  变量名  in 列表

do

​	命令

done

until 循环

until  条件

do

​		命令1

​		命令2

done

while  条件

do

​		命令1 

done

df  查看磁盘空间使用情况

df |grep  /dev/sda1   按行提取数据

df |grep  /dev/sda1 |awk '{print $5}'  按行提取后，awk 按列提取

sed   's/old/new/g'   s表示对文本进行替换，old是要替换的子串，new替换old，g表示全部替换

df |grep  /dev/sda1 |awk '{print $5}' | sed 's/%//g' 



文本过滤

find   ./  -name "*.sh"    查找当前目录下以.sh结尾的文件

find  ./  -name "[A-Z]*"   查找所有以大写字母开头的文件



xargs

find ./ -name "*.sh" | grep "13"  这个是把find的全部给grep，如果find的太多，会造成环境参数太多，执行不了

grep是有没有匹配到13的

xargs  一行一行取数据

find ./ -name "*.sh" | xargs  grep "13"



find   在指定路径下查找文件, 大范围查找

再在文件中，按照行按照列， 检索想要的信息

grep 、sed、awk

grep 按行处理文本

grep  "aaaaa" a.txt   在文件中查找有aaaa的行

grep -n "aaaaa" a.txt   在文件中查找有aaaa的行   -n显示行号

-v显示没有的

grep -v "aaaaa" a.txt 



awk -F ":"  '{print  $1 }'  ./passwd   打印passwd文件的按：分割的第一列

awk -F ":"  '{print  $1 "\t"  $3}'  ./passwd   打印passwd文件的按：分割的第一列 tab 第三列

awk -F ":"  '$3 > 500 {print  $1 "\t"  $3}'  ./passwd



基础很重要

工具

ipcs，查看共享内存

