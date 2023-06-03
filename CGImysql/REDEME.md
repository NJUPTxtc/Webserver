####加入Mysql功能使用的API介绍：

##### 1.  mysql_init(MYSQL *mysql)

```
官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-init.html

mysql_init(MYSQL *mysql)
参数：
	分配或初始化与mysql_real_connect()相适应的MYSQL对象。如果mysql是NULL指针，该函数将分配、初始化、并返回新对象。否则，将初始化对象，并返回对象的地址。如果mysql_init()分配了新的对象，当调用mysql_close()来关闭连接时。将释放该对象。
返回值：
	成功：返回初始化的MYSQL*句柄
	失败：如果无足够内存以分配新的对象，返回NULL。
```
##### 2.  mysql_real_connect

	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-real-connect.html
	
	MYSQL * mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag)
	参数：
		mysql：mysql_init()函数返回的地址
		host：为null或 localhost时连接的是本地的计算机
		user：数据库名，例如：root
		passwd：要连接的数据库的密码，例如：123456
		db：要连接的数据库中的哪个库，例如：要使用Webserver库
		port：数据库的端口号。如果port不为0，该值将用作TCP/IP连接的端口号
		unix_socket：使用 unix连接方式，unix_socket为null时，表明不使用socket或管道机制
		clientflag：这个参数经常设置为0。但可以通过设置来启用某些功能。
	返回值：
		不成功：返回NULL
		成功：返回与第一个参数相同的值。

##### 3.  mysql_query

	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-query.html
	
	int mysql_query(MYSQL *mysql, const char *stmt_str)
	介绍：
		执行以空终止的字符串stmt_str指向的 SQL 语句。
	参数：
		stmt_str：mysql语句
			单语句不要加';'，多语句可以用';'隔开
	返回值：
		成功：返回0
		失败：非0值

##### 4. mysql_store_result

	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-store-result.html
	
	MYSQL_RES *mysql_store_result(MYSQL *mysql)
	介绍：
		它是同步函数，如果要与数据库进行异步通信的话，用mysql_store_result_nonblocking()。在调用完mysql_query()后，必须为每个成功生成结果集的语句调用mysql_store_result()。在处理完结果集之后，还必须调用mysql_free_result()。
	返回值：
		成功：返回指向包含结果集的MYSQL_RES结果结构的指针。
		失败：没有返回结果集或者出现错误，返回NULL

##### 5. mysql_free_result

	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-free-result.html
	
	void mysql_free_result(MYSQL_RES *result)
	介绍：
		mysql_free_result()释放由mysql_store_result()，mysql_use_result()，mysql_list_dbs()为结果集分配的内存。完成结果集后，必须通过调用mysql_free_result()释放其使用的内存。	
	参数：
		result：指向包含结果集的MYSQL_RES结果结构的指针

##### 6. mysql_num_fields()
	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-num-fields.html
	
	unsigned int mysql_num_fields(MYSQL_RES *result)
	介绍：
		返回结果集中的列数
	参数：
		result：指向包含结果集的MYSQL_RES结果结构的指针
	返回值：
		返回结果集中的列数

##### 7. mysql_fetch_fields
	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-fetch-fields.html
	
	MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result)
	介绍：
		返回结果集的所有MYSQL_FIELD结构的数组。每个结构都为结果集的一列提供字段定义。
	参数：
		result：指向包含结果集的MYSQL_RES结果结构的指针
	返回值：
		返回结果集所有列的MYSQL_FIELD结构数组。

##### 8. mysql_fetch_row
	官方介绍：https://dev.mysql.com/doc/c-api/8.0/en/mysql-fetch-row.html
	
	MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
	介绍：
		检索结果集中的下一行。该行中的值数由mysql_num_fields(result)给出。如果row保留对mysql_fetch_row()的调用的返回值，则指向这些值的指针将作为row[0]到row[mysql_num_fields(result)-1]进行访问。该行中的NULL值由NULL指针指示。
	参数：
		result：指向包含结果集的MYSQL_RES结果结构的指针
	返回值：
		下一行的MYSQL_ROW结构或者NULL





####RAII机制的介绍：

```
参考资料:
	https://zhuanlan.zhihu.com/p/389300115
	https://baijiahao.baidu.com/s?id=1716641957959725509&wfr=spider&for=pc
```