package main

import java.time.LocalDateTime
import java.io.File
import java.util.Calendar
import java.util.concurrent.TimeUnit

fun publishFile(filename: String)
{
    val p = Runtime.getRuntime().exec("./m2tools News $filename")
    p.waitFor()
    Runtime.getRuntime().exec("rm $filename")
}

fun genNewsFile(): String
{
    val cal = java.util.Calendar.getInstance()
    val filename = "${cal.get(Calendar.DAY_OF_MONTH)}_${cal.get(Calendar.MONTH)}_${cal.get(Calendar.YEAR)}"

    var filecontent = genRedditContent()
    filecontent += genHackernewsContent()

    File(filename).writeText(filecontent)

    return filename
}

fun main(args: Array<String>)
{
    val filename = genNewsFile()
    //publishFile(filename)

    println("Done")
}