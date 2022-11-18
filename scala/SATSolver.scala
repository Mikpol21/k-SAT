package kSAT
import CNF._


trait SATSolver {
    def solve(cnf: CNF): Boolean
}

object SATSolver{

    val algorithms = List(
        "UnitClause",
        "UnitClauseWithMajority"
    )

    def apply(name: String): SATSolver = 
        name match {
            case "UnitClause" => UnitClause
            case "UC" => UnitClause
            case "UnitClauseWithMajority" => UnitClauseWithMajority
            case "UCM" => UnitClauseWithMajority
            case _ => throw new IllegalArgumentException(s"Unknown SAT solver: $name")
        }

    def run(solver: SATSolver, n: Int, r: Float, testCases: Int): Float = {
        var satisfied = 0
        for(_ <- 0 until testCases){
            val cnf = CNFGenerator.generate(n, (r*n.toFloat).toInt, 3)
            if(solver.solve(cnf)){
                satisfied += 1
            }
        }
        satisfied.toFloat / testCases.toFloat
    }

    def runAll(n: Int, r: Float, testCases: Int): Unit = {
        println(s"Running all SAT solvers on $testCases test cases of $n variables with ration $r")
        for(algorithm <- algorithms){
            val solver = SATSolver(algorithm)
            val ratio = run(solver, n, r, testCases)
            println(s"$algorithm: $ratio")
        }
    }
}