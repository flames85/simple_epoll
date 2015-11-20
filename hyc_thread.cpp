#include "thread.h"

#ifdef WIN32
#include "stdafx.h"  // win文件包含在预编译头文件里
#else
#include <sys/prctl.h>
#endif


HycThread::HycThread(const string &strThreadName):
	m_strThreadName(strThreadName),
#ifdef WIN32
	m_handle(0)
#else
m_id(0)
#endif
{
}

HycThread::~HycThread()
{
}

const string& HycThread::GetThreadName()
{
	return m_strThreadName;
}

void HycThread::SetThreadName(const string &strThreadName)
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

void HycThread::SetNameInternal(DWORD dwThreadID, const char* cThreadName)
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

unsigned __stdcall HycThread::s_ThreadProc(void* self)
{
	// self obj
	HycThread *_self = (HycThread*)self;

	// run proc
	return _self->ThreadProc();
}

HANDLE HycThread::StartThread()
{
	// 线程id
	DWORD nThreadId;

	// 启动线程
	m_handle = (HANDLE)_beginthreadex(NULL, 0, s_ThreadProc, (HycThread *)this, 0, (unsigned int*)&nThreadId);

	// 设置线程名
	SetNameInternal(nThreadId, m_strThreadName.c_str());

	return m_handle;
}

void HycThread::WaitThisThread()
{
	::WaitForSingleObject( m_handle, INFINITE );
}

#else // linux实现方式

void * __stdcall HycThread::s_ThreadProc(void* self)
{
	// self obj
	HycThread *_self = (HycThread*)self;

	// set thread name
	prctl(PR_SET_NAME, _self->m_strThreadName.c_str());

	// run proc
	_self->ThreadProc();

	return NULL;
}

pthread_t HycThread::StartThread()
{
	pthread_create(&m_id, NULL, s_ThreadProc, this);
	return m_id;
}
void HycThread::WaitThisThread()
{
	pthread_join(m_id,NULL);
}
#endif

