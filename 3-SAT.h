#ifndef THREE_SAT_H
#define THREE_SAT_H

#include <stdbool.h>

// Define constants for maximum clauses and variables
#define MAX_CLAUSES 1000
#define MAX_VARIABLES 100

// Global variables for Persistent Memory Logic Loop (PMLL)
extern bool pmll_unsat_patterns[MAX_CLAUSES][MAX_VARIABLES];
extern int pmll_unsat_count;

// Function Declarations

/** 
 * @brief Stores an unsatisfiable pattern into the PMLL storage.
 * 
 * @param formula Pointer to the CNF formula (array of clauses).
 * @param num_clauses Number of clauses in the formula.
 */
void store_unsat_pattern(int **formula, int num_clauses);

/** 
 * @brief Checks if a given CNF formula matches any known unsatisfiable pattern in PMLL.
 * 
 * @param formula Pointer to the CNF formula (array of clauses).
 * @param num_clauses Number of clauses in the formula.
 * @return true if the formula matches an unsatisfiable pattern, false otherwise.
 */
bool is_unsat_pattern(int **formula, int num_clauses);

/** 
 * @brief Evaluates a CNF formula with a given variable assignment.
 * 
 * @param formula Pointer to the CNF formula (array of clauses).
 * @param assignment Array of variable assignments (0 or 1).
 * @param num_clauses Number of clauses in the formula.
 * @param num_variables Number of variables in the formula.
 * @return true if the formula is satisfied, false otherwise.
 */
bool evaluate_formula(int **formula, int *assignment, int num_clauses, int num_variables);

/** 
 * @brief Enhanced SAT Solver using PMLL and additional logic layers.
 * 
 * @param formula Pointer to the CNF formula (array of clauses).
 * @param num_clauses Number of clauses in the formula.
 * @param num_variables Number of variables in the formula.
 * @param assignment Array to store variable assignments (0 or 1).
 * @param depth Current depth of the recursive solver.
 * @return true if a satisfying assignment is found, false otherwise.
 */
bool enhanced_sat_solver(int **formula, int num_clauses, int num_variables, int *assignment, int depth);

/** 
 * @brief Generates a random 3-SAT instance in CNF format.
 * 
 * @param formula Pointer to the array where the generated clauses will be stored.
 * @param num_clauses Number of clauses to generate.
 * @param num_variables Number of variables in the formula.
 */
void generate_random_cnf(int **formula, int num_clauses, int num_variables);

/** 
 * @brief Generates a random 3-SAT instance and saves it to a file.
 * 
 * @param num_variables Number of variables in the instance.
 * @param num_clauses Number of clauses in the instance.
 * @param filename Name of the file where the instance will be saved.
 */
void generate_3sat_instance(int num_variables, int num_clauses, const char *filename);

/** 
 * @brief Logs the results of a SAT solver execution to a file.
 * 
 * @param solver_name Name of the SAT solver used.
 * @param instance_name Name of the 3-SAT instance file.
 * @param result True if the formula is satisfiable, false otherwise.
 * @param time_taken Time taken to solve the instance.
 * @param iterations Number of iterations performed by the solver.
 */
void log_results(const char *solver_name, const char *instance_name, bool result, double time_taken, int iterations);

#endif // THREE_SAT_H

