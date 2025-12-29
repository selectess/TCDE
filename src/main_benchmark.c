#include "core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * TCDE Benchmark - Performance testing of TCDE functionality
 */
int main(void) {
    printf("🏃 TCDE Benchmark Starting...\n");
    printf("=============================\n");
    
    clock_t start = clock();
    
    // Create a larger TCDE field for benchmarking
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    if (!field) {
        printf("❌ Failed to create TCDE field\n");
        return EXIT_FAILURE;
    }
    
    printf("✅ TCDE Field created for benchmarking\n");
    printf("   Capacity: %d centers\n", field->manifold_6d.capacity);
    
    // Simple benchmark: measure field creation time
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("⏱️  Field creation time: %.6f seconds\n", cpu_time_used);
    
    // Clean up
    TCDE_DestroyField(field);
    
    printf("✅ Benchmark completed successfully\n");
    return EXIT_SUCCESS;
}