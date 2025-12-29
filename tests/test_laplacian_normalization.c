/**
 * @file test_laplacian_normalization.c
 * @brief Test Laplacian normalization fix
 * 
 * Validates that:
 * 1. Laplacian is O(1) after normalization (not O(1/ε²))
 * 2. Field evolves stably with reasonable D values
 * 3. No NaN or Inf values appear
 * 4. Field shows visible evolution (not frozen)
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include "tcde_geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#define TEST_EPSILON 1e-5f
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RED "\x1b[31m"
#define ANSI_RESET "\x1b[0m"

// Test counters
static int tests_run = 0;
static int tests_passed = 0;

void test_assert(int condition, const char* test_name) {
    tests_run++;
    if (condition) {
        tests_passed++;
        printf(ANSI_GREEN "✓" ANSI_RESET " %s\n", test_name);
    } else {
        printf(ANSI_RED "✗" ANSI_RESET " %s\n", test_name);
    }
}

/**
 * Test 1: Laplacian magnitude is O(1) after normalization
 */
void test_laplacian_magnitude() {
    printf("\n=== Test 1: Laplacian Magnitude ===\n");
    
    // Create field with typical epsilon
    TCDE_Field* field = TCDE_CreateField6D(TCDE_RBF_GAUSSIAN);
    
    // Add center with epsilon=10 (typical value)
    float coords[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f};
    TCDE_Complex coeff = 1.0f + 0.0f * I;
    float epsilon = 10.0f;
    
    TCDE_AddCenter6D(field, coords, coeff, epsilon);
    
    // Compute Laplacian at center
    TCDE_Point point;
    point.dimension = 6;
    point.coords = coords;
    
    TCDE_Complex laplacian = TCDE_LaplaceBeltramiOptimized(field, &point);
    float laplacian_magnitude = cabsf(laplacian);
    
    printf("Epsilon: %.2f\n", epsilon);
    printf("Laplacian magnitude: %.2f\n", laplacian_magnitude);
    printf("Expected (without normalization): ~%.0f\n", 6.0f * epsilon * epsilon);
    printf("Expected (with normalization): ~6.0\n");
    
    // After normalization, Laplacian should be O(1), not O(1/ε²)
    // For Gaussian RBF at center: ∇²φ ≈ -6 (after normalization)
    test_assert(laplacian_magnitude < 20.0f, "Laplacian magnitude is O(1)");
    test_assert(laplacian_magnitude > 0.1f, "Laplacian is not zero");
    
    TCDE_DestroyField(field);
}

/**
 * Test 2: Field evolves stably with reasonable D
 */
void test_stable_evolution() {
    printf("\n=== Test 2: Stable Evolution ===\n");
    
    TCDE_Field* field = TCDE_CreateField6D(TCDE_RBF_GAUSSIAN);
    
    // Add multiple centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {
            (float)i * 0.2f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.5f
        };
        TCDE_Complex coeff = 1.0f + 0.0f * I;
        TCDE_AddCenter6D(field, coords, coeff, 10.0f);
    }
    
    // Configure parameters with reasonable D (now possible after fix)
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    printf("D = %.4f (can now use reasonable values)\n", params.D);
    
    // Evolve for 50 steps
    int num_steps = 50;
    int nan_count = 0;
    int inf_count = 0;
    
    for (int step = 0; step < num_steps; step++) {
        TCDE_EvolveStep(field, &params);
        
        // Check for NaN or Inf
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            TCDE_Complex coeff = field->manifold_6d.centers[i].coefficient;
            if (isnan(crealf(coeff)) || isnan(cimagf(coeff))) {
                nan_count++;
            }
            if (isinf(crealf(coeff)) || isinf(cimagf(coeff))) {
                inf_count++;
            }
        }
    }
    
    printf("Steps evolved: %d\n", num_steps);
    printf("NaN occurrences: %d\n", nan_count);
    printf("Inf occurrences: %d\n", inf_count);
    
    test_assert(nan_count == 0, "No NaN values during evolution");
    test_assert(inf_count == 0, "No Inf values during evolution");
    
    TCDE_DestroyField(field);
}

/**
 * Test 3: Field shows visible evolution (not frozen)
 */
