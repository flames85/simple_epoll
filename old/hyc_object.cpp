// linux
#include <unistd.h>

// pro
#include "hyc_object.h"

HycObject::HycObject(const string &sName) : m_father(NULL)
{
    m_sName = sName;
}

bool HycObject::AddChild(HycObject* obj)
{
    m_children.insert(obj);
}

bool HycObject::SetFather(HycObject* obj)
{
    m_father = obj;
    if((ret = pipe(m_pipe_fd)) <0)
    {
        cout << "create pipe fail: " << ret << ",errno:" << errno <<endl;
        return false;
    }
    return true;
}

bool HycObject::FindChild(const string &sName, HycObject &obj)
{
    bool bExists = false;
    set<HycObject*>::iterator it = m_children.begin();
    for(; it != m_children.end(); it++) {
        if (it->sName == sName) {
            obj = *it;
            bExists = true;
        }
    }
    return bExists;
}
