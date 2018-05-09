#include "materiagateway.h"

MateriaGateway::MateriaGateway(const QString &ip, QObject *parent)
    : QObject(parent)
    , mClient("Desktop", ip.toStdString())
{

}
