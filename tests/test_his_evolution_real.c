/**
 * @file test_his_evolution_real.c
 * @brief Test if HIS actually changes during system evolution
 */

#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int tests_passed = 0;
static int tests_total = 0;

#define TEST(name, condition) do { \
    tests_total++; \
    if (condition) { \
        tests_passed++; \
        printf("✓ %s\n", name); \
    } else { \
        printf("✗ %s\n", name); \
    } \
} while(0)

int main(void) {
    printf("=== HIS Evolution Real Test ===\n\n");
    
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentitySystem(50, 2.5f);
    TEST("System created", system != NULL);
    
    if (!system) return 1;
    
    // Measure HIS at different evolution stages
    printf("--- HIS Evolution Tracking ---\n");
    
    TCDE_11D_Identity_Metrics metrics;
    float his_values[10];
    int num_measurements = 10;
    int steps_between = 50;
    
    for (int i = 0; i < num_measurements; i++) {
        // Evolve system
        for (int j = 0; j < steps_between; j++) {
            TCDE_Evolve11DIdentitySystem(system, 0.01f);
        }
        
        // Measure HIS
        TCDE_Measure11DIdentityMetrics(system, &metrics);
        his_values[i] = TCDE_ComputeHolisticIdentityScore(&metrics);
        
        printf("Step %3d: HIS = %.6f (time=%.3f)\n", 
               (i+1) * steps_between, his_values[i], system->time);
    }
    
    // Analyze if HIS actually changes
    float min_his = his_values[0];
    float max_his = his_values[0];
    float sum_his = 0.0f;
    
    for (int i = 0; i < num_measurements; i++) {
        if (his_values[i] < min_his) min_his = his_values[i];
        if (his_values[i] > max_his) max_his = his_values[i];
        sum_his += his_values[i];
    }
    
    float avg_his = sum_his / num_measurements;
    float his_range = max_his - min_his;
    float his_variance = 0.0f;
    
    for (int i = 0; i < num_measurements; i++) {
        float diff = his_values[i] - avg_his;
        his_variance += diff * diff;
    }
    his_variance /= num_measurements;
    
    printf("\n--- HIS Analysis ---\n");
    printf("Min HIS:     %.6f\n", min_his);
    printf("Max HIS:     %.6f\n", max_his);
    printf("Average HIS: %.6f\n", avg_his);
    printf("Range:       %.6f\n", his_range);
    printf("Variance:    %.6f\n", his_variance);
    printf("Std Dev:     %.6f\n", sqrt(his_variance));
    
    // Tests for authentic evolution
    TEST("All HIS values valid", min_his >= 0.0f && max_his <= 1.0f);
    TEST("HIS shows variation", his_range > 0.001f);  // At least 0.1% variation
    TEST("HIS variance non-zero", his_variance > 1e-8f);
    TEST("System time advanced", system->time > 0.0f);
    TEST("Evolution steps recorded", system->evolution_steps > 0);
    
    // Check if HIS is not hardcoded (should vary between runs)
    printf("\n--- Authenticity Check ---\n");
    
    // Create second system with different seed
    srand(12345);  // Different seed
    TCDE_11D_Identity_System* system2 = TCDE_Create11DIdentitySystem(50, 2.5f);
    
    if (system2) {
        // Evolve second system same amount
        for (int i = 0; i < num_measurements * steps_between; i++) {
            TCDE_Evolve11DIdentitySystem(system2, 0.01f);
        }
        
        TCDE_Measure11DIdentityMetrics(system2, &metrics);
        float his2 = TCDE_ComputeHolisticIdentityScore(&metrics);
        
        printf("System 1 final HIS: %.6f\n", his_values[num_measurements-1]);
        printf("System 2 final HIS: %.6f\n", his2);
        printf("Difference:         %.6f\n", fabs(his_values[num_measurements-1] - his2));
        
        TEST("Different systems give different HIS", fabs(his_values[num_measurements-1] - his2) > 0.001f);
        
        TCDE_Destroy11DIdentitySystem(system2);
    }
    
    TCDE_Destroy11DIdentitySystem(system);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    if (his_range < 0.001f) {
        printf("\n❌ WARNING: HIS shows minimal variation (%.6f)\n", his_range);
        printf("   This suggests the system may not be evolving authentically\n");
        return 1;
    } else {
        printf("\n✅ HIS shows authentic evolution (range: %.6f)\n", his_range);
        return (success_rate >= 90.0f) ? 0 : 1;
    }
}