#ifndef HYC_NORMAL_TASK_H
#define HYC_NORMAL_TASK_H

#include "hyc_task.h"

class HycNormalTask : public HycTask
{
public:
    HycNormalTask();

    virtual void TriggerNewConnection(int socket) ;
    virtual void TriggerReadReady(int socket) ;
    virtual void TriggerTimeout(int nType);
    virtual void TriggerMessage(char* data, int len);
};

#endif // HYC_NORMAL_TASK_H
