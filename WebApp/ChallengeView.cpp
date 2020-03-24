#include "ChallengeView.hpp"

ChallengeView::ChallengeView(ChallengeModel& model)
: mModel(model)
{
   addWidget(createButton("Add", std::bind(&ChallengeModel::onAddClick, this)));

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
       
       cell->addWidget(std::make_unique<ChallengeView>(items[i]));
    }
}