package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.Date
import java.util.concurrent.TimeUnit
import net.dean.jraw.http.*
import net.dean.jraw.oauth.*
import net.dean.jraw.models.*

fun genTestRedditContent(): String
{
    val userAgent = UserAgent("linux", "snake.materia.newsreader", "v1", "theramsnake");
    val credentials = Credentials.script("theramsnake", "rtff6#yo", "DVOqwIAoJxN5NA", "1pfVk4fZ1mwSct2fL-72GbMpF-E");
    val adapter = OkHttpNetworkAdapter(userAgent);
    val reddit = OAuthHelper.automatic(adapter, credentials);

    val paginator = reddit
       .subreddit("VrGaming")
       .posts()
       .timePeriod(TimePeriod.DAY)
       .sorting(SubredditSort.NEW)
       .build();

    val firstThreePages = paginator.accumulate(3);

    val timeNow: Date = java.util.Calendar.getInstance().time

    var result = ""
    for(x in firstThreePages)
    {
        for(y in x.getChildren())
        {
            val diffInMillies = Math.abs(timeNow.time - y.created.time);
            if(TimeUnit.HOURS.convert(diffInMillies, TimeUnit.MILLISECONDS) < 25)
            {
                result += "<li>${y.getTitle()}</li>"
            }
            else
            {
                break
            }
        }
    }

    return result
}

fun publishFile(filename: String)
{
    Runtime.getRuntime().exec("./m2tools News $filename") // -> delete after
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    val filecontent = genTestRedditContent()

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    publishFile(filename)

    println("Done")
}