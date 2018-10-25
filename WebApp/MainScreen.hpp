#pragma once

#include <Wt/WContainerWidget.h>
#include "MateriaClient.hpp"

class MainScreen : public Wt::WContainerWidget
{
public:
   MainScreen(MateriaClient& materiaClient);
};