package kSAT

object Util {

    val debug = false

    def timed[A](name: String)(x: => A) = {
        val start = System.currentTimeMillis()
        val result = x
        val end = System.currentTimeMillis()
        if(debug) println(s"$name took ${end-start} ms")
        result
    }
}