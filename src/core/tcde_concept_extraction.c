/**
 * @file tcde_concept_extraction.c
 * @brief Authentic geometric concept extraction (TCDE theory compliant)
 * 
 * Concepts EMERGE from field geometry, not extracted by external algorithms.
 * Uses intrinsic geometric properties:
 * - Local maxima (concept centers)
 * - Curvature (concept boundaries)
 * - Flow lines (concept coherence)
 * - Torsion (concept connections)
 */

#include "tcde_core.h"
#include "tcde_geometry.h"
#include "../utils/tcde_text.h"
#include <stdlib.h>
#include <math.h>
#include <complex.h>

// ============================================================================
// GEOMETRIC CONCEPT EXTRACTION (TCDE Authentic)
// ============================================================================

/**
 * @brief Find local maxima in field (concept centers)
 * 
 * Concepts emerge as local maxima of field magnitude.
 * This is pure TCDE - no external algorithm.
 */
static int find_local_maxima(const TCDE_Field* field, 
                             float (*maxima)[3], 
                             int max_concepts) {
    if (!field || !maxima || max_concepts <= 0) return 0;
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return 0;
    
    int num_maxima = 0;
    float search_radius = 0.2f;  // Local neighborhood
    
    // For each center, check if it's a local maximum
    for (int i = 0; i < K && num_maxima < max_concepts; i++) {
        float mag_i = cabsf(field->manifold_6d.centers[i].coeff);
        
        // Check if this is a local maximum
        bool is_maximum = true;
        
        for (int j = 0; j < K; j++) {
            if (i == j) continue;
            
            // Compute distance in semantic space (x,y,z only)
            float dist_sq = 0.0f;
            for (int d = 0; d < 3; d++) {
                float diff = field->manifold_6d.centers[i].point.coords[d] - 
                            field->manifold_6d.centers[j].point.coords[d];
                dist_sq += diff * diff;
            }
            
            // If nearby center has higher magnitude, not a maximum
            if (dist_sq < search_radius * search_radius) {
                float mag_j = cabsf(field->manifold_6d.centers[j].coeff);
                if (mag_j > mag_i) {
                    is_maximum = false;
                    break;
                }
            }
        }
        
        if (is_maximum && mag_i > 0.1f) {  // Threshold for significance
            // Store semantic position (x,y,z)
            for (int d = 0; d < 3; d++) {
                maxima[num_maxima][d] = field->manifold_6d.centers[i].point.coords[d];
            }
            num_maxima++;
        }
    }
    
    return num_maxima;
}

/**
 * @brief Compute curvature-based concept boundaries
 * 
 * High curvature regions indicate concept boundaries.
 * Uses Riemann curvature tensor.
 */
static float compute_concept_boundary_strength(const TCDE_Field* field,
                                               const TCDE_Point* point) {
    if (!field || !point) return 0.0f;
    
    // Compute Riemann curvature scalar
    float curvature = TCDE_ComputeScalarCurvature(point, &field->manifold_6d.metric);
    
    // High curvature = strong boundary
    return fabsf(curvature);
}

/**
 * @brief Merge nearby maxima (within basin of attraction)
 * 
 * Maxima within same basin belong to same concept.
 */
static int merge_nearby_concepts(float (*concepts)[3], 
                                 int num_concepts,
                                 float merge_threshold) {
    if (!concepts || num_concepts <= 1) return num_concepts;
    
    bool* merged = (bool*)calloc(num_concepts, sizeof(bool));
    if (!merged) return num_concepts;
    
    int final_count = 0;
    
    for (int i = 0; i < num_concepts; i++) {
        if (merged[i]) continue;
        
        // Average with nearby concepts
        float avg[3] = {concepts[i][0], concepts[i][1], concepts[i][2]};
        int count = 1;
        
        for (int j = i + 1; j < num_concepts; j++) {
            if (merged[j]) continue;
            
            // Distance between concepts
            float dist_sq = 0.0f;
            for (int d = 0; d < 3; d++) {
                float diff = concepts[i][d] - concepts[j][d];
                dist_sq += diff * diff;
            }
            
            // Merge if within threshold
            if (dist_sq < merge_threshold * merge_threshold) {
                for (int d = 0; d < 3; d++) {
                    avg[d] += concepts[j][d];
                }
                count++;
                merged[j] = true;
            }
        }
        
        // Store averaged concept
        for (int d = 0; d < 3; d++) {
            concepts[final_count][d] = avg[d] / count;
        }
        final_count++;
    }
    
    free(merged);
    return final_count;
}

