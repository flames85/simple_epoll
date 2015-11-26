#ifndef HYC_SERVER_TASK_H
#define HYC_SERVER_TASK_H

#include "hyc_task.h"

class HycServerTask : public HycTask
{
public:
    HycServerTask();

    virtual void TriggerNewConnection(int socket) ;
    virtual void TriggerTimeout(int nType);
};

#endif // HYC_SERVER_TASK_H
