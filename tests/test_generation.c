/**
 * @file test_generation.c
 * @brief TCDE Generation System Test Suite
 * 
 * Tests the generation capabilities of TCDE including:
 * - Pattern generation
 * - Field generation
 * - Center generation
 * - Emergent structure generation
 * 
 * @version 1.0
 * @date December 14, 2025
 */

#include "core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// ============================================================================
// TEST FUNCTIONS
// ============================================================================

/**
 * @brief Test generation system creation
 */
bool test_generation_creation() {
    printf("  Testing generation system creation...\n");
    
    // Create a field for generation testing
    TCDE_Field* field = TCDE_CreateField(10, 2.5f);
    if (!field) {
        printf("    ❌ Failed to create field\n");
        return false;
    }
    
    // Verify field is properly initialized
    if (field->manifold_6d.capacity != 10) {
        printf("    ❌ Field capacity incorrect: %d != 10\n", field->manifold_6d.capacity);
        TCDE_DestroyField(field);
        return false;
    }
    
    if (field->fractal_dimension != 2.5f) {
        printf("    ❌ Fractal dimension incorrect: %f != 2.5\n", field->fractal_dimension);
        TCDE_DestroyField(field);
        return false;
    }
    
    TCDE_DestroyField(field);
    printf("    ✅ Generation system creation successful\n");
    return true;
}

/**
 * @brief Test pattern generation
 */
