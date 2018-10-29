#pragma once

#include <vector>
#include <functional>
#include <Wt/WDialog.h>

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

class CommonDialogManager
{  
public:
   typedef std::function<void(std::vector<std::string>&)> TCallback;
   static void showDialogSimple(const std::string& caption, const std::vector<std::string>& fields, TCallback& callback);
   static void showDialog(const std::string& caption, const std::vector<FieldInfo>& fields, TCallback& callback); 
   static void showOneLineDialog(const std::string& caption, const std::string & fieldName, const std::string& defaultValue, std::function<void(std::string)>& callback);
   static void showConfirmationDialog(const std::string& text, std::function<void(void)>& callback);
   static void showMessage(const std::string& text);
   static void showLinesDialog(const std::vector<Wt::WString>& lines, std::function<void(const std::vector<Wt::WString>&)> callback);
   static void showChoiseDialog(const std::vector<std::string>& options, std::function<void(const std::size_t&)> callback);
   
private:
   static Wt::WDialog* createDialog(const std::string& caption, const std::vector<FieldInfo>& fields, TCallback& callback);
};
