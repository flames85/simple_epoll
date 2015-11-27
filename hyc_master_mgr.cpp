
// pro
#include "hyc_master_mgr.h"
#include "hyc_task_master.h"

HycMasterMgr::HycMasterMgr()
{

}

HycMasterMgr* HycMasterMgr::m_Instance = NULL;

HycMasterMgr* HycMasterMgr::GetInstance(){
    if(!m_Instance)
    {
        m_Instance = new HycMasterMgr();
    }
    return m_Instance;
}

HycTaskMaster* HycMasterMgr::CreateMaster(const string &sName)
{
    HycTaskMaster *master = NULL;

    master = GetMaster(sName);

    if(!master)
    {
        master = new HycTaskMaster(sName);
        m_allMaster[sName] = master;
    }
    return master;
}

HycTaskMaster* HycMasterMgr::GetMaster(const string &sName)
{
    HycTaskMaster *master = NULL;

    map<string, HycTaskMaster*>::iterator it = m_allMaster.find(sName);

    if(it != m_allMaster.end())
    {
        master = it->second;
    }

    return master;
}

