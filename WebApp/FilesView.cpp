#include "FilesView.hpp"
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

void FilesView::addRow(int row, const boost::filesystem::path p)
{
   {
      auto cell = mTable->elementAt(row, 0);

      auto cb = new Wt::WCheckBox("");
      cb->setChecked(mSharedFiles.end() != mSharedFiles.find(p));
      cell->addWidget(std::unique_ptr<Wt::WCheckBox>(cb));
      cb->checked().connect(std::bind(&FilesView::onChecked, this, p));
      cb->unChecked().connect(std::bind(&FilesView::onUnChecked, this, p));

      auto a = new Wt::WAnchor();
      a->setText(p.filename().string());
      auto resource = std::make_shared<Wt::WFileResource>(p.string());
      resource->suggestFileName(p.filename().string());
      a->setLink(Wt::WLink(resource));

      cell->addWidget(std::unique_ptr<Wt::WAnchor>(a));

      cell->clicked().connect(std::bind(&FilesView::onClick, this, std::placeholders::_1, cell, p));
   }
}

void FilesView::loadSharedFilesList()
{
   std::ifstream d_file("/materia/shared_files_list.txt", std::ios_base::in);
   std::string line;

   while( std::getline( d_file, line ) ) 
   {
      mSharedFiles.insert( line );
   }
}

void FilesView::saveSharedFilesList()
{
   std::ofstream file("/materia/shared_files_list.txt", std::ios_base::trunc | std::ios_base::out);
   for(auto p : mSharedFiles)
   {
      file << p.string() << std::endl;
   }
}

void FilesView::onChecked(const boost::filesystem::path path)
{
   mSharedFiles.insert(path);
   saveSharedFilesList();
}

void FilesView::onUnChecked(const boost::filesystem::path path)
{
   mSharedFiles.erase(path);
   saveSharedFilesList();
}

FilesView::FilesView()
: mUpload(nullptr)
{
   setMargin(10);

   loadSharedFilesList();

   Wt::WPushButton *uploadButton = addNew<Wt::WPushButton>("Upload");
   uploadButton->setMargin(10, Wt::Side::Top);
   uploadButton->addStyleClass("btn-primary");

   auto gb = new Wt::WGroupBox();
   addWidget(std::unique_ptr<Wt::WGroupBox>(gb));
   
   mTable = new Wt::WTable();
   mTable->setWidth(Wt::WLength("100%"));
   mTable->addStyleClass("table-bordered");
   mTable->addStyleClass("table-hover");
   mTable->addStyleClass("table-striped");
   gb->addWidget(std::unique_ptr<Wt::WTable>(mTable));

   std::vector<boost::filesystem::path> files;

   boost::filesystem::path p("/materia/files");
   boost::filesystem::directory_iterator end_itr;

   for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
   {
      if (boost::filesystem::is_regular_file(itr->path())) 
      {
         files.push_back(itr->path());
      }
   }

   for(std::size_t i = 0; i < files.size(); ++i)
   {
      addRow(i, files[i]);
   }

   uploadButton->clicked().connect([=] {
      mUpload->upload();
   });

   createNewFileUpload();
}

void FilesView::createNewFileUpload()
{
   if(mUpload != nullptr)
   {
      removeChild(mUpload);
   }

   mUpload = addNew<Wt::WFileUpload>();
   mUpload->setFileTextSize(50);
   mUpload->setProgressBar(Wt::cpp14::make_unique<Wt::WProgressBar>());
   mUpload->setMargin(10, Wt::Side::Right);

   mUpload->uploaded().connect([=] {
      auto src = boost::filesystem::path(mUpload->spoolFileName());
      auto dest = boost::filesystem::path("/materia/files") / boost::filesystem::path(mUpload->clientFileName().narrow()).filename();

      try
      {
         boost::filesystem::copy(src, dest);
         addRow(mTable->rowCount(), dest);
      }
      catch(...)
      {
         CommonDialogManager::showMessage("An error occured.");
      }

      createNewFileUpload();
   });

   mUpload->fileTooLarge().connect([=] {
      CommonDialogManager::showMessage("File is too large.");
   });
}

void FilesView::onClick(Wt::WMouseEvent ev, Wt::WTableCell* cell, const boost::filesystem::path path)
{
   if(ev.modifiers().test(Wt::KeyboardModifier::Control))
   {
      std::function<void()> elementDeletedFunc = [=] () {
         mTable->removeRow(cell->row());
         boost::filesystem::remove(path);
         };

      CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
   }
}
