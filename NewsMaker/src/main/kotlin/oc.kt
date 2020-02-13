package main

fun parseOcPage(page: String): List<String>
{
    var result = mutableListOf<String>()

    var curPos = 0
    while(true)
    {
        val newIndex = page.indexOf("<a _ngcontent-sc37=\"\" href=", curPos)

        if(newIndex == -1)
        {
            break
        }

        val endIndex = page.indexOf("</a", newIndex)

        val ref = page.substring(newIndex, endIndex + 4)
        result.add(ref)

        curPos = endIndex
    }

    return result
}

fun genOpenCriticContent(): String
{
    val allGames = mutableListOf<String>()
    val r = khttp.get("https://opencritic.com/browse/pc/last90/score")
    allGames += parseOcPage(r.text)

    val r2 = khttp.get("https://www.opencritic.com/browse/pc/last90/score?page=2")
    allGames += parseOcPage(r2.text)

    val r3 = khttp.get("https://www.opencritic.com/browse/pc/last90/score?page=3")
    allGames += parseOcPage(r3.text)
    
    allGames.shuffle()
    return composeHtmlList("Top Opencritic games", allGames.take(3)) 
}