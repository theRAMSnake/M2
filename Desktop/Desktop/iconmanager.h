#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QPixmap>
#include <memory>
#include <MateriaEmu/materiaclient.h>

//Now use local storage, later use materia container with icons
class IconManager
{
public:
    IconManager();

    std::shared_ptr<QPixmap> get(const materia::Id& id);
};

#endif // ICONMANAGER_H
