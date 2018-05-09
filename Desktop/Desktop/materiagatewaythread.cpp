#include "materiagatewaythread.h"

MateriaGatewayThread::MateriaGatewayThread(const QString &ip, const QString &port)
    : mIp(ip)
    , mPort(port)
{

}

void MateriaGatewayThread::run()
{
    exec();
}
