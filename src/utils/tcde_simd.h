/**
 * @file tcde_simd.h
 * @brief TCDE SIMD Optimizations Header
 */

#ifndef TCDE_SIMD_H
#define TCDE_SIMD_H

#include "../core/tcde_core.h"
#include <stdbool.h>
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if CPU supports AVX2
 * @return true if AVX2 is supported
 */
bool TCDE_HasAVX2Support();

/**
 * @brief Vectorized RBF evaluation using AVX2
 * @param centers Centers array [n][6]
 * @param coeffs Coefficients array [n]
 * @param widths Width parameters [n]
 * @param n Number of centers (should be multiple of 8)
 * @param point Query point
 * @return Field value at point
 * 
 * @complexity O(n) with 4-8x speedup on AVX2-capable CPUs
 */
TCDE_Complex TCDE_EvaluateRBF_SIMD(const float centers[][6],
                                    const TCDE_Complex* coeffs,
                                    const float* widths,
                                    int n,
                                    const TCDE_Point* point);

#ifdef __cplusplus
}
#endif

#endif // TCDE_SIMD_H
