#ifndef MATERIAPROXY_H
#define MATERIAPROXY_H

#include <QObject>
#include <QThread>

class MateriaGateway;

class MateriaProxy : public QObject
{
    Q_OBJECT
public:
    explicit MateriaProxy(const QString& ip, const QString& port, QObject *parent = nullptr);

signals:

public slots:

private:
    QThread* mMateriaGatewayThread;
    MateriaGateway* mMateriaGateway;
};

#endif // MATERIAPROXY_H
