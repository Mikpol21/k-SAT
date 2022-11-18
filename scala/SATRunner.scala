package kSAT
object SATRunner {
    def main(args: Array[String]): Unit = {
        val solver = SATSolver(args(0))
        val r: Float = args(1).toFloat
        val testInput = List((10, 1000), (100, 1000), (1000, 1000), (10000, 100))//, (100000, 10))
        val k: Int = if(args.length > 2) args(2).toInt else 3
        //val testInput = List((10, 1))
        for((n, testCases) <- testInput) {
            var satisfied = 0
            var numberOfZeroes = 0;
            for(_ <- 0 until testCases){
                val cnf = CNFGenerator.generate(n, (r*n.toFloat).toInt, k)
                if(solver.solve(cnf))
                    satisfied += 1
                else
                    numberOfZeroes += cnf.numberOfZeroClauses
            }
            val ratio: Float = satisfied.toFloat / testCases.toFloat
            val averageZeroes: Float = numberOfZeroes.toFloat / (testCases - satisfied).toFloat
            println(s"$n variables, ratio: $r, run on $testCases instances")
            println(s"satisfied: $ratio\naverage number of zero clauses: $averageZeroes")
        }
    }
}