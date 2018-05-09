#ifndef MATERIAGATEWAYTHREAD_H
#define MATERIAGATEWAYTHREAD_H

#include <QObject>
#include <QThread>

class MateriaGatewayThread : public QThread
{
    Q_OBJECT

public:
    MateriaGatewayThread(const QString &ip, const QString &port);

protected:
    void run() override;

private:
    QString mIp;
    QString mPort;
};

#endif // MATERIAGATEWAYTHREAD_H
