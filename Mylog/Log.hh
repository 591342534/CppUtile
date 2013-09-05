/*********************************************************************
*
* Copyright (c) 2013, UTStarcom (ShenZhen) Technology Ltd.
*       All rights reserved.
*
* Filename: Log.hh
* Abstract: Describe the purpose of the file
*
* Version : 0.1
* Author  : Jaxon Jia (UTSC0365)
* Date    : 2013/09/03
*
**********************************************************************/
#ifndef __Log_h__
#define __Log_h__

#include <string>
#include <stdio.h>
using namespace std;

////��ʵ��API
#define SET_SINGLE_LOG_NAME(filepath)			SET_LOG_NAME("DEFAULT", filepath)
#define SET_SINGLE_LOG_LEVEL(level)				SET_LOG_LEVEL("DEFAULT", level)
#define SET_SINGLE_LOG_SIZE(size)				SET_LOG_SIZE("DEFAULT", size) 

//��ʵ��API
#define SET_LOG_NAME(module, filepath)		CLogFactory::get_instance(module)->set_log_filepath(filepath)
#define SET_LOG_LEVEL(module, level)		CLogFactory::get_instance(module)->set_log_level(level)
#define SET_LOG_SIZE(module, size)			CLogFactory::get_instance(module)->set_log_size(size)

#define TRACE(module, format, ...)			CLogFactory::get_instance(module)->writeline(LOG_LEVEL_TRACE, format, ## __VA_ARGS__)
#define INFO(module, format, ...)			CLogFactory::get_instance(module)->writeline(LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define WARNING(module, format, ...)		CLogFactory::get_instance(module)->writeline(LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define ERROR(module, format, ...)			CLogFactory::get_instance(module)->writeline(LOG_LEVEL_ERROR, format, ## __VA_ARGS__)

//��־����
enum _log_level
{
	LOG_LEVEL_ERROR		= 1,  //����
	LOG_LEVEL_WARNING	= 2,  //����
	LOG_LEVEL_INFO      = 4,  //��ͨ
	LOG_LEVEL_TRACE     = 8,  //����
	LOG_LEVEL_MAX
};

//�û���־��������
#define LOG_ERROR        LOG_LEVEL_ERROR
#define LOG_WARNING      (LOG_ERROR | LOG_LEVEL_WARNING)
#define LOG_INFO         (LOG_WARNING | LOG_LEVEL_INFO)
#define LOG_TRACE        (LOG_INFO | LOG_LEVEL_TRACE)

#ifndef __MY_LOCK__
#define __MY_LOCK__

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

class MutexLock
{
#ifdef WIN32
private:
	CRITICAL_SECTION m_lock;
public:
	MutexLock(void) {InitializeCriticalSection(&m_lock);}
	virtual ~MutexLock(void) {DeleteCriticalSection(&m_lock);}
	void lock() {EnterCriticalSection(&m_lock);}
	void unlock() {LeaveCriticalSection(&m_lock);}
#else
private:
	pthread_mutex_t m_lock;
public:
	MutexLock(void) {pthread_mutex_init(&m_lock, NULL);}
	virtual ~MutexLock(void) {pthread_mutex_destroy(&m_lock);}
	void lock() {pthread_mutex_lock(&m_lock);}
	void unlock() {pthread_mutex_unlock(&m_lock);}
#endif
};
#endif //__MY_LOCK__

typedef unsigned int  uint;
typedef unsigned long uint32;

#define MAX_TIME_STR_LEN	30

class CLog
{
public:
	CLog(void);
	~CLog(void);

	//������־�ļ�·��
	void set_log_filepath(const string filepath) {m_log_filename = filepath; mk_dir(); init();}

	//������־�ļ���С�л�
	void set_log_size(uint size) {m_log_size = size;}

	//������־����
	void set_log_level(uint level) {m_log_level = (level > LOG_LEVEL_MAX) ? LOG_LEVEL_MAX : level;}
	
	//д��һ����־
	int writeline(uint level, const char* format_str, ...);

private:
	void	init();									//��ʼ������
	bool	mk_dir();								//���Ŀ¼�Ƿ����,�����ڵĻ�ѭ������
	char*	get_time_str(bool is_write=true);		//��ȡ��ǰʱ���ַ���
	bool	rename_file();							//ȡ��ǰʱ����������־�ļ�

private:
	MutexLock	m_lock;								//ͬ����,���ڶ��߳�ͬ��д
	string		m_log_filename;						//��־�ļ�·����
	FILE*		m_fp;								//��־�ļ����
	uint		m_log_level;						//���õ���־����
	uint		m_log_size;							//���õ���־�ļ���С
	
	char		m_time_str[MAX_TIME_STR_LEN];		//д����
};

#define MAX_LOG_INSTANCE	10
typedef struct _log_instance
{
	char* name;
	CLog* plog;
}log_inst;

class CLogFactory
{
public:
	CLogFactory(void);
	virtual ~CLogFactory(void);

	static CLog* get_instance(const char* name);
	static void  free_instance(const char* name);

private:
	static void  free_all_inst();

private:
	static log_inst		m_inst_list[MAX_LOG_INSTANCE];	//logʵ���б�
	static uint			m_inst_num;						//logʵ������
	static MutexLock	m_lock;							//ͬ����,���ڶ��߳�ͬ��д
};

#endif // __Log_h__

