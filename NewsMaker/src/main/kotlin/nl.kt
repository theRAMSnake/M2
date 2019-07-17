package main

fun parseNlPage(page: String): String
{
   val index = page.indexOf("<span>Next Launch&colon; <a href=", 0)

   if(index == -1)
   {
      return "error"
   }

   val endIndex = page.indexOf("</span", index)

   if(endIndex == -1)
   {
      return "error"
   }

   return page.substring(index, endIndex + 6)
}

fun genNextLaunchContent(): String
{
    val r = khttp.get("https://www.spaceflightinsider.com/launch-schedule/")

    return composeHtmlList("Next launch", mutableListOf(parseNlPage(r.text)))
}