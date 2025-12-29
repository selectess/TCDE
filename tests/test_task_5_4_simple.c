/**
 * @file test_task_5_4_simple.c
 * @brief Simple test for Task 5.4 - Cross-modal similarity
 * 
 * Minimal test without evolution dependencies
 */

#include "../src/core/tcde_core.h"
#include "../src/utils/tcde_multimodal.h"
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

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Task 5.4: Cross-Modal Similarity (Simple Test)           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Test 1: Self-similarity
    printf("Test 1: Self-Similarity\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Add centers in visual modality
        for (int i = 0; i < 10; i++) {
            float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};  // τ₁=1.0
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float sim = TCDE_CrossModalSimilarity(field, 0.0f, 0.0f);
        TEST("Self-similarity > 0.8", sim > 0.8f);
        printf("  Similarity: %.3f\n", sim);
        
        TCDE_DestroyField(field);
    }
    
    // Test 2: Distant modalities
    printf("\nTest 2: Distant Modalities\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Visual (m=0.0)
        for (int i = 0; i < 10; i++) {
            float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};  // τ₁=1.0
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Emotional (m=0.8)
        for (int i = 0; i < 10; i++) {
            float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.8f};  // τ₁=1.0
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float sim = TCDE_CrossModalSimilarity(field, 0.0f, 0.8f);
        TEST("Distant modalities < 0.5", sim < 0.5f);
        printf("  Similarity: %.3f\n", sim);
        
        TCDE_DestroyField(field);
    }
    
    // Test 3: Adjacent modalities
    printf("\nTest 3: Adjacent Modalities\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        // Visual (m=0.0)
        for (int i = 0; i < 10; i++) {
            float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};  // τ₁=1.0
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Auditory (m=0.2)
        for (int i = 0; i < 10; i++) {
            float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.2f};  // τ₁=1.0
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float sim = TCDE_CrossModalSimilarity(field, 0.0f, 0.2f);
        TEST("Adjacent modalities 0.4-0.9", sim > 0.4f && sim < 0.9f);
        printf("  Similarity: %.3f\n", sim);
        
        TCDE_DestroyField(field);
    }
    
    // Test 4: Symmetry
    printf("\nTest 4: Symmetry\n");
    {
        TCDE_Field* field = TCDE_CreateField(100, 2.5f);
        
        for (int i = 0; i < 10; i++) {
            float coords_v[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};  // τ₁=1.0
            TCDE_Point pv = TCDE_CreatePoint(6, coords_v);
            TCDE_AddCenter6D(field, &pv, 1.0f, 0.1f);
            TCDE_FreePoint(&pv);
            
            float coords_s[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};  // τ₁=1.0
            TCDE_Point ps = TCDE_CreatePoint(6, coords_s);
            TCDE_AddCenter6D(field, &ps, 1.0f, 0.1f);
            TCDE_FreePoint(&ps);
        }
        
        float sim_12 = TCDE_CrossModalSimilarity(field, 0.0f, 0.4f);
        float sim_21 = TCDE_CrossModalSimilarity(field, 0.4f, 0.0f);
        
        TEST("Symmetry |S(1,2) - S(2,1)| < 0.01", fabsf(sim_12 - sim_21) < 0.01f);
        printf("  S(0.0, 0.4) = %.3f\n", sim_12);
        printf("  S(0.4, 0.0) = %.3f\n", sim_21);
        
        TCDE_DestroyField(field);
    }
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Results: %d/%d tests passed (%.1f%%)                      ║\n", 
           tests_passed, tests_total, 100.0f * tests_passed / tests_total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 5.4 COMPLETE: Cross-modal similarity via geodesic distance\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed\n");
        return 1;
    }
}
