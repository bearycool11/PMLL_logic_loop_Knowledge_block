#ifndef THREE_SAT_LOG_H
#define THREE_SAT_LOG_H

#include <stdbool.h>

// Function Declarations

/**
 * @brief Logs the results of a SAT solver execution to a file.
 *
 * @param solver_name Name of the SAT solver used.
 * @param instance_name Name of the 3-SAT instance file.
 * @param result True if the formula is satisfiable, false otherwise.
 * @param time_taken Time taken to solve the instance, in seconds.
 * @param iterations Number of iterations performed by the solver (or -1 if unavailable).
 */
void log_results(const char *solver_name, const char *instance_name, bool result, double time_taken, int iterations);

/**
 * @brief Loads a CNF formula from a file.
 *
 * @param filename Name of the CNF file to load.
 * @return Pointer to a dynamically allocated 2D array representing the formula.
 */
int **load_cnf(const char *filename);

/**
 * @brief Retrieves the number of iterations performed by the PMLL SAT solver.
 *
 * @return Number of iterations performed during the last execution.
 */
int get_pml_iterations();

/**
 * @brief Runs the MiniSat solver on a given CNF file.
 *
 * @param filename Name of the CNF file to solve.
 */
void run_miniSat(const char *filename);

/**
 * @brief Frees the dynamically allocated memory for a CNF formula.
 *
 * @param formula Pointer to the 2D array representing the CNF formula.
 * @param num_clauses Number of clauses in the formula.
 */
void free_formula(int **formula, int num_clauses);

/**
 * @brief Runs comparison tests for PMLL and MiniSat solvers.
 *
 * @param num_variables Number of variables in the 3-SAT instance.
 * @param num_clauses Number of clauses in the 3-SAT instance.
 */
void run_comparison_tests(int num_variables, int num_clauses);

#endif // THREE_SAT_LOG_H
