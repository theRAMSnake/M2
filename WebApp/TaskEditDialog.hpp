#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextEdit.h>
#include <Wt/WPushButton.h>
#include "GoalsComboBox.hpp"

class TaskEditDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const std::string&, const std::string&, const materia::Id&)> TOnOkCallback;
   TaskEditDialog(
      const std::string& title, 
      const std::string& desc, 
      const materia::Id& parentGoalId, 
      const std::vector<StrategyModel::Goal>& goals,
      TOnOkCallback cb
      )
   {
      setWidth(Wt::WLength("75%"));
      mTitle = new Wt::WLineEdit(title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      auto cmb = contents()->addWidget(std::make_unique<GoalComboBox>(goals));
      cmb->select(parentGoalId);

      mDesc = new Wt::WTextEdit();
      mDesc->setHeight(500);
      mDesc->setConfigurationSetting("branding", false);
      mDesc->setConfigurationSetting("elementpath", false);
      mDesc->setConfigurationSetting("Browser_spellcheck", true);
      mDesc->setConfigurationSetting("statusbar", false);
      mDesc->setConfigurationSetting("menubar", "edit format table");
      
      mDesc->setExtraPlugins("colorpicker, textcolor, searchreplace, table, lists");
      mDesc->setToolBar(0, "bold italic | link | forecolor backcolor | fontsizeselect | numlist bullist");
      mDesc->setText(desc);
      
      contents()->addWidget(std::unique_ptr<Wt::WTextEdit>(mDesc));

      auto ok = new Wt::WPushButton("Accept");
      ok->setDefault(true);
      ok->clicked().connect(std::bind([=]() {
        accept();
      }));
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));

      auto cancel = new Wt::WPushButton("Cancel");
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
      cancel->clicked().connect(this, &Wt::WDialog::reject);

      rejectWhenEscapePressed();

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
           cb(mTitle->text().narrow(), mDesc->text().narrow(), cmb->getSelectedGoalId());
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WTextEdit* mDesc;
};