/**
 * @file tcde_adaptive_metric.h
 * @brief Adaptive metric for TCDE (Pure Theory)
 * 
 * Implements field-dependent metric and coupled evolution
 * Pure TCDE theory from docs/implementation_Model.md
 */

#ifndef TCDE_ADAPTIVE_METRIC_H
#define TCDE_ADAPTIVE_METRIC_H

#include "../core/tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// ADAPTIVE METRIC FUNCTIONS
// ============================================================================

/**
 * @brief Update metric based on field amplitude
 * 
 * Theory: g_ij(Φ) = g_ij^0 + α·|Φ|²·δ_ij
 * 
 * @param field Field to update metric for
 * 
 * The metric adapts to field amplitude, creating dynamic geometry.
 * This is a core feature of authentic TCDE theory.
 */
void TCDE_UpdateAdaptiveMetric(TCDE_Field* field);

/**
 * @brief Evolve metric coupled with field
 * 
 * Theory: ∂g_ij/∂t = κ·(T_ij - ⟨T⟩g_ij)
 * 
 * @param field Field with metric to evolve
 * @param dt Time step
 * 
 * The metric evolves based on energy-momentum tensor,
 * creating geometric feedback with the field.
 */
void TCDE_EvolveMetricCoupled(TCDE_Field* field, float dt);

/**
 * @brief Evolve field and metric together (coupled system)
 * 
 * @param field Field to evolve
 * @param dt Total time step
 * @param num_steps Number of sub-steps
 * 
 * Authentic TCDE evolution where field and geometry
 * influence each other dynamically.
 */
void TCDE_EvolveCoupledSystem(TCDE_Field* field, float dt, int num_steps);

/**
 * @brief Compute metric derivative
 * 
 * Theory: ∂g_ij/∂x^k for adaptive metric
 * 
 * @param field Field
 * @param point Point to evaluate at
 * @param i First metric index
 * @param j Second metric index
 * @param k Derivative index
 * @return Metric derivative value
 * 
 * For adaptive metric, derivatives are non-zero.
 */
float TCDE_ComputeMetricDerivative(const TCDE_Field* field,
                                   const TCDE_Point* point,
                                   int i, int j, int k);

/**
 * @brief Get metric component at a point
 * 
 * @param field Field
 * @param point Point to evaluate at
 * @param i First index
 * @param j Second index
 * @return Metric component g_ij(point)
 * 
 * For adaptive metric, this depends on field amplitude.
 */
float TCDE_GetMetricComponent(const TCDE_Field* field,
                              const TCDE_Point* point,
                              int i, int j);

#ifdef __cplusplus
}
#endif

#endif // TCDE_ADAPTIVE_METRIC_H
