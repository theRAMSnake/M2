package main

import kotlinx.serialization.*
import kotlinx.serialization.json.Json

fun loadHackernewsPage(): String
{
    val r = khttp.get("https://hacker-news.firebaseio.com/v0/newstories.json?")
    return r.text
}

@Serializable
data class HackerNewsItem(val score: Int, val title: String)

fun loadItem(id: Int): HackerNewsItem
{
    val r = khttp.get("https://hacker-news.firebaseio.com/v0/item/$id.json?")
    return Json.nonstrict.parse(HackerNewsItem.serializer(), r.text)
}

@Serializable
data class Ids(val value: List<Int>)

fun genHackernewsContent(): String
{
    val allItems = mutableListOf<String>()

    val page = loadHackernewsPage()
    val json = "{\"value\":$page}"
    val itemsOnPage = Json.parse(Ids.serializer(), json)

    for(x in itemsOnPage.value)
    {
        try
        {
            val item = loadItem(x)
            if(item.score > 100)
            {
                allItems.add(item.title)
                if(allItems.size > 25)
                {
                    break
                }
            }
        }
        catch(e: Exception)
        {
            
        }
    }

    return composeHtmlList("HackerNews", allItems)
}