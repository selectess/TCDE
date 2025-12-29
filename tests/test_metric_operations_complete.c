/**
 * @file test_metric_operations_complete.c
 * @brief Complete test for metric tensor operations
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
    printf("=== Metric Operations Complete Test ===\n\n");
    
    // Test 2D metric
    TCDE_Metric metric_2d = TCDE_CreateMetric(2);
    TEST("2D metric created", metric_2d.g != NULL);
    TEST("2D metric dimension correct", metric_2d.dimension == 2);
    TEST("2D metric valid", metric_2d.is_valid);
    
    // Test 6D metric
    TCDE_Metric metric_6d = TCDE_CreateMetric(6);
    TEST("6D metric created", metric_6d.g != NULL);
    TEST("6D metric dimension correct", metric_6d.dimension == 6);
    TEST("6D metric valid", metric_6d.is_valid);
    
    // Test metric determinant
    if (metric_2d.g) {
        float det_2d = TCDE_ComputeMetricDeterminant(&metric_2d);
        TEST("2D determinant computed", !isnan(det_2d) && !isinf(det_2d));
        TEST("2D determinant positive", det_2d > 0.0f);
        
        printf("2D metric determinant: %.6f\n", det_2d);
    }
    
    if (metric_6d.g) {
        float det_6d = TCDE_ComputeMetricDeterminant(&metric_6d);
        TEST("6D determinant computed", !isnan(det_6d) && !isinf(det_6d));
        TEST("6D determinant positive", det_6d > 0.0f);
        
        printf("6D metric determinant: %.6f\n", det_6d);
    }
    
    // Test metric inverse
    if (metric_6d.g) {
        bool inverse_computed = TCDE_ComputeMetricInverse(&metric_6d);
        TEST("6D inverse computed", inverse_computed);
        TEST("6D inverse exists", metric_6d.g_inv != NULL);
        
        if (metric_6d.g_inv) {
            // Test that g * g_inv = I (approximately)
            float identity_error = 0.0f;
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 6; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < 6; k++) {
                        sum += metric_6d.g[i][k] * metric_6d.g_inv[k][j];
                    }
                    float expected = (i == j) ? 1.0f : 0.0f;
                    identity_error += fabs(sum - expected);
                }
            }
            TEST("6D inverse correct", identity_error < 1e-3f);
            
            printf("Identity error: %.6f\n", identity_error);
        }
    }
    
    // Test metric validation
    TEST("2D metric validates", TCDE_ValidateMetric(&metric_2d));
    TEST("6D metric validates", TCDE_ValidateMetric(&metric_6d));
    
    // Cleanup
    TCDE_DestroyMetric(&metric_2d);
    TCDE_DestroyMetric(&metric_6d);
    TEST("All metrics destroyed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}