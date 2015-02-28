//==============================================================================
// 
// Function KnapSack:
//   Solve the Knapsack problem using dynamic programming
//
//==============================================================================


#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <vector>
#include <ilcplex/ilocplex.h>

/**
 * Returns the maximum value that can be put in a knapsack of capacity W
 * @param W:    Total weight of the knapsack
 * @param wt:   Weight of each object
 * @param val:  Value of each object
 * @param n:    Number of objects
 * @param oSol: Array of the choosen elements
 *              (Assume that it is initialize with n elements !)
 * @return: the optimal value of the knapsack
 */
int knapSack(int W, std::vector<int> wt, std::vector<int> val, int n, IloNumArray oSol);

#endif

