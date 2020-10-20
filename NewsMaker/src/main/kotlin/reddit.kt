package main

import net.dean.jraw.http.*
import net.dean.jraw.oauth.*
import net.dean.jraw.models.*
import net.dean.jraw.RedditClient
import java.util.Date
import java.io.*
import kotlinx.serialization.*
import kotlinx.serialization.json.Json

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

@kotlinx.serialization.Serializable
data class NewsConfig(val id: String, val typename: String, val reddit: String)

@kotlinx.serialization.Serializable
data class ObjectList(val id: String, val typename: String, val object_list: List<NewsConfig>)

fun loadSubreddits(): List<String>
{
    val op = "{\"operation\":\"query\", \"ids\":[\"config.news\"]}"
    val p = Runtime.getRuntime().exec(arrayOf("./m3tools", op))
    val input = BufferedReader(InputStreamReader(p.getInputStream()));
    p.waitFor()
    
    val lines = input.readText()
    input.close()
    print(lines)
    val obj = Json.parse(ObjectList.serializer(), lines)

    return obj.object_list[0].reddit.split(";")
}

fun genRedditContent(): String
{
   val userAgent = UserAgent("linux", "snake.materia.newsreader", "v1", "theramsnake");
   val credentials = Credentials.script("theramsnake", "rtff6#yo", "DVOqwIAoJxN5NA", "1pfVk4fZ1mwSct2fL-72GbMpF-E");
   val adapter = OkHttpNetworkAdapter(userAgent);
   val reddit = OAuthHelper.automatic(adapter, credentials);

   val subreddits = loadSubreddits()
   print(subreddits)

   var result = ""
   for(x in subreddits)
   {
      try {
         result += genContentForSubreddit(x, reddit)   
      }
      catch(e: Exception) {
      }
   }

   return result
}