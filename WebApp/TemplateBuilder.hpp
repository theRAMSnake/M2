#pragma once

class TemplateBuilder
{
public:
   template<class TCtrl>
   static WTemplate* makeTable(const unsigned int numRows, const unsigned int numCols)
   {
      std::string result = "<table>";

      for(auto r = 0; r < numRows; ++r)
      {
         result += "<tr>";

         for(auto c = 0; c < numCols; ++c)
         {
            result += "<td>${cell" + std::to_string(r) + "_" + std::to_string(c) + "}</td>";
         }

         result += "</tr>";
      }

      result += "</table>"

      auto templ = new Wt::WTemplate(result);
      for(auto r = 0; r < numRows; ++r)
      {
         for(auto c = 0; c < numCols; ++c)
         {
            auto cell = "cell" + std::to_string(r) + "_" + std::to_string(c) + "}";
            templ->bindWidget(cell, Wt::cpp14::make_unique<TCtrl>());
         }
      }

      return templ;
   }
};