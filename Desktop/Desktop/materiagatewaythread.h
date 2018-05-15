#ifndef MATERIAGATEWAYTHREAD_H
#define MATERIAGATEWAYTHREAD_H

#include <QObject>
#include <QThread>

class MateriaGatewayThread : public QThread
{
    Q_OBJECT

public:

protected:
    void run() override;

private:
};

#endif // MATERIAGATEWAYTHREAD_H
