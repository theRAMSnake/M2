#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WFileUpload.h>
#include <Wt/WTable.h>
#include <boost/filesystem.hpp>

class FilesView : public Wt::WContainerWidget
{
public:
   FilesView();

private:
   void createNewFileUpload();
   void addRow(int row, const boost::filesystem::path p);
   void onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const boost::filesystem::path path);
   void onChecked(const boost::filesystem::path path);
   void onUnChecked(const boost::filesystem::path path);

   void loadSharedFilesList();
   void saveSharedFilesList();

   Wt::WTable* mTable;
   Wt::WFileUpload* mUpload;
   std::set<boost::filesystem::path> mSharedFiles;
};