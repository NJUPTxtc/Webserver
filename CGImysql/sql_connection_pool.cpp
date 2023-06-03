#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

using namespace std;

connection_pool::connection_pool() {
	m_CurConn = 0;
	m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance() {
	static connection_pool connPool;
	return &connPool;
}

/* 该函数的作用是初始化数据库连接池，具体分析如下：
1.将传入的数据库连接信息url、User、PassWord、DBName、Port、MaxConn、close_log分别赋值给连接池对象的成员变量
2.循环创建MaxConn条数据库连接,并将连接对象添加到连接池中
3.对于每一个连接对象,首先调用mysql_init函数初始化连接对象,如果初始化失败,则输出错误日志并退出程序
4.调用mysql_real_connect函数连接到MySQL服务器,如果连接失败,则输出错误日志并退出程序
5.如果连接成功，则将连接对象添加到连接池中，并将空闲连接数量m_FreeConn加1
6.初始化连接池的信号量reserve为m_FreeConn,即最大连接次数
7.将连接池的最大连接数m_MaxConn设置为当前的空闲连接数m_FreeConn
需要注意的是,该函数在创建连接对象时,采用了循环的方式创建多个连接对象,并将连接对象添加到连接池中,从而提高了数据库连接的复用性和性能 */
void connection_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log) {
	//初始化数据库信息
	m_url = url;
	m_Port = Port;
	m_User = User;
	m_PassWord = PassWord;
	m_DatabaseName = DBName;
	m_close_log = close_log;

	//创建MaxConn条数据库连接
	for (int i = 0; i < MaxConn; i++) {
		MYSQL *con = NULL;
		con = mysql_init(con);
		//初始化失败
		if (con == NULL) {
			LOG_ERROR("MySQL Error");
			exit(1);
		}

		//连接到MySQL服务器
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);
		
		//连接失败
		if (con == NULL) {
			LOG_ERROR("MySQL Error");
			exit(1);
		}

		//更新连接池和空闲连接数量
		connList.push_back(con);
		++m_FreeConn;
	}

	reserve = sem(m_FreeConn);//将信号量初始化为最大连接次数
	m_MaxConn = m_FreeConn;//将连接池的最大连接数m_MaxConn设置为当前的空闲连接数m_FreeConn
}

//当有请求时,从数据库连接池中返回一个可用连接,更新使用和空闲连接数
MYSQL *connection_pool::GetConnection() {
	MYSQL *con = NULL;//连接池中没有可用连接

	if (0 == connList.size()) return NULL;

	//取出连接,信号量原子减1,如果当前值为0,则等待其他线程释放连接
	reserve.wait();
	
	lock.lock();

	//从连接池中取出第一个连接对象con,并将其从连接池中移除
	con = connList.front();
	connList.pop_front();

	//更新连接池的空闲连接数量m_FreeConn和当前使用连接数量m_CurConn
	--m_FreeConn;
	++m_CurConn;

	lock.unlock();
	return con;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MYSQL *con) {
	if (NULL == con)
		return false;

	lock.lock();

	connList.push_back(con);
	++m_FreeConn;
	--m_CurConn;

	lock.unlock();

	//释放连接原子加1
	reserve.post();
	return true;
}

//销毁数据库连接池
void connection_pool::DestroyPool() {
	lock.lock();
	if (connList.size() > 0) {
		list<MYSQL *>::iterator it;

		//遍历连接池中的所有连接对象并关闭连接
		for (it = connList.begin(); it != connList.end(); ++it) {
			MYSQL *con = *it;
			mysql_close(con);
		}
		m_CurConn = 0;
		m_FreeConn = 0;
		connList.clear();//清空连接池中的所有连接对象
	}

	lock.unlock();
}

//当前空闲的连接数
int connection_pool::GetFreeConn() {
	return this->m_FreeConn;
}

connection_pool::~connection_pool() {
	DestroyPool();
}

connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool) {
	*SQL = connPool->GetConnection();//调用连接池对象的GetConnection函数获取一个连接对象,并将其赋值给参数SQL
	
	//将连接对象conRAII和连接池对象poolRAII分别赋值为SQL和connPool
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII() {
	poolRAII->ReleaseConnection(conRAII);//调用连接池对象的ReleaseConnection将连接对象conRAII释放回连接池
}