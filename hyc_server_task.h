#ifndef HYC_SERVER_TASK_H
#define HYC_SERVER_TASK_H

// pro
#include "hyc_task.h"

class HycServerTask : public HycTask
{
public:
    HycServerTask(const string &sName);
    virtual ~HycServerTask();

    virtual void TriggerNewConnection(int socket) ;
    virtual void TriggerMessage(char* sData, int nLen);

private:

    int nCount;
};

#endif // HYC_SERVER_TASK_H
