// linux
#include <string.h>
// pro
#include "hyc_task.h"

HycTask::HycTask(const string &sName):m_sName(sName)
{
    memset(m_pipe_fd, 0x0, sizeof(m_pipe_fd));
}
HycTask::~HycTask()
{

}

void HycTask::TriggerNewConnection(int socket)
{
    cout << "HycTask::TriggerNewConnection:" << socket << endl;
}

void HycTask::TriggerReadReady(int socket)
{
    cout << "HycTask::TriggerReadReady:" << socket << endl;
}

void HycTask::TriggerTimeout(int nType)
{
    cout << "HycTask::TriggerTimeout:" << nType << endl;
}

void HycTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycTask::TriggerMessage:" << sData << " " << nLen << endl;
}

bool HycTask::PostEvent(const HycEvent &event)
{
    if(write(m_pipe_fd[1], &event, sizeof(event)) <= 0)
        return false;
    return true;
}

bool HycTask::Event(HycEvent &event)
{
    if(read(m_pipe_fd[0], &event, sizeof(event)) <= 0)
        return false;
    return true;
}
