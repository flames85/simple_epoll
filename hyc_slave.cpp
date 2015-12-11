// linux
#include <string.h>
// pro
#include "hyc_slave.h"

HycSlave::HycSlave(const string &sName):m_sName(sName)
{

}
HycSlave::~HycSlave()
{

}

void HycSlave::TriggerNewConnection(int socket, HycSlave **receiveSlave)
{
    cout << "HycSlave::TriggerNewConnection:" << socket << endl;
}

void HycSlave::TriggerReadReady()
{
    cout << "HycSlave::TriggerReadReady:" << m_socket << endl;
}

void HycSlave::TriggerTimeout(int nType)
{
    cout << "HycSlave::TriggerTimeout:" << nType << endl;
}

void HycSlave::TriggerMessage(char* sData)
{
    cout << "HycSlave::TriggerMessage:" << sData << endl;
}


