#include "RewardView.hpp"
#include "WidgetFactory.hpp"
#include "dialog/CommonDialogManager.hpp"

#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>

#include <boost/lexical_cast.hpp>

bool isPositiveInteger(const string& s){
  return !s.empty() && s.find_first_not_of( "0123456789" ) == string::npos;
}

class RewardItemDialog : public Wt::WDialog
{
public:
   typedef std::function<void(const RewardModel::Item&)> TOnOkCallback;
   RewardItemDialog(const RewardModel::Item& src, TOnOkCallback cb)
   {
      setWidth(Wt::WLength("35%"));
      auto title = new Wt::WLineEdit();
      title->setText(src.name);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(title));

      auto gb = contents()->addWidget(Wt::cpp14::make_unique<Wt::WGroupBox>());

      gb->addStyleClass("row justify-content-center");

      auto amount = gb->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>());
      amount->setText(std::to_string(src.amount));
      amount->setWidth(Wt::WLength("10%"));
      amount->addStyleClass("col-md-6");
      amount->setMargin(Wt::WLength("30%"), Wt::Side::Left);
      amount->setMargin(Wt::WLength("10%"), Wt::Side::Right);

      auto amountMax = gb->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>());
      amountMax->setText(std::to_string(src.amountMax));
      amountMax->setWidth(Wt::WLength("10%"));
      amountMax->addStyleClass("col-md-6");
      amountMax->setMargin(Wt::WLength("10%"), Wt::Side::Left);
      amountMax->setMargin(Wt::WLength("30%"), Wt::Side::Right);

      auto ok = new Wt::WPushButton("Accept");
      ok->setDefault(true);
      ok->clicked().connect(std::bind([=]() {
          if (!title->text().empty() &&
            isPositiveInteger(amount->text().narrow()) &&
            isPositiveInteger(amountMax->text().narrow())
            )
          {
             accept();
          }
      }));
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));

      auto cancel = new Wt::WPushButton("Cancel");
      footer()->addWidget(std::unique_ptr<Wt::WPushButton>(cancel));
      cancel->clicked().connect(this, &Wt::WDialog::reject);

      rejectWhenEscapePressed();

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
            RewardModel::Item resultItem(src);
            resultItem.name = title->text().narrow();
            resultItem.amount = boost::lexical_cast<unsigned int>(amount->text());
            resultItem.amountMax = boost::lexical_cast<unsigned int>(amountMax->text());

            cb(resultItem);
        }

        delete this;
      }));
   }

private:
   std::shared_ptr<Wt::WButtonGroup> mGbRec;
};

//-----------------------------------------------------------------------------------------

class RewardPoolView : public Wt::WPaintedWidget
{
public:
    RewardPoolView(const RewardModel::Item& item)
    : mItem(item)
    {
        setWidth(1850);
        setHeight(48);
        setMargin(20, Wt::Side::Left);
    }

protected:
   void paintEvent(Wt::WPaintDevice *paintDevice) 
   {
      Wt::WPainter painter(paintDevice);

      painter.drawText(Wt::WRectF{0, 0, paintDevice->width().toPixels() / 5, paintDevice->height().toPixels()}, 
        Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle, 
        mItem.name + "(" + std::to_string(mItem.amount) + "/" + std::to_string(mItem.amountMax) + ")"
        );

      if(mItem.amountMax > 0)
      {
        double sizePerBlock = (paintDevice->width().toPixels() * 0.8) / (mItem.amountMax);

        auto greenBrush = Wt::WBrush(Wt::StandardColor::DarkGreen);
        auto grayBrush = Wt::WBrush(Wt::StandardColor::Gray);

        for(unsigned int i = 0; i < mItem.amountMax; ++i)
        {
            painter.fillRect({2 + paintDevice->width().toPixels() / 5 + i * (sizePerBlock), 4, sizePerBlock - 4, paintDevice->height().toPixels() - 8},
                i < mItem.amount ? greenBrush : grayBrush);
        }
      }
   }

private:
    const RewardModel::Item mItem;
};

//-----------------------------------------------------------------------------------------

RewardView::RewardView(RewardModel& model)
: mModel(model)
{
   addWidget(createButton("Add Pool", std::bind(&RewardView::onAddPoolClick, this)));
   addWidget(createButton("Add Points", std::bind(&RewardView::onAddPointsClick, this)));

   auto gb = addWidget(std::make_unique<Wt::WGroupBox>());
   mTable = gb->addWidget(createList());

   refreshList();
}

void RewardView::refreshList()
{
    mTable->clear();

    auto items = mModel.get();
    for(std::size_t i = 0; i < items.size(); ++i)
    {
       auto cell = mTable->elementAt(i, 0);
       
       cell->addWidget(std::make_unique<RewardPoolView>(items[i]));

       cell->clicked().connect(std::bind(&RewardView::onItemClick, this, std::placeholders::_1, items[i]));
       cell->doubleClicked().connect(std::bind(&RewardView::onItemDoubleClick, this, items[i]));
    }
}

void RewardView::onAddPointsClick()
{
    mModel.addPoints(1);
    refreshList();
}

void RewardView::onAddPoolClick()
{
    RewardModel::Item newItem {materia::Id::Invalid, "", 0, 0};
    RewardItemDialog* dlg = new RewardItemDialog(newItem, [=](const RewardModel::Item& result) 
    {
        materia::Id id = mModel.add(result);

        if(id != materia::Id::Invalid)
        {
            refreshList();
        }
    });

    dlg->show();
}

void RewardView::onItemDoubleClick(const RewardModel::Item& item)
{
    RewardItemDialog* dlg = new RewardItemDialog(item, [=](const RewardModel::Item& result) 
    {
        mModel.replace(result);
        refreshList();
    });

    dlg->show();
}

void RewardView::onItemClick(Wt::WMouseEvent ev, const RewardModel::Item& item)
{
   if(ev.modifiers().test(Wt::KeyboardModifier::Control))
   {
      std::function<void()> elementDeletedFunc = [=] () {
         mModel.erase(item.id);
         refreshList();
         };

      CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
   }
   
}