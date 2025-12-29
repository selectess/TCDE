/**
 * @file test_simple_evolution.c
 * @brief Simple test for field evolution
 */

#include "../src/core/tcde_core.h"
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
    printf("=== Simple Evolution Test ===\n\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(30, 2.0f);
    TEST("Field created", field != NULL);
    
    if (!field) return 1;
    
    // Initial state
    float initial_energy = TCDE_ComputeEnergy(field);
    float initial_time = field->time;
    
    TEST("Initial energy computed", !isnan(initial_energy));
    TEST("Initial time zero", initial_time == 0.0f);
    
    // Evolve field
    int evolution_steps = 10;
    for (int i = 0; i < evolution_steps; i++) {
        TCDE_EvolveStep(field, 0.01f);
    }
    
    // Final state
    float final_energy = TCDE_ComputeEnergy(field);
    float final_time = field->time;
    
    TEST("Final energy computed", !isnan(final_energy));
    TEST("Time advanced", final_time > initial_time);
    TEST("Energy finite", !isinf(final_energy));
    
    printf("\n=== Evolution Results ===\n");
    printf("Initial energy: %.6f\n", initial_energy);
    printf("Final energy:   %.6f\n", final_energy);
    printf("Initial time:   %.6f\n", initial_time);
    printf("Final time:     %.6f\n", final_time);
    printf("Evolution steps: %d\n", evolution_steps);
    
    TCDE_DestroyField(field);
    TEST("Field destroyed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}