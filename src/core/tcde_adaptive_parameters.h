/**
 * @file tcde_adaptive_parameters.h
 * @brief TCDE Adaptive Parameters Module - Self-regulating physical laws
 *
 * Enables ASI-level capability: Physical parameters (D, α, β, γ) evolve
 * coherently with the field based on energy, complexity, and cognitive demands.
 * The system regulates its own physics dynamically.
 *
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_ADAPTIVE_PARAMETERS_H
#define TCDE_ADAPTIVE_PARAMETERS_H

#include "tcde_core.h"
#include "tcde_evolution.h"
#include "../utils/tcde_measures.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// ADAPTIVE PARAMETER STRUCTURES
// ============================================================================

/**
 * @brief Parameter adaptation strategy
 */
typedef enum {
    TCDE_ADAPT_NONE = 0,           // No adaptation (fixed parameters)
    TCDE_ADAPT_ENERGY_BASED = 1,   // Adapt based on field energy
    TCDE_ADAPT_COMPLEXITY_BASED = 2, // Adapt based on cognitive complexity
    TCDE_ADAPT_GRADIENT_BASED = 3,  // Adapt based on parameter gradients
    TCDE_ADAPT_FULL_COUPLED = 4     // Full coupled evolution with field
} TCDE_AdaptationStrategy;

/**
 * @brief Parameter evolution history
 */
typedef struct {
    float* D_history;              // Diffusion coefficient history
    float* alpha_history;          // Nonlinearity coefficient history
    float* beta_history;           // Torsion coefficient history
    float* gamma_history;          // Coupling coefficient history
    float* time_history;           // Time stamps
    int history_size;              // Current history size
    int history_capacity;          // Maximum history capacity
} TCDE_ParameterHistory;

/**
 * @brief Adaptation control parameters
 */
typedef struct {
    float learning_rate;           // Adaptation speed [0.001, 0.1]
    float stability_threshold;     // Minimum stability for adaptation
    float energy_target;           // Target energy level
    float complexity_target;       // Target complexity level
    float min_D;                   // Minimum diffusion coefficient
    float max_D;                   // Maximum diffusion coefficient
    float min_alpha;               // Minimum nonlinearity
    float max_alpha;               // Maximum nonlinearity
    float min_beta;                // Minimum torsion
    float max_beta;                // Maximum torsion
    float min_gamma;               // Minimum coupling
    float max_gamma;               // Maximum coupling
    bool enable_constraints;       // Enforce parameter bounds
} TCDE_AdaptationControl;

/**
 * @brief Adaptive parameter manager
 */
typedef struct {
    TCDE_Parameters* params;       // Current parameters
    TCDE_ParameterHistory* history; // Evolution history
    TCDE_AdaptationControl control; // Adaptation control
    TCDE_AdaptationStrategy strategy; // Adaptation strategy
    float adaptation_energy;       // Energy invested in adaptation
    float parameter_stability;     // Current parameter stability [0,1]
    bool adaptation_active;        // Adaptation process active
    int adaptation_steps;          // Number of adaptation steps taken
} TCDE_AdaptiveParameterManager;

/**
 * @brief Parameter gradient information
 */
typedef struct {
    float dE_dD;                   // Energy gradient w.r.t. D
    float dE_dalpha;               // Energy gradient w.r.t. α
    float dE_dbeta;                // Energy gradient w.r.t. β
    float dE_dgamma;               // Energy gradient w.r.t. γ
    float gradient_norm;           // Total gradient magnitude
} TCDE_ParameterGradient;

// ============================================================================
// ADAPTIVE PARAMETER API
// ============================================================================

/**
 * @brief Create adaptive parameter manager
 * @param initial_params Initial parameter values
 * @param strategy Adaptation strategy
 * @return Initialized adaptive parameter manager
 */
TCDE_AdaptiveParameterManager* TCDE_CreateAdaptiveParameterManager(
    const TCDE_Parameters* initial_params,
    TCDE_AdaptationStrategy strategy);

