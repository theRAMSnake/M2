package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.concurrent.TimeUnit

fun publishFile(filename: String, password: String)
{
    val p = Runtime.getRuntime().exec("./m2tools $password News $filename")
    p.waitFor()
    Runtime.getRuntime().exec("rm $filename")
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    var filecontent = genRedditContent()
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
        filecontent += genOpenCriticContent()
    }
    catch(e: Exception)
    {
        filecontent += "<h1>OC Failed </h1>"
    }

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

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    publishFile(filename, args[0])

    println("Done")
}