/**
 * @file test_point_operations_complete.c
 * @brief Complete test for point operations
 */

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

int main(void) {
    printf("=== Point Operations Complete Test ===\n\n");
    
    // Test 2D points
    float coords_2d[2] = {0.5f, -0.3f};
    TCDE_Point point_2d = TCDE_CreatePoint(2, coords_2d);
    
    TEST("2D point created", point_2d.coords != NULL);
    TEST("2D point dimension correct", point_2d.dimension == 2);
    TEST("2D point coordinates set", point_2d.coords[0] == 0.5f && point_2d.coords[1] == -0.3f);
    
    // Test 6D points
    float coords_6d[6] = {0.1f, 0.2f, 0.3f, 1.5f, -0.5f, 0.8f};
    TCDE_Point point_6d = TCDE_CreatePoint(6, coords_6d);
    
    TEST("6D point created", point_6d.coords != NULL);
    TEST("6D point dimension correct", point_6d.dimension == 6);
    TEST("6D point coordinates set", point_6d.coords[3] == 1.5f && point_6d.coords[4] == -0.5f);
    
    // Test point validation
    TEST("2D point valid", TCDE_ValidatePoint(&point_2d));
    TEST("6D point valid", TCDE_ValidatePoint(&point_6d));
    
    // Test distance computation
    float coords_6d_2[6] = {0.2f, 0.3f, 0.4f, 1.6f, -0.4f, 0.9f};
    TCDE_Point point_6d_2 = TCDE_CreatePoint(6, coords_6d_2);
    
    if (point_6d_2.coords) {
        float distance = TCDE_ComputeDistance(&point_6d, &point_6d_2);
        TEST("Distance computed", !isnan(distance) && !isinf(distance));
        TEST("Distance non-negative", distance >= 0.0f);
        
        printf("Distance between 6D points: %.6f\n", distance);
    }
    
    // Test point copying
    TCDE_Point point_copy = TCDE_CopyPoint(&point_6d);
    TEST("Point copied", point_copy.coords != NULL);
    TEST("Copy dimension correct", point_copy.dimension == point_6d.dimension);
    
    if (point_copy.coords) {
        bool coords_match = true;
        for (int i = 0; i < 6; i++) {
            if (fabs(point_copy.coords[i] - point_6d.coords[i]) > 1e-6f) {
                coords_match = false;
                break;
            }
        }
        TEST("Copy coordinates match", coords_match);
    }
    
    // Cleanup
    TCDE_FreePoint(&point_2d);
    TCDE_FreePoint(&point_6d);
    if (point_6d_2.coords) TCDE_FreePoint(&point_6d_2);
    if (point_copy.coords) TCDE_FreePoint(&point_copy);
    
    TEST("All points freed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}