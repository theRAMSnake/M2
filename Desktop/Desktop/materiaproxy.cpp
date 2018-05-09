#include "materiaproxy.h"
#include "materiagatewaythread.h"
#include "materiagateway.h"

MateriaProxy::MateriaProxy(const QString &ip, const QString &port, QObject *parent)
: QObject(parent)
, mMateriaGatewayThread(new MateriaGatewayThread(ip, port))
{
    mMateriaGateway = new MateriaGateway(ip, port);

    //connect here

    mMateriaGatewayThread->start();

    mMateriaGateway->moveToThread(mMateriaGatewayThread);
}
