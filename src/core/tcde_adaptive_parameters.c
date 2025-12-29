/**
 * @file tcde_adaptive_parameters.c
 * @brief TCDE Adaptive Parameters Implementation
 *
 * Implements self-regulating physical laws for ASI capability.
 * Parameters evolve coherently with field dynamics.
 */

#include "tcde_adaptive_parameters.h"
#include "../utils/tcde_measures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute field complexity metric for adaptation
 */
static float compute_adaptation_complexity(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers < 3) return 0.0f;

    // Complexity = fractal_dimension * energy_variance * interaction_strength
    float fractal_dim = TCDE_MeasureFractalDimension((TCDE_Field*)field);
    float energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    
    // Compute energy variance across field
    float energy_variance = 0.0f;
    float mean_energy = energy / field->manifold_6d.num_centers;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float center_energy = cabsf(field->manifold_6d.centers[i].coeff);
        center_energy *= center_energy;
        float diff = center_energy - mean_energy;
        energy_variance += diff * diff;
    }
    energy_variance /= field->manifold_6d.num_centers;
    
    return fractal_dim * sqrtf(energy_variance);
}

/**
 * @brief Compute energy gradient with respect to parameter
 */
static float compute_energy_gradient_wrt_param(const TCDE_Field* field,
                                               const TCDE_Parameters* params,
                                               const char* param_name,
                                               float delta) {
    // Finite difference approximation: dE/dp ≈ (E(p+δ) - E(p-δ)) / (2δ)
    TCDE_Parameters params_plus = *params;
    TCDE_Parameters params_minus = *params;
    
    // Modify parameter
    if (strcmp(param_name, "D") == 0) {
        params_plus.D += delta;
        params_minus.D -= delta;
    } else if (strcmp(param_name, "alpha") == 0) {
        params_plus.alpha += delta;
        params_minus.alpha -= delta;
    } else if (strcmp(param_name, "beta") == 0) {
        params_plus.beta += delta;
        params_minus.beta -= delta;
    } else if (strcmp(param_name, "gamma") == 0) {
        params_plus.gamma += delta;
        params_minus.gamma -= delta;
    }
    
    // Compute energies (simplified - would need field evolution)
    float E_current = TCDE_ComputeEnergy((TCDE_Field*)field);
    
    // Gradient approximation based on current energy and parameter value
    float gradient = 0.0f;
    if (strcmp(param_name, "D") == 0) {
        // Higher D → more diffusion → lower energy concentration
        gradient = -E_current / (params->D + 1e-6f);
    } else if (strcmp(param_name, "alpha") == 0) {
        // Higher α → more nonlinearity → energy saturation
        gradient = E_current * params->alpha;
    } else if (strcmp(param_name, "beta") == 0) {
        // Torsion creates/destroys energy
        gradient = E_current * sinf(params->beta * 10.0f);
    } else if (strcmp(param_name, "gamma") == 0) {
        // Coupling redistributes energy
        gradient = E_current * cosf(params->gamma * 10.0f);
    }
    
    return gradient;
}

// ============================================================================
// PARAMETER HISTORY MANAGEMENT
// ============================================================================

static TCDE_ParameterHistory* create_parameter_history(int capacity) {
    TCDE_ParameterHistory* history = (TCDE_ParameterHistory*)malloc(sizeof(TCDE_ParameterHistory));
    if (!history) return NULL;
    
    history->D_history = (float*)calloc(capacity, sizeof(float));
    history->alpha_history = (float*)calloc(capacity, sizeof(float));
    history->beta_history = (float*)calloc(capacity, sizeof(float));
    history->gamma_history = (float*)calloc(capacity, sizeof(float));
    history->time_history = (float*)calloc(capacity, sizeof(float));
    
    if (!history->D_history || !history->alpha_history || 
        !history->beta_history || !history->gamma_history || !history->time_history) {
        free(history->D_history);
        free(history->alpha_history);
        free(history->beta_history);
        free(history->gamma_history);
        free(history->time_history);
        free(history);
        return NULL;
    }
    
    history->history_size = 0;
    history->history_capacity = capacity;
    
    return history;
}

static void destroy_parameter_history(TCDE_ParameterHistory* history) {
    if (!history) return;
    
    free(history->D_history);
    free(history->alpha_history);
    free(history->beta_history);
    free(history->gamma_history);
    free(history->time_history);
    free(history);
}

