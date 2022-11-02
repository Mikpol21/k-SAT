package kSAT
import CNF._


trait SATSolver {
    def solve(cnf: CNF): Boolean
}

object SATSolver{
    def apply(name: String): SATSolver = 
        name match {
            case "UnitClause" => UnitClause
            case "UC" => UnitClause
            case "UnitClauseWithMajority" => UnitClauseWithMajority
            case "UCM" => UnitClauseWithMajority
            case _ => throw new IllegalArgumentException(s"Unknown SAT solver: $name")
        }
}