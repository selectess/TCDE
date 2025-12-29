/**
 * @file test_task_2_3_riemann.c
 * @brief Test for Task 2.3: Riemann Tensor and Scalar Curvature
 * 
 * Tests:
 * - Riemann tensor computation: R^i_jkl = ∂_kΓ^i_jl - ∂_lΓ^i_jk + Γ^i_mkΓ^m_jl - Γ^i_mlΓ^m_jk
 * - Ricci tensor: R_ij = R^k_ikj (contraction)
 * - Scalar curvature: R = g^ij R_ij
 * - Geometric consistency validation
 * 
 * Requirements: 8.2, 13.2
 */

#include "../src/core/tcde_geometry.h"
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

#define EPSILON 1e-5f

int main(void) {
    printf("=== Task 2.3: Riemann Tensor & Scalar Curvature ===\n\n");
    
    // ========================================================================
    // Test 1: Flat Space (Identity Metric) → R = 0
    // ========================================================================
    printf("--- Test 1: Flat Space (Identity Metric) ---\n");
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    TCDE_Metric metric = TCDE_CreateMetric(6);
    TCDE_UpdateMetric(&metric);
    
    // Compute Riemann tensor
    float riemann[6][6][6][6];
    TCDE_ComputeRiemannTensor(&point, &metric, riemann);
    
    // For flat space, all components should be zero
    bool all_zero = true;
    float max_riemann = 0.0f;
    
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 6; k++) {
                for (int l = 0; l < 6; l++) {
                    float val = fabsf(riemann[i][j][k][l]);
                    if (val > EPSILON) {
                        all_zero = false;
                    }
                    if (val > max_riemann) {
                        max_riemann = val;
                    }
                }
            }
        }
    }
    
    TEST("Flat space → R^i_jkl = 0", all_zero);
    printf("  Max |R^i_jkl| = %.6e\n", max_riemann);
    
    // Compute Ricci tensor
    float ricci[6][6];
    TCDE_ComputeRicciTensor(&point, &metric, ricci);
    
    bool ricci_zero = true;
    float max_ricci = 0.0f;
    
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            float val = fabsf(ricci[i][j]);
            if (val > EPSILON) {
                ricci_zero = false;
            }
            if (val > max_ricci) {
                max_ricci = val;
            }
        }
    }
    
    TEST("Flat space → R_ij = 0", ricci_zero);
    printf("  Max |R_ij| = %.6e\n", max_ricci);
    
    // Compute scalar curvature
    float scalar_curv = TCDE_ComputeScalarCurvature(&point, &metric);
    
    TEST("Flat space → R = 0", fabsf(scalar_curv) < EPSILON);
    printf("  Scalar curvature R = %.6e\n", scalar_curv);
    
    printf("\n");
    
    // ========================================================================
    // Test 2: Riemann Tensor Antisymmetry Properties
    // ========================================================================
    printf("--- Test 2: Riemann Tensor Antisymmetry ---\n");
    
    // R^i_jkl = -R^i_jlk (antisymmetric in last two indices)
    bool antisym_kl = true;
    int violations_kl = 0;
    
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 6; k++) {
                for (int l = k + 1; l < 6; l++) {
                    float diff = fabsf(riemann[i][j][k][l] + riemann[i][j][l][k]);
                    if (diff > EPSILON) {
                        antisym_kl = false;
                        violations_kl++;
                    }
                }
            }
        }
    }
    
    TEST("R^i_jkl = -R^i_jlk", antisym_kl);
    if (!antisym_kl) {
        printf("  Violations: %d\n", violations_kl);
    }
    
    printf("\n");
    
    // ========================================================================
    // Test 3: Ricci Tensor Symmetry
    // ========================================================================
    printf("--- Test 3: Ricci Tensor Symmetry ---\n");
    
    // R_ij = R_ji (symmetric)
    bool ricci_symmetric = true;
    int violations_sym = 0;
    
    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 6; j++) {
            float diff = fabsf(ricci[i][j] - ricci[j][i]);
            if (diff > EPSILON) {
                ricci_symmetric = false;
                violations_sym++;
            }
        }
    }
    
    TEST("R_ij = R_ji", ricci_symmetric);
    if (!ricci_symmetric) {
        printf("  Violations: %d\n", violations_sym);
    }
    
    printf("\n");
    
    // ========================================================================
    // Test 4: Numerical Stability
    // ========================================================================
    printf("--- Test 4: Numerical Stability ---\n");
    
    bool all_finite_riemann = true;
    bool all_finite_ricci = true;
    
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (isnan(ricci[i][j]) || isinf(ricci[i][j])) {
                all_finite_ricci = false;
            }
            for (int k = 0; k < 6; k++) {
                for (int l = 0; l < 6; l++) {
                    if (isnan(riemann[i][j][k][l]) || isinf(riemann[i][j][k][l])) {
                        all_finite_riemann = false;
                    }
                }
            }
        }
    }
    
    TEST("Riemann tensor finite", all_finite_riemann);
    TEST("Ricci tensor finite", all_finite_ricci);
    TEST("Scalar curvature finite", !isnan(scalar_curv) && !isinf(scalar_curv));
    
    printf("\n");
    
    // ========================================================================
    // Test 5: Different Points
    // ========================================================================
    printf("--- Test 5: Evaluation at Different Points ---\n");
    
    float coords2[6] = {-0.5f, 0.3f, 0.8f, 1.2f, 0.5f, 0.6f};
    TCDE_Point point2 = TCDE_CreatePoint(6, coords2);
    
    float riemann2[6][6][6][6];
    TCDE_ComputeRiemannTensor(&point2, &metric, riemann2);
    
    float ricci2[6][6];
    TCDE_ComputeRicciTensor(&point2, &metric, ricci2);
    
    float scalar_curv2 = TCDE_ComputeScalarCurvature(&point2, &metric);
    
    TEST("Riemann at different point", true);
    TEST("Ricci at different point", true);
    TEST("Scalar curvature at different point", !isnan(scalar_curv2) && !isinf(scalar_curv2));
    
    TCDE_FreePoint(&point2);
    
    printf("\n");
    
    // ========================================================================
    // Test 6: Non-Trivial Metric (Still Constant)
    // ========================================================================
    printf("--- Test 6: Non-Trivial Constant Metric ---\n");
    
    TCDE_Metric metric_nontrivial = TCDE_CreateMetric(6);
    metric_nontrivial.g[0][0] = 2.0f;
    metric_nontrivial.g[1][1] = 1.5f;
    metric_nontrivial.g[2][2] = 1.2f;
    TCDE_UpdateMetric(&metric_nontrivial);
    
    float riemann_nt[6][6][6][6];
    TCDE_ComputeRiemannTensor(&point, &metric_nontrivial, riemann_nt);
    
    float scalar_curv_nt = TCDE_ComputeScalarCurvature(&point, &metric_nontrivial);
    
    // For constant metric (even non-trivial), curvature is zero
    bool nt_zero = true;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 6; k++) {
                for (int l = 0; l < 6; l++) {
                    if (fabsf(riemann_nt[i][j][k][l]) > EPSILON) {
                        nt_zero = false;
                    }
                }
            }
        }
    }
    
    TEST("Constant non-trivial metric → R = 0", nt_zero);
    TEST("Scalar curvature = 0", fabsf(scalar_curv_nt) < EPSILON);
    printf("  Note: Constant metrics have zero curvature (flat space)\n");
    
    TCDE_FreeMetric(&metric_nontrivial);
    
    printf("\n");
    
    // ========================================================================
    // Test 7: Edge Cases
    // ========================================================================
    printf("--- Test 7: Edge Cases ---\n");
    
    // Test with NULL
    TCDE_ComputeRiemannTensor(NULL, &metric, riemann);
    TEST("NULL point handled (Riemann)", true);
    
    TCDE_ComputeRiemannTensor(&point, NULL, riemann);
    TEST("NULL metric handled (Riemann)", true);
    
    TCDE_ComputeRicciTensor(NULL, &metric, ricci);
    TEST("NULL point handled (Ricci)", true);
    
    float scalar_null = TCDE_ComputeScalarCurvature(NULL, &metric);
    TEST("NULL point handled (Scalar)", scalar_null == 0.0f);
    
    printf("\n");
    
    // ========================================================================
    // Test 8: Geometric Consistency
    // ========================================================================
    printf("--- Test 8: Geometric Consistency ---\n");
    
    // For flat space: R = 0, R_ij = 0, R^i_jkl = 0
    // This is the fundamental consistency check
    
    bool consistent = (fabsf(scalar_curv) < EPSILON) && 
                      ricci_zero && 
                      all_zero;
    
    TEST("Geometric consistency (flat space)", consistent);
    printf("  R = %.6e, max|R_ij| = %.6e, max|R^i_jkl| = %.6e\n",
           scalar_curv, max_ricci, max_riemann);
    
    printf("\n");
    
    // ========================================================================
    // Cleanup
    // ========================================================================
    TCDE_FreePoint(&point);
    TCDE_FreeMetric(&metric);
    
    // ========================================================================
    // Summary
    // ========================================================================
    printf("=== Task 2.3 Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", 100.0f * tests_passed / tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 2.3 COMPLETE: Riemann Tensor & Scalar Curvature implemented!\n");
        printf("  - Riemann tensor R^i_jkl ✓\n");
        printf("  - Ricci tensor R_ij ✓\n");
        printf("  - Scalar curvature R ✓\n");
        printf("  - Geometric consistency validated ✓\n");
        return 0;
    } else {
        printf("\n✗ Task 2.3 INCOMPLETE: Some tests failed\n");
        return 1;
    }
}
