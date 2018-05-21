#include "iconmanager.h"

IconManager::IconManager()
{

}

QPixmap *IconManager::get(const materia::Id &id)
{
    auto p = ("../Desktop/Icons/" + id.guid + ".png");
    return new QPixmap(QString(p.c_str()));
}
