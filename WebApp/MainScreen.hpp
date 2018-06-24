#pragma once

#include <Wt/WContainerWidget.h>
#include <Client/MateriaClient.hpp>

class MainScreen : public Wt::WContainerWidget
{
public:
   MainScreen(materia::MateriaClient& client);
};