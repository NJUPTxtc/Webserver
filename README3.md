#### 相关函数介绍：

#####1.c_str()函数
	c_str()函数返回一个指向正规c字符串的指针,内容和string类的本身对象是一样的,通过string类的c_str()函数能够把string对象转换成c中的字符串的样式

##### 2. strcat()函数
	char *strcat(char *__restrict__ __dest, const char *__restrict__ __src)
	介绍：
		strcat函数又被称为是字符串追加/连接函数，它的功能就是在一个字符串后面追加上另外一个字符串。
		将src字符串追加到dest字符串后边。
	举例：a = "hello", b = "world"
		strcat(a, b + 2); 则a = "hellorld";

##### 3. strncpy()函数
	char *strncpy(char *__restrict__ __dest, const char *__restrict__ __src, size_t __n)
	介绍：
		 把 src 所指向的字符串复制到 dest，最多复制 n 个字符。当 src 的长度小于 n 时，dest 的剩余部分将用空字节填充。

##### 4. getopt()函数
	int getopt(int ___argc, char *const *___argv, const char *__shortopts)
	参考资料：
	https://mculover666.blog.csdn.net/article/details/106646339?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-106646339-blog-100149072.pc_relevant_default&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-106646339-blog-100149072.pc_relevant_default&utm_relevant_index=4
	https://blog.csdn.net/qq_36379998/article/details/100149072
	介绍：
		用来解析命令行选项。
		通过连续调用getopt()解析命令行，每次调用时全局变量optind都得到更新，其中包含着参数列表argv中未处理的下一个元素的索引
	参数：
		argc：参数个数，和main函数的argc一样
		argv：字符串指针，和main函数的argv一样
		shortopts：是选项字符串
			字符代表一个选项；
			没有冒号就表示纯选项，不需要参数；
			一个冒号代表该选项之后必须带有参数，可以使用空格，也可以不使用；
			两个冒号代表选项之后的参数可写可不写
		例如："vha:b:c::"表示：
			支持-v选项，通常用于打印版本号；
			支持-h选项，通常用于打印帮助信息；
			支持-a选项，后边必须带有一个参数；
			支持-b选项，后边必须带有一个参数；
			支持-c选项，后边可以带参数，也可以不带参数。
	返回值：
		成功：返回选项参数
		失败：返回-1

##### 5. gettimeofday()函数
```c++
int gettimeofday(struct timeval*tv, struct timezone *tz);
介绍：
	获取当前精确时间，或者执行计时。
参数：
	tv：保存获取时间结果的结构体
	tz：保存时区结果
	struct timeval {
		long tv_sec; //秒
		long tv_usec;//微秒
	};
	struct timezone {
		int tz_minuteswest;//格林威治时间往西方的时差
		int tz_dsttime;//DST 时间的修正方式
	};
返回值：
	成功：返回0
	失败：返回-1
```
##### 6. mmap()函数

```c++
void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
    比mmap更快的是sendfile()
参考资料：
1.https://blog.csdn.net/qq_41687938/article/details/120478967
2.https://blog.csdn.net/hellozhxy/article/details/115312608?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-115312608-blog-120478967.pc_relevant_antiscanv2&spm=1001.2101.3001.4242.1&utm_relevant_index=2

参数：
	start：映射区的开始地址
	length：映射区的长度
	prot：期望的内存保护标志，不能与文件的打开模式冲突。
	flags：指定映射对象的类型，映射选项和映射页是否可以共享。
	fd：有效的文件描述符。如果MAP_ANONYMOUS被设定，为了兼容问题，其值应为-1.
	offset：被映射对象内容的起点。
返回值：
	成功：返回映射区域的地址指针。
	失败：返回MAP_FAILED（(void*) -1），并设置errno
```

##### 7. munmap()
```c++
int munmap( void * addr, size_t len )
参数：
	addr：映射区域的地址指针，mmap()的返回值
	len：映射区的长度
返回值：
	成功：返回0
	失败：返回-1，并设置errno
```

##### 8. fflush()
	int fflush(FILE *__stream)
	介绍：将缓冲区中的内容写到stream所指的文件中去。若参数stream为NULL，则会将所有打开的文件进行数据的更新。
	返回值：
		成功：返回0
		失败：返回EOF，并设置errno
		
##### 9. fputs()
	int fputs(const char *__restrict__ __s, FILE *__restrict__ __stream)
	介绍：将一个字符串写入stream中
	参数：
		s：要输出的字符串的首地址
		stream：表示向何种流中输出，可以是标准输出流stdout，也可以是文件流。
	返回值：
		成功：返回非0值
		失败：返回EOF