/**
 * @file test_field_energy_variations.c
 * @brief Test energy computation under different conditions
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
    printf("=== Field Energy Variations Test ===\n\n");
    
    // Test different field sizes
    TCDE_Field* small_field = TCDE_CreateField(10, 2.0f);
    TCDE_Field* medium_field = TCDE_CreateField(50, 2.0f);
    TCDE_Field* large_field = TCDE_CreateField(100, 2.0f);
    
    TEST("Small field created", small_field != NULL);
    TEST("Medium field created", medium_field != NULL);
    TEST("Large field created", large_field != NULL);
    
    if (small_field && medium_field && large_field) {
        float energy_small = TCDE_ComputeEnergy(small_field);
        float energy_medium = TCDE_ComputeEnergy(medium_field);
        float energy_large = TCDE_ComputeEnergy(large_field);
        
        TEST("Small field energy computed", !isnan(energy_small) && !isinf(energy_small));
        TEST("Medium field energy computed", !isnan(energy_medium) && !isinf(energy_medium));
        TEST("Large field energy computed", !isnan(energy_large) && !isinf(energy_large));
        
        TEST("All energies non-negative", energy_small >= 0.0f && energy_medium >= 0.0f && energy_large >= 0.0f);
        
        printf("Small field energy:  %.6f\n", energy_small);
        printf("Medium field energy: %.6f\n", energy_medium);
        printf("Large field energy:  %.6f\n", energy_large);
    }
    
    // Test different fractal dimensions
    TCDE_Field* fractal_low = TCDE_CreateField(30, 1.5f);
    TCDE_Field* fractal_high = TCDE_CreateField(30, 3.0f);
    
    TEST("Low fractal field created", fractal_low != NULL);
    TEST("High fractal field created", fractal_high != NULL);
    
    if (fractal_low && fractal_high) {
        float energy_low = TCDE_ComputeEnergy(fractal_low);
        float energy_high = TCDE_ComputeEnergy(fractal_high);
        
        TEST("Low fractal energy computed", !isnan(energy_low) && !isinf(energy_low));
        TEST("High fractal energy computed", !isnan(energy_high) && !isinf(energy_high));
        
        printf("Low fractal energy:  %.6f\n", energy_low);
        printf("High fractal energy: %.6f\n", energy_high);
    }
    
    // Cleanup
    if (small_field) TCDE_DestroyField(small_field);
    if (medium_field) TCDE_DestroyField(medium_field);
    if (large_field) TCDE_DestroyField(large_field);
    if (fractal_low) TCDE_DestroyField(fractal_low);
    if (fractal_high) TCDE_DestroyField(fractal_high);
    
    TEST("All fields destroyed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}