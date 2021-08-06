package main

fun parseNltimesPage(page: String): List<String>
{
    var result = mutableListOf<String>()

    var count = 0

    var curPos = 0
    while(true)
    {
        val start = page.indexOf("news-card__title", curPos)

        if(start == -1)
        {
            break
        }

        val startIndex = page.indexOf("<a href=\"", start)
        val endIndex = page.indexOf("</a>", start)

        val ref = page.substring(startIndex, endIndex + 4)
        result.add(ref)

        curPos = endIndex + 1
        count++
        if (count > 10)
        {
            break
        }
    }

    return result
}

fun genNlTimesContent(): String
{
    val r = khttp.get("https://nltimes.nl")

    val allEntries = parseNltimesPage(r.text)
    
    return composeHtmlList("NL News", allEntries) 
}