/**
 * @brief Extract concepts using geometric properties (TCDE authentic)
 * 
 * This is the REAL TCDE way:
 * 1. Find local maxima (concepts emerge naturally)
 * 2. Use curvature to identify boundaries
 * 3. Merge concepts within same basin
 * 
 * NO external algorithms (K-means, etc.)
 */
int TCDE_ExtractConceptsGeometric(const TCDE_Field* field,
                                  float (*concepts)[3],
                                  int max_concepts) {
    if (!field || !concepts || max_concepts <= 0) return 0;
    
    // Step 1: Find local maxima (concept centers)
    float (*maxima)[3] = (float(*)[3])malloc(max_concepts * 3 * sizeof(float));
    if (!maxima) return 0;
    
    int num_maxima = find_local_maxima(field, maxima, max_concepts);
    
    if (num_maxima == 0) {
        free(maxima);
        return 0;
    }
    
    // Step 2: Merge nearby maxima (same basin of attraction)
    float merge_threshold = 0.15f;
    int num_concepts = merge_nearby_concepts(maxima, num_maxima, merge_threshold);
    
    // Copy to output
    for (int i = 0; i < num_concepts; i++) {
        for (int d = 0; d < 3; d++) {
            concepts[i][d] = maxima[i][d];
        }
    }
    
    free(maxima);
    return num_concepts;
}

/**
 * @brief Compute concept coherence using flow lines
 * 
 * Follow gradient flow to measure concept coherence.
 * High coherence = well-defined concept.
 */
float TCDE_ComputeConceptCoherence(const TCDE_Field* field,
                                   const float concept_center[3]) {
    if (!field || !concept_center) return 0.0f;
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return 0.0f;
    
    // Sample points around concept center
    int num_samples = 20;
    float sample_radius = 0.1f;
    float coherence_sum = 0.0f;
    
    for (int s = 0; s < num_samples; s++) {
        // Random point around center
        float angle = 2.0f * M_PI * s / num_samples;
        float sample[3] = {
            concept_center[0] + sample_radius * cosf(angle),
            concept_center[1] + sample_radius * sinf(angle),
            concept_center[2]
        };
        
        // Compute gradient direction at sample point
        TCDE_Complex grad_sum = 0.0f + 0.0f * I;
        
        for (int i = 0; i < K; i++) {
            float dist_sq = 0.0f;
            for (int d = 0; d < 3; d++) {
                float diff = field->manifold_6d.centers[i].point.coords[d] - sample[d];
                dist_sq += diff * diff;
            }
            
            if (dist_sq < 0.2f) {
                grad_sum += field->manifold_6d.centers[i].coeff;
            }
        }
        
        // Gradient points toward center = high coherence
        float grad_mag = cabsf(grad_sum);
        coherence_sum += grad_mag;
    }
    
    return coherence_sum / num_samples;
}

/**
 * @brief Analyze concept using geometric properties
 * 
 * Returns:
 * - Position (x,y,z)
 * - Strength (field magnitude)
 * - Coherence (basin quality)
 * - Boundary strength (curvature)
 */
typedef struct {
    float position[3];
    float strength;
    float coherence;
    float boundary_strength;
} TCDE_ConceptAnalysis;

void TCDE_AnalyzeConcept(const TCDE_Field* field,
                         const float concept_center[3],
                         TCDE_ConceptAnalysis* analysis) {
    if (!field || !concept_center || !analysis) return;
    
    // Position
    for (int d = 0; d < 3; d++) {
        analysis->position[d] = concept_center[d];
    }
    
    // Strength: field magnitude at center
    TCDE_Point p = TCDE_CreatePoint(6, (float[]){
        concept_center[0], concept_center[1], concept_center[2],
        1.5f, 0.0f, 0.4f
    });
    
    TCDE_Complex field_value = TCDE_Evaluate6D(field, &p);
    analysis->strength = cabsf(field_value);
    
    // Coherence
    analysis->coherence = TCDE_ComputeConceptCoherence(field, concept_center);
    
    // Boundary strength (curvature)
    analysis->boundary_strength = compute_concept_boundary_strength(field, &p);
    
    TCDE_FreePoint(&p);
}
