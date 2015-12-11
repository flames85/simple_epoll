
// pro
#include "hyc_master_mgr.h"
#include "hyc_master.h"

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

void HycMasterMgr::PostEvent(const HycEvent &event)
{
    HycMaster *master = NULL;
    map<string, HycMaster*>::iterator it = m_allMaster.begin();
    for(; it != m_allMaster.end(); it++)
    {
        master = it->second;

        if(!master->PostEvent(event))
        {
            cout << "master:" << master->m_sName << "post event error" << endl;
        }

    }
}

HycMaster* HycMasterMgr::CreateMaster(const string &sName)
{
    HycMaster *master = NULL;

    master = GetMaster(sName);

    if(!master)
    {
        master = new HycMaster(sName);
        m_allMaster[sName] = master;
    }
    return master;
}

HycMaster* HycMasterMgr::GetMaster(const string &sMaster)
{
    HycMaster *master = NULL;

    map<string, HycMaster*>::iterator it = m_allMaster.find(sMaster);

    if(it != m_allMaster.end())
    {
        master = it->second;
    }

    return master;
}