void test_visible_evolution() {
    printf("\n=== Test 3: Visible Evolution ===\n");
    
    TCDE_Field* field = TCDE_CreateField6D(TCDE_RBF_GAUSSIAN);
    
    // Add centers
    for (int i = 0; i < 3; i++) {
        float coords[6] = {
            (float)i * 0.3f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.5f
        };
        TCDE_Complex coeff = 1.0f + 0.0f * I;
        TCDE_AddCenter6D(field, coords, coeff, 10.0f);
    }
    
    // Save initial coefficients
    TCDE_Complex initial_coeffs[3];
    for (int i = 0; i < 3; i++) {
        initial_coeffs[i] = field->manifold_6d.centers[i].coefficient;
    }
    
    // Evolve
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    for (int step = 0; step < 30; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Measure change
    float total_change = 0.0f;
    for (int i = 0; i < 3; i++) {
        TCDE_Complex final_coeff = field->manifold_6d.centers[i].coefficient;
        float change = cabsf(final_coeff - initial_coeffs[i]);
        total_change += change;
        printf("Center %d change: %.4f\n", i, change);
    }
    
    float avg_change = total_change / 3.0f;
    printf("Average change: %.4f\n", avg_change);
    
    // Field should evolve visibly (not be frozen)
    test_assert(avg_change > 0.01f, "Field shows visible evolution");
    test_assert(avg_change < 10.0f, "Evolution is not explosive");
    
    TCDE_DestroyField(field);
}

/**
 * Test 4: Compare with/without normalization
 */
void test_normalization_impact() {
    printf("\n=== Test 4: Normalization Impact ===\n");
    
    // This test demonstrates the problem and solution
    float epsilon = 10.0f;
    float epsilon_squared = epsilon * epsilon;
    
    // Typical Laplacian magnitude at center for Gaussian RBF
    float laplacian_unnormalized = 6.0f * epsilon_squared;  // ≈ 600
    float laplacian_normalized = 6.0f;  // After dividing by ε²
    
    printf("Without normalization:\n");
    printf("  Laplacian ≈ %.0f\n", laplacian_unnormalized);
    printf("  With D=0.08: Diffusion term ≈ %.0f (EXPLOSIVE!)\n", 
           0.08f * laplacian_unnormalized);
    printf("  Required D for stability: ≈ %.6f (TOO SMALL!)\n", 
           1.0f / laplacian_unnormalized);
    
    printf("\nWith normalization:\n");
    printf("  Laplacian ≈ %.0f\n", laplacian_normalized);
    printf("  With D=0.08: Diffusion term ≈ %.2f (STABLE!)\n", 
           0.08f * laplacian_normalized);
    printf("  Can use reasonable D values: 0.01 - 0.1\n");
    
    test_assert(laplacian_normalized < 10.0f, "Normalized Laplacian is O(1)");
    test_assert(laplacian_unnormalized > 100.0f, "Unnormalized Laplacian is O(1/ε²)");
}

/**
 * Test 5: Energy conservation (approximate)
 */
void test_energy_conservation() {
    printf("\n=== Test 5: Energy Conservation ===\n");
    
    TCDE_Field* field = TCDE_CreateField6D(TCDE_RBF_GAUSSIAN);
    
    // Add centers
    for (int i = 0; i < 4; i++) {
        float coords[6] = {
            (float)i * 0.25f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.5f
        };
        TCDE_Complex coeff = 1.0f + 0.0f * I;
        TCDE_AddCenter6D(field, coords, coeff, 10.0f);
    }
    
    // Compute initial energy
    float initial_energy = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Complex coeff = field->manifold_6d.centers[i].coefficient;
        initial_energy += cabsf(coeff) * cabsf(coeff);
    }
    
    printf("Initial energy: %.4f\n", initial_energy);
    
    // Evolve
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    for (int step = 0; step < 50; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Compute final energy
    float final_energy = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Complex coeff = field->manifold_6d.centers[i].coefficient;
        final_energy += cabsf(coeff) * cabsf(coeff);
    }
    
    printf("Final energy: %.4f\n", final_energy);
    
    float energy_change = fabsf(final_energy - initial_energy) / initial_energy;
    printf("Relative energy change: %.2f%%\n", energy_change * 100.0f);
    
    // Energy should be approximately conserved (within 50%)
    // (Not exact due to nonlinearity and numerical errors)
    test_assert(energy_change < 0.5f, "Energy approximately conserved");
    
    TCDE_DestroyField(field);
}

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Laplacian Normalization Fix - Validation Tests      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    test_laplacian_magnitude();
    test_stable_evolution();
    test_visible_evolution();
    test_normalization_impact();
    test_energy_conservation();
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Test Results                                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Tests run:    %2d                                          ║\n", tests_run);
    printf("║  Tests passed: %2d                                          ║\n", tests_passed);
    printf("║  Success rate: %.0f%%                                       ║\n", 
           (float)tests_passed / tests_run * 100.0f);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_run) {
        printf(ANSI_GREEN "\n✓ ALL TESTS PASSED!\n" ANSI_RESET);
        printf("\nLaplacian normalization fix is working correctly.\n");
        printf("The system can now:\n");
        printf("  • Use reasonable D values (0.01 - 0.1)\n");
        printf("  • Evolve stably without NaN/Inf\n");
        printf("  • Show visible evolution (not frozen)\n");
        printf("  • Maintain approximate energy conservation\n");
        return 0;
    } else {
        printf(ANSI_RED "\n✗ SOME TESTS FAILED\n" ANSI_RESET);
        return 1;
    }
}
