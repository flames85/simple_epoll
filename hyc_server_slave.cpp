// linux
#include <arpa/inet.h>
#include <string.h>
// pro
#include "hyc_master.h"
#include "hyc_server_slave.h"
#include "hyc_receiver_slave.h"
#include "hyc_master_mgr.h"
#include "hyc_stl_tool.h"

HycServerSlave::HycServerSlave(const string &sName) :
    HycSlave(sName),
    nCount(0)
{
}

HycServerSlave::~HycServerSlave()
{

}

void HycServerSlave::TriggerNewConnection(int socket, HycSlave **receiveSlave)
{
    string sSlaveName = "slave_" + STL::int2str(nCount++);

    cout << "HycServerSlave::TriggerNewConnection:" << m_sName << ":" << sSlaveName << endl;

    HycReceiverSlave *slave = new HycReceiverSlave(sSlaveName); // todo

    *receiveSlave = slave;

    // 以下代码是发消息,存在延迟

    HycMaster *master = HycMasterMgr::GetInstance()->GetMaster("MASTER"); // todo
    HycEvent event;
    event.type = EVENT_TIMER;

    strncpy(event.detail.timerDetail.sName, sSlaveName.c_str(), sizeof(event.detail.timerDetail.sName)-1);
    event.detail.timerDetail.nInterval = 30;
    event.detail.timerDetail.nFlag = 30;
    event.detail.timerDetail.bRepeat = true;
    master->PostEvent(event);

    strncpy(event.detail.timerDetail.sName, sSlaveName.c_str(), sizeof(event.detail.timerDetail.sName)-1);
    event.detail.timerDetail.nFlag = 10;
    event.detail.timerDetail.nInterval = 10;
    event.detail.timerDetail.bRepeat = true;
    master->PostEvent(event);
}

void HycServerSlave::TriggerMessage(char* sData, int nLen)
{
    cout << "HycServerSlave::TriggerMessage:" << sData << " " << nLen << endl;
}
