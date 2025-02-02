#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "three_sat.h"

// Function to log results to a file
void log_results(const char *solver_name, const char *instance_name, bool result, double time_taken, int iterations) {
    FILE *log_file = fopen("sat_comparison_log.txt", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(log_file, "Solver: %s, Instance: %s, Result: %s, Time: %.6f seconds, Iterations: %d\n",
            solver_name, instance_name, result ? "SATISFIABLE" : "UNSATISFIABLE", time_taken, iterations);

    fclose(log_file);
}

// Placeholder: Load CNF formula from a file
int **load_cnf(const char *filename) {
    // Implement CNF file parsing
    // Return a dynamically allocated 2D array representing the formula
    printf("Loading CNF formula from file: %s\n", filename);
    return NULL;
}

// Placeholder: Get the number of iterations for PMLL solver
int get_pml_iterations() {
    // Implement logic to return the number of iterations for the PMLL solver
    return 0;
}

// Placeholder: Run MiniSat solver
void run_miniSat(const char *filename) {
    // Integrate MiniSat or use an external process to solve the CNF instance
    printf("Running MiniSat solver on instance: %s\n", filename);
}

// Function to free the formula memory
void free_formula(int **formula, int num_clauses) {
    if (formula == NULL) return;
    for (int i = 0; i < num_clauses; i++) {
        free(formula[i]);
    }
    free(formula);
}

// Function to run tests for both PMLL and MiniSat solvers
void run_comparison_tests(int num_variables, int num_clauses) {
    char filename[256];
    sprintf(filename, "3sat_instance_%d_%d.cnf", num_variables, num_clauses);

    // Step 1: Generate random SAT instance
    generate_3sat_instance(num_variables, num_clauses, filename);

    // Step 2: Run the PMLL algorithm
    clock_t start_time = clock();
    int **formula = load_cnf(filename);  // Load the generated CNF formula
    if (formula == NULL) {
        fprintf(stderr, "Error: Failed to load CNF formula from file: %s\n", filename);
        return;
    }

    bool pml_result = enhanced_sat_solver(formula, num_clauses, num_variables, NULL, 0);
    clock_t end_time = clock();
    double pml_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    int pml_iterations = get_pml_iterations();  // Assuming this function tracks the number of iterations
    log_results("PMLL", filename, pml_result, pml_time, pml_iterations);

    printf("PMLL Algorithm: %s (Time: %.6f seconds, Iterations: %d)\n", 
           pml_result ? "Satisfiable" : "Unsatisfiable", pml_time, pml_iterations);

    // Step 3: Run MiniSat solver and measure time
    start_time = clock();
    run_miniSat(filename);
    end_time = clock();
    double miniSat_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    log_results("MiniSat", filename, pml_result, miniSat_time, -1);  // Iterations are not available from MiniSat directly

    printf("MiniSat: Time: %.6f seconds\n", miniSat_time);

    // Free resources
    free_formula(formula, num_clauses);
}

int main() {
    // Set the number of variables and clauses for the test
    int num_variables = 50;
    int num_clauses = 150;

    printf("Running comparison tests for PMLL and MiniSat solvers...\n");

    // Run the comparison tests
    run_comparison_tests(num_variables, num_clauses);

    printf("Comparison tests completed.\n");
    return 0;
}
