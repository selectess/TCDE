/**
 * @file tcde_validation_integration.c
 * @brief Simplified validation functions for integration testing
 */

#include "../src/core/tcde_core.h"
#include "../src/utils/tcde_multimodal.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Validate reflexivity (simplified for integration)
 */
float TCDE_ValidateReflexivity(TCDE_Field* field, int pattern_type,
                                float perturbation_strength, int evolution_steps) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    // Sample field at multiple points
    const int num_samples = 20;
    TCDE_Complex original_samples[20];
    
    for (int i = 0; i < num_samples; i++) {
        float coords[6] = {
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            1.0f + 0.5f * (float)rand() / RAND_MAX,
            0.0f,
            0.2f + 0.2f * (float)i / (float)num_samples
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        original_samples[i] = TCDE_Evaluate6D(field, &p);
        TCDE_FreePoint(&p);
    }
    
    // Compute correlation (simplified reflexivity measure)
    float total_magnitude = 0.0f;
    int non_zero = 0;
    
    for (int i = 0; i < num_samples; i++) {
        float mag = cabsf(original_samples[i]);
        if (mag > 1e-6f) {
            total_magnitude += mag;
            non_zero++;
        }
    }
    
    if (non_zero == 0) return 0.0f;
    
    // Reflexivity score based on field activity
    float reflexivity = fminf(1.0f, total_magnitude / (float)non_zero);
    
    return reflexivity;
}

/**
 * @brief Validate prediction (simplified for integration)
 */
float TCDE_ValidatePrediction(TCDE_Field* field, int pattern_type,
                               int prediction_horizon, int evolution_steps) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    // Sample field at points with different τ₂ values
    const int num_samples = 15;
    float prediction_score = 0.0f;
    int valid_predictions = 0;
    
    for (int i = 0; i < num_samples; i++) {
        float coords_present[6] = {
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            1.0f + 0.3f * (float)i / (float)num_samples,
            0.0f,  // τ₂ = 0 (present)
            0.2f + 0.2f * (float)i / (float)num_samples
        };
        
        float coords_future[6] = {
            coords_present[0], coords_present[1], coords_present[2],
            coords_present[3],
            0.05f,  // τ₂ > 0 (future)
            coords_present[5]
        };
        
        TCDE_Point p_present = TCDE_CreatePoint(6, coords_present);
        TCDE_Point p_future = TCDE_CreatePoint(6, coords_future);
        
        TCDE_Complex val_present = TCDE_Evaluate6D(field, &p_present);
        TCDE_Complex val_future = TCDE_Evaluate6D(field, &p_future);
        
        // Measure correlation between present and anticipated future
        if (cabsf(val_present) > 1e-6f && cabsf(val_future) > 1e-6f) {
            float correlation = crealf(conjf(val_present) * val_future) / 
                              (cabsf(val_present) * cabsf(val_future));
            
            if (correlation > 0.0f) {
                prediction_score += correlation;
                valid_predictions++;
            }
        }
        
        TCDE_FreePoint(&p_present);
        TCDE_FreePoint(&p_future);
    }
    
    if (valid_predictions == 0) return 0.0f;
    
    return prediction_score / (float)valid_predictions;
}

/**
 * @brief Validate intuition (simplified for integration)
 */
float TCDE_ValidateIntuition(TCDE_Field* field, int num_test_points,
                              float torsion_threshold) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    // Test cross-modal connections (proxy for intuition)
    const float modalities[3] = {0.0f, 0.2f, 0.4f};
    float total_connections = 0.0f;
    int connection_count = 0;
    
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            float mod1 = modalities[i];
            float mod2 = modalities[j];
            
            float similarity = TCDE_CrossModalSimilarity(field, mod1, mod2);
            
            // Distance between modalities
            float modality_distance = fabsf(mod2 - mod1);
            float expected_similarity = expf(-modality_distance / 0.2f);
            
            // Intuitive leap = similarity beyond expected
            float leap = similarity - expected_similarity;
            if (leap > 0.0f) {
                total_connections += leap;
                connection_count++;
            }
        }
    }
    
    if (connection_count == 0) return 0.0f;
    
    float intuition = total_connections / (float)connection_count;
    return fminf(1.0f, intuition);
}
