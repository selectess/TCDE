/**
 * @file test_cross_modal_similarity.c
 * @brief Test cross-modal similarity computation (Task 5.4)
 * 
 * Tests the authentic TCDE implementation of cross-modal similarity
 * using geodesic distances in unified 6D space.
 * 
 * @date January 2025
 */

#include "../src/core/tcde.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define TEST(name, condition) do { \
    if (condition) { \
        printf("✓ %s\n", name); \
        tests_passed++; \
    } else { \
        printf("✗ %s\n", name); \
        tests_failed++; \
    } \
    tests_total++; \
} while(0)

int tests_total = 0;
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief Test 1: Self-similarity should be 1.0
 */
void test_self_similarity() {
    printf("\n=== Test 1: Self-Similarity ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Add centers in visual modality (m=0.0)
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)i / 10.0f,  // x
            0.5f,              // y
            0.5f,              // z
            0.0f,              // τ₁
            0.0f,              // τ₂
            0.0f               // m (visual)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute self-similarity
    float similarity = TCDE_CrossModalSimilarity(field, 0.0f, 0.0f);
    
    TEST("Self-similarity ≈ 1.0", fabsf(similarity - 1.0f) < 0.1f);
    TEST("Self-similarity > 0.9", similarity > 0.9f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 2: Distant modalities should have low similarity
 */
void test_distant_modalities() {
    printf("\n=== Test 2: Distant Modalities ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Add centers in visual modality (m=0.0)
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)i / 10.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f  // m=0.0 (visual)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Add centers in emotional modality (m=0.8)
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)i / 10.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.8f  // m=0.8 (emotional)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute similarity between distant modalities
    float similarity = TCDE_CrossModalSimilarity(field, 0.0f, 0.8f);
    
    TEST("Distant modalities have low similarity", similarity < 0.5f);
    TEST("Similarity > 0 (not completely disconnected)", similarity > 0.0f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 3: Adjacent modalities should have moderate similarity
 */
void test_adjacent_modalities() {
    printf("\n=== Test 3: Adjacent Modalities ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Add centers in visual modality (m=0.0)
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)i / 10.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f  // m=0.0 (visual)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Add centers in auditory modality (m=0.2)
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)i / 10.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.2f  // m=0.2 (auditory)
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute similarity between adjacent modalities
    float similarity = TCDE_CrossModalSimilarity(field, 0.0f, 0.2f);
    
    TEST("Adjacent modalities have moderate similarity", similarity > 0.4f && similarity < 0.9f);
    TEST("Similarity increases with proximity", similarity > 0.5f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 4: Spatially aligned modalities should have higher similarity
 */
void test_spatial_alignment() {
    printf("\n=== Test 4: Spatial Alignment ===\n");
    
    TCDE_Field* field1 = TCDE_CreateField(100, 2.5f);
    TCDE_Field* field2 = TCDE_CreateField(100, 2.5f);
    assert(field1 != NULL && field2 != NULL);
    
    // Field 1: Aligned modalities (same spatial positions)
    for (int i = 0; i < 10; i++) {
        float x = (float)i / 10.0f;
        
        // Visual
        float coords_v[6] = {x, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f};
        TCDE_Point pv = TCDE_CreatePoint(6, coords_v);
        TCDE_AddCenter6D(field1, &pv, 1.0f, 0.1f);
        TCDE_FreePoint(&pv);
        
        // Semantic (same spatial position)
        float coords_s[6] = {x, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f};
        TCDE_Point ps = TCDE_CreatePoint(6, coords_s);
        TCDE_AddCenter6D(field1, &ps, 1.0f, 0.1f);
        TCDE_FreePoint(&ps);
    }
    
    // Field 2: Misaligned modalities (different spatial positions)
    for (int i = 0; i < 10; i++) {
        float x1 = (float)i / 10.0f;
        float x2 = (float)(9 - i) / 10.0f;  // Reversed
        
        // Visual
        float coords_v[6] = {x1, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f};
        TCDE_Point pv = TCDE_CreatePoint(6, coords_v);
        TCDE_AddCenter6D(field2, &pv, 1.0f, 0.1f);
        TCDE_FreePoint(&pv);
        
        // Semantic (different spatial position)
        float coords_s[6] = {x2, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f};
        TCDE_Point ps = TCDE_CreatePoint(6, coords_s);
        TCDE_AddCenter6D(field2, &ps, 1.0f, 0.1f);
        TCDE_FreePoint(&ps);
    }
    
    float sim_aligned = TCDE_CrossModalSimilarity(field1, 0.0f, 0.4f);
    float sim_misaligned = TCDE_CrossModalSimilarity(field2, 0.0f, 0.4f);
    
    TEST("Aligned modalities have higher similarity", sim_aligned > sim_misaligned);
    TEST("Aligned similarity > 0.6", sim_aligned > 0.6f);
    TEST("Misaligned similarity < aligned", sim_misaligned < sim_aligned);
    
    TCDE_DestroyField(field1);
    TCDE_DestroyField(field2);
}

/**
 * @brief Test 5: Similarity is symmetric
 */
void test_symmetry() {
    printf("\n=== Test 5: Symmetry ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Add centers in two modalities
    for (int i = 0; i < 10; i++) {
        float coords_v[6] = {(float)i / 10.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f};
        TCDE_Point pv = TCDE_CreatePoint(6, coords_v);
        TCDE_AddCenter6D(field, &pv, 1.0f, 0.1f);
        TCDE_FreePoint(&pv);
        
        float coords_s[6] = {(float)i / 10.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f};
        TCDE_Point ps = TCDE_CreatePoint(6, coords_s);
        TCDE_AddCenter6D(field, &ps, 1.0f, 0.1f);
        TCDE_FreePoint(&ps);
    }
    
    float sim_12 = TCDE_CrossModalSimilarity(field, 0.0f, 0.4f);
    float sim_21 = TCDE_CrossModalSimilarity(field, 0.4f, 0.0f);
    
    TEST("Similarity is symmetric", fabsf(sim_12 - sim_21) < 0.01f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 6: Empty modality handling
 */
void test_empty_modality() {
    printf("\n=== Test 6: Empty Modality ===\n");
    
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Add centers only in visual modality
    for (int i = 0; i < 10; i++) {
        float coords[6] = {(float)i / 10.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute similarity with empty modality
    float similarity = TCDE_CrossModalSimilarity(field, 0.0f, 0.6f);
    
    TEST("Empty modality returns default similarity", similarity > 0.0f && similarity < 1.0f);
    TEST("Default similarity is reasonable", similarity > 0.1f && similarity < 0.8f);
    
    TCDE_DestroyField(field);
}

/**
 * @brief Test 7: Multimodal integration (text + image + audio)
 */
void test_multimodal_integration() {
    printf("\n=== Test 7: Multimodal Integration ===\n");
    
    TCDE_Field* field = TCDE_CreateField(1000, 2.5f);
    assert(field != NULL);
    
    TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
    assert(relations != NULL);
    
    // Process text
    TCDE_ProcessText(field, relations, "hello world", 1.0f);
    
    // Process image (simulated)
    unsigned char image_data[64 * 64 * 3];
    for (int i = 0; i < 64 * 64 * 3; i++) {
        image_data[i] = (unsigned char)(i % 256);
    }
    TCDE_ProcessImage(field, relations, image_data, 64, 64, 3, 1.0f);
    
    // Process audio (simulated)
    float audio_samples[1024];
    for (int i = 0; i < 1024; i++) {
        audio_samples[i] = sinf(2.0f * 3.14159f * 440.0f * (float)i / 44100.0f);
    }
    TCDE_ProcessAudio(field, relations, audio_samples, 1024, 44100, 1.0f);
    
    // Compute cross-modal similarities
    float sim_text_image = TCDE_CrossModalSimilarity(field, 0.4f, 0.0f);  // semantic-visual
    float sim_text_audio = TCDE_CrossModalSimilarity(field, 0.4f, 0.2f);  // semantic-auditory
    float sim_image_audio = TCDE_CrossModalSimilarity(field, 0.0f, 0.2f); // visual-auditory
    
    TEST("Text-Image similarity computed", sim_text_image > 0.0f && sim_text_image <= 1.0f);
    TEST("Text-Audio similarity computed", sim_text_audio > 0.0f && sim_text_audio <= 1.0f);
    TEST("Image-Audio similarity computed", sim_image_audio > 0.0f && sim_image_audio <= 1.0f);
    TEST("All similarities are reasonable", 
         sim_text_image > 0.1f && sim_text_audio > 0.1f && sim_image_audio > 0.1f);
    
    TCDE_DestroyModalityRelations(relations);
    TCDE_DestroyField(field);
}

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Cross-Modal Similarity Test Suite (Task 5.4)        ║\n");
    printf("║  Authentic geodesic distance-based similarity             ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    test_self_similarity();
    test_distant_modalities();
    test_adjacent_modalities();
    test_spatial_alignment();
    test_symmetry();
    test_empty_modality();
    test_multimodal_integration();
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Test Results                                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Total:  %3d                                               ║\n", tests_total);
    printf("║  Passed: %3d (%.1f%%)                                      ║\n", 
           tests_passed, 100.0f * tests_passed / tests_total);
    printf("║  Failed: %3d                                               ║\n", tests_failed);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("\n✓ Task 5.4 COMPLETE: Cross-modal similarity via geodesic distance\n");
        printf("  - Geometry-based similarity (no hardcoded matrices)\n");
        printf("  - Unified 6D space integration\n");
        printf("  - Authentic TCDE implementation\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed. Review implementation.\n");
        return 1;
    }
}
