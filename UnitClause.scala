package kSAT
import scala.util.Random

object UnitClause extends SATSolver{
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
                if(rng.nextBoolean())
                    cnf.satisfy(Positive(variable))
                else
                    cnf.satisfy(Negative(variable))
            }
        }
        cnf.isSatisfied
    }
}