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

   Wt::WTable* mTable;
   Wt::WFileUpload* mUpload;
};