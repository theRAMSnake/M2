#pragma once

#include <Wt/WTemplate.h>

class TemplateBuilder
{
public:
   template<class TCtrl>
   static std::tuple<Wt::WTemplate*, std::vector<TCtrl*>> makeTable(const unsigned int numRows, const unsigned int numCols)
   {
      if(numCols > 12)
      {
         throw -1;
      }

      std::string resultHtml = "<div class=\"container-fluid\">";

      for(unsigned int r = 0; r < numRows; ++r)
      {
         resultHtml += "<span class=\"row\">";

         for(unsigned int c = 0; c < numCols; ++c)
         {
            int styleSize = 12 / numCols;
            resultHtml += "<span class=\"col-md-" + std::to_string(styleSize) + "\">${cell" + std::to_string(r) + "_" + std::to_string(c) + "}</span>";
         }

         resultHtml += "</span>";
      }

      resultHtml += "</div>";

      auto templ = new Wt::WTemplate(resultHtml);
      std::vector<TCtrl*> items;
      for(unsigned int r = 0; r < numRows; ++r)
      {
         for(unsigned int c = 0; c < numCols; ++c)
         {
            auto cell = "cell" + std::to_string(r) + "_" + std::to_string(c);
            auto newItem = std::make_unique<TCtrl>();
            items.push_back(newItem.get());
            templ->bindWidget(cell, std::move(newItem));
         }
      }

      return std::make_tuple(templ, items);
   }
};