// linux
#include <stdio.h> // for perror

// pro
#include "hyc_normal_task.h"


HycNormalTask::HycNormalTask(const string &sName) : HycTask(sName)
{
}

HycNormalTask::~HycNormalTask()
{
    cout << "task delete:" << m_sName << endl;
}

void HycNormalTask::TriggerReadReady(int socket)
{
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

    if (nRead == 0)
    {
        cout << "HycNormalTask::socket-close:" << m_sName << endl;
        HycEvent event;
        event.type = EVENT_REMOVE;
        this->PostEvent(event);
        return;
    }

    cout << "HycNormalTask::TriggerReadReady:"<< m_sName << ":"<< buf << endl;
}

void HycNormalTask::TriggerTimeout(int nType)
{
    cout << "HycNormalTask::TriggerTimeout:" << m_sName << ":" << nType << endl;
}

void HycNormalTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycNormalTask::TriggerMessage:" << m_sName << ":"<< sData << " " << nLen << endl;
}
