package kSAT
import scala.collection.immutable.Vector

class CNFGenerator(n: Int, m: Int){

    def generate(): CNF  = {
        var clauses: Vector[Vars] = Vector.empty
        for(i <- 0 until m)
            clauses = clauses :+ generateClause()
    }

    private def generateClause(): Clause = ???
}