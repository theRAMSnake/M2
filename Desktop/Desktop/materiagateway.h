#ifndef MATERIAGATEWAY_H
#define MATERIAGATEWAY_H

#include <QObject>
#include "MateriaClient.hpp"

class MateriaGateway : public QObject
{
    Q_OBJECT
public:
    MateriaGateway(const QString &ip, QObject *parent = nullptr);

signals:

public slots:

private:
    materia::MateriaClient mClient;
};

#endif // MATERIAGATEWAY_H
