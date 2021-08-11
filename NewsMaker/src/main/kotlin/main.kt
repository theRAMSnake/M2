package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.concurrent.TimeUnit

fun publishFile(filename: String)
{
    val lines = File("$filename").readText().replace("\"", "\\\"");
    run{
        val op = "{\"operation\":\"destroy\", \"id\":\"newsfeed\"}"
        val p = Runtime.getRuntime().exec(arrayOf("./m4tools", op))
        p.waitFor()
    }
    run{
        val op = "{\"operation\":\"create\", \"typename\":\"object\", \"defined_id\":\"newsfeed\", \"params\":{\"content\":\"$lines\"}}"
        val p = Runtime.getRuntime().exec(arrayOf("./m4tools", op))
        p.waitFor()
    }
    Runtime.getRuntime().exec("rm $filename")
}

fun publishSnpContent(content: List<SnPItem>)
{
    var contentJson = "{"
    var i = 0
    for (x in content) 
    {
        if(i != 0)
        {
            contentJson += ","
        }

        contentJson += "\"$i\": {"
        contentJson += "\"id\": \"${i}\","
        contentJson += "\"company\": \"${x.company}\","
        contentJson += "\"ticker\": \"${x.ticker}\","
        contentJson += "\"weight\": ${x.weight},"
        contentJson += "\"price\": ${x.price}"
        contentJson += "}"

        i++
    }
    contentJson += "}"

    run{
        val op = "{\"operation\":\"destroy\", \"id\":\"data.snp\"}"
        val p = Runtime.getRuntime().exec(arrayOf("./m4tools", op))
        p.waitFor()
    }
    run{
        val op = "{\"operation\":\"create\", \"typename\":\"object\", \"defined_id\":\"data.snp\", \"params\":$contentJson}"
        val p = Runtime.getRuntime().exec(arrayOf("./m4tools", op))
        p.waitFor()
    }
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    var filecontent = "<head><style>a { text-decoration: none;} </style> </head><body>"

    filecontent += genNlTimesContent()

    filecontent += genRedditContent()
    try
    {
        filecontent += genHackernewsContent()
    }
    catch(e: Exception)
    {
        filecontent += "<h1>HN Failed </h1>"
    }

    filecontent += genSteamContent()

    filecontent += "</body>"

    print(filecontent)

    File(filename).writeText(filecontent)

    return filename
}

fun readPass(): String
{
    return File("/materia/passwd").readText(Charsets.UTF_8)
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    publishFile(filename)

    val snpContent = genSnpContent()
    publishSnpContent(snpContent)

    println("Done")
}