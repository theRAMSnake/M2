#include "WidgetFactory.hpp"

#include <Wt/WCssDecorationStyle.h>

std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, std::function<void()> handler)
{
   auto result = std::make_unique<Wt::WPushButton>(text);
   result->clicked().connect(handler);
   result->addStyleClass("btn-primary");
   result->setMargin(10);
   return result;
}

std::unique_ptr<Wt::WTable> createList()
{
   auto table = std::make_unique<Wt::WTable>();
   table->setWidth(Wt::WLength("100%"));
   table->addStyleClass("table-bordered");
   table->addStyleClass("table-hover");
   table->addStyleClass("table-striped");
   table->decorationStyle().font().setSize(Wt::WFont::Size::XXLarge);

   return table;
}

std::unique_ptr<Wt::WLabel> createLabel(const std::string& text, const FontSize fSize)
{
   /*SNAKE*/
}

std::unique_ptr<SplitWidget> createSplit(const SplitType splitType, const double firstItemPercentage)
{
   /*SNAKE*/
}

SplitWidget::SplitWidget(const SplitType splitType, const double firstItemPercentage)
{
   /*SNAKE*/
}
