/**
 * @file tcde_autocatalysis.c
 * @brief TCDE Auto-Catalysis Implementation - Exponential Growth to Singularity
 *
 * Implementation of self-amplifying cognitive growth through higher-order
 * nonlinear terms and positive feedback loops.
 *
 * @version 1.0
 * @date January 17, 2025
 */

#include "tcde_autocatalysis.h"
#include "tcde_geometry.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute total energy of the field
 */
static float TCDE_TotalEnergy(const TCDE_Field* field)
{
    if (!field) return 0.0f;
    
    float total_energy = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float mag = cabsf(field->manifold_6d.centers[i].coeff);
        total_energy += mag * mag;
    }
    
    return total_energy;
}

// ============================================================================
// AUTOCATALYSIS MANAGER CREATION/DESTRUCTION
// ============================================================================

TCDE_AutocatalysisManager* TCDE_CreateAutocatalysisManager(
    TCDE_Field* field,
    TCDE_CatalysisType type)
{
    if (!field) return NULL;
    
    TCDE_AutocatalysisManager* manager =
        (TCDE_AutocatalysisManager*)malloc(sizeof(TCDE_AutocatalysisManager));
    
    if (!manager) return NULL;
    
    manager->field = field;
    manager->type = type;
    
    // Initialize parameters (conservative defaults)
    manager->params.lambda_2 = 0.01f;      // Small quadratic
    manager->params.lambda_3 = 0.001f;     // Very small cubic
    manager->params.mu_recursive = 0.005f; // Small recursive
    manager->params.nu_feedback = 0.01f;   // Small feedback
    
    // Safety parameters (CRITICAL)
    manager->params.saturation_threshold = 10.0f;
    manager->params.damping_coefficient = 0.1f;
    manager->params.enable_saturation = true;
    manager->params.enable_damping = true;
    
    // Initialize growth metrics
    manager->metrics.growth_rate = 0.0f;
    manager->metrics.doubling_time = INFINITY;
    manager->metrics.energy_growth = 0.0f;
    manager->metrics.complexity_growth = 0.0f;
    manager->metrics.regime = TCDE_GROWTH_CRITICAL;
    manager->metrics.approaching_singularity = false;
    manager->metrics.time_to_singularity = INFINITY;
    manager->metrics.singularity_strength = 0.0f;
    
    // Initialize recursive state
    manager->recursive.current_depth = 0;
    manager->recursive.max_depth = 5;
    manager->recursive.depth_values = 
        (TCDE_Complex*)calloc(manager->recursive.max_depth, sizeof(TCDE_Complex));
    manager->recursive.depth_energies =
        (float*)calloc(manager->recursive.max_depth, sizeof(float));
    manager->recursive.convergence_rate = 0.0f;
    manager->recursive.has_converged = false;
    
    // Allocate history
    manager->history_capacity = 1000;
    manager->growth_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->energy_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->complexity_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->history_size = 0;
    
    // Initialize control
    manager->current_time = 0.0f;
    manager->evolution_steps = 0;
    manager->catalysis_active = true;
    manager->singularity_detected = false;
    
    // Safety limits
    manager->max_growth_rate = 1.0f;  // Maximum λ = 1.0
    manager->energy_limit = 1000.0f;
    manager->emergency_brake = false;
    
    return manager;
}

