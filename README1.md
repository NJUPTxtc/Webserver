#说明
* 使用 线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Reactor和模拟Proactor均实现) 的并发模型
* 使用状态机解析HTTP请求报文，支持解析GET和POST请求
* 访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件
* 利用RAII机制实现了数据库连接池，减少数据库连接建立与关闭的开销
* 实现同步/异步日志系统，记录服务器运行状态

### 框架
<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j1atq5hj30g60lm0w4.jpg" height="765"/> </div>

### 使用环境

	1. ubuntu 18.04.6
	2. MYSQL 8.0.29

### 运行前需要做的事情：

```mysql
#1，测试前请确认已经安装MYSQL数据库
#2.创建一个database
	CREATE DATABASE Webserver;
#3.进入Webserver库
	USE Webserver;
#4.创建user表
	CREATE TABLE user(
    	username char(50) NOT NULL,
        passwd char(50) NOt NULL,
        primary key(username)
    )ENGINE=InnoDB;

#添加数据的格式
INSERT INTO user(username, passwd) VALUE('name', 'passwd');

#5.修改main.cpp中的数据库初始信息，根据自己设定的修改
string user = "root";
string passwd = "123456";
string databasename = "Webserver";
```
### 目录树

```bash
.
├── build.sh                
├── CGImysql                  数据库程序	
│   ├── README.md
│   ├── sql_connection_pool.cpp
│   └── sql_connection_pool.h
├── Config.cpp                输入解析程序
├── Config.h   
├── http                      http处理程序
│   ├── http_conn.cpp
│   ├── http_conn.h
│   └── README.md
├── LICENSE
├── lock                      线程锁
│   ├── locker.h
│   └── README.md
├── log                       日志程序
│   ├── block_queue.h
│   ├── log.cpp
│   ├── log.h
│   └── README.md
├── main.cpp                  主程序
├── makefile
├── README.md
├── root                      静态资源
│   ├── fans.html
│   ├── favicon.ico
│   ├── frame.jpg
│   ├── judge.html
│   ├── logError.html
│   ├── log.html
│   ├── login.gif
│   ├── loginnew.gif
│   ├── picture.gif
│   ├── picture.html
│   ├── README.md
│   ├── registerError.html
│   ├── register.gif
│   ├── register.html
│   ├── registernew.gif
│   ├── test1.jpg
│   ├── video.gif
│   ├── video.html
│   ├── welcome.html
│   ├── xxx.jpg
│   └── xxx.mp4
├── test_pressure             压力测试
│   ├── README.md
│   └── webbench-1.5
│       ├── ChangeLog
│       ├── COPYRIGHT
│       ├── debian
│       │   ├── changelog
│       │   ├── control
│       │   ├── copyright
│       │   ├── dirs
│       │   └── rules
│       ├── Makefile
│       ├── socket.c
│       ├── tags
│       ├── webbench
│       ├── webbench.1
│       ├── webbench.c
│       └── webbench.o
├── threadpool               线程池
│   ├── README.md
│   └── threadpool.h
├── timer                    定时器
│   ├── lst_timer.cpp
│   ├── lst_timer.h
│   └── README.md
├── webserver
├── webserver.cpp
└── webserver.h

```



#### **已经添加的功能：**

#### 1.服务器与客户端之间的正常通信

	1.服务器与客户端之间的连接
	2.服务器可以正确读取客服端发来的请求信息，请求类型为：GET和POST
	3.服务器可以正确的解析客户端发来的请求
	4.服务器可以正确的发送响应信息

#### 2.日志系统
	1.日志文件放在/var目录下
	2.每个日志文件可以通过初始化来确定是同步日志还是异步日志，以及各种属性
	3.根据日志的输出信息的不同，分别放入不同的文件。例如：可以将error和warning信息放入到error文件中，其余的放在另外一个文件中，这样可以很方便的找到错误信息。

#### 3.定时器功能
	1.如果连接一段时间没有请求或响应（非活跃连接），则会关闭它，避免占用线程资源

#### 4.数据库
	1.可以注册账号密码，来登陆
	2.登陆之后进入可以看图片，视频（仅仅是测试）

#### **日志：**

###### 2023.5.10

	简单版的服务器已经写完了，在学习期间真的是各种苦恼。从0到简单的1，从看懂到跟着写再到理解，过程极其的漫长。
	用webbench做了压力测试，通过了，不过webbench的原理还需要后期来学习。	
	开始尝试加入日志系统。
###### 2023.5.12
	根据需要，将整个的程序分目录实现。
###### 2023.5.13
	将所有的程序，按照需要，分成了不同目录。
###### 2023.5.16
	看了几天的日志系统的原理和代码，还不算太明白太深入的原理，学会了如何启动日志，如何使用日志。
	尝试加入一些功能：将日志文件放在/var目录下，同时将错误信息和警告信息同其他类型的信息分开存储，便于查看与调试。
