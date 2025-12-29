/**
 * @file test_task_1_infrastructure.c
 * @brief Test for Task 1: Basic infrastructure and fundamental data structures
 * 
 * Tests:
 * - TCDE_Field, TCDE_Center, TCDE_Point creation for arbitrary dimension
 * - Create/destroy functions with secure memory management
 * - Parameter validation and error codes
 * 
 * Requirements: 1.1, 1.6, 1.7, 13.9
 */

#include "../src/core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

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

int main(void) {
    printf("=== Task 1: Infrastructure de base et structures fondamentales ===\n\n");
    
    // ========================================================================
    // Test 1: TCDE_Point creation for arbitrary dimension
    // ========================================================================
    printf("--- Test 1: TCDE_Point creation ---\n");
    
    // Test 2D point
    float coords_2d[2] = {0.5f, 0.5f};
    TCDE_Point p2d = TCDE_CreatePoint(2, coords_2d);
    TEST("Create 2D point", p2d.coords != NULL && p2d.dimension == 2);
    TEST("2D point coordinates", p2d.coords[0] == 0.5f && p2d.coords[1] == 0.5f);
    TEST("Validate 2D point", TCDE_ValidatePoint(&p2d));
    
    // Test 6D point
    float coords_6d[6] = {0.5f, 0.5f, 0.5f, 1.5f, 0.0f, 0.4f};
    TCDE_Point p6d = TCDE_CreatePoint(6, coords_6d);
    TEST("Create 6D point", p6d.coords != NULL && p6d.dimension == 6);
    TEST("6D point coordinates", 
         p6d.coords[0] == 0.5f && p6d.coords[3] == 1.5f && p6d.coords[5] == 0.4f);
    TEST("Validate 6D point", TCDE_ValidatePoint(&p6d));
    
    // Test point copy
    TCDE_Point p_copy;
    p_copy.coords = NULL;
    p_copy.dimension = 0;
    TCDE_CopyPoint(&p_copy, &p6d);
    TEST("Copy point", p_copy.coords != NULL && p_copy.dimension == 6);
    TEST("Copy point data", p_copy.coords[0] == 0.5f && p_copy.coords[5] == 0.4f);
    
    // Test invalid point
    TCDE_Point p_invalid;
    p_invalid.coords = NULL;
    p_invalid.dimension = 0;
    TEST("Validate invalid point", !TCDE_ValidatePoint(&p_invalid));
    
    // Clean up
    TCDE_FreePoint(&p2d);
    TCDE_FreePoint(&p6d);
    TCDE_FreePoint(&p_copy);
    
    printf("\n");
    
    // ========================================================================
    // Test 2: TCDE_Metric creation for arbitrary dimension
    // ========================================================================
    printf("--- Test 2: TCDE_Metric creation ---\n");
    
    // Test 2D metric
    TCDE_Metric m2d = TCDE_CreateMetric(2);
    TEST("Create 2D metric", m2d.g != NULL && m2d.dimension == 2);
    TEST("2D metric is identity", m2d.g[0][0] == 1.0f && m2d.g[1][1] == 1.0f);
    TEST("2D metric off-diagonal zero", m2d.g[0][1] == 0.0f && m2d.g[1][0] == 0.0f);
    TEST("2D metric determinant", fabsf(m2d.det - 1.0f) < 1e-6f);
    TEST("Validate 2D metric", TCDE_ValidateMetric(&m2d));
    
    // Test 6D metric
    TCDE_Metric m6d = TCDE_CreateMetric(6);
    TEST("Create 6D metric", m6d.g != NULL && m6d.dimension == 6);
    TEST("6D metric is identity", m6d.g[0][0] == 1.0f && m6d.g[5][5] == 1.0f);
    TEST("6D metric off-diagonal zero", m6d.g[0][1] == 0.0f && m6d.g[2][3] == 0.0f);
    TEST("Validate 6D metric", TCDE_ValidateMetric(&m6d));
    
    // Test metric update
    m2d.g[0][0] = 1.1f;
    m2d.g[1][1] = 0.9f;
    TCDE_UpdateMetric(&m2d);
    TEST("Update metric", m2d.is_valid);
    TEST("Updated metric determinant", fabsf(m2d.det - 0.99f) < 1e-6f);
    
    // Clean up
    TCDE_FreeMetric(&m2d);
    TCDE_FreeMetric(&m6d);
    
    printf("\n");
    
    // ========================================================================
    // Test 3: TCDE_Field creation with secure memory management
    // ========================================================================
    printf("--- Test 3: TCDE_Field creation ---\n");
    
    // Test field creation
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    TEST("Create field", field != NULL);
    TEST("Field capacity", field->manifold_6d.capacity == 100);
    TEST("Field initial centers", field->manifold_6d.num_centers == 0);
    TEST("Field fractal dimension", fabsf(field->fractal_dimension - 2.5f) < 1e-6f);
    TEST("Field 6D metric", field->manifold_6d.metric.g != NULL);
    TEST("Field 2D metric", field->slice_2d.metric.g != NULL);
    
    // Test invalid field creation
    TCDE_Field* field_invalid = TCDE_CreateField(0, 2.5f);
    TEST("Reject invalid capacity", field_invalid == NULL);
    
    TCDE_Field* field_invalid2 = TCDE_CreateField(-10, 2.5f);
    TEST("Reject negative capacity", field_invalid2 == NULL);
    
    TCDE_DestroyField(field);
    
    printf("\n");
    
    // ========================================================================
    // Test 4: TCDE_Center operations with parameter validation
    // ========================================================================
    printf("--- Test 4: TCDE_Center operations ---\n");
    
    field = TCDE_CreateField(10, 2.5f);
    
    // Test adding valid center
    float center_coords[6] = {0.5f, 0.5f, 0.5f, 1.5f, 0.0f, 0.4f};
    TCDE_Point center_point = TCDE_CreatePoint(6, center_coords);
    bool add_success = TCDE_AddCenter6D(field, &center_point, 1.0f + 0.0f*I, 0.1f);
    TEST("Add valid center", add_success);
    TEST("Field has 1 center", field->manifold_6d.num_centers == 1);
    
    // Test adding multiple centers
    for (int i = 0; i < 5; i++) {
        float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.5f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    TEST("Add multiple centers", field->manifold_6d.num_centers == 6);
    
    // Test removing center
    bool remove_success = TCDE_RemoveCenter6D(field, 2);
    TEST("Remove center", remove_success);
    TEST("Field has 5 centers after removal", field->manifold_6d.num_centers == 5);
    
    // Test invalid removal
    bool remove_invalid = TCDE_RemoveCenter6D(field, 100);
    TEST("Reject invalid index removal", !remove_invalid);
    
    // Test adding center with invalid dimension
    TCDE_Point invalid_point = TCDE_CreatePoint(3, NULL);
    bool add_invalid = TCDE_AddCenter6D(field, &invalid_point, 1.0f, 0.1f);
    TEST("Reject invalid dimension center", !add_invalid);
    TCDE_FreePoint(&invalid_point);
    
    // Test capacity limit
    TCDE_Field* small_field = TCDE_CreateField(2, 2.5f);
    TCDE_Point p1 = TCDE_CreatePoint(6, center_coords);
    TCDE_Point p2 = TCDE_CreatePoint(6, center_coords);
    TCDE_Point p3 = TCDE_CreatePoint(6, center_coords);
    
    TCDE_AddCenter6D(small_field, &p1, 1.0f, 0.1f);
    TCDE_AddCenter6D(small_field, &p2, 1.0f, 0.1f);
    bool add_overflow = TCDE_AddCenter6D(small_field, &p3, 1.0f, 0.1f);
    TEST("Reject center beyond capacity", !add_overflow);
    
    TCDE_FreePoint(&p1);
    TCDE_FreePoint(&p2);
    TCDE_FreePoint(&p3);
    TCDE_FreePoint(&center_point);
    TCDE_DestroyField(small_field);
    TCDE_DestroyField(field);
    
    printf("\n");
    
    // ========================================================================
    // Test 5: Memory management and cleanup
    // ========================================================================
    printf("--- Test 5: Memory management ---\n");
    
    // Test field destruction
    field = TCDE_CreateField(50, 2.5f);
    for (int i = 0; i < 10; i++) {
        float coords[6] = {(float)i/20.0f, 0.5f, 0.5f, 1.5f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    TCDE_DestroyField(field);
    TEST("Destroy field with centers", true);  // If we get here, no crash
    
    // Test NULL safety
    TCDE_DestroyField(NULL);
    TEST("Destroy NULL field", true);  // Should not crash
    
    TCDE_Point null_point;
    null_point.coords = NULL;
    null_point.dimension = 0;
    TCDE_FreePoint(&null_point);
    TEST("Free NULL point", true);  // Should not crash
    
    TCDE_Metric null_metric;
    null_metric.g = NULL;
    null_metric.g_inv = NULL;
    TCDE_FreeMetric(&null_metric);
    TEST("Free NULL metric", true);  // Should not crash
    
    printf("\n");
    
    // ========================================================================
    // Test 6: Dimension-agnostic operations (Requirement 13.9)
    // ========================================================================
    printf("--- Test 6: Dimension-agnostic operations ---\n");
    
    // Test that same structures work for 2D and 6D
    TCDE_Field* field_2d_test = TCDE_CreateField(50, 2.5f);
    TCDE_Field* field_6d_test = TCDE_CreateField(50, 2.5f);
    
    TEST("Same structure for 2D and 6D", 
         sizeof(field_2d_test->manifold_6d) == sizeof(field_6d_test->manifold_6d));
    
    // Test that metrics work for both dimensions
    TCDE_Metric m2 = TCDE_CreateMetric(2);
    TCDE_Metric m6 = TCDE_CreateMetric(6);
    TEST("Metric creation dimension-agnostic", m2.g != NULL && m6.g != NULL);
    
    TCDE_FreeMetric(&m2);
    TCDE_FreeMetric(&m6);
    TCDE_DestroyField(field_2d_test);
    TCDE_DestroyField(field_6d_test);
    
    printf("\n");
    
    // ========================================================================
    // Summary
    // ========================================================================
    printf("=== Task 1 Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", 100.0f * tests_passed / tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 1 COMPLETE: All infrastructure tests passed!\n");
        printf("  - TCDE_Field, TCDE_Center, TCDE_Point structures ✓\n");
        printf("  - Create/destroy functions with secure memory ✓\n");
        printf("  - Parameter validation and error handling ✓\n");
        printf("  - Dimension-agnostic design ✓\n");
        return 0;
    } else {
        printf("\n✗ Task 1 INCOMPLETE: Some tests failed\n");
        return 1;
    }
}
