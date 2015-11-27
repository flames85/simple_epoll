#ifndef HYC_NORMAL_TASK_H
#define HYC_NORMAL_TASK_H

#include "hyc_task.h"

class HycNormalTask : public HycTask
{
public:
    HycNormalTask(const string &sName);
    virtual ~HycNormalTask();

    virtual void TriggerReadReady(int socket) ;
    virtual void TriggerTimeout(int nType);
    virtual void TriggerMessage(char* sData, int nLen);

private:

};

#endif // HYC_NORMAL_TASK_H
