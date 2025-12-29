/**
 * @file test_benchmarks.c
 * @brief Test for TCDE Benchmarks Module (Task 17.1)
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#include "../tests/tcde_benchmarks.h"
#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Benchmarks Module Test                              ║\n");
    printf("║  Task 17.1: Performance Benchmarking                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Create a test field with 100 centers
    printf("\nCreating test field with 100 centers...\n");
    TCDE_Field* field = TCDE_CreateField(100, 2.0f);
    if (!field) {
        printf("❌ Failed to create field\n");
        return 1;
    }
    
    // Add centers
    for (int i = 0; i < 100; i++) {
        float coords[6] = {
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            1.0f,
            0.0f,
            0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.5f + 0.2f * I;
        TCDE_AddCenter6D(field, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    printf("Field created with %d centers\n", field->manifold_6d.num_centers);
    
    // Run benchmarks
    printf("\nRunning performance benchmarks...\n");
    TCDE_BenchmarkReport* report = TCDE_RunPerformanceBenchmarks(field, 1000);
    
    if (!report) {
        printf("❌ Failed to run benchmarks\n");
        TCDE_DestroyField(field);
        return 1;
    }
    
    // Print results
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark Results                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    for (int i = 0; i < report->num_results; i++) {
        TCDE_PrintBenchmarkResult(&report->results[i]);
    }
    
    printf("\n");
    printf("Summary:\n");
    printf("  Total time: %.2f ms\n", report->total_time_ms);
    printf("  Tests passed: %d/%d (%.1f%%)\n", 
           report->tests_passed, report->num_results, report->pass_rate * 100.0f);
    printf("  Tests failed: %d\n", report->tests_failed);
    
    // Generate report file
    printf("\nGenerating benchmark report...\n");
    bool report_generated = TCDE_GenerateBenchmarkReport(report, "benchmark_report.txt");
    if (report_generated) {
        printf("✅ Report generated successfully\n");
    }
    
    // Cleanup
    TCDE_FreeBenchmarkReport(report);
    TCDE_DestroyField(field);
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ Benchmarks Module Test Complete                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
