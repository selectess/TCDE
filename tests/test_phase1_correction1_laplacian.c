/**
 * @file test_phase1_correction1_laplacian.c
 * @brief Test Phase 1 - Correction 1: Laplacian Normalization
 * 
 * Validates that the Laplacian normalization fix resolves the stability/evolution dilemma:
 * - No NaN/Inf values (stability)
 * - Field actually evolves (not frozen)
 * - Energy conservation is reasonable
 * - Diffusion coefficient D can be in practical range [0.01, 0.1]
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include "tcde_geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define TEST_PASSED "\033[32m✓ PASSED\033[0m"
#define TEST_FAILED "\033[31m✗ FAILED\033[0m"

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

void test_assert(const char* test_name, bool condition) {
    if (condition) {
        printf("  %s: %s\n", test_name, TEST_PASSED);
        tests_passed++;
    } else {
        printf("  %s: %s\n", test_name, TEST_FAILED);
        tests_failed++;
    }
}

/**
 * Test 1: Laplacian magnitude is O(1) after normalization
 */
bool test_laplacian_magnitude() {
    printf("\n=== Test 1: Laplacian Magnitude O(1) ===\n");
    
    // Create field with typical epsilon values
    TCDE_Field* field = TCDE_CreateField(10, 2.5f);
    
    // Add centers with various epsilon values
    float epsilons[] = {0.05f, 0.1f, 0.2f, 0.5f};
    for (int i = 0; i < 4; i++) {
        float coords[6] = {
            0.3f + i * 0.1f,  // x
            0.5f,             // y
            0.5f,             // z
            1.0f,             // τ₁
            0.0f,             // τ₂
            0.4f              // modality
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, epsilons[i]);
        TCDE_FreePoint(&p);
    }
    
    // Compute Laplacian at each center
    bool all_reasonable = true;
    float max_laplacian = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Point* center_pos = &field->manifold_6d.centers[i].point;
        TCDE_Complex laplacian = TCDE_LaplaceBeltramiOptimized(field, center_pos);
        
        float mag = cabsf(laplacian);
        if (mag > max_laplacian) max_laplacian = mag;
        
        // Check if magnitude is reasonable (O(1) to O(10), not O(100) or O(1000))
        if (mag > 100.0f || isnan(mag) || isinf(mag)) {
            all_reasonable = false;
            printf("  Center %d: Laplacian magnitude = %.2f (TOO LARGE or NaN/Inf)\n", i, mag);
        }
    }
    
    printf("  Maximum Laplacian magnitude: %.3f\n", max_laplacian);
    test_assert("Laplacian magnitude is O(1) to O(10)", all_reasonable && max_laplacian < 100.0f);
    test_assert("No NaN/Inf in Laplacian", all_reasonable);
    
    TCDE_DestroyField(field);
    return all_reasonable;
}

/**
 * Test 2: Stable evolution with practical D values
 */
bool test_stable_evolution() {
    printf("\n=== Test 2: Stable Evolution with D=0.08 ===\n");
    
    TCDE_Field* field = TCDE_CreateField(20, 2.5f);
    
    // Add initial centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {
            0.2f + i * 0.15f,
            0.5f,
            0.5f,
            1.0f,
            0.0f,
            0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Configure parameters with practical D value
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    params.D = 0.08f;  // This should now work without collapse
    
    // Evolve for 50 steps
    bool stable = true;
    for (int step = 0; step < 50; step++) {
        TCDE_EvolveStep(field, &params);
        
        // Check all coefficients for NaN/Inf
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            TCDE_Complex coeff = field->manifold_6d.centers[i].coeff;
            if (isnan(crealf(coeff)) || isnan(cimagf(coeff)) ||
                isinf(crealf(coeff)) || isinf(cimagf(coeff))) {
                printf("  Step %d: NaN/Inf detected in center %d\n", step, i);
                stable = false;
                break;
            }
        }
        
        if (!stable) break;
    }
    
    test_assert("No NaN/Inf after 50 evolution steps", stable);
    
    TCDE_DestroyField(field);
    return stable;
}

/**
 * Test 3: Field actually evolves (not frozen)
 */
