#pragma once

#include <memory>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WLabel.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WFont.h>

enum class SplitType
{
    Vertical,
    Horizontal
};

enum class FontSize
{
    Big,
    Small
};

class SplitWidget : public Wt::WContainerWidget
{
public:
    SplitWidget(const SplitType splitType, const double firstItemPercentage);

    Wt::WContainerWidget& first;
    Wt::WContainerWidget& second;
};

std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, std::function<void()> handler, const int margin = 10);
std::unique_ptr<Wt::WTable> createList();
std::unique_ptr<SplitWidget> createSplit(const SplitType splitType, const double firstItemPercentage);
std::unique_ptr<Wt::WLabel> createLabel(const std::string& text, const Wt::WFont::Size fSize);