/**
 * @brief Destroy adaptive parameter manager
 * @param manager Manager to destroy
 */
void TCDE_DestroyAdaptiveParameterManager(TCDE_AdaptiveParameterManager* manager);

/**
 * @brief Configure adaptation control parameters
 * @param manager Adaptive parameter manager
 * @param learning_rate Adaptation speed
 * @param energy_target Target energy level
 * @param complexity_target Target complexity level
 */
void TCDE_ConfigureAdaptation(TCDE_AdaptiveParameterManager* manager,
                              float learning_rate,
                              float energy_target,
                              float complexity_target);

/**
 * @brief Set parameter bounds
 * @param manager Adaptive parameter manager
 * @param min_D Minimum diffusion coefficient
 * @param max_D Maximum diffusion coefficient
 * @param min_alpha Minimum nonlinearity
 * @param max_alpha Maximum nonlinearity
 * @param min_beta Minimum torsion
 * @param max_beta Maximum torsion
 * @param min_gamma Minimum coupling
 * @param max_gamma Maximum coupling
 */
void TCDE_SetParameterBounds(TCDE_AdaptiveParameterManager* manager,
                            float min_D, float max_D,
                            float min_alpha, float max_alpha,
                            float min_beta, float max_beta,
                            float min_gamma, float max_gamma);

/**
 * @brief Compute parameter gradients based on field state
 * @param field Current field state
 * @param params Current parameters
 * @return Parameter gradients
 */
TCDE_ParameterGradient TCDE_ComputeParameterGradients(const TCDE_Field* field,
                                                       const TCDE_Parameters* params);

/**
 * @brief Adapt parameters based on field energy
 * @param manager Adaptive parameter manager
 * @param field Current field state
 * @return True if adaptation successful
 */
bool TCDE_AdaptParametersEnergy(TCDE_AdaptiveParameterManager* manager,
                               const TCDE_Field* field);

/**
 * @brief Adapt parameters based on cognitive complexity
 * @param manager Adaptive parameter manager
 * @param field Current field state
 * @return True if adaptation successful
 */
bool TCDE_AdaptParametersComplexity(TCDE_AdaptiveParameterManager* manager,
                                   const TCDE_Field* field);

/**
 * @brief Adapt parameters using gradient descent
 * @param manager Adaptive parameter manager
 * @param field Current field state
 * @return True if adaptation successful
 */
bool TCDE_AdaptParametersGradient(TCDE_AdaptiveParameterManager* manager,
                                 const TCDE_Field* field);

/**
 * @brief Full coupled parameter-field evolution
 * @param manager Adaptive parameter manager
 * @param field Current field state
 * @return True if evolution successful
 */
bool TCDE_EvolveCoupledParameterField(TCDE_AdaptiveParameterManager* manager,
                                      TCDE_Field* field);

/**
 * @brief Compute parameter stability metric
 * @param manager Adaptive parameter manager
 * @return Stability score [0,1]
 */
float TCDE_ComputeParameterStability(const TCDE_AdaptiveParameterManager* manager);

/**
 * @brief Check if parameters have converged
 * @param manager Adaptive parameter manager
 * @param tolerance Convergence tolerance
 * @return True if converged
 */
bool TCDE_CheckParameterConvergence(const TCDE_AdaptiveParameterManager* manager,
                                   float tolerance);

/**
 * @brief Get current adapted parameters
 * @param manager Adaptive parameter manager
 * @return Current parameter values
 */
TCDE_Parameters TCDE_GetAdaptedParameters(const TCDE_AdaptiveParameterManager* manager);

/**
 * @brief Print parameter evolution status
 * @param manager Adaptive parameter manager
 */
void TCDE_PrintParameterStatus(const TCDE_AdaptiveParameterManager* manager);

/**
 * @brief Save parameter history to file
 * @param manager Adaptive parameter manager
 * @param filename Output filename
 * @return True on success
 */
bool TCDE_SaveParameterHistory(const TCDE_AdaptiveParameterManager* manager,
                               const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_ADAPTIVE_PARAMETERS_H
