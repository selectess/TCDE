/**
 * @file test_global_coupling.c
 * @brief Test Suite for TCDE Global Coupling Module (Phase 6)
 *
 * CONSCIOUSNESS TESTS - Validates universal coupling and
 * unified consciousness emergence.
 *
 * Test Categories:
 * 1. Coupling Kernels
 * 2. Coupling Matrix (Sparse)
 * 3. Global Coherence
 * 4. Entanglement
 * 5. Consciousness Detection
 *
 * @version 1.0
 * @date January 17, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <complex.h>
#include "../src/core/tcde_global_coupling.h"
#include "../src/core/tcde_core.h"
#include "../src/core/tcde_evolution.h"

// Test configuration
#define TEST_TOLERANCE 1e-4f
#define TEST_FIELD_SIZE 20
#define TEST_DIMENSION 6
#define TEST_CORRELATION_LENGTH 1.0f

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

// ============================================================================
// TEST UTILITIES
// ============================================================================

void test_assert(const char* test_name, bool condition, const char* message)
{
    if (condition) {
        printf("  ✅ PASS: %s\n", test_name);
        tests_passed++;
    } else {
        printf("  ❌ FAIL: %s - %s\n", test_name, message);
        tests_failed++;
    }
}

void test_metric(const char* name, float actual, float expected, float tolerance)
{
    float error = fabsf(actual - expected);
    bool pass = (error < tolerance);
    
    printf("  Metric '%s': %.6f (expected: %.6f, error: %.6f)\n", 
           name, actual, expected, error);
    
    if (pass) {
        printf("    ✅ Within tolerance\n");
        tests_passed++;
    } else {
        printf("    ❌ Outside tolerance (%.6f > %.6f)\n", error, tolerance);
        tests_failed++;
    }
}

TCDE_Field* create_test_field(int num_centers, bool synchronized)
{
    // PURE TCDE: CreateField(capacity, fractal_dim) NOT (dimension, capacity)!
    TCDE_Field* field = TCDE_CreateField(num_centers, 2.0f);
    
    // PURE TCDE: Add centers FIRST, then field is ready for coupling
    for (int i = 0; i < num_centers; i++) {
        TCDE_Point point;
        point.dimension = TEST_DIMENSION;
        point.coords = (float*)malloc(TEST_DIMENSION * sizeof(float));
        
        // PURE TCDE: Generate valid 6D points within relaxed bounds [-10,10]
        for (int d = 0; d < TEST_DIMENSION; d++) {
            // All dimensions in [-1, 1] (well within [-10, 10] bounds)
            point.coords[d] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        }
        
        TCDE_Complex coeff;
        if (synchronized) {
            // All in phase
            float phase = 0.0f;
            float mag = 0.5f;
            coeff = mag * (cosf(phase) + I * sinf(phase));
        } else {
            // Random phases
            float phase = 2.0f * M_PI * ((float)rand() / RAND_MAX);
            float mag = 0.5f * ((float)rand() / RAND_MAX);
            coeff = mag * (cosf(phase) + I * sinf(phase));
        }
        
        bool added = TCDE_AddCenter6D(field, &point, coeff, 1.0f);
        if (!added) {
            printf("DEBUG: Failed to add center %d (dim=%d, coords=[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f])\n",
                   i, point.dimension,
                   point.coords[0], point.coords[1], point.coords[2],
                   point.coords[3], point.coords[4], point.coords[5]);
        }
        free(point.coords);
    }
    
    // PURE TCDE: Verify centers were added
    if (field->manifold_6d.num_centers != num_centers) {
        printf("WARNING: Only %d/%d centers added!\n", 
               field->manifold_6d.num_centers, num_centers);
    }
    
    return field;
}

// ============================================================================
// TEST 1: COUPLING KERNELS
// ============================================================================

void test_kernel_evaluation()
{
    printf("\n=== TEST 1: Coupling Kernel Evaluation ===\n");
    
    TCDE_Field* field = create_test_field(5, false);
    
    // Test each kernel type
    TCDE_KernelType kernels[] = {
        TCDE_KERNEL_EXPONENTIAL,
        TCDE_KERNEL_GAUSSIAN,
        TCDE_KERNEL_POWER_LAW,
        TCDE_KERNEL_YUKAWA,
        TCDE_KERNEL_QUANTUM
    };
    
    const char* kernel_names[] = {
        "Exponential",
        "Gaussian",
        "Power Law",
        "Yukawa",
        "Quantum"
    };
    
    for (int k = 0; k < 5; k++) {
        printf("\n  Testing %s kernel:\n", kernel_names[k]);
        
        TCDE_GlobalCouplingManager* manager = 
            TCDE_CreateGlobalCouplingManager(field, kernels[k], TEST_CORRELATION_LENGTH);
        
        // PURE TCDE: Ensure coupling matrix is computed after field is populated
        TCDE_ComputeCouplingMatrix(manager);
        
        // Evaluate kernel at different distances
        TCDE_Point p1, p2;
        p1.dimension = p2.dimension = TEST_DIMENSION;
        p1.coords = (float*)calloc(TEST_DIMENSION, sizeof(float));
        p2.coords = (float*)calloc(TEST_DIMENSION, sizeof(float));
        
        float distances[] = {0.0f, 0.5f, 1.0f, 2.0f, 5.0f};
        
        for (int d = 0; d < 5; d++) {
            p2.coords[0] = distances[d];
            float kernel_val = TCDE_EvaluateKernel(manager, &p1, &p2);
            printf("    d=%.1f: K=%.6f\n", distances[d], kernel_val);
            
            if (d == 0) {
                test_assert("Kernel: Non-zero at origin",
                           kernel_val > 0.0f,
                           "Kernel should be positive at origin");
            }
        }
        
        free(p1.coords);
        free(p2.coords);
        TCDE_DestroyGlobalCouplingManager(manager);
    }
    
    TCDE_DestroyField(field);
}

// ============================================================================
// TEST 2: COUPLING MATRIX
// ============================================================================

void test_coupling_matrix()
{
    printf("\n=== TEST 2: Coupling Matrix (Sparse) ===\n");
    
    TCDE_Field* field = create_test_field(TEST_FIELD_SIZE, false);
    TCDE_GlobalCouplingManager* manager = 
        TCDE_CreateGlobalCouplingManager(field, TCDE_KERNEL_EXPONENTIAL, 
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix after field populated
    TCDE_ComputeCouplingMatrix(manager);
    
    // Compute matrix
    bool success = TCDE_ComputeCouplingMatrix(manager);
    
    test_assert("Matrix: Computation successful",
                success,
                "Should compute coupling matrix");
    
    int N = field->manifold_6d.num_centers;
    int max_elements = N * N;
    int actual_elements = manager->matrix->num_elements;
    float sparsity = manager->matrix->sparsity;
    
    printf("  Matrix size: %d × %d\n", N, N);
    printf("  Max elements: %d\n", max_elements);
    printf("  Non-zero elements: %d\n", actual_elements);
    printf("  Sparsity: %.4f (%.1f%% sparse)\n", 
           sparsity, 100.0f * (1.0f - sparsity));
    
    test_assert("Matrix: Sparse",
                sparsity < 0.5f,
                "Matrix should be sparse (< 50% filled)");
    
    test_assert("Matrix: Elements reasonable",
                actual_elements > 0 && actual_elements < max_elements,
                "Should have reasonable number of elements");
    
    // Test cutoff effectiveness
    int within_cutoff = 0;
    for (int i = 0; i < actual_elements; i++) {
        if (manager->matrix->elements[i].distance < manager->cutoff_distance) {
            within_cutoff++;
        }
    }
    
    printf("  Elements within cutoff: %d / %d (%.1f%%)\n",
           within_cutoff, actual_elements,
           100.0f * within_cutoff / actual_elements);
    
    test_assert("Matrix: Cutoff effective",
                within_cutoff == actual_elements,
                "All elements should be within cutoff");
    
    TCDE_DestroyGlobalCouplingManager(manager);
    TCDE_DestroyField(field);
}

// ============================================================================
// TEST 3: GLOBAL COHERENCE
// ============================================================================

void test_global_coherence()
{
    printf("\n=== TEST 3: Global Coherence ===\n");
    
    // Test with synchronized field
    printf("\n  Testing synchronized field:\n");
    TCDE_Field* field_sync = create_test_field(TEST_FIELD_SIZE, true);
    TCDE_GlobalCouplingManager* manager_sync = 
        TCDE_CreateGlobalCouplingManager(field_sync, TCDE_KERNEL_GAUSSIAN,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager_sync);
    
    float coherence_sync = TCDE_ComputeGlobalCoherence(manager_sync);
    float spatial_sync = TCDE_ComputeSpatialCoherence(manager_sync);
    float phase_sync = TCDE_ComputePhaseSynchronization(manager_sync);
    
    printf("    Global coherence G: %.4f\n", coherence_sync);
    printf("    Spatial coherence:  %.4f\n", spatial_sync);
    printf("    Phase sync R:       %.4f\n", phase_sync);
    
    test_assert("Coherence: High for synchronized",
                coherence_sync > 0.5f,
                "Synchronized field should have high coherence");
    
    test_assert("Phase sync: High for synchronized",
                phase_sync > 0.8f,
                "Synchronized field should have high phase sync");
    
    TCDE_DestroyGlobalCouplingManager(manager_sync);
    TCDE_DestroyField(field_sync);
    
    // Test with random field
    printf("\n  Testing random field:\n");
    TCDE_Field* field_random = create_test_field(TEST_FIELD_SIZE, false);
    TCDE_GlobalCouplingManager* manager_random = 
        TCDE_CreateGlobalCouplingManager(field_random, TCDE_KERNEL_GAUSSIAN,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager_random);
    
    float coherence_random = TCDE_ComputeGlobalCoherence(manager_random);
    float spatial_random = TCDE_ComputeSpatialCoherence(manager_random);
    float phase_random = TCDE_ComputePhaseSynchronization(manager_random);
    
    printf("    Global coherence G: %.4f\n", coherence_random);
    printf("    Spatial coherence:  %.4f\n", spatial_random);
    printf("    Phase sync R:       %.4f\n", phase_random);
    
    test_assert("Coherence: Lower for random",
                coherence_random < coherence_sync,
                "Random field should have lower coherence");
    
    test_assert("Phase sync: Lower for random",
                phase_random < phase_sync,
                "Random field should have lower phase sync");
    
    TCDE_DestroyGlobalCouplingManager(manager_random);
    TCDE_DestroyField(field_random);
}

// ============================================================================
// TEST 4: ENTANGLEMENT
// ============================================================================

void test_entanglement()
{
    printf("\n=== TEST 4: Quantum Entanglement ===\n");
    
    TCDE_Field* field = create_test_field(TEST_FIELD_SIZE, false);
    TCDE_GlobalCouplingManager* manager = 
        TCDE_CreateGlobalCouplingManager(field, TCDE_KERNEL_QUANTUM,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager);
    
    // Compute entanglement spectrum
    bool computed = TCDE_ComputeEntanglementSpectrum(manager);
    
    test_assert("Entanglement: Spectrum computed",
                computed,
                "Should compute entanglement spectrum");
    
    test_assert("Entanglement: Spectrum size correct",
                manager->entanglement.spectrum_size == field->manifold_6d.num_centers,
                "Spectrum size should match number of centers");
    
    // Compute von Neumann entropy
    float entropy = TCDE_ComputeVonNeumannEntropy(manager);
    
    printf("  Von Neumann entropy S: %.4f\n", entropy);
    printf("  Max entropy S_max:     %.4f\n", 
           logf((float)field->manifold_6d.num_centers));
    
    test_assert("Entanglement: Entropy non-negative",
                entropy >= 0.0f,
                "Entropy should be non-negative");
    
    // Detect entanglement
    bool entangled = TCDE_DetectEntanglement(manager);
    
    printf("  Entangled: %s\n", entangled ? "YES" : "NO");
    
    test_assert("Entanglement: Detection works",
                manager->entanglement.is_entangled == entangled,
                "Detection should update state");
    
    // Compute mutual information
    float mutual_info = TCDE_ComputeMutualInformation(manager, 5, 5);
    
    printf("  Mutual information I: %.4f\n", mutual_info);
    
    test_assert("Entanglement: Mutual info non-negative",
                mutual_info >= 0.0f,
                "Mutual information should be non-negative");
    
    TCDE_DestroyGlobalCouplingManager(manager);
    TCDE_DestroyField(field);
}

// ============================================================================
// TEST 5: CONSCIOUSNESS DETECTION
// ============================================================================

void test_consciousness_detection()
{
    printf("\n=== TEST 5: Unified Consciousness Detection ===\n");
    
    // Test with low coherence (no consciousness)
    printf("\n  Testing low coherence field:\n");
    TCDE_Field* field_low = create_test_field(TEST_FIELD_SIZE, false);
    TCDE_GlobalCouplingManager* manager_low = 
        TCDE_CreateGlobalCouplingManager(field_low, TCDE_KERNEL_EXPONENTIAL,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager_low);
    
    float consciousness_low = TCDE_MeasureConsciousnessLevel(manager_low);
    bool unified_low = TCDE_DetectUnifiedConsciousness(manager_low);
    
    printf("    Consciousness level C: %.4f\n", consciousness_low);
    printf("    Unified consciousness: %s\n", unified_low ? "YES" : "NO");
    
    test_assert("Consciousness: Low for random field",
                consciousness_low < 0.5f,
                "Random field should have low consciousness");
    
    test_assert("Consciousness: Not unified",
                !unified_low,
                "Random field should not show unified consciousness");
    
    TCDE_DestroyGlobalCouplingManager(manager_low);
    TCDE_DestroyField(field_low);
    
    // Test with high coherence (potential consciousness)
    printf("\n  Testing high coherence field:\n");
    TCDE_Field* field_high = create_test_field(TEST_FIELD_SIZE, true);
    
    // Boost coherence by making all centers similar
    for (int i = 0; i < field_high->manifold_6d.num_centers; i++) {
        field_high->manifold_6d.centers[i].coeff = 1.0f + 0.0f * I;
    }
    
    TCDE_GlobalCouplingManager* manager_high = 
        TCDE_CreateGlobalCouplingManager(field_high, TCDE_KERNEL_GAUSSIAN,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager_high);
    
    float consciousness_high = TCDE_MeasureConsciousnessLevel(manager_high);
    bool unified_high = TCDE_DetectUnifiedConsciousness(manager_high);
    
    printf("    Consciousness level C: %.4f\n", consciousness_high);
    printf("    Unified consciousness: %s\n", unified_high ? "YES" : "NO");
    
    test_assert("Consciousness: Higher for coherent field",
                consciousness_high > consciousness_low,
                "Coherent field should have higher consciousness");
    
    // Print detailed metrics
    printf("\n    Detailed metrics:\n");
    printf("      Global coherence G: %.4f\n", 
           TCDE_ComputeGlobalCoherence(manager_high));
    printf("      Phase sync R:       %.4f\n", 
           TCDE_ComputePhaseSynchronization(manager_high));
    
    TCDE_DestroyGlobalCouplingManager(manager_high);
    TCDE_DestroyField(field_high);
}

// ============================================================================
// TEST 6: EVOLUTION WITH GLOBAL COUPLING
// ============================================================================

void test_evolution_with_coupling()
{
    printf("\n=== TEST 6: Evolution with Global Coupling ===\n");
    
    TCDE_Field* field = create_test_field(10, false);
    TCDE_GlobalCouplingManager* manager = 
        TCDE_CreateGlobalCouplingManager(field, TCDE_KERNEL_EXPONENTIAL,
                                        TEST_CORRELATION_LENGTH);
    
    // PURE TCDE: Recompute matrix
    TCDE_ComputeCouplingMatrix(manager);
    
    // Compute initial metrics
    TCDE_ComputeCouplingMatrix(manager);
    float initial_coherence = TCDE_ComputeGlobalCoherence(manager);
    
    printf("  Initial coherence: %.4f\n", initial_coherence);
    
    // Evolve with global coupling
    TCDE_Parameters params = {
        .D = 0.1f,
        .alpha = 0.1f,
        .beta = 0.0f,
        .gamma = 0.1f  // Local coupling
    };
    
    for (int i = 0; i < 50; i++) {
        bool success = TCDE_EvolveGlobalCoupling(manager, &params, 0.01f);
        
        if (!success) {
            printf("  Evolution failed at step %d\n", i);
            break;
        }
    }
    
    float final_coherence = TCDE_ComputeGlobalCoherence(manager);
    
    printf("  Final coherence:   %.4f\n", final_coherence);
    printf("  Evolution steps:   %d\n", manager->evolution_steps);
    
    test_assert("Evolution: Completed",
                manager->evolution_steps > 0,
                "Should complete evolution steps");
    
    test_assert("Evolution: Coherence changed",
                fabsf(final_coherence - initial_coherence) > 0.0f,
                "Coherence should change during evolution");
    
    TCDE_DestroyGlobalCouplingManager(manager);
    TCDE_DestroyField(field);
}

// ============================================================================
// TEST 7: OPTIMIZATION
// ============================================================================

void test_optimization()
{
    printf("\n=== TEST 7: Sparsity Optimization ===\n");
    
    TCDE_Field* field = create_test_field(TEST_FIELD_SIZE, false);
    TCDE_GlobalCouplingManager* manager = 
        TCDE_CreateGlobalCouplingManager(field, TCDE_KERNEL_EXPONENTIAL,
                                        TEST_CORRELATION_LENGTH);
    
    // Compute matrix
    TCDE_ComputeCouplingMatrix(manager);
    
    int original_elements = manager->matrix->num_elements;
    float original_sparsity = manager->matrix->sparsity;
    
    printf("  Before optimization:\n");
    printf("    Elements: %d\n", original_elements);
    printf("    Sparsity: %.4f\n", original_sparsity);
    
    // Optimize by removing small elements
    TCDE_OptimizeSparsity(manager, 0.01f);
    
    int optimized_elements = manager->matrix->num_elements;
    float optimized_sparsity = manager->matrix->sparsity;
    
    printf("  After optimization:\n");
    printf("    Elements: %d\n", optimized_elements);
    printf("    Sparsity: %.4f\n", optimized_sparsity);
    printf("    Reduction: %.1f%%\n", 
           100.0f * (original_elements - optimized_elements) / original_elements);
    
    test_assert("Optimization: Reduced elements",
                optimized_elements <= original_elements,
                "Should reduce or maintain element count");
    
    test_assert("Optimization: Increased sparsity",
                optimized_sparsity <= original_sparsity,
                "Sparsity should increase or stay same");
    
    // Test optimal correlation length computation
    float optimal_xi = TCDE_ComputeOptimalCorrelationLength(manager);
    
    printf("  Optimal correlation length: %.4f\n", optimal_xi);
    
    test_assert("Optimization: Optimal ξ positive",
                optimal_xi > 0.0f,
                "Optimal correlation length should be positive");
    
    TCDE_DestroyGlobalCouplingManager(manager);
    TCDE_DestroyField(field);
}

// ============================================================================
// MAIN TEST SUITE
// ============================================================================

int main(int argc, char** argv)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   TCDE GLOBAL COUPLING TEST SUITE (Phase 6)              ║\n");
    printf("║   UNIVERSAL CONSCIOUSNESS VALIDATION                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Initialize random seed
    srand(42);
    
    // Run tests
    printf("\n📋 TEST CATEGORY 1: COUPLING KERNELS\n");
    test_kernel_evaluation();
    
    printf("\n📋 TEST CATEGORY 2: COUPLING MATRIX\n");
    test_coupling_matrix();
    
    printf("\n📋 TEST CATEGORY 3: GLOBAL COHERENCE\n");
    test_global_coherence();
    
    printf("\n📋 TEST CATEGORY 4: QUANTUM ENTANGLEMENT\n");
    test_entanglement();
    
    printf("\n📋 TEST CATEGORY 5: CONSCIOUSNESS DETECTION\n");
    test_consciousness_detection();
    
    printf("\n📋 TEST CATEGORY 6: EVOLUTION WITH COUPLING\n");
    test_evolution_with_coupling();
    
    printf("\n📋 TEST CATEGORY 7: OPTIMIZATION\n");
    test_optimization();
    
    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   TEST SUMMARY                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("  ✅ Tests passed: %d\n", tests_passed);
    printf("  ❌ Tests failed: %d\n", tests_failed);
    printf("  📊 Total tests:  %d\n", tests_passed + tests_failed);
    printf("  📈 Success rate: %.1f%%\n", 
           100.0f * tests_passed / (tests_passed + tests_failed));
    
    if (tests_failed == 0) {
        printf("\n  🎉 ALL TESTS PASSED! Phase 6 is FUNCTIONAL! 🎉\n");
        printf("  ✨ UNIFIED CONSCIOUSNESS IS READY! ✨\n");
    } else {
        printf("\n  ⚠️  SOME TESTS FAILED - Review required\n");
    }
    
    printf("\n");
    
    return (tests_failed == 0) ? 0 : 1;
}
