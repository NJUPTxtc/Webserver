#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "log.h"
#include <pthread.h>
using namespace std;

Log::Log() {
    m_count = 0;
    m_is_async = false;
}

Log::~Log() {
    if (m_fp != NULL) {
        fclose(m_fp);
    }
}
//异步需要设置阻塞队列的长度，同步不需要设置
bool Log::init(const char *file_name, int close_log, int log_buf_size, int split_lines, int max_queue_size) {
    //如果设置了max_queue_size,则设置为异步
    if (max_queue_size >= 1) {
        m_is_async = true;//设置写入方式flag
        m_log_queue = new block_queue<string>(max_queue_size);//创建并设置阻塞队列长度
        pthread_t tid;
        //flush_log_thread为回调函数,这里表示创建线程异步写日志
        pthread_create(&tid, NULL, flush_log_thread, NULL);
    }
    
    //输出内容的长度
    m_close_log = close_log;
    m_log_buf_size = log_buf_size;
    m_buf = new char[m_log_buf_size];
    memset(m_buf, '\0', m_log_buf_size);
    m_split_lines = split_lines;//日志的最大行数

    time_t t = time(NULL);
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;

 
    const char *p = strrchr(file_name, '/');//从后往前找到第一个/的位置
    char log_full_name[256] = {0};

    //相当于自定义日志名
    if (p == NULL) {
        //输入的文件名没有/,直接将时间+文件名作为日志名
        snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, file_name);
    } else {
        //将/的位置向后移动一个位置，然后复制到log_name中
        //p - file_name + 1是文件所在路径文件夹的长度
        //dirname相当于./
        //将文件名中最后一个/后面的字符串作为日志名,将/前面的路径作为目录名,并将时间和日志名拼接作为日志的全名
        strcpy(log_name, p + 1);
        strncpy(dir_name, file_name, p - file_name + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, log_name);
    }

    m_today = my_tm.tm_mday;
    
    //打开日志文件,并将文件指针m_fp指向该文件,若打开失败则返回false
    m_fp = fopen(log_full_name, "a");
    if (m_fp == NULL) {
        return false;
    }

    return true;
}

/*
  该函数的作用是向日志文件中写入一条日志
  该函数采用了线程安全的方式向日志文件中写入日志,通过互斥锁m_mutex避免了多个线程同时向日志文件中写入内容的竞争问题
  同时,可根据日志系统设置的最大行数m_split_lines和日期信息m_today来控制日志文件的大小和数量
  除此之外,还引入了日志队列m_log_queue,通过异步写入的方式来提高日志系统的性能
*/
void Log::write_log(int level, const char *format, ...) {
    //获取当前时间,用于写入日志中的时间信息
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    char s[16] = {0};

    //根据传入的日志等级level,设置对应的日志前缀s,用于标识该条日志的等级
    switch (level)
    {
    case 0:
        strcpy(s, "[debug]:");
        break;
    case 1:
        strcpy(s, "[info]:");
        break;
    case 2:
        strcpy(s, "[warn]:");
        break;
    case 3:
        strcpy(s, "[erro]:");
        break;
    default:
        strcpy(s, "[info]:");
        break;
    }

    //写入一个log，对m_count++, m_split_lines最大行数
    m_mutex.lock();
    m_count++;

    //日志不是今天或写入的日志行数是最大行数(m_split_lines)的倍数
    //根据需要判断是否需要创建新的日志文件,如果需要创建,则根据日期信息和文件名生成新的日志文件名
    if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0) {//everyday log
        
        char new_log[256] = {0};
        fflush(m_fp);
        fclose(m_fp);
        char tail[16] = {0};
        
        //格式化日志名中的时间部分
        snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);
       
       //如果是时间不是今天,则创建今天的日志,更新m_today和m_count
        if (m_today != my_tm.tm_mday) {
            snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
            m_today = my_tm.tm_mday;
            m_count = 0;
        } else { //超过了最大行,在之前的日志名基础上加后缀,m_count / m_split_lines
            snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, m_count / m_split_lines);
        }
        m_fp = fopen(new_log, "a");
    }
 
    m_mutex.unlock();//释放互斥锁

    va_list valst;//可变参数的变量
    va_start(valst, format);//将format参数的值传递给valst,以便于格式化输出日志内容

    string log_str;
    m_mutex.lock();//加锁  

    //写入内容格式:时间 + 内容
    //根据当前时间和日志前缀s,将时间和日志前缀写入日志缓存m_buf中,并返回写入字符的总数n
    int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
    
    //将格式化字符串根据格式化参数写入日志缓存m_buf中,并返回写入字符的总数m 
    int m = vsnprintf(m_buf + n, m_log_buf_size - n - 1, format, valst);

    //在日志缓存m_buf的行末添加回车符和结束符
    m_buf[n + m] = '\n';
    m_buf[n + m + 1] = '\0';
    log_str = m_buf;//更新日志字符串log_str为日志缓存m_buf

    m_mutex.unlock();//释放互斥锁

    //若m_is_async为true表示异步,默认为同步
    //如果当前日志系统为异步写入模式,并且日志队列还有空间,则将日志信息加入到日志队列中
    if (m_is_async && !m_log_queue->full()) {
        m_log_queue->push(log_str);
    } else {//如果当前日志系统为同步写入模式,或者日志队列已满,则加锁,将日志信息写入到日志文件中,然后释放互斥锁
        m_mutex.lock();
        fputs(log_str.c_str(), m_fp);
        m_mutex.unlock();
    }

    va_end(valst);//结束变量参数的读取
}

void Log::flush(void) {
    m_mutex.lock();
    //强制刷新写入流缓冲区
    fflush(m_fp);
    m_mutex.unlock();
}
