#pragma once

#include <Wt/WTemplate.h>

class TemplateBuilder
{
public:
   template<class TCtrl>
   static std::tuple<Wt::WTemplate*, std::vector<TCtrl*>> makeTable(const unsigned int numRows, const unsigned int numCols)
   {
      std::string resultHtml = "<table style=\"position: absolute; top: 0; bottom: 0; left: 0; right: 0;\">";

      for(unsigned int r = 0; r < numRows; ++r)
      {
         resultHtml += "<tr style=\"height:" + std::to_string(100 / numRows) + "%;\">";

         for(unsigned int c = 0; c < numCols; ++c)
         {
            resultHtml += "<td style=\"width:" + std::to_string(100 / numCols) + "%;\">${cell" + std::to_string(r) + "_" + std::to_string(c) + "}</td>";
         }

         resultHtml += "</tr>";
      }

      resultHtml += "</table>";

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