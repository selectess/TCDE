/**
 * @file tcde_evolution.h
 * @brief TCDE Evolution Module - TDE Integration and Dynamics
 * 
 * Topological Diffusion Equation (TDE) evolution:
 * ∂Φ/∂t = D∇²_g Φ - α|Φ|²Φ + β·T(Φ) + γ·C(Φ)
 * 
 * Components:
 * - Diffusion: D∇²_g Φ (spreading)
 * - Nonlinearity: -α|Φ|²Φ (saturation)
 * - Torsion: β·T(Φ) (intuition/creativity)
 * - Coupling: γ·C(Φ) (semantic coherence)
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_EVOLUTION_H
#define TCDE_EVOLUTION_H

#include "tcde_core.h"
#include "tcde_geometry.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// EVOLUTION PARAMETERS
// ============================================================================

/**
 * @brief TDE evolution parameters
 */
typedef struct {
    float dt;                       // Time step
    float D;                        // Diffusion coefficient
    float alpha;                    // Nonlinearity strength
    float beta;                     // Torsion strength
    float gamma;                    // Coupling strength
    float sigma;                    // Coupling length scale
} TCDE_Parameters;

// ============================================================================
// PARAMETER CONFIGURATION
// ============================================================================

/**
 * @brief Configure evolution parameters based on field properties
 * @param params Output parameters
 * @param field Field
 * 
 * Automatically sets parameters based on:
 * - Fractal dimension
 * - Current energy
 * - Temporal dimension
 * 
 * Default ranges:
 * - dt: 0.001 - 0.01
 * - D: 0.05 - 0.2
 * - alpha: 0.01 - 0.1
 * - beta: 0.005 - 0.05
 * - gamma: 0.01 - 0.05
 * - sigma: 0.1 - 0.3
 */
void TCDE_ConfigureParameters(TCDE_Parameters* params, const TCDE_Field* field);

/**
 * @brief Set custom evolution parameters
 * @param params Output parameters
 * @param dt Time step
 * @param D Diffusion coefficient
 * @param alpha Nonlinearity strength
 * @param beta Torsion strength
 * @param gamma Coupling strength
 * @param sigma Coupling length scale
 */
void TCDE_SetParameters(TCDE_Parameters* params, float dt, float D, float alpha,
                        float beta, float gamma, float sigma);

// ============================================================================
// TDE EVOLUTION
// ============================================================================

/**
 * @brief Evolve field by one timestep using TDE
 * @param field Field to evolve
 * @param params Evolution parameters
 * 
 * Equation: ∂Φ/∂t = D∇²_g Φ - α|Φ|²Φ + β·T(Φ) + γ·C(Φ)
 * 
 * Method: IMEX (Implicit-Explicit) scheme
 * - Diffusion: Implicit (stable for large D)
 * - Nonlinearity: Explicit
 * - Torsion: Explicit
 * - Coupling: Explicit
 * 
 * Updates:
 * - Center coefficients
 * - Field time
 * - Temporal dimension
 * - Energy (invalidated, recomputed on demand)
 * 
 * Complexity: O(K²) dense, O(K log K) with spatial indexing
 */
void TCDE_EvolveStep(TCDE_Field* field, const TCDE_Parameters* params);

/**
 * @brief Evolve field for multiple steps
 * @param field Field to evolve
 * @param params Evolution parameters
 * @param num_steps Number of steps
 * 
 * Convenience function for multi-step evolution
 */
void TCDE_Evolve(TCDE_Field* field, const TCDE_Parameters* params, int num_steps);

// ============================================================================
// TDE TERM COMPUTATION
// ============================================================================

/**
 * @brief Compute diffusion term D∇²_g Φ
 * @param field Field
 * @param point Evaluation point
 * @param D Diffusion coefficient
 * @return Diffusion contribution (complex)
 * 
 * Uses Laplace-Beltrami operator for covariant diffusion
 */
TCDE_Complex TCDE_DiffusionTerm(const TCDE_Field* field, const TCDE_Point* point, 
                                 float D);

/**
 * @brief Compute nonlinearity term -α|Φ|²Φ
 * @param field Field
 * @param point Evaluation point
 * @param alpha Nonlinearity strength
 * @return Nonlinearity contribution (complex)
 * 
 * Prevents unbounded growth, creates saturation
 */
TCDE_Complex TCDE_NonlinearityTerm(const TCDE_Field* field, const TCDE_Point* point,
                                    float alpha);

/**
 * @brief Compute torsion term β·T(Φ)
 * @param field Field
 * @param point Evaluation point
 * @param beta Torsion strength
 * @return Torsion contribution (complex)
 * 
 * Enables non-gradient flow, intuitive leaps
 */
TCDE_Complex TCDE_TorsionTerm(const TCDE_Field* field, const TCDE_Point* point,
                               float beta);

/**
 * @brief Compute coupling term γ·C(Φ)
 * @param field Field
 * @param point Evaluation point
 * @param gamma Coupling strength
 * @param sigma Coupling length scale
 * @param num_samples Monte Carlo samples
 * @return Coupling contribution (complex)
 * 
 * Provides non-local semantic coherence
 */
TCDE_Complex TCDE_CouplingTerm(const TCDE_Field* field, const TCDE_Point* point,
                                float gamma, float sigma, int num_samples);

// ============================================================================
// TEMPORAL DIMENSION
// ============================================================================

/**
 * @brief Compute temporal dimension from field energy
 * @param field Field
 * @return Temporal dimension τ ∈ [1,2)
 * 
 * Formula: τ = 1 + 0.5·(1 + tanh((E - E₀)/σ_E))
 * 
 * Properties:
 * - τ = 1: No anticipation (present-focused)
 * - τ → 2: Strong anticipation (future-oriented)
 * - Emerges from field energy distribution
 */
