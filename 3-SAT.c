#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Maximum values
#define MAX_VARIABLES 100
#define MAX_CLAUSES 1000

// Persistent Memory Logic Loop (PMLL)
bool pmll_unsat_patterns[MAX_CLAUSES][MAX_VARIABLES];
int pmll_unsat_count = 0;

// Function Prototypes
void store_unsat_pattern(int **formula, int num_clauses);
bool is_unsat_pattern(int **formula, int num_clauses);
bool evaluate_formula(int **formula, int *assignment, int num_clauses, int num_variables);
bool enhanced_sat_solver(int **formula, int num_clauses, int num_variables, int *assignment, int depth);
void generate_random_cnf(int **formula, int num_clauses, int num_variables);
void run_test_case(const char *test_name, int **formula, int num_clauses, int num_variables);
void free_formula(int **formula, int num_clauses);

// Store unsatisfiable patterns in PMLL
void store_unsat_pattern(int **formula, int num_clauses) {
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            pmll_unsat_patterns[pmll_unsat_count][i * 3 + j] = formula[i][j];
        }
    }
    pmll_unsat_count++;
}

// Check if a pattern is already flagged as unsatisfiable
bool is_unsat_pattern(int **formula, int num_clauses) {
    for (int p = 0; p < pmll_unsat_count; p++) {
        bool match = true;
        for (int i = 0; i < num_clauses; i++) {
            for (int j = 0; j < 3; j++) {
                if (pmll_unsat_patterns[p][i * 3 + j] != formula[i][j]) {
                    match = false;
                    break;
                }
            }
            if (!match) break;
        }
        if (match) return true;
    }
    return false;
}

// Evaluate a CNF formula
bool evaluate_formula(int **formula, int *assignment, int num_clauses, int num_variables) {
    for (int i = 0; i < num_clauses; i++) {
        bool clause_satisfied = false;
        for (int j = 0; j < 3; j++) {
            int var = formula[i][j];
            if ((var > 0 && assignment[var - 1] == 1) || (var < 0 && assignment[-var - 1] == 0)) {
                clause_satisfied = true;
                break;
            }
        }
        if (!clause_satisfied) {
            return false;
        }
    }
    return true;
}

// Enhanced SAT Solver
bool enhanced_sat_solver(int **formula, int num_clauses, int num_variables, int *assignment, int depth) {
    if (is_unsat_pattern(formula, num_clauses)) {
        return false;
    }

    if (depth == num_variables) {
        if (evaluate_formula(formula, assignment, num_clauses, num_variables)) {
            return true;
        } else {
            store_unsat_pattern(formula, num_clauses);
            return false;
        }
    }

    assignment[depth] = 0;
    if (enhanced_sat_solver(formula, num_clauses, num_variables, assignment, depth + 1)) {
        return true;
    }

    assignment[depth] = 1;
    return enhanced_sat_solver(formula, num_clauses, num_variables, assignment, depth + 1);
}

// Generate random CNF formula
void generate_random_cnf(int **formula, int num_clauses, int num_variables) {
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            int var = (rand() % num_variables) + 1;
            if (rand() % 2 == 0) var = -var;
            formula[i][j] = var;
        }
    }
}

// Run a test case
void run_test_case(const char *test_name, int **formula, int num_clauses, int num_variables) {
    int *assignment = (int *)calloc(num_variables, sizeof(int));
    printf("\n%s:\n", test_name);

    if (enhanced_sat_solver(formula, num_clauses, num_variables, assignment, 0)) {
        printf("Satisfying assignment found:\n");
        for (int i = 0; i < num_variables; i++) {
            printf("x%d = %d\n", i + 1, assignment[i]);
        }
    } else {
        printf("No satisfying assignment exists.\n");
    }

    free(assignment);
}

// Free allocated memory for formula
void free_formula(int **formula, int num_clauses) {
    for (int i = 0; i < num_clauses; i++) {
        free(formula[i]);
    }
    free(formula);
}

// Main function
int main() {
    srand(time(0));

    // Test Case 1: Predefined satisfiable formula
    int **formula1 = (int **)malloc(4 * sizeof(int *));
    for (int i = 0; i < 4; i++) formula1[i] = (int *)malloc(3 * sizeof(int));
    formula1[0][0] = 1; formula1[0][1] = 2; formula1[0][2] = 3;
    formula1[1][0] = -1; formula1[1][1] = 4; formula1[1][2] = 5;
    formula1[2][0] = -2; formula1[2][1] = -3; formula1[2][2] = 6;
    formula1[3][0] = 1; formula1[3][1] = -4; formula1[3][2] = 6;
    run_test_case("Test Case 1: Predefined Satisfiable Formula", formula1, 4, 6);
    free_formula(formula1, 4);

    // Test Case 2: Predefined unsatisfiable formula
    int **formula2 = (int **)malloc(3 * sizeof(int *));
    for (int i = 0; i < 3; i++) formula2[i] = (int *)malloc(3 * sizeof(int));
    formula2[0][0] = 1; formula2[0][1] = 2; formula2[0][2] = 0;
    formula2[1][0] = -1; formula2[1][1] = -2; formula2[1][2] = 0;
    formula2[2][0] = 1; formula2[2][1] = -1; formula2[2][2] = 0;
    run_test_case("Test Case 2: Predefined Unsatisfiable Formula", formula2, 3, 4);
    free_formula(formula2, 3);

    // Test Case 3: Random formula
    int num_variables = 8, num_clauses = 6;
    int **formula3 = (int **)malloc(num_clauses * sizeof(int *));
    for (int i = 0; i < num_clauses; i++) formula3[i] = (int *)malloc(3 * sizeof(int));
    generate_random_cnf(formula3, num_clauses, num_variables);
    run_test_case("Test Case 3: Random Formula", formula3, num_clauses, num_variables);
    free_formula(formula3, num_clauses);

    return 0;
}
