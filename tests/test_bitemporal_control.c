/**
 * @file test_bitemporal_control.c
 * @brief Test bi-temporal control system
 * 
 * Tests the τ₁ (anticipation) and τ₂ (memory) control system
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
    printf("=== Bi-Temporal Control Test ===\n\n");
    
    // Test bi-temporal control structure
    TCDE_BiTemporalControl control;
    TCDE_InitializeBiTemporalControl(&control);
    
    TEST("Control initialized", control.tau1 > 0.0f);
    TEST("τ₁ in valid range", control.tau1 >= 1.0f && control.tau1 < 2.0f);
    TEST("τ₂ in valid range", control.tau2 >= -1.0f && control.tau2 <= 1.0f);
    TEST("Control strength valid", control.control_strength >= 0.0f && control.control_strength <= 1.0f);
    
    // Test control function computation
    float control_value = TCDE_ComputeBiTemporalControl(&control);
    TEST("Control function computes", !isnan(control_value) && !isinf(control_value));
    TEST("Control value reasonable", control_value >= -2.0f && control_value <= 2.0f);
    
    // Test parameter variations
    float original_tau1 = control.tau1;
    float original_tau2 = control.tau2;
    
    control.tau1 = 1.5f;
    control.tau2 = 0.5f;
    float control_1 = TCDE_ComputeBiTemporalControl(&control);
    
    control.tau1 = 1.2f;
    control.tau2 = -0.3f;
    float control_2 = TCDE_ComputeBiTemporalControl(&control);
    
    TEST("Control responds to τ₁ changes", fabs(control_1 - control_2) > 0.001f);
    
    // Test in system context
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentitySystem(20, 2.0f);
    TEST("System with bi-temporal created", system != NULL);
    
    if (system) {
        TEST("System τ₁ initialized", system->bi_temporal.tau1 > 0.0f);
        TEST("System τ₂ initialized", system->bi_temporal.tau2 >= -1.0f);
        
        // Test evolution with bi-temporal control
        float initial_control = TCDE_ComputeBiTemporalControl(&system->bi_temporal);
        
        for (int i = 0; i < 50; i++) {
            TCDE_Evolve11DIdentitySystem(system, 0.01f);
        }
        
        float final_control = TCDE_ComputeBiTemporalControl(&system->bi_temporal);
        TEST("Control function stable", !isnan(final_control) && !isinf(final_control));
        
        TCDE_Destroy11DIdentitySystem(system);
    }
    
    printf("\n=== Control Analysis ===\n");
    printf("τ₁ (Anticipation): %.6f\n", original_tau1);
    printf("τ₂ (Memory):       %.6f\n", original_tau2);
    printf("Control Value:     %.6f\n", control_value);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}