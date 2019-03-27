package main

fun composeHtmlList(name: String, values: List<String>): String
{
   var result = "<h1>$name</h1>"

   for(x in values)
   {
      result += "<li>${x}</li>"
   }

   return result
}

//Should be last due to parser error
fun less(a: Long, b:Long) = a < b