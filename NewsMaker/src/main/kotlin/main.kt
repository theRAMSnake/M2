package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.Date
import java.util.concurrent.TimeUnit
import net.dean.jraw.http.*
import net.dean.jraw.oauth.*
import net.dean.jraw.models.*
import net.dean.jraw.RedditClient
import org.jsoup.*
import kotlinx.serialization.json.JSON

fun genContentForSubreddit(name: String, reddit: RedditClient): String
{
    val paginator = reddit
       .subreddit(name)
       .posts()
       .timePeriod(TimePeriod.DAY)
       .sorting(SubredditSort.NEW)
       .build();

    val firstPages = paginator.accumulate(5);

    val timeNow: Date = java.util.Calendar.getInstance().time
    
    val items = mutableListOf<Submission>()

    for(x in firstPages)
    {
        for(y in x.getChildren())
        {
            val diffInMillies = Math.abs(timeNow.time - y.created.time)

            if(less(diffInMillies, 90000000))
            {
                items.add(y)
            }
            else
            {
                break
            }
        }
    }

    items.sortByDescending{ it.score }

    var result = "<h1>$name</h1>"

    var i = 0
    for(x in items)
    {
        if(i > 24)
        {
            break
        }

        result += "<li>${x.getTitle()}</li>"
        ++i
    }

    return result
}

fun genRedditContent(): String
{
    val userAgent = UserAgent("linux", "snake.materia.newsreader", "v1", "theramsnake");
    val credentials = Credentials.script("theramsnake", "rtff6#yo", "DVOqwIAoJxN5NA", "1pfVk4fZ1mwSct2fL-72GbMpF-E");
    val adapter = OkHttpNetworkAdapter(userAgent);
    val reddit = OAuthHelper.automatic(adapter, credentials);

    val subreddits = listOf("WorldNews", "Science", "Space", "VrGaming", "Programming", "Productivity")

    var result = ""
    for(x in subreddits)
    {
        result += genContentForSubreddit(x, reddit)
    }

    return result
}

fun loadHackernewsPage(pageNumber: Int): String
{
    val doc = Jsoup.connect("https://hacker-news.firebaseio.com/v0/newstories.json?").get()
    return doc.outerHtml()
}

fun genHackernewsContent(): String
{
    var i = 1
    val allItems = mutableListOf<String>()
    mainLoop@ while(true)
    {
        val page = loadHackernewsPage(i++)
        val itemsOnPage = JSON.parse(Int.serializer().list, page)

        for(x in itemsOnPage)
        {
            val item = loadItem(x)
            if(item.score > 100)
            {
                allItems.add(item.title)
                if(allItems.size > 25)
                {
                    break@mainLoop
                }
            }
        }
    }

    return composeHtmlList("HackerNews", allItems)
}

fun publishFile(filename: String)
{
    val p = Runtime.getRuntime().exec("./m2tools News $filename")
    p.waitFor()
    Runtime.getRuntime().exec("rm $filename")
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    val filecontent = genRedditContent()
    filecontent += genHackernewsContent()

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    publishFile(filename)

    println("Done")
}