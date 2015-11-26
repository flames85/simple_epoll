#ifndef HYC_THREAD_H
#define HYC_THREAD_H

#include <string>
#include <iostream>

#ifdef WIN32

#else
#include <pthread.h>
#endif

using namespace std;

class HycThread
{
public:
    HycThread(const string &strThreadName = "");
    virtual ~HycThread();

    virtual int               ThreadProc() = 0;
    const string&             GetThreadName();
    void                      SetThreadName(const string &strThreadName);
    void                      WaitThisThread();
#ifdef WIN32
    static unsigned __stdcall s_ThreadProc(void* self);
    HANDLE                    StartThread();
private:
    void                      SetNameInternal(DWORD dwThreadID, const char*     HycThreadName);
public:
    HANDLE                    m_handle;
#else
    static void * __stdcall  s_ThreadProc(void* self);
    pthread_t                StartThread();
public:
    pthread_t                m_id;
#endif
private:
    string                   m_strThreadName;
};

#endif // HYC_THREAD_H

