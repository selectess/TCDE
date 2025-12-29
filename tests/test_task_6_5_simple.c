/**
 * @file test_task_6_5_simple.c
 * @brief Simple test for Task 6.5 - Fractal Dimension Validation
 */

#include "../src/core/tcde_core.h"
#include "../tests/tcde_validation.h"
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

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Task 6.5: Fractal Dimension Validation                   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    srand((unsigned int)time(NULL));
    
    // Test 1: Empty field
    printf("\nTest 1: Empty Field\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        float dimension = TCDE_VerifyFractalDimension(field, 0.1f, 0.5f, 8);
        printf("  Fractal dimension: %.3f\n", dimension);
        TEST("Empty field has dimension ~2", fabsf(dimension - 2.0f) < 0.1f);
        TCDE_DestroyField(field);
    }
    
    // Test 2: Simple field
    printf("\nTest 2: Simple Regular Field\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        for (int i = 0; i < 9; i++) {
            float coords[6] = {0.2f + 0.3f * (i % 3), 0.2f + 0.3f * (i / 3), 0.5f, 1.0f, 0.0f, 0.4f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        float dimension = TCDE_VerifyFractalDimension(field, 0.1f, 0.5f, 8);
        printf("  Fractal dimension: %.3f\n", dimension);
        TEST("Regular field has valid dimension", dimension >= 2.0f && dimension <= 3.0f);
        TCDE_DestroyField(field);
    }
    
    // Test 3: Complex field
    printf("\nTest 3: Complex Field\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.8f);
        for (int i = 0; i < 20; i++) {
            float coords[6] = {
                (float)rand() / RAND_MAX,
                (float)rand() / RAND_MAX,
                (float)rand() / RAND_MAX,
                1.0f, 0.0f, 0.4f
            };
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 0.8f, 0.08f);
            TCDE_FreePoint(&p);
        }
        float dimension = TCDE_VerifyFractalDimension(field, 0.05f, 0.4f, 10);
        printf("  Fractal dimension: %.3f\n", dimension);
        TEST("Complex field has valid dimension", dimension >= 2.0f && dimension <= 3.0f);
        TCDE_DestroyField(field);
    }
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Results: %d/%d tests passed (%.1f%%)                      ║\n", 
           tests_passed, tests_total, 100.0f * tests_passed / tests_total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 6.5 VALIDATED: Fractal dimension measurement\n");
        printf("  - COGNITIVE VALIDATION FRAMEWORK COMPLETE!\n");
        return 0;
    }
    return 1;
}
