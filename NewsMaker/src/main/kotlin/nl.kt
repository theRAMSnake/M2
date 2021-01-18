package main

fun parseNlPage(page: String): List<String>
{
    var result = mutableListOf<String>()

    run {
        val regex = "<span class=\"launchdate\">(.*)</span>.*<span class=\"mission\">(.*)</span>".toRegex()
        val matches = regex.findAll(page)

        matches.forEach { matchResult ->
                    result.add("<h3>" + matchResult.groupValues[1] + ": " + matchResult.groupValues[2] + "</h3>")
                }
    }
    run {
        val regex = "<div class=\"missdescrip\">(.*)</div>".toRegex()
        val matches = regex.findAll(page)

        var listIndex = 0
        matches.forEach { matchResult ->
                    result[listIndex] += "<div>" + matchResult.groupValues[1] + "</div>"
                    listIndex++
                }
    }

    return result
}

fun genNextLaunchContent(): String
{
    val r = khttp.get("https://spaceflightnow.com/launch-schedule/")

    return composeHtmlList("Next Info", parseNlPage(r.text).take(3))
}