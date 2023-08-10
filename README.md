# 家庭服务机器人仿真--[ATRI](ATRI "高性能哒~")
Author : ShiQiao Chen(陈世侨)


Affiliation: WuHan University of Technology

### 这个项目是什么
这个项目是家庭机器人仿真的项目，点击查看[具体规则](ATRI/doc/2013/HomeRule2013.pdf "你说的对，但是这比赛规则已经从13年开始就基本不变了，而且13年那一版本的规则才是 detail 最多的")



### 如何食用本代码
0. 首先要有Linux下C++11，CMAKE的基础，了解面向对象编程。
1. 先看官方给出的[build过程](ATRI/doc/2013/build.pdf)，尝试先build一遍，搞清楚这个项目分为服务端和客户端，我们只需要写客户端就好。
2. 客户端的代码在[这个路径下](ATRI/example)。
3. 代码架构可以大致分成两个模块，语句/指令解析模块 和 规划模块。
4. build完成后，执行run.sh就好，其中关键的指令是
./atri -nlp 1 -err 1 -ask_2 0 -cons 2 -path ../example/words.txt

|参数| 描述| 值|
|-|-|-|
|-nlp|自然语言处理|0/1|
|-err|纠错模式|0/1|
|-ask_2|是否每一次都询问两遍|0/1|
|-cons|约束模式|0：不管约束条件<br>1:维持所有约束条件<br>2: 自动决策每个约束是否维持<br> 默认为2|
|-path|词语表路径|sting|


### 语句/指令解析
指令的格式标准，解析比较简单，语句的解析就比较困难，因此写了个[自然语言解析器](ATRI/example/parser.hpp)，其原理对应编译原理中的词法分析与语法分析的内容，使用下推自动机的算法进行解析，有兴趣的可以参考龙书第3章与第4章。

### 规划
规划算法参考[PPT](ATRI/doc/planning_slides.pdf)中的回归规划(Regration)。

**注意**这个算法并非最优，比不过西工大的算法，最优规划问题属于NP-hard问题，此处有改进空间。

