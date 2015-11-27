#ifndef HYC_MASTER_MGR_H
#define HYC_MASTER_MGR_H

// stl
#include <string>
#include <map>

using namespace std;

class HycTaskMaster;

class HycMasterMgr
{
public:
    HycMasterMgr();

    static HycMasterMgr* GetInstance();

    HycTaskMaster* CreateMaster(const string &sName);

    HycTaskMaster* GetMaster(const string &sName);

private:

    map<string, HycTaskMaster*>     m_allMaster;

    static HycMasterMgr             *m_Instance;
};

#endif // HYC_MASTER_MGR_H