bool test_visible_evolution() {
    printf("\n=== Test 3: Visible Evolution ===\n");
    
    TCDE_Field* field = TCDE_CreateField(20, 2.5f);
    
    // Add centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {0.3f + i * 0.1f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Save initial coefficients
    TCDE_Complex* initial_coeffs = malloc(field->manifold_6d.num_centers * sizeof(TCDE_Complex));
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        initial_coeffs[i] = field->manifold_6d.centers[i].coeff;
    }
    
    // Evolve
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    params.D = 0.08f;
    
    for (int step = 0; step < 50; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Check if coefficients changed
    float total_change = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Complex current = field->manifold_6d.centers[i].coeff;
        TCDE_Complex initial = initial_coeffs[i];
        float change = cabsf(current - initial);
        total_change += change;
    }
    
    float avg_change = total_change / field->manifold_6d.num_centers;
    printf("  Average coefficient change: %.6f\n", avg_change);
    
    bool evolved = avg_change > 0.001f;  // Should have noticeable change
    test_assert("Field evolved (avg change > 0.001)", evolved);
    
    free(initial_coeffs);
    TCDE_DestroyField(field);
    return evolved;
}

/**
 * Test 4: Energy conservation is reasonable
 */
bool test_energy_conservation() {
    printf("\n=== Test 4: Energy Conservation ===\n");
    
    TCDE_Field* field = TCDE_CreateField(20, 2.5f);
    
    // Add centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {0.3f + i * 0.1f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute initial energy
    float initial_energy = TCDE_ComputeEnergy(field);
    printf("  Initial energy: %.6f\n", initial_energy);
    
    // Evolve
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    params.D = 0.08f;
    params.alpha = 0.01f;  // Small nonlinearity for stabilization
    
    for (int step = 0; step < 50; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Compute final energy
    float final_energy = TCDE_ComputeEnergy(field);
    printf("  Final energy: %.6f\n", final_energy);
    
    // Energy should decrease (dissipation) but not collapse to zero
    float energy_ratio = final_energy / initial_energy;
    printf("  Energy ratio (final/initial): %.3f\n", energy_ratio);
    
    bool reasonable = (energy_ratio > 0.1f) && (energy_ratio < 10.0f);
    test_assert("Energy ratio in reasonable range [0.1, 10]", reasonable);
    
    TCDE_DestroyField(field);
    return reasonable;
}

/**
 * Test 5: Normalization impact on different epsilon values
 */
bool test_normalization_impact() {
    printf("\n=== Test 5: Normalization Impact ===\n");
    
    TCDE_Field* field = TCDE_CreateField(10, 2.5f);
    
    // Test with small and large epsilon
    float test_epsilons[] = {0.01f, 0.05f, 0.1f, 0.5f, 1.0f};
    int num_tests = sizeof(test_epsilons) / sizeof(test_epsilons[0]);
    
    bool all_stable = true;
    
    for (int t = 0; t < num_tests; t++) {
        // Clear field
        while (field->manifold_6d.num_centers > 0) {
            TCDE_RemoveCenter6D(field, 0);
        }
        
        // Add center with specific epsilon
        float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, test_epsilons[t]);
        TCDE_FreePoint(&p);
        
        // Compute Laplacian
        TCDE_Point* center_pos = &field->manifold_6d.centers[0].point;
        TCDE_Complex laplacian = TCDE_LaplaceBeltramiOptimized(field, center_pos);
        float mag = cabsf(laplacian);
        
        printf("  ε=%.3f: Laplacian magnitude = %.3f", test_epsilons[t], mag);
        
        if (mag > 100.0f || isnan(mag) || isinf(mag)) {
            printf(" [UNSTABLE]\n");
            all_stable = false;
        } else {
            printf(" [OK]\n");
        }
    }
    
    test_assert("All epsilon values produce stable Laplacian", all_stable);
    
    TCDE_DestroyField(field);
    return all_stable;
}

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  PHASE 1 - CORRECTION 1: LAPLACIAN NORMALIZATION TEST     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Run all tests
    test_laplacian_magnitude();
    test_stable_evolution();
    test_visible_evolution();
    test_energy_conservation();
    test_normalization_impact();
    
    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TEST SUMMARY                                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Passed: %2d                                                ║\n", tests_passed);
    printf("║  Failed: %2d                                                ║\n", tests_failed);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED! Laplacian normalization is working correctly.\n");
        printf("   The stability/evolution dilemma is RESOLVED.\n\n");
        return 0;
    } else {
        printf("\n⚠️  SOME TESTS FAILED. Further investigation needed.\n\n");
        return 1;
    }
}