###### 2023.5.17
	可以将日志文件放在/var目录下了。
###### 2023.5.19
	终于实现了日志文件在/var目录下，同时错误信息和警告信息与其他信息分开存储。
###### 2023.5.20
	开始加入定时器功能，以便于将非活跃的连接关闭。
###### 2023.5.21
	一开始想着不按照树上的写（书上用的升序定时器链表），想着用时间轮或者时间堆来做定时器，但是不知道怎么调整时间。先按照书上的来吧，后期在修改一下。
###### 2023.5.24
	定时器加入了，但是压测过不去，总是会在某个地方卡住。
###### 2023.5.25
	调了一下午的bug,最终发现是因为有几个变量没有初始化，还有一句话没有写。哎。。。。
	调完了bug后，顺便做了一下压测，过了。哈哈哈哈哈。
###### 2023.5.26
	开始学习数据库模块。
###### 2023.5.27
	数据库模块的.cpp文件和.h文件已经写好了，修改了webserver类中的init函数，添加了数据库的数据。需要继续处理POST请求。
###### 2023.5.28
	数据库模块已经加上了，但是注册和登陆界面点击注册或登陆出错，暂时没找到原因所在。头大。。。。。。
###### 2023.2.30
	将昨天出现的bug解决了，出现问题的原因是因为编写程序不认真导致的。😔
	同时也发现了权限问题，好事。哈哈哈哈哈哈哈😊，顺便学习了一下如何修改权限，用chmod命令。
	加上的数据库模块已经能用了，压测也过了，还需要自己改下一操作文件，用别人的文件不太好😂。
	还有主函数解析函数没有加入，明天继续。
###### 2023.6.1
	今天将解析函数加上了，可以在运行时添加一些参数，来修改一些默认参数。
	还将html的文件修改了一下（简单修改一下，不会前端的代码😂）
	还有部分代码格式没有修改，明天修改一下。
###### 2023.6.3
	这几天写了一下程序的运行流程，还有一些函数的介绍，方便学习和回忆。


#### **遇到的bug：**


###### 1.用压力测试软件测试：每次总会卡住，无法再重新访问服务器。
	这个bug已经解决，原因应该是：有部分变量没有初始化，同时有些模式没有加（因为分为proactor 和 reactor，我只写了一种，但是加了变量来分开，没有初始化）。
###### 2.刚开始加入数据库时，出出现编译不通过，或者通过后，程序直接退出了（错误信息：连接数据库不成功）
	这个不是bug。在刚开始加入数据库时，有些知识还不懂。
	解决方法：需要在makefile中链接数据库。需要先创建一个数据库，然后在连接才行。

###### 3. MYSQL Error

	显示错误：MYSQL Error
		1.数据库需要先自己创建一个表之后才能使用，如果没有创建响应的表就使用的话，会出现这种错误
		2.还有一种错误原因是动静态库的链接错误。
		刚开始的错误写法-L/usr/lib/x86_64-linux-gnu
		正确写法：-I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
		-L后边应该是啥，根据系统的不同，需要略微修改一下，例如：在CentOS7中为：-L/usr/lib64/mysql -lmysqlclient

######4.首界面可以显示，在已有账号或注册界面一点击登陆或注册，就提示段错误

    1. 在已有账号界面一点击登陆就提示“段错误”
        通过调试，发现没有进入CHECK_STATE_CONTENT；
        然后继续查找原因发现没有进入parse_headers()中的 if(m_content_length != 0)这一句；
        然后继续查找原因发现是m_content_length在有POST请求时，在这之前是0，但是read()读到的是具体的数值，这说明m_content_length没有被正确赋值；
        所以继续查找，发现：parse_headers()中处理Content_Length头部字段时，写错了一个符号，将"Content-Length:"写成了"Content_Length:"。
        解决了该bug。
    2. 在注册界面一点击注册，如果没有重名的情况下就提示“段错误”，有重名的情况下，正确显示
        通过调试发现：int res = mysql_query(mysql, sqlInsert);这一句出现了“段错误”；
        发现mysql的信息没有，怀疑是什么地方没有关联这个mysql变量；
        通过调试发现，果然没有初始化，在ThreadPool类的构造函数中没有初始化，同时，在run中也没有和数据库关联起来。
        解决了该bug。

###### 5. 运行时，不在root权限下运行，会出现“段错误（核心已转储）”

	这个错误是在解决错误4的过程中发现的。
	
	经过测试发现：将日志的文件放在根目录下不会出现该问题，放在其他文件夹下就会有这种问题。
	解决方法：
		修改var的权限，在项目根目录下用chmod 777 var（如果有当日的文件，删掉，或者修改权限）
		或者 在root权限下运行




#### 还需要学习的内容
	1. 定时器时间轮和时间堆的原理
	2. 单例模式和多例模式
	3. webbench的原理

#### 致谢

Linux高性能服务器编程，游双著.
https://github.com/qinguoyi/TinyWebServer