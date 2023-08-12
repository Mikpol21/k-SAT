# Satisfiability of random instances of the $k$-SAT problem
In this repository, I've stored the source code used for the my Bachelor's Thesis titled "Algorithmic Satisfiability of the Random $k$-SAT Problem".

The 'src' directory contains implementations of popular $k$-SAT heuristics like Unit Clause and Walksat, as well as more advanced algorithms with theoretical significance. To learn more, please refer to the thesis itself in the file "random_k-SAT.pdf." An abstract of the thesis is provided below.

## Abstract
 
 We study a variation of the pivotal problem in computer science - the random $k$-SAT problem. Here, the goal is to find a satisfying assignment of a randomly generated input formula. Interestingly, instances of the random $k$-SAT problem exhibit a sharp transition, switching from satisfiable to unsatisfiable when $\alpha$ - the ratio of clauses to variables crosses the satisfiability threshold $\alpha_{k}$. Despite the lack of rigorous determinations, empirical results have yielded remarkably precise conjectures for the corresponding satisfiability thresholds in random 3-SAT and 4-SAT instances.
    
One of the approaches for studying the satisfiability threshold is by designing algorithms to find a satisfying assignment since obtaining rigorous guarantees of their success implies the satisfiability of the corresponding  $k$-SAT instance. In this report, we take a closer look at the two most dominant classes of such algorithms for the unresolved cases of random 3-SAT and 4-SAT. 

The first class uses simple rules that exploit the formula's local structure to obtain rigorous performance guarantees. The relative simplicity of these rules enables precise analysis, but it also limits their power, which we will show by proving an upper bound on the performance of one of the most-popular such algorithms.

The second class uses elaborate but non-rigorous techniques inspired by statistical physics with great empirical results around the conjectured satisfiability threshold, but their analysis is still an open problem. We present the two most common algorithms from this class and combine them to obtain a polynomial time algorithm with state-of-the-art results.

Finally, we investigate a potential intersection between these two algorithmic classes by designing an algorithm that combines the efficiency and local perspective of one class with the global perspective of the other.