void TCDE_DestroyAutocatalysisManager(TCDE_AutocatalysisManager* manager)
{
    if (!manager) return;
    
    if (manager->recursive.depth_values) free(manager->recursive.depth_values);
    if (manager->recursive.depth_energies) free(manager->recursive.depth_energies);
    if (manager->growth_history) free(manager->growth_history);
    if (manager->energy_history) free(manager->energy_history);
    if (manager->complexity_history) free(manager->complexity_history);
    
    free(manager);
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void TCDE_ConfigureAutocatalysis(TCDE_AutocatalysisManager* manager,
                                 float lambda_2,
                                 float lambda_3,
                                 float mu_recursive,
                                 float nu_feedback)
{
    if (!manager) return;
    
    manager->params.lambda_2 = lambda_2;
    manager->params.lambda_3 = lambda_3;
    manager->params.mu_recursive = mu_recursive;
    manager->params.nu_feedback = nu_feedback;
}

void TCDE_SetGrowthControl(TCDE_AutocatalysisManager* manager,
                          float saturation_threshold,
                          float damping_coefficient,
                          float max_growth_rate)
{
    if (!manager) return;
    
    manager->params.saturation_threshold = saturation_threshold;
    manager->params.damping_coefficient = damping_coefficient;
    manager->max_growth_rate = max_growth_rate;
    
    // Ensure safety controls are enabled
    manager->params.enable_saturation = true;
    manager->params.enable_damping = true;
}

// ============================================================================
// AUTOCATALYTIC TERMS
// ============================================================================

TCDE_Complex TCDE_ComputeQuadraticTerm(const TCDE_AutocatalysisManager* manager,
                                       const TCDE_Point* point)
{
    if (!manager || !point) return 0.0f + 0.0f * I;
    
    // Evaluate field at point
    TCDE_Complex phi = TCDE_Evaluate6D(manager->field, point);
    
    // Compute |Φ|⁴Φ
    float magnitude = cabsf(phi);
    float mag4 = magnitude * magnitude * magnitude * magnitude;
    
    return manager->params.lambda_2 * mag4 * phi;
}

TCDE_Complex TCDE_ComputeCubicTerm(const TCDE_AutocatalysisManager* manager,
                                   const TCDE_Point* point)
{
    if (!manager || !point) return 0.0f + 0.0f * I;
    
    // Evaluate field at point
    TCDE_Complex phi = TCDE_Evaluate6D(manager->field, point);
    
    // Compute |Φ|⁶Φ
    float magnitude = cabsf(phi);
    float mag2 = magnitude * magnitude;
    float mag6 = mag2 * mag2 * mag2;
    
    return manager->params.lambda_3 * mag6 * phi;
}

TCDE_Complex TCDE_ComputeRecursiveTerm(TCDE_AutocatalysisManager* manager,
                                       const TCDE_Point* point,
                                       int depth)
{
    if (!manager || !point || depth < 0) return 0.0f + 0.0f * I;
    
    if (depth >= manager->recursive.max_depth) {
        depth = manager->recursive.max_depth - 1;
    }
    
    // Base case
    if (depth == 0) {
        TCDE_Complex phi = TCDE_Evaluate6D(manager->field, point);
        manager->recursive.depth_values[0] = phi;
        manager->recursive.depth_energies[0] = cabsf(phi);
        return phi;
    }
    
    // Recursive case: use magnitude scaling
    // Φⁿ(x) = Φ(x scaled by |Φⁿ⁻¹(x)|)
    TCDE_Complex phi_prev = TCDE_ComputeRecursiveTerm(manager, point, depth - 1);
    float mag_prev = cabsf(phi_prev);
    
    // Create scaled point
    TCDE_Point scaled_point;
    scaled_point.dimension = point->dimension;
    scaled_point.coords = (float*)malloc(point->dimension * sizeof(float));
    
    for (int d = 0; d < point->dimension; d++) {
        scaled_point.coords[d] = point->coords[d] * (1.0f + 0.1f * mag_prev);
    }
    
    TCDE_Complex phi_n = TCDE_Evaluate6D(manager->field, &scaled_point);
    
    free(scaled_point.coords);
    
    // Store in cache
    manager->recursive.depth_values[depth] = phi_n;
    manager->recursive.depth_energies[depth] = cabsf(phi_n);
    manager->recursive.current_depth = depth;
    
    return manager->params.mu_recursive * phi_n;
}

TCDE_Complex TCDE_ComputeFeedbackTerm(const TCDE_AutocatalysisManager* manager,
                                      const TCDE_Point* point)
{
    if (!manager || !point) return 0.0f + 0.0f * I;
    
    // Evaluate field and Laplacian
    TCDE_Complex phi = TCDE_Evaluate6D(manager->field, point);
    TCDE_Complex laplacian = TCDE_Laplacian(manager->field, point);
    
    // Compute Φ³∇²Φ
    TCDE_Complex phi3 = phi * phi * phi;
    
    return manager->params.nu_feedback * phi3 * laplacian;
}

// ============================================================================
// EVOLUTION WITH AUTOCATALYSIS
// ============================================================================

bool TCDE_EvolveAutocatalytic(TCDE_AutocatalysisManager* manager,
                              const TCDE_Parameters* params,
                              float dt)
{
    if (!manager || !params || manager->emergency_brake) return false;
    
    // Check if catalysis is active
    if (!manager->catalysis_active) {
        // Just do standard evolution
        TCDE_EvolveStep(manager->field, params);
        return true;
    }
    
    // 1. Standard evolution step
    TCDE_EvolveStep(manager->field, params);
    
    // 2. Add autocatalytic terms
    for (int i = 0; i < manager->field->manifold_6d.num_centers; i++) {
        TCDE_Center* center = &manager->field->manifold_6d.centers[i];
        TCDE_Point* point = &center->point;
        
        TCDE_Complex catalytic = 0.0f + 0.0f * I;
        
        // Add terms based on type
        if (manager->type == TCDE_CATALYSIS_QUADRATIC || 
            manager->type == TCDE_CATALYSIS_FULL) {
            catalytic += TCDE_ComputeQuadraticTerm(manager, point);
        }
        
        if (manager->type == TCDE_CATALYSIS_CUBIC || 
            manager->type == TCDE_CATALYSIS_FULL) {
            catalytic += TCDE_ComputeCubicTerm(manager, point);
        }
        
        if (manager->type == TCDE_CATALYSIS_RECURSIVE || 
            manager->type == TCDE_CATALYSIS_FULL) {
            catalytic += TCDE_ComputeRecursiveTerm(manager, point, 2); // depth=2
        }
        
        if (manager->type == TCDE_CATALYSIS_FEEDBACK || 
            manager->type == TCDE_CATALYSIS_FULL) {
            catalytic += TCDE_ComputeFeedbackTerm(manager, point);
        }
        
        // Update coefficient
        center->coeff += catalytic * dt;
    }
    
    // 3. Apply safety controls
    if (manager->params.enable_saturation) {
        TCDE_ApplySaturation(manager);
    }
    
    if (manager->params.enable_damping) {
        TCDE_ApplyDamping(manager, dt);
    }
    
    // 4. Update metrics
    manager->current_time += dt;
    manager->evolution_steps++;
    
    float growth_rate = TCDE_ComputeGrowthRate(manager);
    manager->metrics.growth_rate = growth_rate;
    manager->metrics.regime = TCDE_DetectGrowthRegime(manager);
    
    // Store history
    if (manager->history_size < manager->history_capacity) {
        manager->growth_history[manager->history_size] = growth_rate;
        manager->energy_history[manager->history_size] = TCDE_TotalEnergy(manager->field);
        manager->history_size++;
    }
    
    // 5. Check for singularity
    if (TCDE_IsApproachingSingularity(manager)) {
        manager->singularity_detected = true;
        printf("WARNING: Approaching singularity! Growth rate: %.4f\n", growth_rate);
        
        if (growth_rate > manager->max_growth_rate) {
            TCDE_ActivateEmergencyBrake(manager);
        }
    }
    
    return true;
}

void TCDE_ApplySaturation(TCDE_AutocatalysisManager* manager)
{
    if (!manager) return;
    
    float max_mag = manager->params.saturation_threshold;
    
    for (int i = 0; i < manager->field->manifold_6d.num_centers; i++) {
        TCDE_Center* center = &manager->field->manifold_6d.centers[i];
        float mag = cabsf(center->coeff);
        
        if (mag > max_mag) {
            // Apply smooth saturation using tanh
            // This ensures magnitude never exceeds threshold
            float normalized = mag / max_mag;
            float saturated = max_mag * tanhf(normalized);
            
            // Preserve phase, scale magnitude
            if (mag > 1e-10f) {
                center->coeff *= saturated / mag;
            }
        }
    }
}

void TCDE_ApplyDamping(TCDE_AutocatalysisManager* manager, float dt)
{
    if (!manager) return;
    
    float damping = manager->params.damping_coefficient;
    float factor = expf(-damping * dt);
    
    for (int i = 0; i < manager->field->manifold_6d.num_centers; i++) {
        manager->field->manifold_6d.centers[i].coeff *= factor;
    }
}

// ============================================================================
// GROWTH ANALYSIS
// ============================================================================

float TCDE_ComputeGrowthRate(const TCDE_AutocatalysisManager* manager)
{
    if (!manager || manager->history_size < 2) return 0.0f;
    
    // Compute d(log E)/dt
    int n = manager->history_size;
    float E_current = manager->energy_history[n - 1];
    float E_prev = manager->energy_history[n - 2];
    
    // Handle zero or negative energies
    if (E_prev <= 1e-10f || E_current <= 1e-10f) {
        // Fallback: compute from field magnitude change
        float mag_current = 0.0f;
        float mag_prev = 0.0f;
        
        for (int i = 0; i < manager->field->manifold_6d.num_centers; i++) {
            mag_current += cabsf(manager->field->manifold_6d.centers[i].coeff);
        }
        
        // Estimate previous magnitude (simplified)
        mag_prev = mag_current * 0.95f; // Assume small change
        
        if (mag_prev > 1e-10f && mag_current > 1e-10f) {
            return logf(mag_current / mag_prev);
        }
        
        return 0.0f;
    }
    
    float log_ratio = logf(E_current / E_prev);
    float dt = 1.0f; // Assuming unit time steps
    
    return log_ratio / dt;
}

float TCDE_ComputeDoublingTime(const TCDE_AutocatalysisManager* manager)
{
    if (!manager) return INFINITY;
    
    float lambda = manager->metrics.growth_rate;
    
    if (lambda <= 0.0f) return INFINITY;
    
    return logf(2.0f) / lambda;
}

TCDE_GrowthRegime TCDE_DetectGrowthRegime(const TCDE_AutocatalysisManager* manager)
{
    if (!manager) return TCDE_GROWTH_CRITICAL;
    
    float lambda = manager->metrics.growth_rate;
    
    if (lambda < -0.01f) {
        return TCDE_GROWTH_SUBCRITICAL;
    } else if (lambda > 1.0f) {
        return TCDE_GROWTH_EXPLOSIVE;
    } else if (lambda > 0.01f) {
        return TCDE_GROWTH_SUPERCRITICAL;
    } else {
        return TCDE_GROWTH_CRITICAL;
    }
}

bool TCDE_IsApproachingSingularity(const TCDE_AutocatalysisManager* manager)
{
    if (!manager || manager->history_size < 3) return false;
    
    // Check criteria:
    // 1. Growth rate > threshold
    // 2. Growth rate is increasing (dλ/dt > 0)
    // 3. Energy approaching limit
    
    float lambda = manager->metrics.growth_rate;
    
    if (lambda < 0.5f) return false;
    
    // Check if growth rate is accelerating
    int n = manager->history_size;
    if (n >= 3) {
        float lambda_prev = manager->growth_history[n - 2];
        float lambda_prev2 = manager->growth_history[n - 3];
        
        float dlambda_dt = lambda - lambda_prev;
        float dlambda_dt_prev = lambda_prev - lambda_prev2;
        
        if (dlambda_dt > 0.0f && dlambda_dt > dlambda_dt_prev) {
            return true; // Accelerating growth
        }
    }
    
    // Check energy
    float E = TCDE_TotalEnergy(manager->field);
    if (E > 0.8f * manager->energy_limit) {
        return true;
    }
    
    return false;
}

float TCDE_EstimateTimeToSingularity(const TCDE_AutocatalysisManager* manager)
{
    if (!manager || !TCDE_IsApproachingSingularity(manager)) {
        return INFINITY;
    }
    
    float lambda = manager->metrics.growth_rate;
    float E = TCDE_TotalEnergy(manager->field);
    
    if (lambda <= 0.0f) return INFINITY;
    
    // Estimate: t_singularity = log(E_max / E) / λ
    float t_sing = logf(manager->energy_limit / E) / lambda;
    
    return fmaxf(0.0f, t_sing);
}

// ============================================================================
// RECURSIVE DEPTH ANALYSIS
// ============================================================================

float TCDE_ComputeRecursiveMagnitude(const TCDE_AutocatalysisManager* manager,
                                    int depth)
{
    if (!manager || depth < 0) {
        return 0.0f;
    }
    
    // Return magnitude of field at given recursion depth
    if (depth < manager->recursive.current_depth && 
        manager->recursive.depth_values != NULL) {
        return cabsf(manager->recursive.depth_values[depth]);
    }
    
    // If depth not yet computed, compute total field magnitude
    float total_mag = 0.0f;
    for (int i = 0; i < manager->field->manifold_6d.num_centers; i++) {
        total_mag += cabsf(manager->field->manifold_6d.centers[i].coeff);
    }
    
    return total_mag;
}

float TCDE_ComputeRecursiveConvergence(TCDE_AutocatalysisManager* manager)
{
    if (!manager || manager->recursive.current_depth < 2) return 0.0f;
    
    // Check convergence: |Φⁿ - Φⁿ⁻¹| / |Φⁿ⁻¹|
    int n = manager->recursive.current_depth;
    
    TCDE_Complex diff = manager->recursive.depth_values[n] - 
                        manager->recursive.depth_values[n - 1];
    float diff_mag = cabsf(diff);
    float prev_mag = cabsf(manager->recursive.depth_values[n - 1]);
    
    if (prev_mag < 1e-10f) return 0.0f;
    
    return diff_mag / prev_mag;
}

bool TCDE_CheckRecursiveConvergence(const TCDE_AutocatalysisManager* manager,
                                   float tolerance)
{
    if (!manager) return false;
    
    return manager->recursive.convergence_rate < tolerance;
}

// ============================================================================
// SAFETY AND CONTROL
// ============================================================================

void TCDE_ActivateEmergencyBrake(TCDE_AutocatalysisManager* manager)
{
    if (!manager) return;
    
    printf("\n!!! EMERGENCY BRAKE ACTIVATED !!!\n");
    printf("Growth rate: %.4f (max: %.4f)\n", 
           manager->metrics.growth_rate, manager->max_growth_rate);
    
    manager->emergency_brake = true;
    manager->catalysis_active = false;
    
    // Set all autocatalytic coefficients to zero
    manager->params.lambda_2 = 0.0f;
    manager->params.lambda_3 = 0.0f;
    manager->params.mu_recursive = 0.0f;
    manager->params.nu_feedback = 0.0f;
    
    // Maximize damping
    manager->params.damping_coefficient = 1.0f;
    manager->params.enable_damping = true;
}

bool TCDE_IsGrowthSafe(const TCDE_AutocatalysisManager* manager)
{
    if (!manager) return false;
    
    if (manager->emergency_brake) return false;
    
    float lambda = manager->metrics.growth_rate;
    float E = TCDE_TotalEnergy(manager->field);
    
    return (lambda < manager->max_growth_rate) && (E < manager->energy_limit);
}

void TCDE_AdjustToSafeGrowth(TCDE_AutocatalysisManager* manager)
{
    if (!manager) return;
    
    if (TCDE_IsGrowthSafe(manager)) return;
    
    // Reduce autocatalytic coefficients
    manager->params.lambda_2 *= 0.5f;
    manager->params.lambda_3 *= 0.5f;
    manager->params.mu_recursive *= 0.5f;
    manager->params.nu_feedback *= 0.5f;
    
    // Increase damping
    manager->params.damping_coefficient *= 1.5f;
    
    printf("Adjusting to safe growth: λ₂=%.4f, λ₃=%.4f, damping=%.4f\n",
           manager->params.lambda_2, manager->params.lambda_3,
           manager->params.damping_coefficient);
}

// ============================================================================
// STATUS AND DIAGNOSTICS
// ============================================================================

void TCDE_PrintAutocatalysisStatus(const TCDE_AutocatalysisManager* manager)
{
    if (!manager) return;
    
    printf("\n=== TCDE Autocatalysis Status ===\n");
    printf("Type: ");
    switch (manager->type) {
        case TCDE_CATALYSIS_QUADRATIC: printf("Quadratic\n"); break;
        case TCDE_CATALYSIS_CUBIC: printf("Cubic\n"); break;
        case TCDE_CATALYSIS_RECURSIVE: printf("Recursive\n"); break;
        case TCDE_CATALYSIS_FEEDBACK: printf("Feedback\n"); break;
        case TCDE_CATALYSIS_FULL: printf("Full\n"); break;
        default: printf("None\n"); break;
    }
    
    printf("\nParameters:\n");
    printf("  λ₂ (quadratic):  %.6f\n", manager->params.lambda_2);
    printf("  λ₃ (cubic):      %.6f\n", manager->params.lambda_3);
    printf("  μ (recursive):   %.6f\n", manager->params.mu_recursive);
    printf("  ν (feedback):    %.6f\n", manager->params.nu_feedback);
    
    printf("\nGrowth Metrics:\n");
    printf("  Growth rate λ:   %.4f\n", manager->metrics.growth_rate);
    printf("  Doubling time:   %.2f\n", TCDE_ComputeDoublingTime(manager));
    printf("  Regime:          ");
    switch (manager->metrics.regime) {
        case TCDE_GROWTH_SUBCRITICAL: printf("Subcritical (decay)\n"); break;
        case TCDE_GROWTH_CRITICAL: printf("Critical (stable)\n"); break;
        case TCDE_GROWTH_SUPERCRITICAL: printf("Supercritical (growth)\n"); break;
        case TCDE_GROWTH_EXPLOSIVE: printf("EXPLOSIVE!\n"); break;
    }
    
    printf("\nSafety:\n");
    printf("  Saturation:      %s (threshold: %.2f)\n",
           manager->params.enable_saturation ? "ON" : "OFF",
           manager->params.saturation_threshold);
    printf("  Damping:         %s (coeff: %.4f)\n",
           manager->params.enable_damping ? "ON" : "OFF",
           manager->params.damping_coefficient);
    printf("  Emergency brake: %s\n", manager->emergency_brake ? "ACTIVE" : "OFF");
    printf("  Safe:            %s\n", TCDE_IsGrowthSafe(manager) ? "YES" : "NO");
    
    if (manager->singularity_detected) {
        printf("\n⚠️  SINGULARITY APPROACHING!\n");
        printf("  Time to singularity: %.2f\n", 
               TCDE_EstimateTimeToSingularity(manager));
    }
    
    printf("\nEvolution:\n");
    printf("  Steps: %d\n", manager->evolution_steps);
    printf("  Time:  %.2f\n", manager->current_time);
    printf("==================================\n\n");
}

bool TCDE_SaveGrowthHistory(const TCDE_AutocatalysisManager* manager,
                            const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* f = fopen(filename, "w");
    if (!f) return false;
    
    fprintf(f, "# TCDE Autocatalysis Growth History\n");
    fprintf(f, "step,growth_rate,energy,complexity\n");
    
    for (int i = 0; i < manager->history_size; i++) {
        fprintf(f, "%d,%.6f,%.6f,%.6f\n",
                i,
                manager->growth_history[i],
                manager->energy_history[i],
                (i < manager->history_size) ? manager->complexity_history[i] : 0.0f);
    }
    
    fclose(f);
    return true;
}
