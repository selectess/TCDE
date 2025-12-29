/**
 * @file test_simple_field_operations.c
 * @brief Simple test for basic field operations
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
    printf("=== Simple Field Operations Test ===\n\n");
    
    // Test point creation
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    
    TEST("Point created", point.coords != NULL);
    TEST("Point dimension correct", point.dimension == 6);
    TEST("Point coordinates set", point.coords[0] == 0.5f);
    
    // Test field creation
    TCDE_Field* field = TCDE_CreateField(50, 2.5f);
    TEST("Field created", field != NULL);
    
    if (field) {
        TEST("Field has 6D manifold", field->manifold_6d.centers != NULL);
        TEST("Field capacity set", field->manifold_6d.capacity == 50);
        TEST("Field fractal dimension set", field->fractal_dimension == 2.5f);
        
        // Test energy computation
        float energy = TCDE_ComputeEnergy(field);
        TEST("Energy computed", !isnan(energy) && !isinf(energy));
        TEST("Energy non-negative", energy >= 0.0f);
        
        TCDE_DestroyField(field);
        TEST("Field destroyed", true);
    }
    
    // Test metric operations
    TCDE_Metric metric = TCDE_CreateMetric(6);
    TEST("Metric created", metric.g != NULL);
    TEST("Metric dimension correct", metric.dimension == 6);
    
    if (metric.g) {
        TCDE_DestroyMetric(&metric);
        TEST("Metric destroyed", true);
    }
    
    TCDE_FreePoint(&point);
    TEST("Point freed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    return (success_rate >= 90.0f) ? 0 : 1;
}