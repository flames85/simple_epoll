#include "hyc_server_task.h"
#include "hyc_normal_task.h"

HycServerTask::HycServerTask()
{
}

void HycServerTask::TriggerNewConnection(int socket)
{
    cout << "HycNormalTask::TriggerNewConnection" << endl;
}

void HycServerTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycServerTask::TriggerMessage:" << sData << " " << nLen << endl;
}
