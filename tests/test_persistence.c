/**
 * @file test_persistence.c
 * @brief Test TCDE Persistence Module (End-to-Infinite)
 * 
 * Tests state serialization and deserialization.
 * 
 * @version 1.0
 * @date October 22, 2025
 */

#include "../src/core/tcde.h"
#include "tcde_persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TEST_FILE "test_state.tcde"

/**
 * @brief Test basic save/load cycle
 */
bool test_save_load_basic() {
    printf("\n=== Test: Basic Save/Load ===\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    if (!field) {
        printf("❌ Failed to create field\n");
        return false;
    }
    
    // Set some properties (don't add centers for now - test minimal state)
    field->time = 123.456f;
    field->fractal_dimension = 2.5f;
    field->temporal_dimension = 1.0f;
    
    printf("Original field: %d centers, time=%.3f\n", 
           field->manifold_6d.num_centers, field->time);
    
    // Save state
    printf("Saving state to %s...\n", TEST_FILE);
    if (!TCDE_SaveState(field, TEST_FILE)) {
        printf("❌ Failed to save state\n");
        TCDE_DestroyField(field);
        return false;
    }
    printf("✅ State saved successfully\n");
    
    // Verify file
    if (!TCDE_VerifyStateFile(TEST_FILE)) {
        printf("❌ State file verification failed\n");
        TCDE_DestroyField(field);
        return false;
    }
    printf("✅ State file verified\n");
    
    // Load state
    printf("Loading state from %s...\n", TEST_FILE);
    TCDE_Field* loaded = TCDE_LoadState(TEST_FILE);
    if (!loaded) {
        printf("❌ Failed to load state\n");
        TCDE_DestroyField(field);
        return false;
    }
    printf("✅ State loaded successfully\n");
    
    // Verify loaded data
    printf("Loaded field: %d centers, time=%.3f\n",
           loaded->manifold_6d.num_centers, loaded->time);
    
    bool match = true;
    
    if (loaded->manifold_6d.num_centers != field->manifold_6d.num_centers) {
        printf("❌ Number of centers mismatch: %d vs %d\n",
               loaded->manifold_6d.num_centers, field->manifold_6d.num_centers);
        match = false;
    }
    
    if (fabsf(loaded->time - field->time) > 1e-6f) {
        printf("❌ Time mismatch: %.6f vs %.6f\n", loaded->time, field->time);
        match = false;
    }
    
    if (fabsf(loaded->fractal_dimension - field->fractal_dimension) > 1e-6f) {
        printf("❌ Fractal dimension mismatch: %.6f vs %.6f\n",
               loaded->fractal_dimension, field->fractal_dimension);
        match = false;
    }
    
    // Check first center
    if (match && loaded->manifold_6d.num_centers > 0) {
        TCDE_Center* c1 = &field->manifold_6d.centers[0];
        TCDE_Center* c2 = &loaded->manifold_6d.centers[0];
        
        for (int i = 0; i < 6; i++) {
            if (fabsf(c1->point.coords[i] - c2->point.coords[i]) > 1e-6f) {
                printf("❌ Center 0 coord[%d] mismatch: %.6f vs %.6f\n",
                       i, c1->point.coords[i], c2->point.coords[i]);
                match = false;
                break;
            }
        }
        
        if (cabsf(c1->coeff - c2->coeff) > 1e-6f) {
            printf("❌ Center 0 coefficient mismatch\n");
            match = false;
        }
    }
    
    if (match) {
        printf("✅ All data matches!\n");
    }
    
    // Cleanup
    TCDE_DestroyField(field);
    TCDE_DestroyField(loaded);
    remove(TEST_FILE);
    
    return match;
}

int main() {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Persistence Module Test (End-to-Infinite)         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    bool all_passed = true;
    
    all_passed &= test_save_load_basic();
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    if (all_passed) {
        printf("║  ✅ ALL TESTS PASSED                                     ║\n");
    } else {
        printf("║  ❌ SOME TESTS FAILED                                    ║\n");
    }
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    return all_passed ? 0 : 1;
}
