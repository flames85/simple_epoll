#ifndef HYC_SLAVE_H
#define HYC_SLAVE_H

#include <iostream>
#include <set>
#include <string>
#include <errno.h>

using namespace std;

#define BUFF_SIZE 1024


class HycSlave;

enum HycEventType
{
    EVENT_STANDBY = 0,      //
    EVENT_LINTEN,           //  // test
    EVENT_RECEIVE,          //  // test
    EVENT_REMOVE,           //   // test
    EVENT_TIMER,            //
    EVENT_MESSAGE           //
};

// to group
struct StandbyDetail{
    HycSlave      *slave;
};
struct ListenDetail {
    HycSlave        *slave;
    unsigned long  s_addr; // ip
    short          nPort;
};
struct ReceiveDetail {
    HycSlave        *slave;
    int            socket;
};

struct RemoveDetail {
    char          sName[256];
};

// slave
struct TimerDetail {
    char          sName[256];

    int           nFlag;
    int           nInterval;
    bool          bRepeat;
};
struct MessageDetail {
    char          sName[256];

    char          sData[1024];
};

union EventDetail {
    StandbyDetail   standbydetail;
    ListenDetail    listenDetail;
    ReceiveDetail   receiveDetail;
    RemoveDetail    removeDetail;
    TimerDetail     timerDetail;
    MessageDetail   messageDetail;
};

struct HycEvent {
    HycEventType type;
    EventDetail  detail;
};

class HycSlave
{
public:
    HycSlave(const string &sName);
    virtual ~HycSlave();

    virtual void TriggerNewConnection(int socket, HycSlave **receiveSlave);
    virtual void TriggerReadReady();
    virtual void TriggerTimeout(int nType);
    virtual void TriggerMessage(char* sData);

protected:

public:

    string              m_sName;
    int                 m_socket;

private:

};


#endif
