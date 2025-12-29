/**
 * @file tcde_spontaneous_emergence.h
 * @brief TCDE Spontaneous Emergence Detection and Analysis
 * 
 * Detects and analyzes spontaneous emergence patterns in TCDE fields.
 * Emergence occurs when new properties or behaviors arise that are not
 * present in individual components but emerge from their interactions.
 * 
 * @version 1.0
 * @date December 14, 2025
 */

#ifndef TCDE_SPONTANEOUS_EMERGENCE_H
#define TCDE_SPONTANEOUS_EMERGENCE_H

#include "tcde_core.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// SPONTANEOUS EMERGENCE STRUCTURES
// ============================================================================

/**
 * @brief Spontaneous emergence detection system
 * 
 * Monitors TCDE fields for spontaneous emergence of new patterns,
 * behaviors, or properties that arise from field interactions.
 */
typedef struct {
    float emergence_threshold;      // Threshold for detecting emergence
    float spontaneity_factor;       // Measure of spontaneity [0, 1]
    int emergence_count;            // Number of emergences detected
    bool is_active;                 // Whether detection is active
    
    // Detection metrics
    float pattern_novelty;          // How novel detected patterns are
    float complexity_increase;      // Increase in system complexity
    float coherence_shift;          // Change in field coherence
    
    // Temporal tracking
    float last_detection_time;      // Time of last emergence detection
    float detection_frequency;      // Emergences per time unit
    
    // Emergence history
    float* emergence_times;         // Times when emergence occurred
    float* emergence_intensities;   // Intensity of each emergence
    int history_size;               // Size of history arrays
    int history_count;              // Number of recorded emergences
} TCDE_SpontaneousEmergence;

// ============================================================================
// SPONTANEOUS EMERGENCE API
// ============================================================================

/**
 * @brief Create spontaneous emergence detector
 * 
 * @param threshold Emergence detection threshold [0.1, 1.0]
 * @return Pointer to emergence detector, NULL on error
 */
TCDE_SpontaneousEmergence* TCDE_CreateSpontaneousEmergence(float threshold);

/**
 * @brief Destroy spontaneous emergence detector
 * 
 * @param emergence Emergence detector to destroy
 */
void TCDE_DestroySpontaneousEmergence(TCDE_SpontaneousEmergence* emergence);

/**
 * @brief Detect spontaneous emergence in field
 * 
 * Analyzes the field for signs of spontaneous emergence:
 * - Sudden increase in complexity
 * - New pattern formation
 * - Coherence phase transitions
 * - Unexpected field behaviors
 * 
 * @param emergence Emergence detector
 * @param field TCDE field to analyze
 * @return true if emergence detected
 */
bool TCDE_DetectSpontaneousEmergence(TCDE_SpontaneousEmergence* emergence, 
                                     const TCDE_Field* field);

/**
 * @brief Measure emergence intensity
 * 
 * Quantifies the intensity of the most recent emergence event.
 * 
 * @param emergence Emergence detector
 * @return Emergence intensity [0, 1]
 */
float TCDE_MeasureEmergenceIntensity(const TCDE_SpontaneousEmergence* emergence);

/**
 * @brief Update emergence detection parameters
 * 
 * @param emergence Emergence detector
 * @param threshold New detection threshold
 * @param spontaneity_factor New spontaneity factor
 */
void TCDE_UpdateEmergenceParameters(TCDE_SpontaneousEmergence* emergence,
                                    float threshold, float spontaneity_factor);

/**
 * @brief Get emergence detection statistics
 * 
 * @param emergence Emergence detector
 * @param total_count Output: total emergences detected
 * @param avg_intensity Output: average emergence intensity
 * @param detection_rate Output: emergences per time unit
 */
void TCDE_GetEmergenceStatistics(const TCDE_SpontaneousEmergence* emergence,
                                 int* total_count, float* avg_intensity, 
                                 float* detection_rate);

/**
 * @brief Reset emergence detection history
 * 
 * @param emergence Emergence detector
 */
void TCDE_ResetEmergenceHistory(TCDE_SpontaneousEmergence* emergence);

#ifdef __cplusplus
}
#endif

#endif // TCDE_SPONTANEOUS_EMERGENCE_H