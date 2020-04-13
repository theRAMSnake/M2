package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.concurrent.TimeUnit

fun publishFile(filename: String, password: String)
{
    val p = Runtime.getRuntime().exec("./m2tools $password createPage News $filename")
    p.waitFor()
    Runtime.getRuntime().exec("rm $filename")
}

fun genNewsFile(password: String): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    var filecontent = "<head><style>a { text-decoration: none;} </style> </head><body>"

    filecontent += genRedditContent(password)
    try
    {
        filecontent += genHackernewsContent()
    }
    catch(e: Exception)
    {
        filecontent += "<h1>HN Failed </h1>"
    }

    filecontent += genSteamContent()

    try
    {
        filecontent += genNextLaunchContent()
    }
    catch(e: Exception)
    {
        filecontent += "<h1>Next Launch Failed </h1>"
    }

    try
    {
        filecontent += genMetacriticContent()
    }
    catch(e: Exception)
    {
        filecontent += "<h1>Metacritic Failed</h1>"
    }

    filecontent += "</body>"

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile(args[0])
    publishFile(filename, args[0])

    println("Done")
}