package kSAT
object SATRunner {
    def main(args: Array[String]): Unit = {
        val solver = SATSolver(args(0))
        val r: Float = args(1).toFloat
        val testInput = List((10, 100000), (100, 10000), (1000, 1000), (10000, 100), (100000, 10))

        for((n, testCases) <- testInput) {
            var satisfied = 0
            for(_ <- 0 until testCases){
                val cnf = CNFGenerator.generate(n, (r*n.toFloat).toInt, 3)
                if(solver.solve(cnf))
                    satisfied += 1
            }
            val ratio: Float = satisfied.toFloat / testCases.toFloat
            println(s"num of variables = $n\nsatisfied: $ratio")
        }
    }
}