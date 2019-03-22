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

fun genContentForSubreddit(name: String, reddit: RedditClient): String
{
    val paginator = reddit
       .subreddit(name)
       .posts()
       .timePeriod(TimePeriod.DAY)
       .sorting(SubredditSort.NEW)
       .build();

    val firstThreePages = paginator.accumulate(3);

    val timeNow: Date = java.util.Calendar.getInstance().time
    
    var result = "<h1>$name</h1>"

    for(x in firstThreePages)
    {
        for(y in x.getChildren())
        {
            val diffInMillies = Math.abs(timeNow.time - y.created.time)

            if(less(diffInMillies, 90000000))
            {
                result += "<li>${y.getTitle()}</li>"
            }
            else
            {
                return result
            }
        }
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

fun publishFile(filename: String)
{
    //Runtime.getRuntime().exec("./m2tools News $filename") // -> delete after
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    val filecontent = genRedditContent()

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    publishFile(filename)

    println("Done")
}