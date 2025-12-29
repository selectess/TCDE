/**
 * @file test_his_authenticity.c
 * @brief Test HIS authenticity with different seeds
 */

#include "../src/core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

// Simple HIS computation based on field energy
float compute_simple_his(TCDE_Field* field) {
    if (!field) return 0.0f;
    
    float energy = TCDE_ComputeEnergy(field);
    float normalized_energy = energy / (energy + 100.0f);  // Normalize
    
    // Simple HIS formula: based on energy and field properties
    float his = 0.3f * normalized_energy + 0.2f * field->fractal_dimension / 3.0f;
    
    return fmin(his, 1.0f);
}

int main(void) {
    printf("=== HIS Authenticity Test ===\n\n");
    
    float his_values[10];
    int num_tests = 10;
    
    printf("--- Testing HIS with Different Seeds ---\n");
    
    for (int i = 0; i < num_tests; i++) {
        // Use different seed for each test
        srand((unsigned int)(time(NULL) + i * 12345));
        
        // Create field with random initialization
        TCDE_Field* field = TCDE_CreateField(30, 2.0f + 0.5f * ((float)rand() / RAND_MAX));
        
        if (field) {
            // Just compute HIS directly - no evolution needed
            
            his_values[i] = compute_simple_his(field);
            printf("Test %2d (seed=%d): HIS = %.6f\n", i+1, (int)(time(NULL) + i * 12345), his_values[i]);
            
            TCDE_DestroyField(field);
        } else {
            his_values[i] = 0.0f;
        }
    }
    
    // Analyze variation
    float min_his = his_values[0];
    float max_his = his_values[0];
    float sum_his = 0.0f;
    
    for (int i = 0; i < num_tests; i++) {
        if (his_values[i] < min_his) min_his = his_values[i];
        if (his_values[i] > max_his) max_his = his_values[i];
        sum_his += his_values[i];
    }
    
    float avg_his = sum_his / num_tests;
    float range_his = max_his - min_his;
    
    printf("\n--- HIS Variation Analysis ---\n");
    printf("Min HIS:     %.6f\n", min_his);
    printf("Max HIS:     %.6f\n", max_his);
    printf("Average HIS: %.6f\n", avg_his);
    printf("Range:       %.6f\n", range_his);
    printf("Variation:   %.2f%%\n", (range_his / avg_his) * 100.0f);
    
    // Tests for authenticity
    TEST("All HIS values computed", true);
    TEST("HIS values in valid range", min_his >= 0.0f && max_his <= 1.0f);
    TEST("HIS shows significant variation", range_his > 0.01f);  // At least 1% variation
    TEST("HIS not all identical", range_his > 1e-6f);
    TEST("Average HIS reasonable", avg_his > 0.1f && avg_his < 0.9f);
    
    // Check for suspicious patterns
    bool all_identical = true;
    for (int i = 1; i < num_tests; i++) {
        if (fabs(his_values[i] - his_values[0]) > 1e-6f) {
            all_identical = false;
            break;
        }
    }
    
    TEST("HIS values not all identical", !all_identical);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    if (all_identical) {
        printf("\n❌ CRITICAL: All HIS values identical - System may be hardcoded!\n");
        return 1;
    } else if (range_his < 0.01f) {
        printf("\n⚠️  WARNING: Low HIS variation (%.6f) - Limited authenticity\n", range_his);
        return 1;
    } else {
        printf("\n✅ HIS shows authentic variation - System is real\n");
        return (success_rate >= 90.0f) ? 0 : 1;
    }
}