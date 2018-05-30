#include "iconmanager.h"

IconManager::IconManager()
{

}

std::shared_ptr<QPixmap> IconManager::get(const materia::Id &id)
{
    auto p = ("../Desktop/Icons/" + id.guid + ".png");
    return std::shared_ptr<QPixmap>(new QPixmap(QString(p.c_str())));
}
