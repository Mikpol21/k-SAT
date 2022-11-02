package kSAT
import scala.util.Random

object UnitClauseWithMajority extends SATSolver{
    def solve(cnf: CNF): Boolean = {
        val rng = new Random()
        while(cnf.variables.nonEmpty) {
            val unitClauses = cnf.getUnitClauses
            if(unitClauses.nonEmpty){
                
                val unitClause = unitClauses.head
                val literal = cnf.clauses(unitClause).head
                //println(s"choosing ${literal} to satisfy in:\n ${cnf}")               
                cnf.satisfy(literal)
            }
            else{
                val variable = cnf.variables.head
                val clauses = cnf.variablesToClauses.getOrElse(variable, Set.empty)
                val positive = clauses.count(c => cnf.clauses(c).exists(l => l.isPositive))
                val negative = clauses.count(c => cnf.clauses(c).exists(l => l.isNegative))
                if(positive >= negative)
                    cnf.satisfy(Positive(variable))
                else
                    cnf.satisfy(Negative(variable))
            }
        }
        return cnf.isSatisfied
    }
}