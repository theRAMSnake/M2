package main

fun parseMcPage(page: String, userScoreRating: String): List<String>
{
    var result = mutableListOf<String>()

    var curPos = 0
    while(true)
    {
        val newIndexOfRating = page.indexOf(userScoreRating, curPos)

        if(newIndexOfRating == -1)
        {
            break
        }

        val startIndex = page.substring(0, newIndexOfRating).lastIndexOf("<a href=\"/game/pc/")
        val endIndex = page.indexOf("</a>", startIndex)

        val ref = page.substring(startIndex, endIndex + 4)
        result.add(ref)

        curPos = newIndexOfRating + 1
    }

    return result
}

fun genMetacriticContent(): String
{
    val allGames = mutableListOf<String>()
    val r = khttp.get("https://www.metacritic.com/browse/games/release-date/new-releases/pc/userscore")

    allGames += parseMcPage(r.text, "textscore_outstanding")
    allGames += parseMcPage(r.text, "textscore_favorable")
    
    allGames.shuffle()
    return composeHtmlList("Top Metacritic games", allGames.take(3)) 
}