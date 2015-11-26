#include "hyc_normal_task.h"

HycNormalTask::HycNormalTask()
{
}

void HycNormalTask::TriggerReadReady(int socket)
{
    cout << "HycNormalTask::TriggerReadReady" << endl;

    char buf[BUFSIZ] = {0};
    int nTotal = 0;
    int nRead = 0;
    while ((nRead = read(socket, buf + nTotal, BUFSIZ-1)) > 0)
    {
        nTotal += nRead;
    }
    if (nRead == -1 && errno != EAGAIN)
    {
        perror("read error");
    }
}

void HycNormalTask::TriggerTimeout(int nType)
{
    cout << "HycNormalTask::TriggerTimeout" << endl;
}

void HycNormalTask::TriggerMessage(char* sData, int nLen)
{
    cout << "HycNormalTask::TriggerMessage:" << sData << " " << nLen << endl;
}
