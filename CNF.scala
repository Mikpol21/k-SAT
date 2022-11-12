package kSAT
import scala.collection.mutable.{HashMap, Map, Set, MultiMap}
import CNF._
import Util.timed

trait Literal{
    def variable: Var
    def negate: Literal
    def isPositive: Boolean
    def isNegative: Boolean = !isPositive
}

case class Positive(variable: Var) extends Literal {
    def negate: Literal = Negative(variable)
    def isPositive: Boolean = true
    override def toString: String = variable.toString
}
case class Negative(variable: Var) extends Literal {
    def negate: Literal = Positive(variable)
    def isPositive: Boolean = false
    override def toString: String = "not " + variable.toString
}

class CNF(val clauses: Array[Clause]){

    final val m = clauses.length
    val variables: Set[Var] = Set.empty[Var]
    val variablesToClauses: MultiMap[Var, Int] = new HashMap[Var, Set[Int]] with MultiMap[Var, Int]
    val sizeToClauses: MultiMap[Int, Int] = new HashMap[Int, Set[Int]] with MultiMap[Int, Int]

    timed("Setting up set of variables"){
        for(i <- 0 until m)
            for(literal <- clauses(i))
                variables += literal.variable
    }

    timed("Setting up variablesToClauses")(
        for(i <- 0 until m)
            for(literal <- clauses(i))
                variablesToClauses.addBinding(literal.variable, i)
    )
    timed("Setting up sizeToClauses"){
        for(i <- 0 until m)
            sizeToClauses.addBinding(clauses(i).length, i)
    }

    final val n = variables.size



    def getUnitClauses: Set[Int] = sizeToClauses.getOrElse(1, Set.empty[Int])


    def getVariableClauses(variable : Var): Set[Int] = variablesToClauses.getOrElse(variable, Set.empty[Int])

    def satisfy(literal: Literal): Unit = {
        val clausesWithVar = getVariableClauses(literal.variable)
        for(clauseId <- clausesWithVar){
            if(clauses(clauseId) contains literal){
                // removing mapping from variable to clause
                for(variable <- clauses(clauseId).map(_.variable))
                    variablesToClauses.removeBinding(variable, clauseId)

                sizeToClauses.removeBinding(clauses(clauseId).length, clauseId)
                clauses(clauseId) = List(Positive(-1)) // marking as satisfied
            }
            else if(clauses(clauseId) contains literal.negate){
                val size = clauses(clauseId).length
                sizeToClauses.removeBinding(size, clauseId)
                clauses(clauseId) = clauses(clauseId).filter(_ != literal.negate)

                sizeToClauses.addBinding(clauses(clauseId).length, clauseId)
            }
        }
        variables -= literal.variable
    }

    override def toString: String = {
        var str = ""
        for(j <- 0 until clauses.length){
            val clause = clauses(j)
            str += "("
            for(i <- 0 until clause.length) {
                str += clause(i).toString
                if(i != clause.length - 1)
                    str += " or "
            }
            str += ")"
            if(j != clauses.length - 1)
                str += " and "
        }
        return str
    }

    def isSatisfied: Boolean = !sizeToClauses.contains(0) || sizeToClauses(0).isEmpty

    def numberOfZeroClauses: Int = sizeToClauses.getOrElse(0, Set.empty[Int]).size


}


object CNF {
    def apply(clauses: Array[Clause]): CNF = new CNF(clauses)
    type Var = Int
    type Clause = List[Literal]
}