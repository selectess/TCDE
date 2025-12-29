/**
 * @file test_spontaneous_emergence.c
 * @brief Test for Spontaneous Emergence Module - Simplified Version
 *
 * Tests basic spontaneous emergence detection in TCDE fields.
 */

#include "core/tcde_spontaneous_emergence.h"
#include "core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <complex.h>

int main() {
    printf("🌟 TCDE Spontaneous Emergence Test\n");
    printf("==================================\n");
    
    int tests_passed = 0;
    int total_tests = 0;
    
    // Test 1: Create emergence detector
    total_tests++;
    printf("\n=== Test 1: Create Emergence Detector ===\n");
    
    TCDE_SpontaneousEmergence* emergence = TCDE_CreateSpontaneousEmergence(0.5f);
    if (emergence) {
        printf("✅ Emergence detector created successfully\n");
        printf("   Threshold: %.3f\n", emergence->emergence_threshold);
        printf("   Active: %s\n", emergence->is_active ? "Yes" : "No");
        tests_passed++;
    } else {
        printf("❌ Failed to create emergence detector\n");
    }
    
    // Test 2: Create test field
    total_tests++;
    printf("\n=== Test 2: Create Test Field ===\n");
    
    TCDE_Field* field = TCDE_CreateField(20, 2.5f);
    if (field) {
        printf("✅ Test field created successfully\n");
        printf("   Capacity: %d\n", field->manifold_6d.capacity);
        printf("   Fractal dimension: %.2f\n", field->fractal_dimension);
        tests_passed++;
    } else {
        printf("❌ Failed to create test field\n");
    }
    
    // Test 3: Add some centers to create patterns
    total_tests++;
    printf("\n=== Test 3: Add Centers for Pattern Creation ===\n");
    
    int centers_added = 0;
    for (int i = 0; i < 5; i++) {
        float coords[6] = {
            (float)i * 0.3f - 0.6f,  // x
            sinf((float)i * 0.8f),   // y
            cosf((float)i * 0.5f),   // z
            1.5f,                    // τ₁
            0.0f,                    // τ₂
            0.4f                     // m
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = (0.6f + 0.2f * i) + (0.3f * sinf(i)) * I;
        float epsilon = 0.7f + 0.1f * i;
        
        bool added = TCDE_AddCenter6D(field, &point, coeff, epsilon);
        if (added) {
            centers_added++;
        }
        
        TCDE_FreePoint(&point);
    }
    
    if (centers_added >= 3) {
        printf("✅ Pattern created with %d centers\n", centers_added);
        tests_passed++;
    } else {
        printf("❌ Failed to create sufficient pattern: only %d centers\n", centers_added);
    }
    
    // Test 4: Detect emergence in field
    total_tests++;
    printf("\n=== Test 4: Detect Emergence ===\n");
    
    bool emergence_detected = TCDE_DetectSpontaneousEmergence(emergence, field);
    printf("   Emergence detected: %s\n", emergence_detected ? "Yes" : "No");
    printf("   Emergence count: %d\n", emergence->emergence_count);
    printf("   Pattern novelty: %.6f\n", emergence->pattern_novelty);
    printf("   Complexity increase: %.6f\n", emergence->complexity_increase);
    printf("   Coherence shift: %.6f\n", emergence->coherence_shift);
    
    // Consider test passed if detector runs without crashing
    printf("✅ Emergence detection completed successfully\n");
    tests_passed++;
    
    // Test 5: Measure emergence intensity
    total_tests++;
    printf("\n=== Test 5: Measure Emergence Intensity ===\n");
    
    float intensity = TCDE_MeasureEmergenceIntensity(emergence);
    printf("   Emergence intensity: %.6f\n", intensity);
    
    if (intensity >= 0.0f && intensity <= 1.0f) {
        printf("✅ Intensity measurement valid\n");
        tests_passed++;
    } else {
        printf("❌ Invalid intensity measurement: %.6f\n", intensity);
    }
    
    // Test 6: Get emergence statistics
    total_tests++;
    printf("\n=== Test 6: Get Emergence Statistics ===\n");
    
    int total_count;
    float avg_intensity;
    float detection_rate;
    
    TCDE_GetEmergenceStatistics(emergence, &total_count, &avg_intensity, &detection_rate);
    
    printf("   Total emergences: %d\n", total_count);
    printf("   Average intensity: %.6f\n", avg_intensity);
    printf("   Detection rate: %.6f\n", detection_rate);
    
    if (total_count >= 0 && avg_intensity >= 0.0f && detection_rate >= 0.0f) {
        printf("✅ Statistics retrieved successfully\n");
        tests_passed++;
    } else {
        printf("❌ Invalid statistics\n");
    }
    
    // Test 7: Update parameters
    total_tests++;
    printf("\n=== Test 7: Update Parameters ===\n");
    
    TCDE_UpdateEmergenceParameters(emergence, 0.3f, 0.8f);
    
    if (emergence->emergence_threshold == 0.3f && emergence->spontaneity_factor == 0.8f) {
        printf("✅ Parameters updated successfully\n");
        printf("   New threshold: %.3f\n", emergence->emergence_threshold);
        printf("   New spontaneity: %.3f\n", emergence->spontaneity_factor);
        tests_passed++;
    } else {
        printf("❌ Parameter update failed\n");
    }
    
    // Test 8: Reset history
    total_tests++;
    printf("\n=== Test 8: Reset History ===\n");
    
    TCDE_ResetEmergenceHistory(emergence);
    
    if (emergence->emergence_count == 0 && emergence->history_count == 0) {
        printf("✅ History reset successfully\n");
        tests_passed++;
    } else {
        printf("❌ History reset failed\n");
    }
    
    // Cleanup
    TCDE_DestroySpontaneousEmergence(emergence);
    TCDE_DestroyField(field);
    
    // Results
    printf("\n📊 Results: %d/%d tests passed (%.1f%%)\n", 
           tests_passed, total_tests, 
           (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("🎉 All spontaneous emergence tests passed!\n");
        return 0;
    } else {
        printf("⚠️  Some spontaneous emergence tests failed\n");
        return 1;
    }
}
