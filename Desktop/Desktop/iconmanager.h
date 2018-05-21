#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QPixmap>
#include <MateriaEmu/materiaclient.h>

//Now use local storage, later use materia container with icons
class IconManager
{
public:
    IconManager();

    QPixmap* get(const materia::Id& id);
};

#endif // ICONMANAGER_H
