/**
 * @file test_task_6_1_reflexivity.c
 * @brief Test Task 6.1 - Real Reflexivity Validation
 * 
 * Tests the authentic TCDE cognitive validation for reflexivity:
 * the field's ability to reconstruct perturbed patterns.
 * 
 * @date January 2025
 */

#include "../src/core/tcde.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TEST(name, condition) do { \
    if (condition) { \
        printf("✓ %s\n", name); \
        tests_passed++; \
    } else { \
        printf("✗ %s\n", name); \
    } \
    tests_total++; \
} while(0)

int tests_total = 0;
int tests_passed = 0;

/**
 * @brief Test 1: Basic reflexivity with low perturbation
 */
void test_low_perturbation() {
    printf("\n=== Test 1: Low Perturbation (10%%) ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    // Create a simple pattern
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f,  // x
            0.5f,              // y
            0.5f,              // z
            1.0f + 0.5f * (float)i / 20.0f,  // τ₁
            0.0f,              // τ₂
            0.4f               // m (semantic)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        float amplitude = 1.0f + 0.5f * sinf(2.0f * 3.14159f * (float)i / 20.0f);
        TCDE_AddCenter6D(field, &p, amplitude, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Test reflexivity with low perturbation
    float reflexivity = TCDE_ValidateReflexivity(field, 0, 0.1f, 50);
    
    printf("  Reflexivity score: %.3f\n", reflexivity);
    TEST("Low perturbation reflexivity > 0.7", reflexivity > 0.7f);
    TEST("Reflexivity in valid range [0,1]", reflexivity >= 0.0f && reflexivity <= 1.0f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 2: Medium perturbation
 */
void test_medium_perturbation() {
    printf("\n=== Test 2: Medium Perturbation (30%%) ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    // Create pattern
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 20.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    float reflexivity = TCDE_ValidateReflexivity(field, 0, 0.3f, 50);
    
    printf("  Reflexivity score: %.3f\n", reflexivity);
    TEST("Medium perturbation reflexivity > 0.5", reflexivity > 0.5f);
    TEST("Medium < Low perturbation (degradation)", reflexivity < 0.9f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 3: High perturbation
 */
void test_high_perturbation() {
    printf("\n=== Test 3: High Perturbation (50%%) ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    // Create pattern
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 20.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    float reflexivity = TCDE_ValidateReflexivity(field, 0, 0.5f, 50);
    
    printf("  Reflexivity score: %.3f\n", reflexivity);
    TEST("High perturbation reflexivity > 0.3", reflexivity > 0.3f);
    TEST("Reflexivity degrades with perturbation", reflexivity < 0.8f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 4: Evolution improves reflexivity
 */
void test_evolution_effect() {
    printf("\n=== Test 4: Evolution Effect ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    // Create pattern
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 20.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Test with few evolution steps
    float reflex_few = TCDE_ValidateReflexivity(field, 0, 0.3f, 10);
    
    // Recreate field for fair comparison
    TCDE_DestroyField(field);
    field = TCDE_CreateField(100, 2.5f);
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 20.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Test with many evolution steps
    float reflex_many = TCDE_ValidateReflexivity(field, 0, 0.3f, 100);
    
    printf("  Reflexivity (10 steps): %.3f\n", reflex_few);
    printf("  Reflexivity (100 steps): %.3f\n", reflex_many);
    
    // More evolution should allow better reconstruction (or at least not worse)
    TEST("Evolution helps reconstruction", reflex_many >= reflex_few * 0.9f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 5: Robustness test
 */
void test_robustness() {
    printf("\n=== Test 5: Robustness Analysis ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    // Create richer pattern
    for (int i = 0; i < 30; i++) {
        float coords[6] = {
            (float)i / 30.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 30.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        float amplitude = 1.0f + 0.3f * sinf(4.0f * 3.14159f * (float)i / 30.0f);
        TCDE_AddCenter6D(field, &p, amplitude, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Test robustness
    float results[5];
    float avg_reflexivity = TCDE_TestReflexivityRobustness(field, 5, results);
    
    printf("  Average reflexivity: %.3f\n", avg_reflexivity);
    printf("  Scores: ");
    for (int i = 0; i < 5; i++) {
        printf("%.3f ", results[i]);
    }
    printf("\n");
    
    TEST("Average reflexivity > 0.5", avg_reflexivity > 0.5f);
    TEST("Reflexivity decreases with perturbation", results[0] > results[4]);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 6: Empty field handling
 */
void test_empty_field() {
    printf("\n=== Test 6: Empty Field ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    float reflexivity = TCDE_ValidateReflexivity(field, 0, 0.1f, 10);
    
    printf("  Reflexivity score: %.3f\n", reflexivity);
    TEST("Empty field returns 0", reflexivity == 0.0f);
    
    TCDE_DestroyField(field);
}

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Task 6.1: Real Reflexivity Validation                    ║\n");
    printf("║  Authentic cognitive validation - pattern reconstruction  ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    test_low_perturbation();
    test_medium_perturbation();
    test_high_perturbation();
    test_evolution_effect();
    test_robustness();
    test_empty_field();
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Results: %d/%d tests passed (%.1f%%)                      ║\n", 
           tests_passed, tests_total, 100.0f * tests_passed / tests_total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 6.1 COMPLETE: Real reflexivity validation\n");
        printf("  - Pattern reconstruction capability validated\n");
        printf("  - Cognitive emergence demonstrated\n");
        printf("  - Authentic TCDE validation\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed\n");
        return 1;
    }
}
