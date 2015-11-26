#ifndef HYC_OBJECT_H
#define HYC_OBJECT_H

// stl
#include <string>
#include <set>

using namespace std;

class HycObject
{
public:
    HycObject(const string &sName = "");
    bool AddChild(HycObject* obj);  // 用于清理
    bool SetFather(HycObject* obj); // 用于找到obj
    bool FindChild(const string &sName, HycObject &obj);


public:


protected:
    string              m_sName;
    HycObject           *m_father;
};

#endif // HYC_OBJECT_H