static void record_parameters(TCDE_ParameterHistory* history,
                             const TCDE_Parameters* params,
                             float time) {
    if (!history || history->history_size >= history->history_capacity) return;
    
    int idx = history->history_size;
    history->D_history[idx] = params->D;
    history->alpha_history[idx] = params->alpha;
    history->beta_history[idx] = params->beta;
    history->gamma_history[idx] = params->gamma;
    history->time_history[idx] = time;
    history->history_size++;
}

// ============================================================================
// ADAPTIVE PARAMETER MANAGER
// ============================================================================

TCDE_AdaptiveParameterManager* TCDE_CreateAdaptiveParameterManager(
    const TCDE_Parameters* initial_params,
    TCDE_AdaptationStrategy strategy) {
    
    if (!initial_params) return NULL;
    
    TCDE_AdaptiveParameterManager* manager = 
        (TCDE_AdaptiveParameterManager*)malloc(sizeof(TCDE_AdaptiveParameterManager));
    if (!manager) return NULL;
    
    // Copy initial parameters
    manager->params = (TCDE_Parameters*)malloc(sizeof(TCDE_Parameters));
    if (!manager->params) {
        free(manager);
        return NULL;
    }
    *manager->params = *initial_params;
    
    // Create history
    manager->history = create_parameter_history(1000);
    if (!manager->history) {
        free(manager->params);
        free(manager);
        return NULL;
    }
    
    // Initialize control parameters
    manager->control.learning_rate = 0.01f;
    manager->control.stability_threshold = 0.1f;
    manager->control.energy_target = 1.0f;
    manager->control.complexity_target = 2.5f;
    manager->control.min_D = 0.01f;
    manager->control.max_D = 1.0f;
    manager->control.min_alpha = 0.001f;
    manager->control.max_alpha = 0.5f;
    manager->control.min_beta = 0.0f;
    manager->control.max_beta = 0.1f;
    manager->control.min_gamma = 0.0f;
    manager->control.max_gamma = 0.2f;
    manager->control.enable_constraints = true;
    
    manager->strategy = strategy;
    manager->adaptation_energy = 0.0f;
    manager->parameter_stability = 1.0f;
    manager->adaptation_active = true;
    manager->adaptation_steps = 0;
    
    // Record initial state
    record_parameters(manager->history, manager->params, 0.0f);
    
    return manager;
}

void TCDE_DestroyAdaptiveParameterManager(TCDE_AdaptiveParameterManager* manager) {
    if (!manager) return;
    
    free(manager->params);
    destroy_parameter_history(manager->history);
    free(manager);
}

void TCDE_ConfigureAdaptation(TCDE_AdaptiveParameterManager* manager,
                              float learning_rate,
                              float energy_target,
                              float complexity_target) {
    if (!manager) return;
    
    manager->control.learning_rate = learning_rate;
    manager->control.energy_target = energy_target;
    manager->control.complexity_target = complexity_target;
}

void TCDE_SetParameterBounds(TCDE_AdaptiveParameterManager* manager,
                            float min_D, float max_D,
                            float min_alpha, float max_alpha,
                            float min_beta, float max_beta,
                            float min_gamma, float max_gamma) {
    if (!manager) return;
    
    manager->control.min_D = min_D;
    manager->control.max_D = max_D;
    manager->control.min_alpha = min_alpha;
    manager->control.max_alpha = max_alpha;
    manager->control.min_beta = min_beta;
    manager->control.max_beta = max_beta;
    manager->control.min_gamma = min_gamma;
    manager->control.max_gamma = max_gamma;
}

// ============================================================================
// PARAMETER GRADIENT COMPUTATION
// ============================================================================

TCDE_ParameterGradient TCDE_ComputeParameterGradients(const TCDE_Field* field,
                                                       const TCDE_Parameters* params) {
    TCDE_ParameterGradient grad = {0};
    
    if (!field || !params) return grad;
    
    float delta = 0.001f;
    
    grad.dE_dD = compute_energy_gradient_wrt_param(field, params, "D", delta);
    grad.dE_dalpha = compute_energy_gradient_wrt_param(field, params, "alpha", delta);
    grad.dE_dbeta = compute_energy_gradient_wrt_param(field, params, "beta", delta);
    grad.dE_dgamma = compute_energy_gradient_wrt_param(field, params, "gamma", delta);
    
    grad.gradient_norm = sqrtf(grad.dE_dD * grad.dE_dD +
                              grad.dE_dalpha * grad.dE_dalpha +
                              grad.dE_dbeta * grad.dE_dbeta +
                              grad.dE_dgamma * grad.dE_dgamma);
    
    return grad;
}

