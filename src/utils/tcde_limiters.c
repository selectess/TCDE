/**
 * @file tcde_limiters.c
 * @brief Adaptive Limiters Implementation
 */

#include "tcde_limiters.h"
#include "../core/tcde_geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

TCDE_LimiterConfig TCDE_CreateDefaultLimiters(void) {
    TCDE_LimiterConfig config;
    
    // Energy limits
    config.max_energy = 10000.0f;
    config.max_center_amplitude = 100.0f;
    
    // Gradient limits
    config.max_gradient_magnitude = 1000.0f;
    config.max_laplacian_magnitude = 10000.0f;
    
    // Geometric limits
    config.min_epsilon = 0.01f;
    config.max_epsilon = 10.0f;
    config.min_metric_det = 1e-6f;
    config.max_metric_det = 1e6f;
    
    // Curvature limits
    config.max_riemann_component = 1000.0f;
    config.max_scalar_curvature = 10000.0f;
    
    // Adaptive behavior
    config.auto_adjust = true;
    config.safety_factor = 0.8f;
    
    // Statistics
    config.num_energy_clips = 0;
    config.num_gradient_clips = 0;
    config.num_epsilon_clips = 0;
    config.num_metric_clips = 0;
    
    return config;
}

bool TCDE_ApplyEnergyLimiter(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return false;
    
    // Compute current energy
    float energy = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float amplitude = cabsf(field->manifold_6d.centers[i].coeff);
        energy += amplitude * amplitude;
    }
    
    // Check if limiting needed
    if (energy <= config->max_energy) {
        return false;
    }
    
    // Scale all coefficients proportionally
    float scale = sqrtf(config->max_energy / energy);
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        field->manifold_6d.centers[i].coeff *= scale;
    }
    
    // Invalidate cached energy
    field->energy_valid = false;
    
    config->num_energy_clips++;
    return true;
}

int TCDE_ApplyAmplitudeLimiter(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return 0;
    
    int num_clipped = 0;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Complex coeff = field->manifold_6d.centers[i].coeff;
        float amplitude = cabsf(coeff);
        
        if (amplitude > config->max_center_amplitude) {
            // Preserve phase, clip amplitude
            float phase = cargf(coeff);
            field->manifold_6d.centers[i].coeff = 
                config->max_center_amplitude * (cosf(phase) + I * sinf(phase));
            num_clipped++;
        }
    }
    
    if (num_clipped > 0) {
        field->energy_valid = false;
    }
    
    return num_clipped;
}

int TCDE_ApplyEpsilonLimiter(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return 0;
    
    int num_clipped = 0;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float* eps = &field->manifold_6d.centers[i].epsilon;
        
        if (*eps < config->min_epsilon) {
            *eps = config->min_epsilon;
            num_clipped++;
        } else if (*eps > config->max_epsilon) {
            *eps = config->max_epsilon;
            num_clipped++;
        }
    }
    
    if (num_clipped > 0) {
        config->num_epsilon_clips++;
    }
    
    return num_clipped;
}

int TCDE_ApplyMetricLimiter(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return 0;
    
    int num_adjusted = 0;
    
    // Check global metric
    TCDE_Metric* metric = &field->manifold_6d.metric;
    
    if (metric->is_valid) {
        if (metric->det < config->min_metric_det) {
            // Metric too singular - add regularization
            for (int i = 0; i < metric->dimension; i++) {
                metric->g[i][i] += config->min_metric_det;
            }
            metric->is_valid = false;  // Force recomputation
            num_adjusted++;
        } else if (metric->det > config->max_metric_det) {
            // Metric too large - scale down
            float scale = sqrtf(config->max_metric_det / metric->det);
            for (int i = 0; i < metric->dimension; i++) {
                for (int j = 0; j < metric->dimension; j++) {
                    metric->g[i][j] *= scale;
                }
            }
            metric->is_valid = false;  // Force recomputation
            num_adjusted++;
        }
    }
    
    // Check individual center metrics
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Metric* center_metric = &field->manifold_6d.centers[i].metric;
        
        if (center_metric->is_valid) {
            if (center_metric->det < config->min_metric_det) {
                for (int d = 0; d < center_metric->dimension; d++) {
                    center_metric->g[d][d] += config->min_metric_det;
                }
                center_metric->is_valid = false;
                num_adjusted++;
            } else if (center_metric->det > config->max_metric_det) {
                float scale = sqrtf(config->max_metric_det / center_metric->det);
                for (int d = 0; d < center_metric->dimension; d++) {
                    for (int j = 0; j < center_metric->dimension; j++) {
                        center_metric->g[d][j] *= scale;
                    }
                }
                center_metric->is_valid = false;
                num_adjusted++;
            }
        }
    }
    
    if (num_adjusted > 0) {
        config->num_metric_clips++;
    }
    
    return num_adjusted;
}

