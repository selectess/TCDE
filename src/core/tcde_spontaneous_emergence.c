/**
 * @file tcde_spontaneous_emergence.c
 * @brief TCDE Spontaneous Emergence Detection and Analysis Implementation
 * 
 * @version 1.0
 * @date December 14, 2025
 */

#include "tcde_spontaneous_emergence.h"
#include "tcde_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// ============================================================================
// SPONTANEOUS EMERGENCE IMPLEMENTATION
// ============================================================================

TCDE_SpontaneousEmergence* TCDE_CreateSpontaneousEmergence(float threshold) {
    if (threshold < 0.1f || threshold > 1.0f) {
        return NULL;
    }
    
    TCDE_SpontaneousEmergence* emergence = malloc(sizeof(TCDE_SpontaneousEmergence));
    if (!emergence) return NULL;
    
    // Initialize parameters
    emergence->emergence_threshold = threshold;
    emergence->spontaneity_factor = 0.5f;
    emergence->emergence_count = 0;
    emergence->is_active = true;
    
    // Initialize metrics
    emergence->pattern_novelty = 0.0f;
    emergence->complexity_increase = 0.0f;
    emergence->coherence_shift = 0.0f;
    
    // Initialize temporal tracking
    emergence->last_detection_time = 0.0f;
    emergence->detection_frequency = 0.0f;
    
    // Initialize history arrays
    emergence->history_size = 100;
    emergence->history_count = 0;
    emergence->emergence_times = calloc(emergence->history_size, sizeof(float));
    emergence->emergence_intensities = calloc(emergence->history_size, sizeof(float));
    
    if (!emergence->emergence_times || !emergence->emergence_intensities) {
        TCDE_DestroySpontaneousEmergence(emergence);
        return NULL;
    }
    
    return emergence;
}

void TCDE_DestroySpontaneousEmergence(TCDE_SpontaneousEmergence* emergence) {
    if (!emergence) return;
    
    if (emergence->emergence_times) {
        free(emergence->emergence_times);
    }
    if (emergence->emergence_intensities) {
        free(emergence->emergence_intensities);
    }
    
    free(emergence);
}

bool TCDE_DetectSpontaneousEmergence(TCDE_SpontaneousEmergence* emergence, 
                                     const TCDE_Field* field) {
    if (!emergence || !field || !emergence->is_active) {
        return false;
    }
    
    // Measure current field properties
    float current_energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    int current_centers = field->manifold_6d.num_centers;
    
    if (current_centers == 0) {
        return false;
    }
    
    // Calculate complexity metrics
    float complexity = (float)current_centers * logf(current_energy + 1.0f);
    
    // Calculate coherence (phase alignment between centers)
    float coherence = 0.0f;
    int coherence_pairs = 0;
    
    for (int i = 0; i < current_centers && i < 20; i++) {
        for (int j = i + 1; j < current_centers && j < 20; j++) {
            TCDE_Complex c1 = field->manifold_6d.centers[i].coeff;
            TCDE_Complex c2 = field->manifold_6d.centers[j].coeff;
            
            float phase1 = cargf(c1);
            float phase2 = cargf(c2);
            float phase_diff = fabsf(phase1 - phase2);
            
            if (phase_diff > M_PI) phase_diff = 2.0f * M_PI - phase_diff;
            
            coherence += 1.0f - (phase_diff / M_PI);
            coherence_pairs++;
        }
    }
    
    if (coherence_pairs > 0) {
        coherence /= coherence_pairs;
    }
    
    // Detect emergence based on multiple criteria
    bool emergence_detected = false;
    float emergence_intensity = 0.0f;
    
    // Criterion 1: Sudden complexity increase
    if (complexity > emergence->emergence_threshold * 10.0f) {
        emergence->complexity_increase = complexity / 10.0f;
        emergence_intensity += 0.4f * emergence->complexity_increase;
        emergence_detected = true;
    }
    
    // Criterion 2: Coherence phase transition
    float coherence_change = fabsf(coherence - 0.5f);  // Deviation from neutral
    if (coherence_change > emergence->emergence_threshold) {
        emergence->coherence_shift = coherence_change;
        emergence_intensity += 0.3f * coherence_change;
        emergence_detected = true;
    }
    
    // Criterion 3: Pattern novelty (energy distribution)
    float energy_variance = 0.0f;
    float mean_energy = current_energy / current_centers;
    
    for (int i = 0; i < current_centers; i++) {
        float center_energy = cabsf(field->manifold_6d.centers[i].coeff);
        float diff = center_energy - mean_energy;
        energy_variance += diff * diff;
    }
    energy_variance /= current_centers;
    
    float novelty = sqrtf(energy_variance) / (mean_energy + 1e-6f);
    if (novelty > emergence->emergence_threshold) {
        emergence->pattern_novelty = novelty;
        emergence_intensity += 0.3f * novelty;
        emergence_detected = true;
    }
    
    // Record emergence if detected
    if (emergence_detected) {
        emergence->emergence_count++;
        
        // Update temporal tracking
        float current_time = field->time;
        emergence->last_detection_time = current_time;
        
        // Calculate detection frequency
        if (emergence->history_count > 0) {
            float time_span = current_time - emergence->emergence_times[0];
            emergence->detection_frequency = (time_span > 0.0f) ? 
                (float)emergence->history_count / time_span : 0.0f;
        }
        
        // Add to history
        if (emergence->history_count < emergence->history_size) {
            emergence->emergence_times[emergence->history_count] = current_time;
            emergence->emergence_intensities[emergence->history_count] = emergence_intensity;
            emergence->history_count++;
        } else {
            // Shift history arrays
            for (int i = 0; i < emergence->history_size - 1; i++) {
                emergence->emergence_times[i] = emergence->emergence_times[i + 1];
                emergence->emergence_intensities[i] = emergence->emergence_intensities[i + 1];
            }
            emergence->emergence_times[emergence->history_size - 1] = current_time;
            emergence->emergence_intensities[emergence->history_size - 1] = emergence_intensity;
        }
        
        return true;
    }
    
    return false;
}

