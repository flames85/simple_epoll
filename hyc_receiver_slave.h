#ifndef HYC_RECEIVER_SLAVE_H
#define HYC_RECEIVER_SLAVE_H

#include "hyc_slave.h"

class HycReceiverSlave : public HycSlave
{
public:
    HycReceiverSlave(const string &sName);
    virtual ~HycReceiverSlave();

    virtual void TriggerReadReady() ;
    virtual void TriggerTimeout(int nType);
    virtual void TriggerMessage(char* sData, int nLen);

private:

};

#endif // HYC_RECEIVER_SLAVE_H
