package kSAT
import scala.collection.mutable.Vector
import scala.collection.mutable.Map
import scala.collection.mutable.Set

type Var = Int
type Clause = List[Literal]

trait Literal{
    def variable: Var
    def negate: Literal
}

case class Positive(variable: Var) extends Literal {
    def negate: Literal = Negative(variable)
}
case class Negative(variable: Var) extends Literal {
    def negate: Literal = Positive(variable)
}

case class CNF(clauses: Array[Clause], variables: Set[Var], variablesToClauses: Map[Var, Set[Int]]){
    final val m = clauses.length()
    final val n = variables.length()

    def satisfy(literal: Literal): Unit = {
        val clausesWithVar = variablesToClauses.getOrElse(literal.variable, Set.empty)
        for(clauseId <- clausesWithVar){
            if(clauses(clauseId) contains literal)
                
        }
    }
}