bool test_pattern_generation() {
    printf("  Testing pattern generation...\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(20, 2.3f);
    if (!field) {
        printf("    ❌ Failed to create field\n");
        return false;
    }
    
    // Generate initial pattern by adding centers
    int centers_added = 0;
    for (int i = 0; i < 5; i++) {
        float coords[6] = {
            (float)i * 0.2f - 0.4f,  // x
            sinf((float)i * 0.5f),   // y
            cosf((float)i * 0.3f),   // z
            1.5f,                    // τ₁
            0.0f,                    // τ₂
            0.4f                     // m
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = (0.5f + 0.1f * i) + (0.2f * sinf(i)) * I;
        float epsilon = 0.8f + 0.1f * i;
        
        bool added = TCDE_AddCenter6D(field, &point, coeff, epsilon);
        if (added) {
            centers_added++;
        }
        
        TCDE_FreePoint(&point);
    }
    
    if (centers_added != 5) {
        printf("    ❌ Pattern generation failed: only %d/5 centers added\n", centers_added);
        TCDE_DestroyField(field);
        return false;
    }
    
    // Test pattern evaluation
    float test_coords[6] = {0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 0.4f};
    TCDE_Point test_point = TCDE_CreatePoint(6, test_coords);
    TCDE_Complex field_value = TCDE_Evaluate6D(field, &test_point);
    
    float magnitude = cabsf(field_value);
    if (magnitude < 1e-6f) {
        printf("    ❌ Generated pattern has zero magnitude: %f\n", magnitude);
        TCDE_FreePoint(&test_point);
        TCDE_DestroyField(field);
        return false;
    }
    
    printf("    ✅ Pattern generated with magnitude: %f\n", magnitude);
    
    TCDE_FreePoint(&test_point);
    TCDE_DestroyField(field);
    return true;
}

/**
 * @brief Test emergent structure generation
 */
bool test_emergent_structure_generation() {
    printf("  Testing emergent structure generation...\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(15, 2.7f);
    if (!field) {
        printf("    ❌ Failed to create field\n");
        return false;
    }
    
    // Generate clustered centers to create emergent structures
    int cluster_centers = 0;
    
    // Cluster 1: Around origin
    for (int i = 0; i < 3; i++) {
        float angle = (float)i * 2.0f * M_PI / 3.0f;
        float coords[6] = {
            0.2f * cosf(angle),      // x
            0.2f * sinf(angle),      // y
            0.1f,                    // z
            1.5f,                    // τ₁
            0.0f,                    // τ₂
            0.4f                     // m
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.6f + 0.1f * I;
        float epsilon = 0.5f;
        
        bool added = TCDE_AddCenter6D(field, &point, coeff, epsilon);
        if (added) cluster_centers++;
        
        TCDE_FreePoint(&point);
    }
    
    // Cluster 2: Offset location
    for (int i = 0; i < 3; i++) {
        float angle = (float)i * 2.0f * M_PI / 3.0f;
        float coords[6] = {
            1.0f + 0.2f * cosf(angle),  // x
            0.5f + 0.2f * sinf(angle),  // y
            0.2f,                       // z
            1.5f,                       // τ₁
            0.0f,                       // τ₂
            0.4f                        // m
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.5f - 0.1f * I;
        float epsilon = 0.6f;
        
        bool added = TCDE_AddCenter6D(field, &point, coeff, epsilon);
        if (added) cluster_centers++;
        
        TCDE_FreePoint(&point);
    }
    
    if (cluster_centers < 4) {
        printf("    ❌ Insufficient centers for structure generation: %d < 4\n", cluster_centers);
        TCDE_DestroyField(field);
        return false;
    }
    
    // Measure field energy to verify structure
    float energy = TCDE_ComputeEnergy(field);
    if (energy < 1e-3f) {
        printf("    ❌ Generated structure has insufficient energy: %f\n", energy);
        TCDE_DestroyField(field);
        return false;
    }
    
    printf("    ✅ Emergent structure generated with energy: %f\n", energy);
    
    TCDE_DestroyField(field);
    return true;
}

/**
 * @brief Test adaptive generation
 */
bool test_adaptive_generation() {
    printf("  Testing adaptive generation...\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(25, 2.4f);
    if (!field) {
        printf("    ❌ Failed to create field\n");
        return false;
    }
    
    // Add initial seed centers
    float coords1[6] = {0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 0.4f};
    TCDE_Point point1 = TCDE_CreatePoint(6, coords1);
    TCDE_Complex coeff1 = 0.8f + 0.2f * I;
    TCDE_AddCenter6D(field, &point1, coeff1, 0.7f);
    
    float coords2[6] = {0.5f, 0.3f, 0.1f, 1.5f, 0.0f, 0.4f};
    TCDE_Point point2 = TCDE_CreatePoint(6, coords2);
    TCDE_Complex coeff2 = 0.6f - 0.3f * I;
    TCDE_AddCenter6D(field, &point2, coeff2, 0.8f);
    
    int initial_centers = field->manifold_6d.num_centers;
    
    // Simulate adaptive generation by evaluating field at various points
    // and adding centers where field is weak
    int adaptive_centers = 0;
    
    for (int i = 0; i < 10; i++) {
        float test_coords[6] = {
            ((float)rand() / RAND_MAX) * 2.0f - 1.0f,  // x
            ((float)rand() / RAND_MAX) * 2.0f - 1.0f,  // y
            ((float)rand() / RAND_MAX) * 0.5f,         // z
            1.5f,                                      // τ₁
            0.0f,                                      // τ₂
            0.4f                                       // m
        };
        
        TCDE_Point test_point = TCDE_CreatePoint(6, test_coords);
        TCDE_Complex field_value = TCDE_Evaluate6D(field, &test_point);
        
        // If field is weak at this point, add a center
        if (cabsf(field_value) < 0.1f && field->manifold_6d.num_centers < field->manifold_6d.capacity) {
            TCDE_Complex adaptive_coeff = 0.4f + 0.1f * I;
            bool added = TCDE_AddCenter6D(field, &test_point, adaptive_coeff, 0.6f);
            if (added) adaptive_centers++;
        }
        
        TCDE_FreePoint(&test_point);
    }
    
    int final_centers = field->manifold_6d.num_centers;
    
    if (final_centers <= initial_centers) {
        printf("    ❌ No adaptive generation occurred: %d <= %d\n", final_centers, initial_centers);
        TCDE_FreePoint(&point1);
        TCDE_FreePoint(&point2);
        TCDE_DestroyField(field);
        return false;
    }
    
    printf("    ✅ Adaptive generation successful: %d → %d centers\n", initial_centers, final_centers);
    
    TCDE_FreePoint(&point1);
    TCDE_FreePoint(&point2);
    TCDE_DestroyField(field);
    return true;
}

// ============================================================================
// MAIN TEST FUNCTION
// ============================================================================

int main() {
    printf("🧬 TCDE Generation Test Suite\n");
    printf("============================\n");
    
    int tests_passed = 0;
    int total_tests = 0;
    
    // Test 1: Generation System Creation
    total_tests++;
    if (test_generation_creation()) {
        printf("✅ Generation creation test passed\n");
        tests_passed++;
    } else {
        printf("❌ Generation creation test failed\n");
    }
    
    // Test 2: Pattern Generation
    total_tests++;
    if (test_pattern_generation()) {
        printf("✅ Pattern generation test passed\n");
        tests_passed++;
    } else {
        printf("❌ Pattern generation test failed\n");
    }
    
    // Test 3: Emergent Structure Generation
    total_tests++;
    if (test_emergent_structure_generation()) {
        printf("✅ Emergent structure generation test passed\n");
        tests_passed++;
    } else {
        printf("❌ Emergent structure generation test failed\n");
    }
    
    // Test 4: Adaptive Generation
    total_tests++;
    if (test_adaptive_generation()) {
        printf("✅ Adaptive generation test passed\n");
        tests_passed++;
    } else {
        printf("❌ Adaptive generation test failed\n");
    }
    
    // Results
    printf("\n📊 Results: %d/%d tests passed (%.1f%%)\n", 
           tests_passed, total_tests, 
           (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("🎉 All generation tests passed!\n");
        return 0;
    } else {
        printf("⚠️  Some generation tests failed\n");
        return 1;
    }
}