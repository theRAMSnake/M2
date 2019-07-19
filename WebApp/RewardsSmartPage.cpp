#include "RewardsSmartPage.hpp"
#include "materia/JournalModel.hpp"
#include "materia/FreeDataModel.hpp"
#include <regex>

struct RewardItem
{
    std::string name;
    int count;
    int max;
};

std::vector<RewardItem> parseItems(const std::vector<std::string>& contentItems)
{
    std::vector<RewardItem> result;

    std::smatch match;
    std::regex reg("<li>(.+) ([0-9]+)/([0-9]+)</li>");

    for(auto x: contentItems)
    {
        if(std::regex_match(x, match, reg))
        {
            result.push_back({match[1], std::stoi(match[2].str()), std::stoi(match[3].str())});
        }
    }

    return result;
}

void replaceItem(const RewardItem& item, std::string& content)
{
    std::regex reg("<li>" + item.name + " ([0-9]+)/([0-9]+)</li>");

    content = std::regex_replace(content, reg, "<li>" + item.name + " " + std::to_string(item.count) + "/" +
        std::to_string(item.max) + "</li>");
}

bool hasNotCompleteItems(const std::vector<RewardItem>& items)
{
    for(auto x : items)
    {
        if(x.count < x.max)
        {
            return true;
        }
    }

    return false;
}

void RewardsSmartPage::update(JournalModel& journal, FreeDataModel& fd)
{
    try
    {
        srand(time(0));

        auto ppResource = fd.get("PP");
        auto rewardsPageId = journal.searchIndex("Rewards");
        auto rewardsPage = journal.loadContent(rewardsPageId);

        if(ppResource && !rewardsPage.empty() && ppResource->value != 0)
        {
            auto items = parseItems(parsePagetoList(rewardsPage));
            while(ppResource->value > 0 && hasNotCompleteItems(items))
            {
                auto& randomItem = items[rand() % items.size()];
                if(randomItem.count < randomItem.max)
                {
                    randomItem.count++;
                }
                else
                {
                    continue;
                }

                ppResource->value--;
            }

            for(auto x : items)
            {
                replaceItem(x, rewardsPage);
            }

            fd.set(*ppResource);
            journal.saveContent(rewardsPageId, rewardsPage);
        }
    }
    catch(...)
    {
        
    }
}