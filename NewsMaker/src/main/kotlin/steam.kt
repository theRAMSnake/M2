package main

import java.time.LocalDateTime
import java.util.Calendar
import java.util.Date
import java.util.Locale
import java.text.SimpleDateFormat

fun extractGames(page: String): List<String>
{
    var result = mutableListOf<String>()

    val beginPos = page.indexOf("search_result_container")
    val endPos = page.indexOf("search_pagination\">")

    var curPos = beginPos
    while(true)
    {
        val newIndex = page.indexOf("<a href=\"https://store.steampowered.com/app", curPos)
        //println("n $newIndex")

        if(newIndex > endPos || newIndex == -1)
        {
            break
        }

        val appIndex = page.indexOf("app/", newIndex)
        val endAppIndex = page.indexOf("/?", newIndex)
        //println("a $appIndex")
        //println("ea $endAppIndex")

        val title = page.substring(appIndex + 4, endAppIndex)
        //println(title)
        result.add(page.substring(newIndex, page.indexOf("\"", newIndex + 20) + 1) + ">$title</a>")

        curPos = newIndex + 1
    }

    return result
}

fun genNewVrGamesContent(): String
{
    val allNewVrGames = mutableListOf<String>()
    for(x in 1..5)
    {
        val r = khttp.get("https://store.steampowered.com/search/?sort_by=Released_DESC&vrsupport=402&page=$x")
        allNewVrGames += extractGames(r.text)
    }
    
    allNewVrGames.shuffle()
    return composeHtmlList("New VR games", allNewVrGames.take(5)) 
}

fun genTopVrGamesContent(): String
{
    val allVrGames = mutableListOf<String>()
    for(x in 1..5)
    {
        val r = khttp.get("https://store.steampowered.com/search/?vrsupport=401&filter=topsellers&page=$x")
        allVrGames += extractGames(r.text)
    }
    
    allVrGames.shuffle()
    return composeHtmlList("Top VR games", allVrGames.take(5)) 
}

fun genNewGamesContent(): String
{
    val allVrGames = mutableListOf<String>()
    val r = khttp.get("https://store.steampowered.com/search/?sort_by=Released_DESC&os=win&filter=popularnew&page=1")
    allVrGames += extractGames(r.text)
    
    allVrGames.shuffle()
    return composeHtmlList("Top nonVR games", allVrGames.take(3)) 
}

fun loadNewsPage(appId: String): String
{
    val r = khttp.get("https://steamcommunity.com/games/$appId/announcements/")
    return r.text
}

fun parseDate(entry: String): Date
{
    val trimmed = entry.trim() + " 1999"

    return SimpleDateFormat("dd MMM yyyy").parse(trimmed)
}

data class NewsEntry(val date: Date, val link: String)

fun parseNews(page: String): List<NewsEntry>
{
    val result = mutableListOf<NewsEntry>()

    var curPos = page.indexOf("<div class=\"announcement\">")
    while(curPos != -1)
    {
        val hrefIndex = page.indexOf("<a class=\"large_title\"", curPos)
        val hrefEnd = page.indexOf("</a>", hrefIndex)

        val href = page.substring(hrefIndex, hrefEnd + 4)

        val datePrefix = page.indexOf("<div class=\"announcement_byline\">", curPos)
        val dateFrom = page.indexOf(">", datePrefix)
        val dateTo = page.indexOf("@", datePrefix)

        try
        {
            val date = parseDate(page.substring(dateFrom + 1, dateTo))

            result.add(NewsEntry(date, href))
        }
        catch(e: java.text.ParseException)
        {

        }
        
        curPos = page.indexOf("<div class=\"announcement\">", curPos + 1)
    }

    return result
}

fun genNewsContent(): String
{
    val cal = java.util.Calendar.getInstance()
    cal.add(java.util.Calendar.DAY_OF_MONTH, -1)
    
    val apps = listOf("238960", "494150", "963930", "365360", "839960", "566860") //loadFromFile
    val suitableNews = mutableListOf<String>()

    for(x in apps)
    {
        val page = loadNewsPage(x)
        val news = parseNews(page)

        for(y in news)
        {
            val curDate = java.util.Calendar.getInstance()
            curDate.time = y.date
            if(cal.get(Calendar.DAY_OF_MONTH) == curDate.get(Calendar.DAY_OF_MONTH) &&
                cal.get(Calendar.MONTH) == curDate.get(Calendar.MONTH))
            {
                suitableNews.add(y.link)
            }
        }
    }

    return composeHtmlList("Steam news", suitableNews)
}

fun genSteamContent(): String
{
    var result = ""

    result += genNewVrGamesContent()
    result += genTopVrGamesContent()
    result += genNewGamesContent()
    result += genNewsContent()
    
    return result
}