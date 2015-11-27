// linux
#include <arpa/inet.h>
// pro
#include "hyc_task_master.h"
#include "hyc_server_task.h"
#include "hyc_normal_task.h"
#include "hyc_master_mgr.h"
#include "hyc_stl_tool.h"

HycServerTask::HycServerTask(const string &sName) :
    HycTask(sName),
    nCount(0)
{
}

HycServerTask::~HycServerTask()
{

}

void HycServerTask::TriggerNewConnection(int socket)
{
    string sTaskName = "task_" + STL::int2str(nCount++);

    cout << "HycNormalTask::TriggerNewConnection:" << m_sName << ":" << sTaskName << endl;

    HycNormalTask *task = new HycNormalTask(sTaskName); // todo

    HycTaskMaster *master = HycMasterMgr::GetInstance()->GetMaster("MASTER"); // todo

    HycEvent event;
    event.type = EVENT_RECEIVE;
    event.detail.receiveDetail.task = task;
    event.detail.receiveDetail.socket = socket;
    master->PostEvent(event);
}

void HycServerTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycServerTask::TriggerMessage:" << sData << " " << nLen << endl;
}
