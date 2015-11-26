// linux
#include <arpa/inet.h>
// pro
#include "hyc_server_task.h"
#include "hyc_normal_task.h"

HycServerTask::HycServerTask()
{
    master = Mgr->getMaster();

    // 1. 先向master注册
    HycEvent event;
    event.type = EVENT_REGISTER;
    event.detail.registerdetail.task = this;
    master->PostEvent(event);

    // 2. 再向task发数据
    event.type = EVENT_LINTEN;
    event.detail.listenDetail.s_addr = htonl(INADDR_ANY);
    event.detail.listenDetail.nPort = htons(1234);
    this->PostEvent(event);
}

void HycServerTask::TriggerNewConnection(int socket)
{
    cout << "HycNormalTask::TriggerNewConnection" << endl;

    HycNormalTask *task = new HycNormalTask;

    master = Mgr->getMaster();

    // 1. 先向master注册
    HycEvent event;
    event.type = EVENT_REGISTER;
    event.detail.registerdetail.task = task;
    master->PostEvent(event);

    // 2. 再向task发数据
    event.type = EVENT_RECEIVE;
    event.detail.receiveDetail.socket = socket;
    task->PostEvent(event);
}

void HycServerTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycServerTask::TriggerMessage:" << sData << " " << nLen << endl;
}
