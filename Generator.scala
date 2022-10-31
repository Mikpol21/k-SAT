package kSAT
import scala.util.Random
import CNF._
import Util.timed

object CNFGenerator{
    val rng = new Random()

    def generate(n: Int, m: Int, k: Int): CNF  = {
        var clauses: Array[Clause] = Array.fill[Clause](m)(Nil)
        timed("generating clauses"){
            for(i <- 0 until m)
                clauses(i) = generateClause(n, k)
        }
        
        timed("Setting all up")(new CNF(clauses.toArray))
    }

    private def generateClause(n: Int, k: Int): Clause = {
        var clause: Clause = Nil
        var seen: Set[Var] = Set.empty
        for(i <- 0 until k) {
            var variable = rng.nextInt(n) + 1
            while(seen contains variable)
                variable = rng.nextInt(n) + 1
            seen = seen + variable
            clause = clause :+ (if(rng.nextBoolean()) Positive(variable) else Negative(variable))
        }
        clause
    }
}