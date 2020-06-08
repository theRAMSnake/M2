package main

import net.dean.jraw.http.*
import net.dean.jraw.oauth.*
import net.dean.jraw.models.*
import net.dean.jraw.RedditClient
import java.util.Date
import java.io.*

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
   return composeHtmlList(name, items.take(25).map{"<a href=\"${it.url}\">${it.title}</a>"})
}

fun loadSubreddits(password: String): List<String>
{
    val p = Runtime.getRuntime().exec("./m2tools loadPage reddit tmp.page")
    p.waitFor()

    val lines = File("tmp.page").readLines()
    val result = mutableListOf<String>()
    for(l in lines)
    {
       if(l.startsWith("<li"))
       {
          var o = l.replace("<li>", "")
          o = o.replace("</li>", "")

          result.add(o)
       }
    }

    return result
}

fun genRedditContent(password: String): String
{
   val userAgent = UserAgent("linux", "snake.materia.newsreader", "v1", "theramsnake");
   val credentials = Credentials.script("theramsnake", "rtff6#yo", "DVOqwIAoJxN5NA", "1pfVk4fZ1mwSct2fL-72GbMpF-E");
   val adapter = OkHttpNetworkAdapter(userAgent);
   val reddit = OAuthHelper.automatic(adapter, credentials);

   val subreddits = loadSubreddits(password)

   var result = ""
   for(x in subreddits)
   {
      try {
         result += genContentForSubreddit(x, reddit)   
         //Thread.sleep(60000)
      }
      catch(e: Exception) {
         
      }
      
   }

   return result
}