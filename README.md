# A Sandbox for Judger

## Brief Introduction

This is a simple sandbox developed for an online judge platform which I developed together with others [zhuqinggang][https://coding.net/u/zhuqinggang].The sandbox is based on maybe an ancient one, and I fixed some bugs and tested it, in order to get it running on my computer.

This sandbox and its runner are still under development. If you are interested in developing an online judge, comments and issues are greatly appreciated. 

> 'Cause I'm not fluent in English, other document is written in Chinese.



# 评测姬沙箱后台 (雾)

## 简介

这个沙箱是为了和别人合作开发一个OJ而写的。别人：[zhuqinggang](https://coding.net/u/zhuqinggang)

这个小项目是基于一个别人写的古老的沙箱写的。原项目[Eevee](https://github.com/Hexcles/Eevee) 。我修了几个巨大的bug然后稍加改进就写了这个东西。

这个项目还在开发之中(也就是说：很不稳定且测试比较不完全)。欢迎issue和评论交流

一些文档：[配置文件等](https://github.com/ridethepig/JudgeGirl/blob/master/%E9%A2%98%E7%9B%AE%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6%E6%A0%BC%E5%BC%8F%E8%A7%84%E8%8C%83.md)

# **本项目正在进行重构, 本分支已停止维护**

# **This Project is Now Under Refaction. This Branch will be NO LONGER MAINTAINED.**



# How to build & run

### Step 1

First make sure that you have installed cmake (>= 2.8)

* Enter Directory ``Runner`` 
* Get the src files compiled using cmake (If you don't like cmake, you can also compile them as you like. Remember to add -std=c++11 tag)

### Step2

* Enter Directory ``Sandbox``
* Just ``make`` them

### Step3

* Make a root directory and copy these two executables into it.
* Make a sub-directory called ``sandbox`` under your root directory and then move ``caretaker`` into ``./sandbox``
* Create a config file named ``sandbox.conf`` under your root directory

### Step4

Follow another document and run it happily

# 如何构建并运行

### 第一步

进入``Runner``目录，使用cmake来编译这个项目，不过你也可以直接写一条g++命令，记得使用c++11来编译

### 第二步

进入``Sandbox``目录，直接键入make命令即可

### 第三步

新建一个文件夹作为评测机的根目录，然后在建立一个``sandbox``目录。

把得到的两个可执行文件弄进来，一个把``caretaker``那个放在sandbox目录下，另一个就放在根目录下

然后自行新建一个配置文件，名为``sandbox.conf``。然后根据另一个说明文件就可以愉快的运行了