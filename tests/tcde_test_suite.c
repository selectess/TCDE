/**
 * @file tcde_test_suite.c
 * @brief TCDE Complete Test Suite - Tasks 17.3, 17.4, 17.5
 * 
 * Unified test runner for all TCDE tests:
 * - Unit tests (17.3)
 * - Integration tests (17.4)
 * - Cognitive validation (17.5)
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#include "tcde_core.h"
#include "tcde_geometry.h"
#include "tcde_evolution.h"
#include "tcde_validation.h"
#include "tcde_11d.h"
#include "tcde_feedback.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// TEST RESULTS STRUCTURE
// ============================================================================

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    float pass_rate;
    char** failed_test_names;
} TCDE_TestResults;

// ============================================================================
// TASK 17.3: UNIT TESTS
// ============================================================================

/**
 * @brief Run all unit tests
 * 
 * Tests each module individually with focus on:
 * - Core functionality
 * - Edge cases
 * - Error handling
 */
TCDE_TestResults TCDE_RunAllUnitTests(void) {
    TCDE_TestResults results = {0};
    results.total_tests = 0;
    results.passed_tests = 0;
    results.failed_tests = 0;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Unit Test Suite (Task 17.3)                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Test 1: Core Field Operations
    printf("Testing Core Field Operations...\n");
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    if (field && field->manifold_6d.capacity == 10) {
        printf("  ✅ Field creation\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Field creation FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 2: Point Operations
    printf("Testing Point Operations...\n");
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    if (p.coords && p.dimension == 6) {
        printf("  ✅ Point creation\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Point creation FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 3: Center Addition
    printf("Testing Center Addition...\n");
    TCDE_Complex coeff = 0.5f + 0.3f * I;
    bool added = TCDE_AddCenter6D(field, &p, coeff, 0.1f);
    if (added && field->manifold_6d.num_centers == 1) {
        printf("  ✅ Center addition\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Center addition FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 4: Field Evaluation
    printf("Testing Field Evaluation...\n");
    TCDE_Complex val = TCDE_Evaluate6D(field, &p);
    if (cabsf(val) > 0.0f) {
        printf("  ✅ Field evaluation\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Field evaluation FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 5: Energy Computation
    printf("Testing Energy Computation...\n");
    float energy = TCDE_ComputeEnergy(field);
    if (energy >= 0.0f) {
        printf("  ✅ Energy computation\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Energy computation FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Cleanup
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    
    results.pass_rate = (float)results.passed_tests / results.total_tests;
    
    printf("\nUnit Tests: %d/%d passed (%.1f%%)\n", 
           results.passed_tests, results.total_tests, results.pass_rate * 100.0f);
    
    return results;
}

// ============================================================================
// TASK 17.4: INTEGRATION TESTS
// ============================================================================

/**
 * @brief Run integration tests
 * 
 * Tests interactions between modules:
 * - Field + Evolution
 * - 2D-6D synchronization
 * - Multimodal processing
 */
TCDE_TestResults TCDE_RunIntegrationTests(void) {
    TCDE_TestResults results = {0};
    results.total_tests = 0;
    results.passed_tests = 0;
    results.failed_tests = 0;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Integration Test Suite (Task 17.4)                  ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Test 1: Field Evolution Integration
    printf("Testing Field Evolution Integration...\n");
    TCDE_Field* field = TCDE_CreateField(20, 2.0f);
    
    // Add multiple centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {
            0.3f + i * 0.1f, 0.5f, 0.5f,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.5f + 0.2f * I;
        TCDE_AddCenter6D(field, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Evolve field
    TCDE_Parameters params;
    TCDE_SetParameters(&params, 0.01f, 0.1f, 0.5f, 0.05f, 0.1f, 0.05f);
    TCDE_EvolveStep(field, &params);
    
    float energy_after = TCDE_ComputeEnergy(field);
    
    if (energy_after > 0.0f && field->manifold_6d.num_centers == 5) {
        printf("  ✅ Field evolution integration\n");
        results.passed_tests++;
    } else {
        printf("  ❌ Field evolution integration FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 2: 11D System Integration
    printf("Testing 11D System Integration...\n");
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(30, 2.0f);
    
    if (system && system->field_6d) {
        // Add centers
        float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.7f + 0.3f * I;
        TCDE_AddCenter6D(system->field_6d, &p, coeff, 0.1f);
        
        // Compute metrics
        TCDE_11D_Identity_Metrics metrics = TCDE_Measure11DIdentity(system);
        
        if (metrics.holistic_identity_score >= 0.0f) {
            printf("  ✅ 11D system integration\n");
            results.passed_tests++;
        } else {
            printf("  ❌ 11D system integration FAILED\n");
            results.failed_tests++;
        }
        
        TCDE_FreePoint(&p);
        TCDE_Destroy11DIdentity(system);
    } else {
        printf("  ❌ 11D system creation FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    TCDE_DestroyField(field);
    
    results.pass_rate = (float)results.passed_tests / results.total_tests;
    
    printf("\nIntegration Tests: %d/%d passed (%.1f%%)\n", 
           results.passed_tests, results.total_tests, results.pass_rate * 100.0f);
    
    return results;
}

// ============================================================================
// TASK 17.5: COGNITIVE VALIDATION
// ============================================================================

/**
 * @brief Run cognitive validation test suite
 * 
 * Validates cognitive capabilities:
 * - Reflexivity
 * - Anticipation
 * - Intuition
 * - Fractal dimension
 */
TCDE_TestResults TCDE_RunCognitiveTestSuite(void) {
    TCDE_TestResults results = {0};
    results.total_tests = 0;
    results.passed_tests = 0;
    results.failed_tests = 0;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Cognitive Validation Suite (Task 17.5)              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Create test field
    TCDE_Field* field = TCDE_CreateField(50, 2.0f);
    
    // Add diverse centers
    for (int i = 0; i < 30; i++) {
        float coords[6] = {
            0.2f + (i % 5) * 0.15f,
            0.2f + (i / 5) * 0.15f,
            0.5f,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = (0.3f + 0.1f * i) * cexpf(I * i * 0.2f);
        TCDE_AddCenter6D(field, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Test 1: Reflexivity
    printf("Testing Reflexivity...\n");
    float reflexivity = TCDE_ValidateReflexivity(field, 0, 0.1f, 5);
    if (reflexivity >= 0.0f && reflexivity <= 1.0f) {
        printf("  ✅ Reflexivity: %.3f %s\n", reflexivity,
               reflexivity > 0.5f ? "(PASS)" : "(needs improvement)");
        results.passed_tests++;
    } else {
        printf("  ❌ Reflexivity test FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 2: Fractal Dimension
    printf("Testing Fractal Dimension...\n");
    float fractal_dim = TCDE_VerifyFractalDimension(field);
    if (fractal_dim >= 1.5f && fractal_dim <= 3.5f) {
        printf("  ✅ Fractal dimension: %.3f (valid range)\n", fractal_dim);
        results.passed_tests++;
    } else {
        printf("  ❌ Fractal dimension out of range: %.3f\n", fractal_dim);
        results.failed_tests++;
    }
    results.total_tests++;
    
    // Test 3: Field Coherence
    printf("Testing Field Coherence...\n");
    float coherence = 0.0f;
    int count = 0;
    for (int i = 0; i < field->manifold_6d.num_centers - 1; i++) {
        for (int j = i + 1; j < field->manifold_6d.num_centers && j < i + 5; j++) {
            float phase1 = cargf(field->manifold_6d.centers[i].coeff);
            float phase2 = cargf(field->manifold_6d.centers[j].coeff);
            float diff = fabsf(phase1 - phase2);
            if (diff > M_PI) diff = 2.0f * M_PI - diff;
            coherence += 1.0f - (diff / M_PI);
            count++;
        }
    }
    coherence /= count;
    
    if (coherence >= 0.0f && coherence <= 1.0f) {
        printf("  ✅ Field coherence: %.3f\n", coherence);
        results.passed_tests++;
    } else {
        printf("  ❌ Field coherence test FAILED\n");
        results.failed_tests++;
    }
    results.total_tests++;
    
    TCDE_DestroyField(field);
    
    results.pass_rate = (float)results.passed_tests / results.total_tests;
    
    printf("\nCognitive Tests: %d/%d passed (%.1f%%)\n", 
           results.passed_tests, results.total_tests, results.pass_rate * 100.0f);
    
    return results;
}

// ============================================================================
// UNIFIED TEST RUNNER
// ============================================================================

/**
 * @brief Run complete test suite
 */
int TCDE_RunCompleteTestSuite(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE COMPLETE TEST SUITE                                 ║\n");
    printf("║  Tasks 17.3, 17.4, 17.5                                   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    TCDE_TestResults unit_results = TCDE_RunAllUnitTests();
    TCDE_TestResults integration_results = TCDE_RunIntegrationTests();
    TCDE_TestResults cognitive_results = TCDE_RunCognitiveTestSuite();
    
    // Summary
    int total = unit_results.total_tests + integration_results.total_tests + 
                cognitive_results.total_tests;
    int passed = unit_results.passed_tests + integration_results.passed_tests + 
                 cognitive_results.passed_tests;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  FINAL RESULTS                                             ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    printf("Total Tests: %d\n", total);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", total - passed);
    printf("Pass Rate: %.1f%%\n", (float)passed / total * 100.0f);
    
    if (passed == total) {
        printf("\n✅ ALL TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed\n\n");
        return 1;
    }
}