float TCDE_MeasureEmergenceIntensity(const TCDE_SpontaneousEmergence* emergence) {
    if (!emergence || emergence->history_count == 0) {
        return 0.0f;
    }
    
    // Return the most recent emergence intensity
    return emergence->emergence_intensities[emergence->history_count - 1];
}

void TCDE_UpdateEmergenceParameters(TCDE_SpontaneousEmergence* emergence,
                                    float threshold, float spontaneity_factor) {
    if (!emergence) return;
    
    if (threshold >= 0.1f && threshold <= 1.0f) {
        emergence->emergence_threshold = threshold;
    }
    
    if (spontaneity_factor >= 0.0f && spontaneity_factor <= 1.0f) {
        emergence->spontaneity_factor = spontaneity_factor;
    }
}

void TCDE_GetEmergenceStatistics(const TCDE_SpontaneousEmergence* emergence,
                                 int* total_count, float* avg_intensity, 
                                 float* detection_rate) {
    if (!emergence) {
        if (total_count) *total_count = 0;
        if (avg_intensity) *avg_intensity = 0.0f;
        if (detection_rate) *detection_rate = 0.0f;
        return;
    }
    
    if (total_count) {
        *total_count = emergence->emergence_count;
    }
    
    if (avg_intensity) {
        if (emergence->history_count > 0) {
            float sum = 0.0f;
            for (int i = 0; i < emergence->history_count; i++) {
                sum += emergence->emergence_intensities[i];
            }
            *avg_intensity = sum / emergence->history_count;
        } else {
            *avg_intensity = 0.0f;
        }
    }
    
    if (detection_rate) {
        *detection_rate = emergence->detection_frequency;
    }
}

void TCDE_ResetEmergenceHistory(TCDE_SpontaneousEmergence* emergence) {
    if (!emergence) return;
    
    emergence->emergence_count = 0;
    emergence->history_count = 0;
    emergence->last_detection_time = 0.0f;
    emergence->detection_frequency = 0.0f;
    
    emergence->pattern_novelty = 0.0f;
    emergence->complexity_increase = 0.0f;
    emergence->coherence_shift = 0.0f;
    
    if (emergence->emergence_times) {
        memset(emergence->emergence_times, 0, 
               emergence->history_size * sizeof(float));
    }
    if (emergence->emergence_intensities) {
        memset(emergence->emergence_intensities, 0, 
               emergence->history_size * sizeof(float));
    }
}