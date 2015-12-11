#ifndef HYC_MASTER_MGR_H
#define HYC_MASTER_MGR_H

// stl
#include <string>
#include <map>

// pro
#include "hyc_master.h"

using namespace std;

class HycSlave;
class HycMaster;

class HycMasterMgr
{
public:
    HycMasterMgr();

    static HycMasterMgr* GetInstance();

    void PostEvent(const HycEvent &event);

    HycMaster* CreateMaster(const string &sName);

    HycMaster* GetMaster(const string &sMaster);


private:

    map<string, HycMaster*>         m_allMaster;

    static HycMasterMgr             *m_Instance;
};

#endif // HYC_MASTER_MGR_H
