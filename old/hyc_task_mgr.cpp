#include "hyc_task_mgr.h"

HycTaskMgr::HycTaskMgr()
{
}



bool HycTaskMgr::AddTask(const string &sName, const HycTask *task)
{
    m_taskMap[sName] = task;
}

HycTask* HycTaskMgr::GetTask(const string &sName)
{
    return m_taskMap[sName];
}
