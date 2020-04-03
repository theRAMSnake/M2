#include "WidgetFactory.hpp"

#include <Wt/WCssDecorationStyle.h>

#include <fmt/format.h>

std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, std::function<void()> handler, const int margin)
{
   auto result = std::make_unique<Wt::WPushButton>(text);
   result->clicked().connect(handler);
   result->addStyleClass("btn-primary");
   result->setMargin(margin);
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

std::unique_ptr<Wt::WLabel> createLabel(const std::string& text, const Wt::WFont::Size fSize)
{
   auto result = std::make_unique<Wt::WLabel>(text);

   result->decorationStyle().font().setSize(fSize);

   return result;
}

std::unique_ptr<SplitWidget> createSplit(const SplitType splitType, const double firstItemPercentage)
{
   return std::make_unique<SplitWidget>(splitType, firstItemPercentage);
}

SplitWidget::SplitWidget(const SplitType splitType, const double firstItemPercentage)
: first(*addWidget(std::make_unique<Wt::WContainerWidget>()))
, second(*addWidget(std::make_unique<Wt::WContainerWidget>()))
{
   if(splitType == SplitType::Horizontal)
   {
      first.setWidth(fmt::format("{}%", int(firstItemPercentage * 100)));
      second.setWidth(fmt::format("{}%", int(100 - firstItemPercentage * 100)));
   }
   else
   {
      first.setHeight(fmt::format("{}%", int(firstItemPercentage * 100)));
      second.setHeight(fmt::format("{}%", int(100 - firstItemPercentage * 100)));
   }
}
