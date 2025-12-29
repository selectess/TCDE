/**
 * @file test_task_2_1_metric.c
 * @brief Test for Task 2.1: TCDE_Metric structure with arbitrary dimension support
 * 
 * Tests:
 * - Identity metric (baseline) implementation
 * - Metric inversion and determinant calculation
 * - Basic geodesic distance computation
 * 
 * Requirements: 1.4, 8.5, 13.2
 */

#include "../src/core/tcde_core.h"
#include "../src/core/tcde_geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Test counter
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

#define EPSILON 1e-6f

int main(void) {
    printf("=== Task 2.1: TCDE_Metric avec support dimension arbitraire ===\n\n");
    
    // ========================================================================
    // Test 1: Identity metric (baseline) for 2D
    // ========================================================================
    printf("--- Test 1: Identity metric 2D ---\n");
    
    TCDE_Metric m2d = TCDE_CreateMetric(2);
    
    // Test identity initialization
    TEST("2D metric created", m2d.g != NULL && m2d.g_inv != NULL);
    TEST("2D metric dimension", m2d.dimension == 2);
    TEST("2D metric g[0][0] = 1", fabsf(m2d.g[0][0] - 1.0f) < EPSILON);
    TEST("2D metric g[1][1] = 1", fabsf(m2d.g[1][1] - 1.0f) < EPSILON);
    TEST("2D metric g[0][1] = 0", fabsf(m2d.g[0][1]) < EPSILON);
    TEST("2D metric g[1][0] = 0", fabsf(m2d.g[1][0]) < EPSILON);
    
    // Test inverse is also identity
    TEST("2D inverse g_inv[0][0] = 1", fabsf(m2d.g_inv[0][0] - 1.0f) < EPSILON);
    TEST("2D inverse g_inv[1][1] = 1", fabsf(m2d.g_inv[1][1] - 1.0f) < EPSILON);
    TEST("2D inverse g_inv[0][1] = 0", fabsf(m2d.g_inv[0][1]) < EPSILON);
    
    // Test determinant
    TEST("2D determinant = 1", fabsf(m2d.det - 1.0f) < EPSILON);
    TEST("2D metric is valid", m2d.is_valid);
    
    TCDE_FreeMetric(&m2d);
    printf("\n");
    
    // ========================================================================
    // Test 2: Identity metric (baseline) for 6D
    // ========================================================================
    printf("--- Test 2: Identity metric 6D ---\n");
    
    TCDE_Metric m6d = TCDE_CreateMetric(6);
    
    // Test identity initialization
    TEST("6D metric created", m6d.g != NULL && m6d.g_inv != NULL);
    TEST("6D metric dimension", m6d.dimension == 6);
    
    // Test diagonal elements
    bool diagonal_correct = true;
    for (int i = 0; i < 6; i++) {
        if (fabsf(m6d.g[i][i] - 1.0f) > EPSILON) {
            diagonal_correct = false;
            break;
        }
    }
    TEST("6D metric diagonal = 1", diagonal_correct);
    
    // Test off-diagonal elements
    bool off_diagonal_zero = true;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (i != j && fabsf(m6d.g[i][j]) > EPSILON) {
                off_diagonal_zero = false;
                break;
            }
        }
    }
    TEST("6D metric off-diagonal = 0", off_diagonal_zero);
    
    // Test inverse
    bool inverse_correct = true;
    for (int i = 0; i < 6; i++) {
        if (fabsf(m6d.g_inv[i][i] - 1.0f) > EPSILON) {
            inverse_correct = false;
            break;
        }
    }
    TEST("6D inverse diagonal = 1", inverse_correct);
    
    // Test determinant
    TEST("6D determinant = 1", fabsf(m6d.det - 1.0f) < EPSILON);
    TEST("6D metric is valid", m6d.is_valid);
    
    TCDE_FreeMetric(&m6d);
    printf("\n");
    
    // ========================================================================
    // Test 3: Metric inversion for 2D
    // ========================================================================
    printf("--- Test 3: Metric inversion 2D ---\n");
    
    m2d = TCDE_CreateMetric(2);
    
    // Modify metric to non-identity
    m2d.g[0][0] = 2.0f;
    m2d.g[1][1] = 3.0f;
    m2d.g[0][1] = 0.5f;
    m2d.g[1][0] = 0.5f;  // Symmetric
    
    // Update inverse and determinant
    TCDE_UpdateMetric(&m2d);
    
    // Test determinant: det = 2*3 - 0.5*0.5 = 6 - 0.25 = 5.75
    float expected_det = 5.75f;
    TEST("2D modified determinant", fabsf(m2d.det - expected_det) < EPSILON);
    TEST("2D modified metric valid", m2d.is_valid);
    
    // Test inverse: g^{-1} = (1/det) * [[g11, -g01], [-g10, g00]]
    float expected_inv_00 = 3.0f / expected_det;
    float expected_inv_11 = 2.0f / expected_det;
    float expected_inv_01 = -0.5f / expected_det;
    
    TEST("2D inverse [0][0]", fabsf(m2d.g_inv[0][0] - expected_inv_00) < EPSILON);
    TEST("2D inverse [1][1]", fabsf(m2d.g_inv[1][1] - expected_inv_11) < EPSILON);
    TEST("2D inverse [0][1]", fabsf(m2d.g_inv[0][1] - expected_inv_01) < EPSILON);
    
    // Verify g * g^{-1} = I
    float product_00 = m2d.g[0][0] * m2d.g_inv[0][0] + m2d.g[0][1] * m2d.g_inv[1][0];
    float product_11 = m2d.g[1][0] * m2d.g_inv[0][1] + m2d.g[1][1] * m2d.g_inv[1][1];
    float product_01 = m2d.g[0][0] * m2d.g_inv[0][1] + m2d.g[0][1] * m2d.g_inv[1][1];
    
    TEST("2D g * g^{-1} [0][0] = 1", fabsf(product_00 - 1.0f) < EPSILON);
    TEST("2D g * g^{-1} [1][1] = 1", fabsf(product_11 - 1.0f) < EPSILON);
    TEST("2D g * g^{-1} [0][1] = 0", fabsf(product_01) < EPSILON);
    
    TCDE_FreeMetric(&m2d);
    printf("\n");
    
    // ========================================================================
    // Test 4: Metric validation
    // ========================================================================
    printf("--- Test 4: Metric validation ---\n");
    
    // Valid metric (positive definite)
    m2d = TCDE_CreateMetric(2);
    m2d.g[0][0] = 2.0f;
    m2d.g[1][1] = 2.0f;
    TCDE_UpdateMetric(&m2d);
    TEST("Valid positive definite metric", TCDE_ValidateMetric(&m2d));
    TCDE_FreeMetric(&m2d);
    
    // Invalid metric (negative diagonal)
    m2d = TCDE_CreateMetric(2);
    m2d.g[0][0] = -1.0f;
    TCDE_UpdateMetric(&m2d);
    TEST("Reject negative diagonal", !TCDE_ValidateMetric(&m2d));
    TCDE_FreeMetric(&m2d);
    
    // Invalid metric (asymmetric)
    m2d = TCDE_CreateMetric(2);
    m2d.g[0][1] = 0.5f;
    m2d.g[1][0] = 0.3f;  // Not symmetric
    TEST("Reject asymmetric metric", !TCDE_ValidateMetric(&m2d));
    TCDE_FreeMetric(&m2d);
    
    // NULL metric
    TEST("Reject NULL metric", !TCDE_ValidateMetric(NULL));
    
    printf("\n");
    
    // ========================================================================
    // Test 5: Basic geodesic distance with identity metric
    // ========================================================================
    printf("--- Test 5: Geodesic distance (identity metric) ---\n");
    
    // 2D Euclidean distance
    float coords1_2d[2] = {0.0f, 0.0f};
    float coords2_2d[2] = {3.0f, 4.0f};
    TCDE_Point p1_2d = TCDE_CreatePoint(2, coords1_2d);
    TCDE_Point p2_2d = TCDE_CreatePoint(2, coords2_2d);
    m2d = TCDE_CreateMetric(2);
    
    float dist_2d = TCDE_GeodesicDistance(&p1_2d, &p2_2d, &m2d);
    float expected_dist_2d = 5.0f;  // sqrt(3^2 + 4^2) = 5
    TEST("2D Euclidean distance", fabsf(dist_2d - expected_dist_2d) < EPSILON);
    
    TCDE_FreePoint(&p1_2d);
    TCDE_FreePoint(&p2_2d);
    TCDE_FreeMetric(&m2d);
    
    // 6D Euclidean distance
    float coords1_6d[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    float coords2_6d[6] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    TCDE_Point p1_6d = TCDE_CreatePoint(6, coords1_6d);
    TCDE_Point p2_6d = TCDE_CreatePoint(6, coords2_6d);
    m6d = TCDE_CreateMetric(6);
    
    float dist_6d = TCDE_GeodesicDistance(&p1_6d, &p2_6d, &m6d);
    float expected_dist_6d = 1.0f;  // Distance in x direction only
    TEST("6D Euclidean distance", fabsf(dist_6d - expected_dist_6d) < EPSILON);
    
    TCDE_FreePoint(&p1_6d);
    TCDE_FreePoint(&p2_6d);
    TCDE_FreeMetric(&m6d);
    
    printf("\n");
    
    // ========================================================================
    // Test 6: Geodesic distance with non-identity metric
    // ========================================================================
    printf("--- Test 6: Geodesic distance (non-identity metric) ---\n");
    
    // Create 2D metric with scaling
    m2d = TCDE_CreateMetric(2);
    m2d.g[0][0] = 4.0f;  // Scale x by 2
    m2d.g[1][1] = 1.0f;  // Keep y unchanged
    TCDE_UpdateMetric(&m2d);
    
    // Distance between (0,0) and (1,0)
    float coords_a[2] = {0.0f, 0.0f};
    float coords_b[2] = {1.0f, 0.0f};
    TCDE_Point pa = TCDE_CreatePoint(2, coords_a);
    TCDE_Point pb = TCDE_CreatePoint(2, coords_b);
    
    float dist_scaled = TCDE_GeodesicDistance(&pa, &pb, &m2d);
    float expected_scaled = 2.0f;  // sqrt(1^2 * 4) = 2
    TEST("2D scaled metric distance", fabsf(dist_scaled - expected_scaled) < EPSILON);
    
    TCDE_FreePoint(&pa);
    TCDE_FreePoint(&pb);
    TCDE_FreeMetric(&m2d);
    
    printf("\n");
    
    // ========================================================================
    // Test 7: Geodesic distance squared (optimization)
    // ========================================================================
    printf("--- Test 7: Geodesic distance squared ---\n");
    
    coords1_2d[0] = 0.0f; coords1_2d[1] = 0.0f;
    coords2_2d[0] = 3.0f; coords2_2d[1] = 4.0f;
    p1_2d = TCDE_CreatePoint(2, coords1_2d);
    p2_2d = TCDE_CreatePoint(2, coords2_2d);
    m2d = TCDE_CreateMetric(2);
    
    float dist_sq = TCDE_GeodesicDistanceSquared(&p1_2d, &p2_2d, &m2d);
    float expected_dist_sq = 25.0f;  // 3^2 + 4^2 = 25
    TEST("2D distance squared", fabsf(dist_sq - expected_dist_sq) < EPSILON);
    
    // Verify consistency: dist^2 = (dist_sq)
    float dist = TCDE_GeodesicDistance(&p1_2d, &p2_2d, &m2d);
    TEST("Distance squared consistency", fabsf(dist * dist - dist_sq) < EPSILON);
    
    TCDE_FreePoint(&p1_2d);
    TCDE_FreePoint(&p2_2d);
    TCDE_FreeMetric(&m2d);
    
    printf("\n");
    
    // ========================================================================
    // Test 8: Optimized geodesic distance (block-diagonal)
    // ========================================================================
    printf("--- Test 8: Optimized geodesic distance (6D block-diagonal) ---\n");
    
    coords1_6d[0] = 0.0f; coords1_6d[1] = 0.0f; coords1_6d[2] = 0.0f;
    coords1_6d[3] = 1.0f; coords1_6d[4] = 0.0f; coords1_6d[5] = 0.0f;
    
    coords2_6d[0] = 1.0f; coords2_6d[1] = 0.0f; coords2_6d[2] = 0.0f;
    coords2_6d[3] = 1.0f; coords2_6d[4] = 0.0f; coords2_6d[5] = 0.0f;
    
    p1_6d = TCDE_CreatePoint(6, coords1_6d);
    p2_6d = TCDE_CreatePoint(6, coords2_6d);
    m6d = TCDE_CreateMetric(6);
    
    float dist_general = TCDE_GeodesicDistance(&p1_6d, &p2_6d, &m6d);
    float dist_optimized = TCDE_GeodesicDistanceOptimized(&p1_6d, &p2_6d, &m6d);
    
    TEST("Optimized distance matches general", 
         fabsf(dist_optimized - dist_general) < EPSILON);
    TEST("Optimized distance value", fabsf(dist_optimized - 1.0f) < EPSILON);
    
    TCDE_FreePoint(&p1_6d);
    TCDE_FreePoint(&p2_6d);
    TCDE_FreeMetric(&m6d);
    
    printf("\n");
    
    // ========================================================================
    // Test 9: Triangle inequality verification
    // ========================================================================
    printf("--- Test 9: Triangle inequality ---\n");
    
    // Create three points forming a triangle
    float coords_p[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    float coords_q[6] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    float coords_r[6] = {0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f};
    
    TCDE_Point p = TCDE_CreatePoint(6, coords_p);
    TCDE_Point q = TCDE_CreatePoint(6, coords_q);
    TCDE_Point r = TCDE_CreatePoint(6, coords_r);
    m6d = TCDE_CreateMetric(6);
    
    bool triangle_valid = TCDE_VerifyTriangleInequality(&p, &q, &r, &m6d);
    TEST("Triangle inequality holds", triangle_valid);
    
    TCDE_FreePoint(&p);
    TCDE_FreePoint(&q);
    TCDE_FreePoint(&r);
    TCDE_FreeMetric(&m6d);
    
    printf("\n");
    
    // ========================================================================
    // Test 10: Dimension-agnostic operations (Requirement 13.2)
    // ========================================================================
    printf("--- Test 10: Dimension-agnostic operations ---\n");
    
    // Test that same functions work for 2D and 6D
    TCDE_Metric m2 = TCDE_CreateMetric(2);
    TCDE_Metric m6 = TCDE_CreateMetric(6);
    
    TEST("Same create function for 2D and 6D", m2.g != NULL && m6.g != NULL);
    TEST("Same validate function for 2D and 6D", 
         TCDE_ValidateMetric(&m2) && TCDE_ValidateMetric(&m6));
    
    // Test update works for both
    m2.g[0][0] = 1.5f;
    m6.g[0][0] = 1.5f;
    TCDE_UpdateMetric(&m2);
    TCDE_UpdateMetric(&m6);
    TEST("Same update function for 2D and 6D", m2.is_valid && m6.is_valid);
    
    TCDE_FreeMetric(&m2);
    TCDE_FreeMetric(&m6);
    
    printf("\n");
    
    // ========================================================================
    // Summary
    // ========================================================================
    printf("=== Task 2.1 Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", 100.0f * tests_passed / tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 2.1 COMPLETE: All metric tests passed!\n");
        printf("  - Identity metric (baseline) ✓\n");
        printf("  - Metric inversion and determinant ✓\n");
        printf("  - Basic geodesic distance ✓\n");
        printf("  - Dimension-agnostic design ✓\n");
        return 0;
    } else {
        printf("\n✗ Task 2.1 INCOMPLETE: Some tests failed\n");
        return 1;
    }
}
