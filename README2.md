### 主程序执行流程

#### 1. 解析函数
	先解析主函数的输入参数，确定端口号等信息。
#### 2. 初始化函数
	初始化相关参数：端口号，数据库数据库名，数据库密码，数据库，日志写入方式，线程数量，是否优雅关闭，触发方式，数据库最大对象个数（最大相当于线程数，是否开启日志，并发模型）。
#### 3. 日志系统
	如果m_close_log = 0，说明开启日志，再根据m_is_async来选择是同步日志还是异步日志，默认为0，即同步日志。
	如果m_close_log = 1，说明不开启日志。
#### 4. 数据库
	初始化数据库
	读取数据库的表，获得结果集，并将结果集同时存放到map中，以便于查找用户名和密码。
#### 5. 线程池
	根据解析函数获得的线程数或者使用默认设定的线程数创建线程池，用于处理事件。
#### 6. 事件监听
```
创建监听的套接字，用于监听
    a. 创建套socket 							  	    socket
    b. 命名socket（绑定）,绑定ip地址和端口号				 bind
    c. 监听socket 									listen
为了能够同时监听多个文件描述符，使用epoll I/O复用技术		
    d. 初始化定时器（用于关闭非活跃的连接）
    e. 创建epoll对象 epoll_creat
```
##### 7. 事件处理
	两种高效的事件触发模式：Reactor和Proactor
		Reactor模式：主程序只负责监听socket上是否有事件发生，有的话就通知工作线程。读写数据，接受新的连接，以及处理客户请求均在工作线程中完成。
		Proactor模式：所有的I/O操作交给主线程和内核处理（读写操作均有主线程完成），工作线程仅负责业务逻辑。
	
	a. epoll_wait将监听到的事件放到数组中，然后遍历。
	b.判断是连接新客户、读操作还是写操作：
		b1. 如果是新客户连接，用accept连接，然后初始化连接和定时器后，加入放进用户数组中
			分为水平触发方式（可以多次通知，所以可以不用立即处理）和边沿触发方式（只触发一次，故用while一次性处理完）
		b2. 如果是读操作：
	        Reactor模式：将该事件放入请求队列中，交给工作线程去处理。
	        Proactor模式：主线程读，读完之后，交给工作线程去操作。
		b3. 如果是写操作，将该事件放入请求队列中，交给工作线程去处理
	        Reactor模式：将该事件放入请求队列中，交给工作线程去操作。
	        Proactor模式：工作线程将要发送的数据写入到写缓冲区中后，主线程发送数据。


### 工作线程执行流程
a. worker() -> run()-> process() -> process_read()和process_write()
	a1. process_read() --> getLine() --> parse_request_line() --> parse_header() --> parse_content()-->do_request()
	a2. 和process_write() --> add_status_line() --> add_headers() --> add_content()
		a21. add_status_line() ---> add_response()
		a22. add_headers() ---> add_contentLength() ---> add_linger() ---> add_blank_line()
		a23. add_content() ---> add_response()

##### 1. 创建线程
	在主程序中已经创建好了线程池，里边有thread_num个线程，设置为线程脱离（自动回收）  		pthread_create
	创建的线程时，给它们都指定将要运行的函数Worker。
##### 2. 创建Apppend函数
	该函数用于往请求队列中添加任务
##### 3 Worker函数
	该函数从请求队列中获取任务来处理。使用Run函数。
	创建的线程均 执行这个函数，所以创建的多个线程通过竞争机制来获得处理任务，实现了多并发。
##### 4. Run函数
	从队列中获取任务, 然后根据模式的不同做出不同的处理：
		Reactor模式：判断是读还是写
			如果是读，一次性读取后，通过process函数来处理
			如果是写，一次性写完数据
		Proactor模式：主程序已经完成了读写操作，通过process函数来处理。

##### 5. Process函数
	Process函数处理http请求，分为：
		a.处理读操作		rrocess_read
		b.处理写操作		process_write
##### 6. ProcessRead函数
	该函数主要是解析获得的http数据（保存在读缓冲区中），从中得到请求行、头部信息、请求体内容。分别用到了下列函数：
	getLine函数：获取一行数据，用\r\n作为一行数据的结束。
	parse_request_line函数：解析http请求行的信息。
	parse_headers函数：解析http请求的头部信息。
	parse_content函数：解析http请求的消息体信息。
	do_request函数：如果确认获得是正确的http请求，分析请求的文目标件的属性，如果目标文件存在，不是目录，该用户有权限获得该目标文件，则使用mmap将目标文件的地址映射到指定位置，并告知客户端调用成功。
##### 7. ProcessWrite
	该函数主要是将要发送的http响应数据写到写缓冲区中。分别用到了下列函数：
	add_status_line函数：添加响应行数据到写缓冲区中。
	add_headers函数：添加头部字段数据到写缓冲区中。
	add_content_length函数：添加Content-Length头部字段数据。
	add_linger函数：添加Connection头部字段数据。
	add_blank_line函数：添加回车换行数据。
	add_content函数：添加消息体数据到写缓冲区中。
	add_response函数：用分散写方式，将内容保存到写缓冲区中。