bool TCDE_ApplyGradientLimiter(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return false;
    if (field->manifold_6d.num_centers == 0) return false;
    
    bool limited = false;
    
    // Sample gradient at center locations
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Point* point = &field->manifold_6d.centers[i].point;
        
        // Compute gradient
        TCDE_Complex gradient[6];
        TCDE_Gradient(field, point, gradient);
        
        // Compute magnitude
        float mag_sq = 0.0f;
        for (int d = 0; d < point->dimension; d++) {
            mag_sq += cabsf(gradient[d]) * cabsf(gradient[d]);
        }
        float magnitude = sqrtf(mag_sq);
        
        // If gradient too large, scale down coefficient
        if (magnitude > config->max_gradient_magnitude) {
            float scale = config->max_gradient_magnitude / magnitude;
            field->manifold_6d.centers[i].coeff *= scale;
            limited = true;
        }
    }
    
    if (limited) {
        field->energy_valid = false;
        config->num_gradient_clips++;
    }
    
    return limited;
}

bool TCDE_ApplyAllLimiters(TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config) return false;
    
    bool any_limited = false;
    
    // Order matters: epsilon → metric → amplitude → energy → gradient
    
    // 1. Epsilon limiter (affects RBF evaluation)
    if (TCDE_ApplyEpsilonLimiter(field, config) > 0) {
        any_limited = true;
    }
    
    // 2. Metric limiter (affects geometry)
    if (TCDE_ApplyMetricLimiter(field, config) > 0) {
        any_limited = true;
    }
    
    // 3. Amplitude limiter (affects individual centers)
    if (TCDE_ApplyAmplitudeLimiter(field, config) > 0) {
        any_limited = true;
    }
    
    // 4. Energy limiter (affects global scale)
    if (TCDE_ApplyEnergyLimiter(field, config)) {
        any_limited = true;
    }
    
    // 5. Gradient limiter (affects evolution)
    if (TCDE_ApplyGradientLimiter(field, config)) {
        any_limited = true;
    }
    
    return any_limited;
}

bool TCDE_CheckLimits(const TCDE_Field* field, const TCDE_LimiterConfig* config) {
    if (!field || !config) return false;
    
    // Check energy
    float energy = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float amplitude = cabsf(field->manifold_6d.centers[i].coeff);
        energy += amplitude * amplitude;
    }
    if (energy > config->max_energy) return false;
    
    // Check amplitudes
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float amplitude = cabsf(field->manifold_6d.centers[i].coeff);
        if (amplitude > config->max_center_amplitude) return false;
    }
    
    // Check epsilons
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float eps = field->manifold_6d.centers[i].epsilon;
        if (eps < config->min_epsilon || eps > config->max_epsilon) return false;
    }
    
    // Check metrics
    if (field->manifold_6d.metric.is_valid) {
        float det = field->manifold_6d.metric.det;
        if (det < config->min_metric_det || det > config->max_metric_det) return false;
    }
    
    return true;
}

void TCDE_AutoAdjustLimiters(const TCDE_Field* field, TCDE_LimiterConfig* config) {
    if (!field || !config || !config->auto_adjust) return;
    if (field->manifold_6d.num_centers == 0) return;
    
    // Analyze field statistics
    float max_amplitude = 0.0f;
    float total_energy = 0.0f;
    float max_epsilon = 0.0f;
    float min_epsilon = INFINITY;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float amplitude = cabsf(field->manifold_6d.centers[i].coeff);
        max_amplitude = fmaxf(max_amplitude, amplitude);
        total_energy += amplitude * amplitude;
        
        float eps = field->manifold_6d.centers[i].epsilon;
        max_epsilon = fmaxf(max_epsilon, eps);
        min_epsilon = fminf(min_epsilon, eps);
    }
    
    // Adjust limits with safety factor
    float safety = config->safety_factor;
    
    // Energy: set to 2x current with safety factor
    config->max_energy = total_energy * 2.0f / safety;
    
    // Amplitude: set to 2x current max with safety factor
    config->max_center_amplitude = max_amplitude * 2.0f / safety;
    
    // Epsilon: expand range slightly
    config->min_epsilon = min_epsilon * 0.5f;
    config->max_epsilon = max_epsilon * 2.0f;
}

void TCDE_PrintLimiterStats(const TCDE_LimiterConfig* config) {
    if (!config) return;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  LIMITER STATISTICS                                        ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Energy clips:    %6d                                  ║\n", config->num_energy_clips);
    printf("║  Gradient clips:  %6d                                  ║\n", config->num_gradient_clips);
    printf("║  Epsilon clips:   %6d                                  ║\n", config->num_epsilon_clips);
    printf("║  Metric clips:    %6d                                  ║\n", config->num_metric_clips);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Configuration:                                            ║\n");
    printf("║    Max Energy:         %.2e                            ║\n", config->max_energy);
    printf("║    Max Amplitude:      %.2e                            ║\n", config->max_center_amplitude);
    printf("║    Epsilon Range:      [%.2e, %.2e]                ║\n", 
           config->min_epsilon, config->max_epsilon);
    printf("║    Auto-adjust:        %s                                  ║\n", 
           config->auto_adjust ? "ON " : "OFF");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void TCDE_ResetLimiterStats(TCDE_LimiterConfig* config) {
    if (!config) return;
    
    config->num_energy_clips = 0;
    config->num_gradient_clips = 0;
    config->num_epsilon_clips = 0;
    config->num_metric_clips = 0;
}
