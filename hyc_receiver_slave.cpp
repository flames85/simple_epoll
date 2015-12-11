// linux
#include <stdio.h> // for perror
#include <string.h>

// pro
#include "hyc_master_mgr.h"
#include "hyc_receiver_slave.h"

HycReceiverSlave::HycReceiverSlave(const string &sName) : HycSlave(sName)
{
}

HycReceiverSlave::~HycReceiverSlave()
{
    cout << "HycReceiverSlave delete:" << m_sName << endl;
}

void HycReceiverSlave::TriggerReadReady()
{
    char buf[BUFF_SIZE] = {0};
    int nTotal = 0;
    int nRead = 0;
    while ((nRead = read(m_socket, buf + nTotal, BUFF_SIZE-1)) > 0)
    {
        nTotal += nRead;
    }
    if (nRead == -1 && errno != EAGAIN)
    {
        perror("read error");
        HycEvent event;
        event.type = EVENT_REMOVE;
        memset(event.detail.removeDetail.sName, 0x0, sizeof(event.detail.removeDetail.sName));
        memcpy(event.detail.removeDetail.sName, m_sName.c_str(), sizeof(event.detail.removeDetail.sName) - 1);
        HycMasterMgr::GetInstance()->PostEvent(event);
    }

    if (nRead == 0)
    {
        cout << "HycReceiverSlave::socket-close:" << m_sName << endl;
        HycEvent event;
        event.type = EVENT_REMOVE;
        memset(event.detail.removeDetail.sName, 0x0, sizeof(event.detail.removeDetail.sName));
        memcpy(event.detail.removeDetail.sName, m_sName.c_str(), sizeof(event.detail.removeDetail.sName) - 1);
        HycMasterMgr::GetInstance()->PostEvent(event);
        return;
    }

    cout << "HycReceiverSlave::TriggerReadReady:"<< m_sName << ":"<< buf << endl;
}

void HycReceiverSlave::TriggerTimeout(int nType)
{
    cout << "HycReceiverSlave::TriggerTimeout:" << m_sName << ":" << nType << endl;
}

void HycReceiverSlave::TriggerMessage(char* sData, int nLen)
{
    cout << "HycReceiverSlave::TriggerMessage:" << m_sName << ":"<< sData << " " << nLen << endl;
}
