/**
 * @file tcde_benchmarks.h
 * @brief TCDE Performance Benchmarking Module
 * 
 * This module provides comprehensive performance benchmarking for all
 * critical TCDE operations. It measures execution times, throughput,
 * and compares results against target performance metrics.
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#ifndef TCDE_BENCHMARKS_H
#define TCDE_BENCHMARKS_H

#include "tcde_core.h"
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// BENCHMARK STRUCTURES
// ============================================================================

/**
 * @brief Single operation benchmark result
 */
typedef struct {
    const char* operation_name;     // Name of operation
    int num_iterations;             // Number of iterations
    double total_time_ms;           // Total time in milliseconds
    double avg_time_ms;             // Average time per iteration
    double min_time_ms;             // Minimum time
    double max_time_ms;             // Maximum time
    double std_dev_ms;              // Standard deviation
    double target_time_ms;          // Target performance
    bool meets_target;              // Whether target is met
} TCDE_BenchmarkResult;

/**
 * @brief Complete benchmark report
 */
typedef struct {
    int num_results;                // Number of benchmark results
    TCDE_BenchmarkResult* results;  // Array of results
    double total_time_ms;           // Total benchmark time
    int tests_passed;               // Number of tests meeting targets
    int tests_failed;               // Number of tests failing targets
    float pass_rate;                // Percentage of tests passed
    time_t timestamp;               // When benchmark was run
} TCDE_BenchmarkReport;

// ============================================================================
// CORE BENCHMARKING FUNCTIONS
// ============================================================================

/**
 * @brief Run complete performance benchmark suite
 * 
 * Benchmarks all critical operations:
 * - Field evaluation
 * - Gradient computation
 * - Laplacian computation
 * - TDE evolution step
 * - Geometric operations
 * - Multimodal processing
 * 
 * @param field Field to benchmark on
 * @param num_iterations Number of iterations per test
 * @return Complete benchmark report (must be freed with TCDE_FreeBenchmarkReport)
 */
TCDE_BenchmarkReport* TCDE_RunPerformanceBenchmarks(TCDE_Field* field,
                                                      int num_iterations);

/**
 * @brief Generate formatted benchmark report
 * 
 * Creates a human-readable report with:
 * - Summary statistics
 * - Individual operation results
 * - Comparison with targets
 * - Performance recommendations
 * 
 * @param report Benchmark report
 * @param filename Output filename (NULL for stdout)
 * @return Success flag
 */
bool TCDE_GenerateBenchmarkReport(const TCDE_BenchmarkReport* report,
                                   const char* filename);

/**
 * @brief Free benchmark report
 * 
 * @param report Report to free
 */
void TCDE_FreeBenchmarkReport(TCDE_BenchmarkReport* report);

// ============================================================================
// INDIVIDUAL OPERATION BENCHMARKS
// ============================================================================

/**
 * @brief Benchmark field evaluation
 * 
 * Target: < 0.01ms for K=100 centers
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkEvaluation(TCDE_Field* field,
                                               int num_iterations);

/**
 * @brief Benchmark gradient computation
 * 
 * Target: < 0.02ms for K=100 centers
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkGradient(TCDE_Field* field,
                                             int num_iterations);

/**
 * @brief Benchmark Laplacian computation
 * 
 * Target: < 0.05ms for K=100 centers
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkLaplacian(TCDE_Field* field,
                                              int num_iterations);

/**
 * @brief Benchmark TDE evolution step
 * 
 * Target: < 1ms for K=100 centers
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkTDEStep(TCDE_Field* field,
                                            int num_iterations);

/**
 * @brief Benchmark geodesic distance computation
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkGeodesicDistance(TCDE_Field* field,
                                                     int num_iterations);

/**
 * @brief Benchmark Christoffel symbols computation
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkChristoffel(TCDE_Field* field,
                                                int num_iterations);

/**
 * @brief Benchmark Riemann curvature computation
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkRiemannCurvature(TCDE_Field* field,
                                                     int num_iterations);

/**
 * @brief Benchmark KD-Tree operations
 * 
 * @param field Field to benchmark
 * @param num_iterations Number of iterations
 * @return Benchmark result
 */
TCDE_BenchmarkResult TCDE_BenchmarkKDTree(TCDE_Field* field,
                                           int num_iterations);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Print benchmark result to console
 * 
 * @param result Result to print
 */
void TCDE_PrintBenchmarkResult(const TCDE_BenchmarkResult* result);

/**
 * @brief Compare two benchmark reports
 * 
 * Useful for tracking performance changes over time
 * 
 * @param report1 First report
 * @param report2 Second report
 */
void TCDE_CompareBenchmarkReports(const TCDE_BenchmarkReport* report1,
                                   const TCDE_BenchmarkReport* report2);

/**
 * @brief Get system performance info
 * 
 * Returns information about CPU, memory, etc.
 * 
 * @param buffer Buffer to write info to
 * @param buffer_size Size of buffer
 */
void TCDE_GetSystemInfo(char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // TCDE_BENCHMARKS_H
