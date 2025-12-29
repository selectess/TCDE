/**
 * @file tcde_signal_processor.h
 * @brief TCDE Signal Processor - Real-Time Signal Analysis POC
 * 
 * Proof-of-concept implementation demonstrating TCDE's continuous
 * signal processing capabilities with <1ms latency.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#ifndef TCDE_SIGNAL_PROCESSOR_H
#define TCDE_SIGNAL_PROCESSOR_H

#include "../../../src/core/tcde_core.h"
#include "../../../src/core/tcde_geometry.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief Signal features extracted from TCDE field
 */
typedef struct {
    float curvature;            // Scalar curvature R (complexity measure)
    float energy;               // Field energy E = ∫|Φ|² dV
    int num_components;         // H_0: Connected components (modes)
    int num_loops;              // H_1: Loops (periodic patterns)
    float timestamp;            // Time of extraction (seconds)
    float mean;                 // Signal mean
    float variance;             // Signal variance
} SignalFeatures;

/**
 * @brief TCDE Signal Processor state
 */
typedef struct {
    // Configuration
    float sample_rate;          // Hz (e.g., 44100)
    int dimension;              // 6 for full manifold
    int max_centers;            // Capacity (e.g., 100)
    
    // TCDE field
    TCDE_Field* field;          // Continuous field
    
    // Signal buffer (for derivatives)
    float* buffer;              // Circular buffer
    int buffer_size;            // Samples (e.g., 1024)
    int buffer_pos;             // Current position
    
    // State
    uint64_t sample_count;      // Total samples processed
    float time;                 // Current time (seconds)
    
    // Cached features
    SignalFeatures last_features;
    
} TCDESignalProcessor;

// ============================================================================
// API FUNCTIONS
// ============================================================================

/**
 * @brief Create signal processor
 * @param sample_rate Sampling rate in Hz (e.g., 44100)
 * @return Processor instance or NULL on error
 */
TCDESignalProcessor* TCDE_CreateSignalProcessor(float sample_rate);

/**
 * @brief Destroy signal processor and free memory
 * @param proc Processor to destroy
 */
void TCDE_DestroySignalProcessor(TCDESignalProcessor* proc);

/**
 * @brief Process single audio sample
 * @param proc Processor instance
 * @param sample Audio sample value
 * @return Extracted features
 */
SignalFeatures TCDE_ProcessSample(TCDESignalProcessor* proc, float sample);

/**
 * @brief Get current features without processing new sample
 * @param proc Processor instance
 * @return Current features
 */
SignalFeatures TCDE_GetFeatures(TCDESignalProcessor* proc);

/**
 * @brief Reset processor state
 * @param proc Processor instance
 */
void TCDE_ResetProcessor(TCDESignalProcessor* proc);

#ifdef __cplusplus
}
#endif

#endif // TCDE_SIGNAL_PROCESSOR_H
