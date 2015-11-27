// linux
#include <stdio.h> // for perror

// pro
#include "hyc_normal_task.h"


HycNormalTask::HycNormalTask(const string &sName) : HycTask(sName)
{
}

HycNormalTask::~HycNormalTask()
{

}

void HycNormalTask::TriggerReadReady(int socket)
{
    cout << "HycNormalTask::TriggerReadReady" << endl;

    char buf[BUFF_SIZE] = {0};
    int nTotal = 0;
    int nRead = 0;
    while ((nRead = read(socket, buf + nTotal, BUFF_SIZE-1)) > 0)
    {
        nTotal += nRead;
    }
    if (nRead == -1 && errno != EAGAIN)
    {
        perror("read error");
        HycEvent event;
        event.type = EVENT_REMOVE;
        this->PostEvent(event);
    }
//    if (nRead == 0)
//    {
//        HycEvent event;
//        event.type = EVENT_REMOVE;
//        this->PostEvent(event);
//    }
}

void HycNormalTask::TriggerTimeout(int nType)
{
    cout << "HycNormalTask::TriggerTimeout" << endl;
}

void HycNormalTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycNormalTask::TriggerMessage:" << sData << " " << nLen << endl;
}
