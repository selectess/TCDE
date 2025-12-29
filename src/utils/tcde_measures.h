/**
 * @file tcde_measures.h
 * @brief Real measurements for TCDE properties
 * 
 * Pure TCDE theory - measurements instead of parameters
 */

#ifndef TCDE_MEASURES_H
#define TCDE_MEASURES_H

#include "../core/tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Measure fractal dimension using box-counting
 * 
 * Theory: N(r) ~ r^(-d_f)
 * 
 * @param field Field to measure
 * @return Measured fractal dimension [2, 3)
 * 
 * This is a REAL measurement, not a stored parameter.
 */
float TCDE_MeasureFractalDimension(const TCDE_Field* field);

/**
 * @brief Measure correlation dimension
 * 
 * Theory: C(r) ~ r^(d_c)
 * 
 * @param field Field to measure
 * @return Measured correlation dimension
 * 
 * Uses Grassberger-Procaccia method with log-log regression.
 */
float TCDE_MeasureCorrelationDimension(const TCDE_Field* field);

/**
 * @brief Measure cross-modal coherence
 * 
 * @param field Field to measure
 * @return Coherence score [0, 1]
 * 
 * Measures correlation between field values at different m.
 * This is a REAL measurement, not a fixed value.
 */
float TCDE_MeasureCrossModalCoherence(const TCDE_Field* field);

/**
 * @brief Measure anticipation capability
 * 
 * @param field Field to measure
 * @param delta_t Prediction time horizon
 * @return Anticipation score [0, 1]
 * 
 * Measures ability to predict future state.
 * This is a REAL measurement based on prediction accuracy.
 */
float TCDE_MeasureAnticipation(TCDE_Field* field, float delta_t);

#ifdef __cplusplus
}
#endif

#endif // TCDE_MEASURES_H
