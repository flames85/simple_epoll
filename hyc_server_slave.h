#ifndef HYC_SERVER_SLAVE_H
#define HYC_SERVER_SLAVE_H

// pro
#include "hyc_slave.h"

class HycServerSlave : public HycSlave
{
public:
    HycServerSlave(const string &sName);
    virtual ~HycServerSlave();

    virtual void TriggerNewConnection(int socket, HycSlave **receiveSlave) ;
    virtual void TriggerMessage(char* sData, int nLen);

private:

    int nCount;
};

#endif // HYC_SERVER_SLAVE_H
