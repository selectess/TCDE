/**
 * @file tcde_limiters.h
 * @brief Adaptive Limiters for Numerical Stability
 * 
 * Provides adaptive limiters to prevent numerical explosions while
 * preserving system dynamics. These are essential for long-running
 * continuous evolution (End-to-Infinite paradigm).
 * 
 * @version 1.0
 * @date October 2025
 */

#ifndef TCDE_LIMITERS_H
#define TCDE_LIMITERS_H

#include "../core/tcde_core.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// LIMITER CONFIGURATION
// ============================================================================

/**
 * @brief Limiter configuration
 */
typedef struct {
    // Energy limits
    float max_energy;              // Maximum total energy
    float max_center_amplitude;    // Maximum amplitude per center
    
    // Gradient limits
    float max_gradient_magnitude;  // Maximum gradient magnitude
    float max_laplacian_magnitude; // Maximum Laplacian magnitude
    
    // Geometric limits
    float min_epsilon;             // Minimum shape parameter
    float max_epsilon;             // Maximum shape parameter
    float min_metric_det;          // Minimum metric determinant
    float max_metric_det;          // Maximum metric determinant
    
    // Curvature limits
    float max_riemann_component;   // Maximum Riemann tensor component
    float max_scalar_curvature;    // Maximum scalar curvature
    
    // Adaptive behavior
    bool auto_adjust;              // Automatically adjust limits based on field
    float safety_factor;           // Safety margin (0.8 = 80% of theoretical max)
    
    // Statistics
    int num_energy_clips;
    int num_gradient_clips;
    int num_epsilon_clips;
    int num_metric_clips;
} TCDE_LimiterConfig;

// ============================================================================
// LIMITER FUNCTIONS
// ============================================================================

/**
 * @brief Create default limiter configuration
 * 
 * Creates a limiter configuration with safe default values suitable
 * for most applications.
 * 
 * Default values:
 * - max_energy: 10000.0
 * - max_center_amplitude: 100.0
 * - max_gradient_magnitude: 1000.0
 * - max_laplacian_magnitude: 10000.0
 * - min_epsilon: 0.01
 * - max_epsilon: 10.0
 * - min_metric_det: 1e-6
 * - max_metric_det: 1e6
 * - max_riemann_component: 1000.0
 * - max_scalar_curvature: 10000.0
 * - safety_factor: 0.8
 * 
 * @return Default limiter configuration
 */
TCDE_LimiterConfig TCDE_CreateDefaultLimiters(void);

/**
 * @brief Apply energy limiter to field
 * 
 * Limits the total field energy by scaling all center coefficients
 * proportionally if energy exceeds max_energy.
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return true if limiting was applied, false otherwise
 */
bool TCDE_ApplyEnergyLimiter(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Apply amplitude limiter to individual centers
 * 
 * Clips individual center amplitudes to max_center_amplitude while
 * preserving phase.
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return Number of centers that were clipped
 */
int TCDE_ApplyAmplitudeLimiter(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Apply epsilon limiter to shape parameters
 * 
 * Ensures all epsilon values are within [min_epsilon, max_epsilon].
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return Number of epsilons that were clipped
 */
int TCDE_ApplyEpsilonLimiter(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Apply metric limiter
 * 
 * Ensures metric determinants are within safe bounds to prevent
 * numerical instability in geometric computations.
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return Number of metrics that were adjusted
 */
int TCDE_ApplyMetricLimiter(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Apply gradient limiter
 * 
 * Limits gradient magnitudes at field centers to prevent
 * runaway evolution.
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return true if limiting was applied, false otherwise
 */
bool TCDE_ApplyGradientLimiter(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Apply all limiters
 * 
 * Convenience function that applies all limiters in the correct order:
 * 1. Epsilon limiter (affects RBF evaluation)
 * 2. Metric limiter (affects geometry)
 * 3. Amplitude limiter (affects individual centers)
 * 4. Energy limiter (affects global scale)
 * 5. Gradient limiter (affects evolution)
 * 
 * @param field Field to limit
 * @param config Limiter configuration
 * @return true if any limiting was applied, false otherwise
 */
bool TCDE_ApplyAllLimiters(TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Check if field is within limits
 * 
 * Checks if field satisfies all limiter constraints without
 * modifying the field.
 * 
 * @param field Field to check
 * @param config Limiter configuration
 * @return true if field is within all limits, false otherwise
 */
bool TCDE_CheckLimits(const TCDE_Field* field, const TCDE_LimiterConfig* config);

/**
 * @brief Auto-adjust limiter configuration based on field
 * 
 * Analyzes field statistics and adjusts limiter thresholds to be
 * appropriate for the current field scale.
 * 
 * @param field Field to analyze
 * @param config Limiter configuration to adjust
 */
void TCDE_AutoAdjustLimiters(const TCDE_Field* field, TCDE_LimiterConfig* config);

/**
 * @brief Print limiter statistics
 * 
 * Prints statistics about how many times each limiter has been triggered.
 * 
 * @param config Limiter configuration with statistics
 */
void TCDE_PrintLimiterStats(const TCDE_LimiterConfig* config);

/**
 * @brief Reset limiter statistics
 * 
 * Resets all limiter counters to zero.
 * 
 * @param config Limiter configuration
 */
void TCDE_ResetLimiterStats(TCDE_LimiterConfig* config);

#ifdef __cplusplus
}
#endif

#endif // TCDE_LIMITERS_H