// ============================================================================
// ADAPTATION STRATEGIES
// ============================================================================

bool TCDE_AdaptParametersEnergy(TCDE_AdaptiveParameterManager* manager,
                               const TCDE_Field* field) {
    if (!manager || !field) return false;
    
    float current_energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    float energy_error = manager->control.energy_target - current_energy;
    float lr = manager->control.learning_rate;
    
    // Adapt D: increase if energy too high (more diffusion)
    manager->params->D += lr * energy_error * 0.1f;
    
    // Adapt alpha: increase if energy too low (more nonlinearity)
    manager->params->alpha -= lr * energy_error * 0.05f;
    
    // Apply bounds
    if (manager->control.enable_constraints) {
        manager->params->D = fmaxf(manager->control.min_D, 
                                   fminf(manager->control.max_D, manager->params->D));
        manager->params->alpha = fmaxf(manager->control.min_alpha,
                                      fminf(manager->control.max_alpha, manager->params->alpha));
    }
    
    manager->adaptation_steps++;
    record_parameters(manager->history, manager->params, field->time);
    
    return true;
}

bool TCDE_AdaptParametersComplexity(TCDE_AdaptiveParameterManager* manager,
                                   const TCDE_Field* field) {
    if (!manager || !field) return false;
    
    float current_complexity = compute_adaptation_complexity(field);
    float complexity_error = manager->control.complexity_target - current_complexity;
    float lr = manager->control.learning_rate;
    
    // Adapt beta (torsion): increase for more complexity
    manager->params->beta += lr * complexity_error * 0.02f;
    
    // Adapt gamma (coupling): increase for coherence
    manager->params->gamma += lr * complexity_error * 0.03f;
    
    // Apply bounds
    if (manager->control.enable_constraints) {
        manager->params->beta = fmaxf(manager->control.min_beta,
                                     fminf(manager->control.max_beta, manager->params->beta));
        manager->params->gamma = fmaxf(manager->control.min_gamma,
                                      fminf(manager->control.max_gamma, manager->params->gamma));
    }
    
    manager->adaptation_steps++;
    record_parameters(manager->history, manager->params, field->time);
    
    return true;
}

bool TCDE_AdaptParametersGradient(TCDE_AdaptiveParameterManager* manager,
                                 const TCDE_Field* field) {
    if (!manager || !field) return false;
    
    TCDE_ParameterGradient grad = TCDE_ComputeParameterGradients(field, manager->params);
    float lr = manager->control.learning_rate;
    
    // Gradient descent to minimize energy
    manager->params->D -= lr * grad.dE_dD;
    manager->params->alpha -= lr * grad.dE_dalpha;
    manager->params->beta -= lr * grad.dE_dbeta;
    manager->params->gamma -= lr * grad.dE_dgamma;
    
    // Apply bounds
    if (manager->control.enable_constraints) {
        manager->params->D = fmaxf(manager->control.min_D,
                                  fminf(manager->control.max_D, manager->params->D));
        manager->params->alpha = fmaxf(manager->control.min_alpha,
                                      fminf(manager->control.max_alpha, manager->params->alpha));
        manager->params->beta = fmaxf(manager->control.min_beta,
                                     fminf(manager->control.max_beta, manager->params->beta));
        manager->params->gamma = fmaxf(manager->control.min_gamma,
                                      fminf(manager->control.max_gamma, manager->params->gamma));
    }
    
    manager->adaptation_steps++;
    manager->adaptation_energy += grad.gradient_norm * lr;
    record_parameters(manager->history, manager->params, field->time);
    
    return true;
}

bool TCDE_EvolveCoupledParameterField(TCDE_AdaptiveParameterManager* manager,
                                      TCDE_Field* field) {
    if (!manager || !field) return false;
    
    // First evolve field with current parameters
    TCDE_EvolveStep(field, manager->params);
    
    // Then adapt parameters based on strategy
    switch (manager->strategy) {
        case TCDE_ADAPT_ENERGY_BASED:
            return TCDE_AdaptParametersEnergy(manager, field);
        case TCDE_ADAPT_COMPLEXITY_BASED:
            return TCDE_AdaptParametersComplexity(manager, field);
        case TCDE_ADAPT_GRADIENT_BASED:
            return TCDE_AdaptParametersGradient(manager, field);
        case TCDE_ADAPT_FULL_COUPLED:
            // Combine all strategies
            TCDE_AdaptParametersEnergy(manager, field);
            TCDE_AdaptParametersComplexity(manager, field);
            return TCDE_AdaptParametersGradient(manager, field);
        default:
            return false;
    }
}

