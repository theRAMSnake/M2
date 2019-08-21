#pragma once

#include <vector>
#include <functional>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/format.hpp>
#include <Wt/WDialog.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WDateEdit.h>
#include "../WtConverters.hpp"

enum class FieldType
{
   String,
   Time
};

struct FieldInfo
{
    std::string name;
    std::string initialValue;
    FieldType fieldType;
};

class BasicDialog : public Wt::WDialog
{
public:
    BasicDialog(const std::string& caption, const bool supressEnter = false);

    virtual void onAccepted();
    virtual bool verify();
};

template<class T>
class CustomDialog : public BasicDialog
{
public:
   CustomDialog(const std::string& title, const T& initialValue)
   : BasicDialog(title)
   , mVal(initialValue)
   {

   }

   void onAccepted() override
   {

   }

   template<class Y, class Getter, class Assigner>
   void addComboBox(
      const std::string& caption, 
      const std::vector<Y>& values, 
      const typename std::vector<Y>::iterator init,
      const Getter textGetter,
      const Assigner assigner)
   {
      contents()->addNew<Wt::WLabel>(caption);
      auto combo = contents()->addNew<Wt::WComboBox>();

      for(auto iter = values.begin(); iter != values.end(); ++iter)
      {
         combo->addItem(textGetter(*iter));
         if(iter == init)
         {
            combo->setCurrentIndex(combo->count() - 1);
         }
      }  

      combo->changed().connect([=]()
      {
         assigner(mVal, values[combo->currentIndex()]);
      });
   }

   void addLineEdit(const std::string& caption, std::string T::* field)
   {
      contents()->addNew<Wt::WLabel>(caption);
      auto ctrl = contents()->addNew<Wt::WLineEdit>();

      ctrl->setText(mVal.*field);

      ctrl->changed().connect([=]()
      {
         mVal.*field = ctrl->text().narrow();
      });
   }

   void addCurrencyEdit(const std::string& caption, unsigned int T::* field)
   {
      contents()->addNew<Wt::WLabel>(caption);
      auto ctrl = contents()->addNew<Wt::WLineEdit>();

      ctrl->setText(boost::str(boost::format("%1%.%+2d%") % std::to_string(mVal.*field / 100) % std::to_string(mVal.*field % 100)));

      ctrl->changed().connect([=]()
      {
         auto str = ctrl->text().narrow();
         std::istringstream iss (str);

         unsigned int euro = 0;
         unsigned int cents = 0;
         char dot;
         iss >> euro >> dot >> cents;

         mVal.*field = (euro * 100 + cents);
      });
   }

   void addDateEdit(const std::string& caption, std::time_t T::* field, const std::time_t init)
   {
      contents()->addNew<Wt::WLabel>(caption);
      auto ctrl = contents()->addWidget(Wt::cpp14::make_unique<Wt::WDateEdit>());
      ctrl->setDate(timestampToWtDate(init));
      ctrl->setWidth(Wt::WLength("15%"));

      mVal.*field = init;

      ctrl->changed().connect([=]()
      {
         mVal.*field = WtDateToTimeStamp(ctrl->date());
      });
   }

private:
   T mVal;
};

class CommonDialogManager
{  
public:
   typedef std::function<void(std::vector<std::string>&)> TCallback;
   static void showDialogSimple(const std::string& caption, const std::vector<std::string>& fields, TCallback& callback);
   static void showDialog(const std::string& caption, const std::vector<FieldInfo>& fields, TCallback& callback); 
   static void showOneLineDialog(const std::string& caption, const std::string & fieldName, const std::string& defaultValue, std::function<void(std::string)>& callback);
   static void queryNumber(const int initialValue, std::function<void(int)>& callback);
   static void queryDate(const boost::gregorian::date initialValue, std::function<void(boost::gregorian::date)>& callback);
   static void showConfirmationDialog(const std::string& text, std::function<void(void)>& callback);
   static void showMessage(const std::string& text);
   static void showLinesDialog(const std::vector<Wt::WString>& lines, std::function<void(const std::vector<Wt::WString>&)> callback);
   static void showChoiseDialog(const std::vector<std::string>& options, std::function<void(const std::size_t&)> callback);
   static void showDoubleComboDialog(
      const std::vector<std::string>& options, 
      const std::vector<std::string>& options2, 
      std::function<void(const std::size_t&, const std::size_t&)> callback
      );

   template<class T>
   static CustomDialog<T>* createCustomDialog(const std::string& title, const T& initialValue)
   {
      return new CustomDialog<T>(title, initialValue);
   }
   
private:
   static Wt::WDialog* createDialog(const std::string& caption, const std::vector<FieldInfo>& fields, TCallback& callback);
   static Wt::WDialog* createDialogBase(const std::string& caption);
};
