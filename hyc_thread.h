#ifndef THREAD_H
#define THREAD_H

#include <string>
#include <iostream>

#ifdef WIN32

#else
#include <pthread.h>
#endif

using namespace std;

class CThread
{
	public:
		CThread(const string &strThreadName = "");
		virtual ~CThread();

		virtual unsigned          ThreadProc() = 0;
		const string&             GetThreadName();
		void                      SetThreadName(const string &strThreadName);
		void                      WaitThisThread();
#ifdef WIN32
		static unsigned __stdcall s_ThreadProc(void* self);
		HANDLE                    StartThread();
	private:
		void                      SetNameInternal(DWORD dwThreadID, const char* cThreadName);
	public:
		HANDLE                    m_handle;
#else
		static void *             s_ThreadProc(void* self);
		pthread_t                StartThread();
	public:
		pthread_t                m_id;
#endif
	private:
		string                   m_strThreadName;
};

#endif // THREAD_H

#include "stdafx.h"  // win文件包含在预编译头文件里
#include "thread.h"

#ifdef WIN32

#else
#include <sys/prctl.h>
#endif


CThread::CThread(const string &strThreadName):
	m_strThreadName(strThreadName),
#ifdef WIN32
	m_handle(0)
#else
m_id(0)
#endif
{
}

CThread::~CThread()
{
}

const string& CThread::GetThreadName()
{
	return m_strThreadName;
}

void CThread::SetThreadName(const string &strThreadName)
{
	m_strThreadName = strThreadName;
}

#ifdef WIN32 // win实现方式
const DWORD MS_VC_EXCEPTION = 0x406D1388;
// https://msdn.microsoft.com/zh-cn/library/xcb2z8hs.aspx
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void CThread::SetNameInternal(DWORD dwThreadID, const char* cThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = cThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

unsigned __stdcall CThread::s_ThreadProc(void* self)
{
	// self obj
	CThread *_self = (CThread*)self;

	// run proc
	return _self->ThreadProc();
}

HANDLE CThread::StartThread()
{
	// 线程id
	DWORD nThreadId;

	// 启动线程
	m_handle = (HANDLE)_beginthreadex(NULL, 0, s_ThreadProc, (CThread *)this, 0, (unsigned int*)&nThreadId);

	// 设置线程名
	SetNameInternal(nThreadId, m_strThreadName.c_str());

	return m_handle;
}

void CThread::WaitThisThread()
{
	::WaitForSingleObject( m_handle, INFINITE );
}

#else // linux实现方式

void * __stdcall CThread::s_ThreadProc(void* self)
{
	// self obj
	CThread *_self = (CThread*)self;

	// set thread name
	prctl(PR_SET_NAME, _self->m_strThreadName.c_str());

	// run proc
	_self->ThreadProc();

	return NULL;
}

pthread_t CThread::StartThread()
{
	pthread_create(&m_id, NULL, s_ThreadProc, this);
	return m_id;
}
void CThread::WaitThisThread()
{
	pthread_join(m_id,NULL);
}
#endif

