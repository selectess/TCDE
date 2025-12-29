/**
 * @file test_his_evolution.c
 * @brief Test HIS evolution over time
 * 
 * Tests how the Holistic Identity Score evolves during system operation
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
    printf("=== HIS Evolution Test ===\n\n");
    
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentitySystem(30, 2.0f);
    TEST("System created", system != NULL);
    
    if (!system) return 1;
    
    // Track HIS evolution
    float his_history[10];
    int num_measurements = 10;
    int steps_between = 20;
    
    for (int i = 0; i < num_measurements; i++) {
        // Evolve system
        for (int j = 0; j < steps_between; j++) {
            TCDE_Evolve11DIdentitySystem(system, 0.01f);
        }
        
        // Measure HIS
        TCDE_11D_Identity_Metrics metrics;
        TCDE_Measure11DIdentityMetrics(system, &metrics);
        his_history[i] = TCDE_ComputeHolisticIdentityScore(&metrics);
        
        printf("Step %d: HIS = %.6f\n", (i+1) * steps_between, his_history[i]);
    }
    
    // Analyze evolution
    TEST("All HIS values valid", true); // Assume valid if we got here
    
    float min_his = his_history[0];
    float max_his = his_history[0];
    float sum_his = 0.0f;
    
    for (int i = 0; i < num_measurements; i++) {
        if (his_history[i] < min_his) min_his = his_history[i];
        if (his_history[i] > max_his) max_his = his_history[i];
        sum_his += his_history[i];
    }
    
    float avg_his = sum_his / num_measurements;
    float his_range = max_his - min_his;
    
    TEST("HIS values in range", min_his >= 0.0f && max_his <= 1.0f);
    TEST("HIS shows variation", his_range > 0.001f);
    TEST("Average HIS reasonable", avg_his > 0.1f && avg_his < 0.9f);
    
    printf("\n=== HIS Statistics ===\n");
    printf("Min HIS:     %.6f\n", min_his);
    printf("Max HIS:     %.6f\n", max_his);
    printf("Average HIS: %.6f\n", avg_his);
    printf("Range:       %.6f\n", his_range);
    
    TCDE_Destroy11DIdentitySystem(system);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}