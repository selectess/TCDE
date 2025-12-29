/**
 * @file tcde_signal_processor.c
 * @brief TCDE Signal Processor - Implementation
 * 
 * Real-time signal processing using TCDE continuous field dynamics.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#include "tcde_signal_processor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// ============================================================================
// INTERNAL HELPERS
// ============================================================================

/**
 * @brief Compute derivative from circular buffer
 */
static float compute_derivative(const float* buffer, int pos, int size, int order) {
    if (order == 1) {
        // First derivative (backward difference)
        int prev = (pos - 1 + size) % size;
        return buffer[pos] - buffer[prev];
    } else if (order == 2) {
        // Second derivative
        int prev = (pos - 1 + size) % size;
        int prev2 = (pos - 2 + size) % size;
        return buffer[pos] - 2.0f * buffer[prev] + buffer[prev2];
    }
    return 0.0f;
}

/**
 * @brief Compute signal statistics
 */
static void compute_statistics(const float* buffer, int size, float* mean, float* variance) {
    // Mean
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    *mean = sum / size;
    
    // Variance
    float var_sum = 0.0f;
    for (int i = 0; i < size; i++) {
        float diff = buffer[i] - (*mean);
        var_sum += diff * diff;
    }
    *variance = var_sum / size;
}

// ============================================================================
// PUBLIC API
// ============================================================================

TCDESignalProcessor* TCDE_CreateSignalProcessor(float sample_rate) {
    if (sample_rate <= 0.0f) {
        fprintf(stderr, "[ERROR] Invalid sample rate: %.2f\n", sample_rate);
        return NULL;
    }
    
    TCDESignalProcessor* proc = (TCDESignalProcessor*)malloc(sizeof(TCDESignalProcessor));
    if (!proc) {
        fprintf(stderr, "[ERROR] Failed to allocate processor\n");
        return NULL;
    }
    
    // Configuration
    proc->sample_rate = sample_rate;
    proc->dimension = 6;
    proc->max_centers = 100;
    
    // Create TCDE field (fractal_dim = 2.5)
    proc->field = TCDE_CreateField(proc->max_centers, 2.5f);
    if (!proc->field) {
        fprintf(stderr, "[ERROR] Failed to create TCDE field\n");
        free(proc);
        return NULL;
    }
    
    // Allocate circular buffer
    proc->buffer_size = 1024;
    proc->buffer = (float*)calloc(proc->buffer_size, sizeof(float));
    if (!proc->buffer) {
        fprintf(stderr, "[ERROR] Failed to allocate buffer\n");
        TCDE_DestroyField(proc->field);
        free(proc);
        return NULL;
    }
    proc->buffer_pos = 0;
    
    // Initialize state
    proc->sample_count = 0;
    proc->time = 0.0f;
    
    // Initialize features
    memset(&proc->last_features, 0, sizeof(SignalFeatures));
    
    return proc;
}

void TCDE_DestroySignalProcessor(TCDESignalProcessor* proc) {
    if (!proc) return;
    
    if (proc->field) {
        TCDE_DestroyField(proc->field);
    }
    if (proc->buffer) {
        free(proc->buffer);
    }
    free(proc);
}

SignalFeatures TCDE_ProcessSample(TCDESignalProcessor* proc, float sample) {
    if (!proc || !proc->field) {
        SignalFeatures empty = {0};
        return empty;
    }
    
    // 1. Update circular buffer
    proc->buffer[proc->buffer_pos] = sample;
    int current_pos = proc->buffer_pos;
    proc->buffer_pos = (proc->buffer_pos + 1) % proc->buffer_size;
    
    // 2. Compute derivatives (for 6D embedding)
    float s_dot = compute_derivative(proc->buffer, current_pos, proc->buffer_size, 1);
    float s_ddot = compute_derivative(proc->buffer, current_pos, proc->buffer_size, 2);
    
    // Scale derivatives by sample rate
    s_dot *= proc->sample_rate;
    s_ddot *= proc->sample_rate * proc->sample_rate;
    
    // 3. Create 6D embedding point
    // [x, y, z, τ₁, τ₂, m]
    float coords[6] = {
        sample,                                    // x: signal value
        s_dot,                                     // y: velocity
        s_ddot,                                    // z: acceleration
        proc->time,                                // τ₁: current time
        proc->time + 1.0f / proc->sample_rate,     // τ₂: next time
        0.0f                                       // m: modality (audio = 0.0)
    };
    
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    
    // 4. Create complex coefficient (amplitude + phase from derivative)
    TCDE_Complex coeff = sample + I * s_dot;
    
    // 5. Add center to field (epsilon = 0.1 for moderate width)
    bool added = TCDE_AddCenter6D(proc->field, &point, coeff, 0.1f);
    
    // 6. Prune old centers if at capacity
    if (!added && proc->field->manifold_6d.num_centers >= proc->max_centers) {
        // Remove oldest center (index 0)
        TCDE_RemoveCenter6D(proc->field, 0);
        // Try adding again
        TCDE_AddCenter6D(proc->field, &point, coeff, 0.1f);
    }
    
    TCDE_FreePoint(&point);
    
    // 7. Extract features
    SignalFeatures features;
    
    // Energy: Use TCDE's built-in energy computation
    features.energy = TCDE_ComputeEnergy(proc->field);
    
    // Curvature: Compute scalar curvature at current point
    float eval_coords[6] = {sample, s_dot, s_ddot, proc->time, 
                            proc->time + 1.0f/proc->sample_rate, 0.0f};
    TCDE_Point eval_point = TCDE_CreatePoint(6, eval_coords);
    features.curvature = TCDE_ComputeScalarCurvature(&eval_point, 
                                                      &proc->field->manifold_6d.metric);
    TCDE_FreePoint(&eval_point);
    
    // Topology: Simplified - count peaks in recent signal
    int num_peaks = 0;
    for (int i = 1; i < proc->buffer_size - 1; i++) {
        int idx = (current_pos - i + proc->buffer_size) % proc->buffer_size;
        int prev = (idx - 1 + proc->buffer_size) % proc->buffer_size;
        int next = (idx + 1) % proc->buffer_size;
        
        if (proc->buffer[idx] > proc->buffer[prev] && 
            proc->buffer[idx] > proc->buffer[next]) {
            num_peaks++;
        }
    }
    features.num_components = num_peaks;
    features.num_loops = 0;  // Simplified for POC
    
    // Statistics
    compute_statistics(proc->buffer, proc->buffer_size, 
                      &features.mean, &features.variance);
    
    // Timestamp
    features.timestamp = proc->time;
    
    // 8. Update state
    proc->sample_count++;
    proc->time += 1.0f / proc->sample_rate;
    proc->last_features = features;
    
    return features;
}

SignalFeatures TCDE_GetFeatures(TCDESignalProcessor* proc) {
    if (!proc) {
        SignalFeatures empty = {0};
        return empty;
    }
    return proc->last_features;
}

void TCDE_ResetProcessor(TCDESignalProcessor* proc) {
    if (!proc) return;
    
    // Clear buffer
    memset(proc->buffer, 0, proc->buffer_size * sizeof(float));
    proc->buffer_pos = 0;
    
    // Reset state
    proc->sample_count = 0;
    proc->time = 0.0f;
    
    // Clear features
    memset(&proc->last_features, 0, sizeof(SignalFeatures));
    
    // Recreate field (clears all centers)
    if (proc->field) {
        TCDE_DestroyField(proc->field);
    }
    proc->field = TCDE_CreateField(proc->max_centers, 2.5f);
}
