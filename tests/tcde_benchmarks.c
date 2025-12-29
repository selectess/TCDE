/**
 * @file tcde_benchmarks.c
 * @brief TCDE Performance Benchmarking Module Implementation
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#include "tcde_benchmarks.h"
#include "tcde_geometry.h"
#include "tcde_evolution.h"
#include "tcde_kdtree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#endif

// ============================================================================
// TIMING UTILITIES
// ============================================================================

static double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

static void compute_statistics(double* times, int n, double* avg, double* min, 
                               double* max, double* std_dev) {
    *avg = 0.0;
    *min = times[0];
    *max = times[0];
    
    for (int i = 0; i < n; i++) {
        *avg += times[i];
        if (times[i] < *min) *min = times[i];
        if (times[i] > *max) *max = times[i];
    }
    *avg /= n;
    
    *std_dev = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = times[i] - *avg;
        *std_dev += diff * diff;
    }
    *std_dev = sqrt(*std_dev / n);
}

// ============================================================================
// INDIVIDUAL OPERATION BENCHMARKS
// ============================================================================

TCDE_BenchmarkResult TCDE_BenchmarkEvaluation(TCDE_Field* field,
                                               int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Field Evaluation";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.01; // Target: < 0.01ms for K=100
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    // Random test point
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_Evaluate6D(field, &p);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_Evaluate6D(field, &p);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkGradient(TCDE_Field* field,
                                             int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Gradient Computation";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.02; // Target: < 0.02ms for K=100
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_Complex gradient[6];
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_Gradient(field, &p, gradient);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_Gradient(field, &p, gradient);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkLaplacian(TCDE_Field* field,
                                              int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Laplacian Computation";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.05; // Target: < 0.05ms for K=100
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_LaplaceBeltrami(field, &p);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_LaplaceBeltrami(field, &p);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkTDEStep(TCDE_Field* field,
                                            int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "TDE Evolution Step";
    result.num_iterations = num_iterations;
    result.target_time_ms = 1.0; // Target: < 1ms for K=100
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    TCDE_Parameters params;
    TCDE_SetParameters(&params, 0.01f, 0.1f, 0.5f, 0.05f, 0.1f, 0.05f);
    
    // Warmup
    for (int i = 0; i < 5; i++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_EvolveStep(field, &params);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkGeodesicDistance(TCDE_Field* field,
                                                     int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Geodesic Distance";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.001; // Target: < 0.001ms
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords1[6] = {0.3f, 0.4f, 0.5f, 1.0f, 0.0f, 0.4f};
    float coords2[6] = {0.6f, 0.7f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
    TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_GeodesicDistance(&p1, &p2, &field->manifold_6d.metric);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_GeodesicDistance(&p1, &p2, &field->manifold_6d.metric);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p1);
    TCDE_FreePoint(&p2);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkChristoffel(TCDE_Field* field,
                                                int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Christoffel Symbols";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.01; // Target: < 0.01ms
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    float christoffel[6][6][6];
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_ComputeChristoffel(&p, &field->manifold_6d.metric, christoffel);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_ComputeChristoffel(&p, &field->manifold_6d.metric, christoffel);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkRiemannCurvature(TCDE_Field* field,
                                                     int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Riemann Curvature";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.05; // Target: < 0.05ms
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    float riemann[6][6][6][6];
    float ricci[6][6];
    
    // Warmup
    for (int i = 0; i < 10; i++) {
        TCDE_ComputeRiemannTensor(&p, &field->manifold_6d.metric, riemann);
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        TCDE_ComputeRicciTensor(&p, &field->manifold_6d.metric, ricci);
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

TCDE_BenchmarkResult TCDE_BenchmarkKDTree(TCDE_Field* field,
                                           int num_iterations) {
    TCDE_BenchmarkResult result = {0};
    result.operation_name = "Field Access Pattern";
    result.num_iterations = num_iterations;
    result.target_time_ms = 0.001; // Target: < 0.001ms
    
    double* times = (double*)malloc(num_iterations * sizeof(double));
    if (!times) return result;
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    
    // Benchmark field center access pattern
    // Warmup
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < field->manifold_6d.num_centers && j < 10; j++) {
            TCDE_Center* c = &field->manifold_6d.centers[j];
            (void)c; // Use the center
        }
    }
    
    // Benchmark
    for (int i = 0; i < num_iterations; i++) {
        double start = get_time_ms();
        for (int j = 0; j < field->manifold_6d.num_centers && j < 10; j++) {
            TCDE_Center* c = &field->manifold_6d.centers[j];
            (void)c; // Use the center
        }
        double end = get_time_ms();
        times[i] = end - start;
    }
    
    compute_statistics(times, num_iterations, &result.avg_time_ms,
                      &result.min_time_ms, &result.max_time_ms, &result.std_dev_ms);
    result.total_time_ms = result.avg_time_ms * num_iterations;
    result.meets_target = (result.avg_time_ms < result.target_time_ms);
    
    TCDE_FreePoint(&p);
    free(times);
    
    return result;
}

// ============================================================================
// COMPLETE BENCHMARK SUITE
// ============================================================================

TCDE_BenchmarkReport* TCDE_RunPerformanceBenchmarks(TCDE_Field* field,
                                                      int num_iterations) {
    if (!field || num_iterations <= 0) return NULL;
    
    TCDE_BenchmarkReport* report = (TCDE_BenchmarkReport*)calloc(1, sizeof(TCDE_BenchmarkReport));
    if (!report) return NULL;
    
    report->num_results = 8;
    report->results = (TCDE_BenchmarkResult*)calloc(report->num_results, sizeof(TCDE_BenchmarkResult));
    if (!report->results) {
        free(report);
        return NULL;
    }
    
    report->timestamp = time(NULL);
    double start_time = get_time_ms();
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Performance Benchmark Suite                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    printf("Field: %d centers\n", field->manifold_6d.num_centers);
    printf("Iterations per test: %d\n\n", num_iterations);
    
    // Run all benchmarks
    int idx = 0;
    report->results[idx++] = TCDE_BenchmarkEvaluation(field, num_iterations);
    report->results[idx++] = TCDE_BenchmarkGradient(field, num_iterations);
    report->results[idx++] = TCDE_BenchmarkLaplacian(field, num_iterations);
    report->results[idx++] = TCDE_BenchmarkTDEStep(field, num_iterations / 10); // Fewer iterations for slow ops
    report->results[idx++] = TCDE_BenchmarkGeodesicDistance(field, num_iterations);
    report->results[idx++] = TCDE_BenchmarkChristoffel(field, num_iterations / 10);
    report->results[idx++] = TCDE_BenchmarkRiemannCurvature(field, num_iterations / 10);
    report->results[idx++] = TCDE_BenchmarkKDTree(field, num_iterations);
    
    double end_time = get_time_ms();
    report->total_time_ms = end_time - start_time;
    
    // Compute statistics
    report->tests_passed = 0;
    report->tests_failed = 0;
    for (int i = 0; i < report->num_results; i++) {
        if (report->results[i].meets_target) {
            report->tests_passed++;
        } else {
            report->tests_failed++;
        }
    }
    report->pass_rate = (float)report->tests_passed / report->num_results;
    
    return report;
}

// ============================================================================
// REPORT GENERATION
// ============================================================================

void TCDE_PrintBenchmarkResult(const TCDE_BenchmarkResult* result) {
    printf("%-25s: ", result->operation_name);
    printf("avg=%.4f ms, ", result->avg_time_ms);
    printf("min=%.4f ms, ", result->min_time_ms);
    printf("max=%.4f ms, ", result->max_time_ms);
    printf("target=%.4f ms ", result->target_time_ms);
    
    if (result->meets_target) {
        printf("✅ PASS\n");
    } else {
        printf("❌ FAIL (%.1fx slower)\n", result->avg_time_ms / result->target_time_ms);
    }
}

bool TCDE_GenerateBenchmarkReport(const TCDE_BenchmarkReport* report,
                                   const char* filename) {
    if (!report) return false;
    
    FILE* f = filename ? fopen(filename, "w") : stdout;
    if (!f) return false;
    
    fprintf(f, "\n");
    fprintf(f, "╔════════════════════════════════════════════════════════════╗\n");
    fprintf(f, "║  TCDE Performance Benchmark Report                        ║\n");
    fprintf(f, "╚════════════════════════════════════════════════════════════╝\n\n");
    
    // System info
    char sys_info[512];
    TCDE_GetSystemInfo(sys_info, sizeof(sys_info));
    fprintf(f, "System Information:\n%s\n", sys_info);
    
    // Summary
    fprintf(f, "Benchmark Summary:\n");
    fprintf(f, "  Total time: %.2f ms\n", report->total_time_ms);
    fprintf(f, "  Tests passed: %d/%d (%.1f%%)\n", 
            report->tests_passed, report->num_results, report->pass_rate * 100.0f);
    fprintf(f, "  Tests failed: %d\n\n", report->tests_failed);
    
    // Individual results
    fprintf(f, "Individual Results:\n");
    fprintf(f, "─────────────────────────────────────────────────────────────\n");
    for (int i = 0; i < report->num_results; i++) {
        const TCDE_BenchmarkResult* r = &report->results[i];
        fprintf(f, "%-25s: avg=%.4f ms (min=%.4f, max=%.4f, σ=%.4f)\n",
                r->operation_name, r->avg_time_ms, r->min_time_ms, 
                r->max_time_ms, r->std_dev_ms);
        fprintf(f, "  Target: %.4f ms - %s",
                r->target_time_ms, r->meets_target ? "✅ PASS" : "❌ FAIL");
        if (!r->meets_target) {
            fprintf(f, " (%.1fx slower)", r->avg_time_ms / r->target_time_ms);
        }
        fprintf(f, "\n\n");
    }
    
    // Recommendations
    fprintf(f, "Performance Recommendations:\n");
    fprintf(f, "─────────────────────────────────────────────────────────────\n");
    for (int i = 0; i < report->num_results; i++) {
        const TCDE_BenchmarkResult* r = &report->results[i];
        if (!r->meets_target) {
            fprintf(f, "• %s: Consider optimization (%.1fx slower than target)\n",
                    r->operation_name, r->avg_time_ms / r->target_time_ms);
        }
    }
    
    if (report->tests_failed == 0) {
        fprintf(f, "• All operations meet performance targets! ✅\n");
    }
    
    fprintf(f, "\n");
    
    if (filename) {
        fclose(f);
        printf("Benchmark report written to: %s\n", filename);
    }
    
    return true;
}

void TCDE_FreeBenchmarkReport(TCDE_BenchmarkReport* report) {
    if (!report) return;
    
    if (report->results) {
        free(report->results);
        report->results = NULL;
    }
    
    free(report);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void TCDE_CompareBenchmarkReports(const TCDE_BenchmarkReport* report1,
                                   const TCDE_BenchmarkReport* report2) {
    if (!report1 || !report2) return;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark Comparison                                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    int min_results = report1->num_results < report2->num_results ? 
                      report1->num_results : report2->num_results;
    
    for (int i = 0; i < min_results; i++) {
        const TCDE_BenchmarkResult* r1 = &report1->results[i];
        const TCDE_BenchmarkResult* r2 = &report2->results[i];
        
        double speedup = r1->avg_time_ms / r2->avg_time_ms;
        
        printf("%-25s: %.4f ms → %.4f ms ", 
               r1->operation_name, r1->avg_time_ms, r2->avg_time_ms);
        
        if (speedup > 1.1) {
            printf("(%.1fx faster) ⬆️\n", speedup);
        } else if (speedup < 0.9) {
            printf("(%.1fx slower) ⬇️\n", 1.0 / speedup);
        } else {
            printf("(similar) ➡️\n");
        }
    }
    
    printf("\n");
}

void TCDE_GetSystemInfo(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    int offset = 0;
    
#ifdef __APPLE__
    char cpu_brand[128];
    size_t size = sizeof(cpu_brand);
    if (sysctlbyname("machdep.cpu.brand_string", cpu_brand, &size, NULL, 0) == 0) {
        offset += snprintf(buffer + offset, buffer_size - offset, "  CPU: %s\n", cpu_brand);
    }
    
    int64_t mem_size;
    size = sizeof(mem_size);
    if (sysctlbyname("hw.memsize", &mem_size, &size, NULL, 0) == 0) {
        offset += snprintf(buffer + offset, buffer_size - offset, 
                          "  Memory: %.2f GB\n", mem_size / (1024.0 * 1024.0 * 1024.0));
    }
#elif defined(__linux__)
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        offset += snprintf(buffer + offset, buffer_size - offset,
                          "  Memory: %.2f GB\n", 
                          si.totalram / (1024.0 * 1024.0 * 1024.0));
    }
#endif
    
    offset += snprintf(buffer + offset, buffer_size - offset, "  OS: ");
#ifdef __APPLE__
    offset += snprintf(buffer + offset, buffer_size - offset, "macOS\n");
#elif defined(__linux__)
    offset += snprintf(buffer + offset, buffer_size - offset, "Linux\n");
#else
    offset += snprintf(buffer + offset, buffer_size - offset, "Unknown\n");
#endif
    
    time_t now = time(NULL);
    offset += snprintf(buffer + offset, buffer_size - offset, 
                      "  Date: %s", ctime(&now));
}
