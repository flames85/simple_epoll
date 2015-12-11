// linux
#include <arpa/inet.h>
// pro
#include "hyc_master.h"
#include "hyc_master_mgr.h"
#include "hyc_server_slave.h"

int main()
{
    HycMaster *master = HycMasterMgr::GetInstance()->CreateMaster("MASTER");
    int tid = master->Start();

    cout << "tid:" << tid << endl;

    sleep(3);

    // 1. 向task发数据
    HycServerSlave *server = new HycServerSlave("SERVER");
    HycEvent event;
    event.type = EVENT_LINTEN;
    event.detail.listenDetail.slave = server;
    event.detail.listenDetail.s_addr = htonl(INADDR_ANY);
    event.detail.listenDetail.nPort = 12345;
    master->PostEvent(event);

    cout << "PostEvent:listen 12345" << endl;

    sleep(1000);

    return 0;
}
