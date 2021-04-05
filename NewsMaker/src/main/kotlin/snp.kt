package main

import kotlinx.serialization.*
import kotlinx.serialization.json.Json

@Serializable
data class SnPItem(val company: String, val ticker: String, val weight: Double, val price: Double)

fun genSnpContent(): List<SnPItem>
{
    var result = mutableListOf<SnPItem>()
    val page = khttp.get("https://www.slickcharts.com/sp500").text

    var idx = 0
    var from: Int
    var to: Int

    for (i in 1..20) 
    {
        idx = page.indexOf("<td>$i</td>", idx)

        from = page.indexOf("\">", idx)
        to = page.indexOf("</a></td>", from)
        val company = page.substring(from + 2, to)
        idx = to

        from = page.indexOf("\">", idx)
        to = page.indexOf("</a></td>", from)
        val ticker = page.substring(from + 2, to)
        idx = to

        from = page.indexOf("<td>", idx)
        to = page.indexOf("</td>", from)
        val weight = page.substring(from + 4, to).replace(",", "").toDouble()
        idx = to

        from = page.indexOf("&nbsp;&nbsp;", idx)
        to = page.indexOf("</td>", from)
        val price = page.substring(from + 12, to).replace(",", "").toDouble()
        idx = to
        
        result.add(SnPItem(company, ticker, weight, price))
    }

    return result
}