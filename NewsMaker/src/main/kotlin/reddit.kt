package main

import net.dean.jraw.http.*
import net.dean.jraw.oauth.*
import net.dean.jraw.models.*
import net.dean.jraw.RedditClient
import java.util.Date

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
   return composeHtmlList(name, items.take(25).map{it.title})
}

fun genRedditContent(): String
{
   val userAgent = UserAgent("linux", "snake.materia.newsreader", "v1", "theramsnake");
   val credentials = Credentials.script("theramsnake", "rtff6#yo", "DVOqwIAoJxN5NA", "1pfVk4fZ1mwSct2fL-72GbMpF-E");
   val adapter = OkHttpNetworkAdapter(userAgent);
   val reddit = OAuthHelper.automatic(adapter, credentials);

   val subreddits = listOf("WorldNews", "Science", "Space", "VrGaming", "Programming", "Productivity", "Invseting")

   var result = ""
   for(x in subreddits)
   {
      result += genContentForSubreddit(x, reddit)
   }

   return result
}