#include "SharedFilesView.hpp"
#include <boost/filesystem.hpp>
#include <Wt/WPushButton.h>
#include <Wt/WGroupBox.h>
#include <Wt/WFont.h>
#include <Wt/WTable.h>
#include <Wt/WFileResource.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WFileUpload.h>
#include <Wt/WCheckBox.h>
#include <Wt/WProgressBar.h>
#include "dialog/CommonDialogManager.hpp"

void SharedFilesView::loadSharedFilesList()
{
   std::ifstream d_file("/materia/shared_files_list.txt", std::ios_base::in);
   std::string line;

   while( std::getline( d_file, line ) ) 
   {
      mSharedFiles.insert( line );
   }
}

SharedFilesView::SharedFilesView()
{
   setMargin(10);

   loadSharedFilesList();

   auto gb = new Wt::WGroupBox();
   addWidget(std::unique_ptr<Wt::WGroupBox>(gb));
   
   mTable = new Wt::WTable();
   mTable->setWidth(Wt::WLength("100%"));
   mTable->addStyleClass("table-bordered");
   mTable->addStyleClass("table-hover");
   mTable->addStyleClass("table-striped");
   gb->addWidget(std::unique_ptr<Wt::WTable>(mTable));

   for(auto f : mSharedFiles)
   {
      auto cell = mTable->elementAt(mTable->rowCount(), 0);

      auto a = new Wt::WAnchor();
      a->setText(f.filename().string());
      auto resource = std::make_shared<Wt::WFileResource>(f.string());
      resource->suggestFileName(f.filename().string());
      a->setLink(Wt::WLink(resource));

      cell->addWidget(std::unique_ptr<Wt::WAnchor>(a));
   }
}