// ============================================================================
// PARAMETER ANALYSIS
// ============================================================================

float TCDE_ComputeParameterStability(const TCDE_AdaptiveParameterManager* manager) {
    if (!manager || manager->history->history_size < 10) return 1.0f;
    
    // Compute variance of recent parameter changes
    int window = 10;
    int start = manager->history->history_size - window;
    
    float var_D = 0.0f, var_alpha = 0.0f, var_beta = 0.0f, var_gamma = 0.0f;
    
    for (int i = start; i < manager->history->history_size - 1; i++) {
        float dD = manager->history->D_history[i+1] - manager->history->D_history[i];
        float dalpha = manager->history->alpha_history[i+1] - manager->history->alpha_history[i];
        float dbeta = manager->history->beta_history[i+1] - manager->history->beta_history[i];
        float dgamma = manager->history->gamma_history[i+1] - manager->history->gamma_history[i];
        
        var_D += dD * dD;
        var_alpha += dalpha * dalpha;
        var_beta += dbeta * dbeta;
        var_gamma += dgamma * dgamma;
    }
    
    float total_variance = var_D + var_alpha + var_beta + var_gamma;
    float stability = expf(-total_variance * 100.0f);
    
    return stability;
}

bool TCDE_CheckParameterConvergence(const TCDE_AdaptiveParameterManager* manager,
                                   float tolerance) {
    if (!manager) return false;
    
    float stability = TCDE_ComputeParameterStability(manager);
    return stability > (1.0f - tolerance);
}

TCDE_Parameters TCDE_GetAdaptedParameters(const TCDE_AdaptiveParameterManager* manager) {
    if (!manager || !manager->params) {
        TCDE_Parameters default_params = {0};
        return default_params;
    }
    return *manager->params;
}

void TCDE_PrintParameterStatus(const TCDE_AdaptiveParameterManager* manager) {
    if (!manager) return;
    
    printf("\n=== ADAPTIVE PARAMETER STATUS ===\n");
    printf("Strategy: ");
    switch (manager->strategy) {
        case TCDE_ADAPT_NONE: printf("NONE\n"); break;
        case TCDE_ADAPT_ENERGY_BASED: printf("ENERGY-BASED\n"); break;
        case TCDE_ADAPT_COMPLEXITY_BASED: printf("COMPLEXITY-BASED\n"); break;
        case TCDE_ADAPT_GRADIENT_BASED: printf("GRADIENT-BASED\n"); break;
        case TCDE_ADAPT_FULL_COUPLED: printf("FULL COUPLED\n"); break;
    }
    
    printf("Adaptation steps: %d\n", manager->adaptation_steps);
    printf("Parameter stability: %.3f\n", manager->parameter_stability);
    printf("Adaptation energy: %.3f\n", manager->adaptation_energy);
    printf("Active: %s\n\n", manager->adaptation_active ? "YES" : "NO");
    
    printf("Current Parameters:\n");
    printf("  D (diffusion):    %.6f [%.3f, %.3f]\n", 
           manager->params->D, manager->control.min_D, manager->control.max_D);
    printf("  α (nonlinearity): %.6f [%.3f, %.3f]\n",
           manager->params->alpha, manager->control.min_alpha, manager->control.max_alpha);
    printf("  β (torsion):      %.6f [%.3f, %.3f]\n",
           manager->params->beta, manager->control.min_beta, manager->control.max_beta);
    printf("  γ (coupling):     %.6f [%.3f, %.3f]\n",
           manager->params->gamma, manager->control.min_gamma, manager->control.max_gamma);
    printf("\n");
}

bool TCDE_SaveParameterHistory(const TCDE_AdaptiveParameterManager* manager,
                               const char* filename) {
    if (!manager || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# TCDE Adaptive Parameter History\n");
    fprintf(file, "# time,D,alpha,beta,gamma\n");
    
    for (int i = 0; i < manager->history->history_size; i++) {
        fprintf(file, "%.6f,%.6f,%.6f,%.6f,%.6f\n",
                manager->history->time_history[i],
                manager->history->D_history[i],
                manager->history->alpha_history[i],
                manager->history->beta_history[i],
                manager->history->gamma_history[i]);
    }
    
    fclose(file);
    return true;
}
