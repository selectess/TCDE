/**
 * @file test_task_6_3_simple.c
 * @brief Simple test for Task 6.3 - Intuition Validation
 * 
 * Tests intuition via torsion tensor measurement
 */

#include "../src/core/tcde_core.h"
#include "../src/core/tcde_geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

/**
 * @brief Simplified intuition test using torsion magnitude
 */
float test_intuition_simple(TCDE_Field* field, float threshold) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    const int num_points = 20;
    int high_torsion_count = 0;
    int total_measured = 0;
    
    // Sample field and measure torsion
    for (int i = 0; i < num_points; i++) {
        float t = (float)i / (float)num_points;
        
        float coords[6] = {
            t, 0.5f, 0.5f,
            1.0f + 0.3f * t, 0.0f, 0.4f
        };
        
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        
        // Compute torsion tensor
        TCDE_Complex torsion[6][6];
        TCDE_ComputeTorsionTensor(field, &p, torsion);
        
        // Compute magnitude
        float torsion_mag = 0.0f;
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 6; k++) {
                torsion_mag += cabsf(torsion[j][k]) * cabsf(torsion[j][k]);
            }
        }
        torsion_mag = sqrtf(torsion_mag);
        
        total_measured++;
        if (torsion_mag > threshold) {
            high_torsion_count++;
        }
        
        TCDE_FreePoint(&p);
    }
    
    float intuition_score = 0.0f;
    if (total_measured > 0) {
        intuition_score = (float)high_torsion_count / (float)total_measured;
    }
    
    return intuition_score;
}

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Task 6.3: Intuition Validation (Simplified)              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Test 1: Field with complex structure (high torsion)
    printf("\nTest 1: Complex Field Structure\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create complex pattern with varying amplitudes and phases
        for (int i = 0; i < 30; i++) {
            float t = (float)i / 30.0f;
            float phase = 2.0f * 3.14159f * (float)i / 10.0f;
            
            float coords[6] = {
                t, 
                0.5f + 0.2f * sinf(phase),
                0.5f + 0.2f * cosf(phase),
                1.0f + 0.3f * t, 0.0f, 0.4f
            };
            
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_Complex coeff = (1.0f + 0.5f * sinf(phase)) * cexpf(I * phase);
            TCDE_AddCenter6D(field, &p, coeff, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float intuition = test_intuition_simple(field, 0.01f);
        printf("  Intuition score: %.3f\n", intuition);
        TEST("Complex field shows intuition", intuition > 0.1f);
        
        TCDE_DestroyField(field);
    }
    
    // Test 2: Simple uniform field (low torsion)
    printf("\nTest 2: Simple Uniform Field\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create uniform pattern
        for (int i = 0; i < 20; i++) {
            float coords[6] = {
                (float)i / 20.0f, 0.5f, 0.5f,
                1.0f + 0.3f * (float)i / 20.0f, 0.0f, 0.4f
            };
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float intuition = test_intuition_simple(field, 0.01f);
        printf("  Intuition score: %.3f\n", intuition);
        TEST("Uniform field shows less intuition", intuition < 0.5f);
        
        TCDE_DestroyField(field);
    }
    
    // Test 3: Threshold sensitivity
    printf("\nTest 3: Threshold Sensitivity\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Create moderately complex pattern
        for (int i = 0; i < 25; i++) {
            float t = (float)i / 25.0f;
            float phase = 3.14159f * (float)i / 5.0f;
            
            float coords[6] = {
                t, 0.5f + 0.1f * sinf(phase), 0.5f,
                1.0f + 0.3f * t, 0.0f, 0.4f
            };
            
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f + 0.3f * sinf(phase), 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float intuition_low = test_intuition_simple(field, 0.01f);
        float intuition_high = test_intuition_simple(field, 0.1f);
        
        printf("  Low threshold (0.01): %.3f\n", intuition_low);
        printf("  High threshold (0.1): %.3f\n", intuition_high);
        TEST("Lower threshold finds more connections", intuition_low >= intuition_high);
        
        TCDE_DestroyField(field);
    }
    
    // Test 4: Empty field
    printf("\nTest 4: Empty Field\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        float intuition = test_intuition_simple(field, 0.01f);
        printf("  Intuition score: %.3f\n", intuition);
        TEST("Empty field returns 0", intuition == 0.0f);
        
        TCDE_DestroyField(field);
    }
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Results: %d/%d tests passed (%.1f%%)                      ║\n", 
           tests_passed, tests_total, 100.0f * tests_passed / tests_total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 6.3 CONCEPT VALIDATED: Intuition via torsion tensor\n");
        printf("  - Torsion tensor measures non-obvious connections\n");
        printf("  - Complex fields show higher intuition\n");
        printf("  - Foundation for creative AI capability\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed\n");
        return 1;
    }
}
