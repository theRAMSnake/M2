#include "CommonDialogManager.hpp"
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLabel.h>
#include <Wt/WPushButton.h>
#include <Wt/WMessageBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WComboBox.h>
#include <boost/range/algorithm/find.hpp>
#include <boost/algorithm/string.hpp>

void CommonDialogManager::showDialogSimple(const std::string& caption, const std::vector<std::string>& fields, TCallback& callback)
{
    std::vector<FieldInfo> infos;
    for (auto a : fields)
    {
        infos.push_back(FieldInfo{ a });
    }

    showDialog(caption, infos, callback);
}

void CommonDialogManager::showOneLineDialog(const std::string & caption, const std::string & fieldName, const std::string & defaultValue, std::function<void(std::string)>& callback)
{
    TCallback functor = [=](std::vector<std::string> a)->void { callback(a[0]); };
    showDialog(caption, {{fieldName, defaultValue}}, functor);
}

void CommonDialogManager::queryNumber(const int initialValue, std::function<void(int)>& callback)
{
    TCallback functor = [=](std::vector<std::string> a)->void 
    {
        int val = initialValue;
        try
        {
            val = std::stoi(a[0]);
        } 
        catch(std::invalid_argument&)
        {

        }

        callback(val); 
    };
    showDialog("Choose number", {{"Number", std::to_string(initialValue)}}, functor);
}

void CommonDialogManager::showConfirmationDialog(const std::string & text, std::function<void(void)>& callback)
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
        ("Confirm",
            "<p>" + text + "</p>",
            Wt::Icon::Question, Wt::StandardButton::Yes | Wt::StandardButton::No);

    messageBox->buttonClicked().connect(std::bind([=]() {
        if(messageBox->buttonResult() == Wt::StandardButton::Yes)callback();
        delete messageBox;
    }));

    messageBox->show();
    messageBox->setWidth(750);
}

void CommonDialogManager::showMessage(const std::string & text)
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
        ("Confirm",
            "<p>" + text + "</p>",
            Wt::Icon::Critical, Wt::StandardButton::Ok);

    messageBox->buttonClicked().connect(std::bind([=]() {
        delete messageBox;
    }));

    messageBox->show();
    messageBox->setWidth(750);
}

void CommonDialogManager::showDialog(
   const std::string& caption, 
   const std::vector<FieldInfo>& fields, 
   TCallback& callback
)
{
   auto d = createDialog(caption, fields, callback);
   d->show();
}

BasicDialog::BasicDialog(const std::string& caption, const bool suppressEnter)
{
    Wt::WPushButton *ok = new Wt::WPushButton("OK");
    footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));
    if(!suppressEnter)
    {
        ok->setDefault(true);
    }

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel");
    footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
    rejectWhenEscapePressed();

    ok->clicked().connect(std::bind([=]() {
        onAccepted();
    }));

    cancel->clicked().connect(this, &Wt::WDialog::reject);
    setWidth(750);
}

void BasicDialog::onAccepted()
{
    if(verify())
    {
        accept();
    }
    else
    {
        CommonDialogManager::showMessage("Verification failed!");
    }
}

bool BasicDialog::verify()
{
    return true;
}

Wt::WDialog* CommonDialogManager::createDialogBase(const std::string& caption)
{
    return new BasicDialog(caption);
}

Wt::WDialog* CommonDialogManager::createDialog(const std::string& caption, const std::vector<FieldInfo>& fields, TCallback& callback)
{
   Wt::WDialog* dialog = createDialogBase(caption);

   std::vector<Wt::WLabel*> labels;
   std::vector<Wt::WLineEdit*> edits;

    for (auto item : fields)
    {
        Wt::WLabel* labelName = new Wt::WLabel(item.name);
        dialog->contents()->addWidget(std::unique_ptr<Wt::WLabel>(labelName));
        Wt::WLineEdit* editName = new Wt::WLineEdit(Wt::WString::fromUTF8(item.initialValue));
        dialog->contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(editName));
        labelName->setBuddy(editName);

        labels.push_back(labelName);
        edits.push_back(editName);
    }

    dialog->finished().connect(std::bind([=]() {
        if (dialog->result() == Wt::DialogCode::Accepted)
        {
            std::vector<std::string> result;
            for (auto ctrl : edits)
            {
                result.push_back(std::string(ctrl->text().toUTF8()));
            }
            callback(result);
        }

        delete dialog;
    }));
    
    return dialog;
}

void CommonDialogManager::showLinesDialog(const std::vector<Wt::WString>& lines, std::function<void(const std::vector<Wt::WString>&)> callback)
{
    Wt::WDialog* dialog = createDialogBase("");
    
    Wt::WTextArea* linesArea = new Wt::WTextArea;
    dialog->contents()->addWidget(std::unique_ptr<Wt::WTextArea>(linesArea));
    
    for(auto i : lines)
    {
       linesArea->setText(linesArea->text() + "\n" + i); 
    }

    dialog->finished().connect(std::bind([=]() {
        if (dialog->result() == Wt::DialogCode::Accepted)
        {
            std::vector<std::string> result;
            
            std::string utf8 = linesArea->text().toUTF8();
            boost::split(result, utf8, boost::is_any_of("\n"));
            
            std::vector<Wt::WString> realResult;
            for(auto i : result)
            {
               realResult.push_back(Wt::WString::fromUTF8(i));
            }
            callback(realResult);
        }

        delete dialog;
    }));

    dialog->show();
}

void CommonDialogManager::showChoiseDialog(const std::vector<std::string>& options, std::function<void(const std::size_t&)> callback)
{
    Wt::WDialog* dialog = createDialogBase("");

    if(options.size() < 8)
    {
        auto group = std::make_shared<Wt::WButtonGroup>();

        for(std::size_t i = 0; i < options.size(); ++i)
        {
            Wt::WRadioButton *button = dialog->contents()->addWidget(std::make_unique<Wt::WRadioButton>(options[i]));
            button->setInline(false);
            group->addButton(button, i);
        }

        group->setSelectedButtonIndex(0);

        dialog->finished().connect(std::bind([=]() {

            if (dialog->result() == Wt::DialogCode::Accepted)
            {
                callback(static_cast<std::size_t>(group->selectedButtonIndex()));
            }
            
            delete dialog;

        }));
    }
    else
    {
        auto cmb = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
        for(auto x : options)
        {
            cmb->addItem(x);
        }

        cmb->setCurrentIndex(0);

        dialog->finished().connect(std::bind([=]() {

            if (dialog->result() == Wt::DialogCode::Accepted)
            {
                callback(cmb->currentIndex());
            }
            
            delete dialog;

        }));
    }

    dialog->show();
}

void CommonDialogManager::showDoubleComboDialog(
    const std::vector<std::string>& options, 
    const std::vector<std::string>& options2, 
    std::function<void(const std::size_t&, const std::size_t&)> callback
    )
{
    Wt::WDialog* dialog = createDialogBase("");

    auto cmb = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
    for(auto x : options)
    {
        cmb->addItem(x);
    }

    auto cmb2 = dialog->contents()->addWidget(std::make_unique<Wt::WComboBox>());
    for(auto x : options2)
    {
        cmb2->addItem(x);
    }

    cmb->setCurrentIndex(0);
    cmb2->setCurrentIndex(0);

    dialog->finished().connect(std::bind([=]() {

        if (dialog->result() == Wt::DialogCode::Accepted)
        {
            callback(cmb->currentIndex(), cmb2->currentIndex());
        }
        
        delete dialog;

    }));

    dialog->show();
}