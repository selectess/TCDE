/**
 * @file test_task_5_5_simple.c
 * @brief Test for Task 5.5 - Modality Transformation
 * 
 * Tests continuous rotation in modality dimension
 */

#include "../src/core/tcde_core.h"
#include "../src/utils/tcde_multimodal.h"
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
    printf("║  Task 5.5: Modality Transformation                        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    srand((unsigned int)time(NULL));
    
    // Test 1: Basic modality transformation
    printf("\nTest 1: Basic Modality Transformation\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create centers in visual modality (m=0.0)
        for (int i = 0; i < 10; i++) {
            float coords[6] = {
                0.5f + 0.1f * cosf(2*M_PI*i/10),
                0.5f + 0.1f * sinf(2*M_PI*i/10),
                0.5f, 1.0f, 0.0f, 0.0f  // Visual modality
            };
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Transform visual → semantic (0.0 → 0.4)
        bool success = TCDE_ModalityRotate(field, 0.0f, 0.4f, false);
        TEST("Transformation succeeds", success);
        
        // Check that centers moved to semantic modality
        int semantic_count = 0;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            float m = field->manifold_6d.centers[i].point.coords[5];
            if (fabsf(m - 0.4f) < 0.1f) semantic_count++;
        }
        
        printf("  Centers in semantic modality: %d/10\n", semantic_count);
        TEST("Centers transformed to target modality", semantic_count >= 8);
        
        TCDE_DestroyField(field);
    }
    
    // Test 2: Energy preservation
    printf("\nTest 2: Energy Preservation\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create field with known energy
        for (int i = 0; i < 15; i++) {
            float coords[6] = {
                (float)rand() / RAND_MAX,
                (float)rand() / RAND_MAX,
                (float)rand() / RAND_MAX,
                1.0f, 0.0f, 0.2f  // Auditory modality
            };
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float energy_before = TCDE_ComputeEnergy(field);
        printf("  Energy before: %.6f\n", energy_before);
        
        // Transform with energy preservation
        TCDE_ModalityRotate(field, 0.2f, 0.4f, true);
        
        float energy_after = TCDE_ComputeEnergy(field);
        printf("  Energy after:  %.6f\n", energy_after);
        
        float energy_ratio = energy_after / energy_before;
        printf("  Energy ratio:  %.6f\n", energy_ratio);
        
        TEST("Energy preserved", fabsf(energy_ratio - 1.0f) < 0.01f);
        
        TCDE_DestroyField(field);
    }
    
    // Test 3: Modality interpolation
    printf("\nTest 3: Modality Interpolation\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create centers in visual modality
        for (int i = 0; i < 10; i++) {
            float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};
            coords[0] += 0.1f * (float)i / 10.0f;
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Interpolate halfway to semantic (t=0.5)
        bool success = TCDE_ModalityInterpolate(field, 0.0f, 0.4f, 0.5f);
        TEST("Interpolation succeeds", success);
        
        // Check centers are at intermediate modality
        float avg_modality = 0.0f;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            avg_modality += field->manifold_6d.centers[i].point.coords[5];
        }
        avg_modality /= field->manifold_6d.num_centers;
        
        printf("  Average modality: %.3f (expected: 0.2)\n", avg_modality);
        TEST("Interpolation at correct position", fabsf(avg_modality - 0.2f) < 0.05f);
        
        TCDE_DestroyField(field);
    }
    
    // Test 4: Transformation verification
    printf("\nTest 4: Transformation Verification\n");
    {
        TCDE_Field* field_before = TCDE_CreateField(100, 2.5f);
        TCDE_Field* field_after = TCDE_CreateField(100, 2.5f);
        
        // Create identical fields
        for (int i = 0; i < 10; i++) {
            float coords[6] = {
                0.3f + 0.1f * (float)i,
                0.5f, 0.5f, 1.0f, 0.0f, 0.0f
            };
            
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field_before, &p, 1.0f, 0.1f);
            TCDE_AddCenter6D(field_after, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Transform second field
        TCDE_ModalityRotate(field_after, 0.0f, 0.4f, true);
        
        // Verify transformation
        bool valid = TCDE_ModalityVerifyTransform(field_before, field_after, 0.01f);
        TEST("Transformation verification", valid);
        
        TCDE_DestroyField(field_before);
        TCDE_DestroyField(field_after);
    }
    
    // Test 5: Empty field handling
    printf("\nTest 5: Empty Field Handling\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        bool success = TCDE_ModalityRotate(field, 0.0f, 0.4f, false);
        TEST("Empty field transformation succeeds", success);
        
        TCDE_DestroyField(field);
    }
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Results: %d/%d tests passed (%.1f%%)                      ║\n", 
           tests_passed, tests_total, 100.0f * tests_passed / tests_total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 5.5 COMPLETE: Modality transformation\n");
        printf("  - Continuous rotation in m-dimension\n");
        printf("  - Energy preservation\n");
        printf("  - Smooth interpolation\n");
        printf("  - Transformation verification\n");
        printf("  - Multimodal processing COMPLETE!\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed\n");
        return 1;
    }
}