float TCDE_TemporalDimension(const TCDE_Field* field);

/**
 * @brief Update temporal dimension based on current field state
 * @param field Field
 * 
 * Recomputes and updates field->temporal_dimension
 */
void TCDE_UpdateTemporalDimension(TCDE_Field* field);

// ============================================================================
// ADAPTIVE TIMESTEPPING
// ============================================================================

/**
 * @brief Compute adaptive timestep based on field dynamics
 * @param field Field
 * @param params Current parameters
 * @param error_tolerance Target error tolerance
 * @return Suggested timestep
 * 
 * Uses embedded Runge-Kutta method to estimate local truncation error
 * Adjusts timestep to maintain error < tolerance
 */
float TCDE_AdaptiveTimestep(const TCDE_Field* field, const TCDE_Parameters* params,
                             float error_tolerance);

/**
 * @brief Evolve with adaptive timestepping
 * @param field Field
 * @param params Base parameters (dt will be adapted)
 * @param target_time Target evolution time
 * @param error_tolerance Error tolerance
 * @return Number of steps taken
 * 
 * Automatically adjusts timestep to maintain accuracy
 */
int TCDE_EvolveAdaptiveTimestep(TCDE_Field* field, TCDE_Parameters* params,
                                 float target_time, float error_tolerance);

// ============================================================================
// PHASE 3: ADAPTIVE METRIC EVOLUTION
// ============================================================================

/**
 * @brief Evolve field by one timestep with adaptive metric
 * @param field Field to evolve
 * @param base_metric Base metric (before adaptation)
 * @param params Evolution parameters
 * 
 * Uses adaptive metric g_ij(x) = g₀_ij · f(ρ(x)) where ρ = |Φ|²
 * This creates authentic curvature that influences field dynamics.
 * 
 * The metric adapts to local energy density:
 * - High energy → contracted metric (slower dynamics)
 * - Low energy → expanded metric (faster dynamics)
 * 
 * This creates a feedback loop:
 * - Field structure → metric curvature
 * - Metric curvature → field evolution
 * - Field evolution → new structure
 */
void TCDE_EvolveStepAdaptive(TCDE_Field* field, const TCDE_Metric* base_metric,
                             const TCDE_Parameters* params);

/**
 * @brief Evolve field for multiple steps with adaptive metric
 * @param field Field to evolve
 * @param base_metric Base metric
 * @param params Evolution parameters
 * @param num_steps Number of steps
 */
void TCDE_EvolveAdaptive(TCDE_Field* field, const TCDE_Metric* base_metric,
                         const TCDE_Parameters* params, int num_steps);

// ============================================================================
// ENERGY AND CONSERVATION
// ============================================================================

/**
 * @brief Compute energy dissipation rate
 * @param field Field
 * @param params Evolution parameters
 * @return dE/dt
 * 
 * For diffusion-dominated regime (α=β=γ=0): dE/dt ≤ 0
 */
float TCDE_EnergyDissipationRate(const TCDE_Field* field, 
                                  const TCDE_Parameters* params);

/**
 * @brief Check energy conservation
 * @param field Field
 * @param initial_energy Initial energy
 * @param tolerance Relative error tolerance
 * @return true if energy conserved within tolerance
 * 
 * Verifies: |E_current - E_initial| / E_initial < tolerance
 */
bool TCDE_CheckEnergyConservation(const TCDE_Field* field, float initial_energy,
                                   float tolerance);

// ============================================================================
// FRACTAL INITIALIZATION
// ============================================================================

/**
 * @brief Initialize field with fractal structure
 * @param field Field
 * @param amplitude Base amplitude
 * @param scale_factor Scale reduction factor (typically 0.5)
 * @param max_depth Maximum recursion depth
 * 
 * Creates self-similar structure across scales:
 * - Recursive subdivision
 * - Power-law amplitude decay: A_k ∝ scale^(-df/2)
 * - Space-filling distribution (Halton/Sobol sequences)
 * 
 * Target fractal dimension: field->fractal_dimension
 */
void TCDE_InitializeFractal(TCDE_Field* field, float amplitude, 
                             float scale_factor, int max_depth);

/**
 * @brief Verify fractal dimension of field
 * @param field Field
 * @return Measured fractal dimension
 * 
 * Uses box-counting method to measure Hausdorff dimension
 * Should match field->fractal_dimension within ±0.1
 */
float TCDE_VerifyFractalDimension(const TCDE_Field* field);

// ============================================================================
// ADAPTIVE MESH REFINEMENT
// ============================================================================

/**
 * @brief Adapt mesh based on field gradient
 * @param field Field
 * @param theta_refine Refinement threshold
 * @param theta_coarsen Coarsening threshold
 * 
 * Strategy:
 * - Refine: Add centers where ||∇Φ|| > theta_refine
 * - Coarsen: Remove centers where ||∇Φ|| < theta_coarsen
 * 
 * Maintains:
 * - num_centers ≤ capacity
 * - Field continuity
 * - Energy conservation
 */
void TCDE_AdaptMesh(TCDE_Field* field, float theta_refine, float theta_coarsen);

/**
 * @brief Compute mesh quality metrics
 * @param field Field
 * @param uniformity Output: spacing uniformity [0,1]
 * @param coverage Output: domain coverage [0,1]
 * @param efficiency Output: centers per unit error [0,1]
 * 
 * Quality metrics for mesh assessment
 */
void TCDE_MeshQuality(const TCDE_Field* field, float* uniformity, 
                      float* coverage, float* efficiency);

#ifdef __cplusplus
}
#endif

#endif // TCDE_EVOLUTION_H
