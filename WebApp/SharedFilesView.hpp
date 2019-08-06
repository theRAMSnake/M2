#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <boost/filesystem.hpp>

class SharedFilesView : public Wt::WContainerWidget
{
public:
   SharedFilesView();

private:

   void loadSharedFilesList();

   Wt::WTable* mTable;
   std::set<boost::filesystem::path> mSharedFiles;
};