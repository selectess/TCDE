/**
 * @file tcde.h
 * @brief TCDE - Topological Cognitive Diffusive Emergence (Main Header)
 * 
 * Complete unified implementation with modular architecture:
 * - Core: Data structures and basic operations
 * - Geometry: Differential geometry on Riemannian manifolds
 * - Evolution: TDE integration and dynamics
 * - Multimodal: Multi-modal processing pipelines
 * - Validation: Comprehensive testing framework
 * 
 * @version 2.0
 * @date January 17, 2025
 */

#ifndef TCDE_H
#define TCDE_H

// ============================================================================
// VERSION INFORMATION
// ============================================================================

#define TCDE_VERSION_MAJOR 2
#define TCDE_VERSION_MINOR 0
#define TCDE_VERSION_PATCH 0
#define TCDE_VERSION_STRING "2.0.0"

// ============================================================================
// MODULE HEADERS
// ============================================================================

#include "tcde_core.h"
#include "tcde_geometry.h"
#include "tcde_evolution.h"
#include "../utils/tcde_multimodal.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

/**
 * @brief Check TCDE version compatibility
 * @param major Required major version
 * @param minor Required minor version
 * @return true if compatible
 */
#define TCDE_VERSION_CHECK(major, minor) \
    ((TCDE_VERSION_MAJOR > (major)) || \
     (TCDE_VERSION_MAJOR == (major) && TCDE_VERSION_MINOR >= (minor)))

/**
 * @brief Get TCDE version as integer
 */
#define TCDE_VERSION_INT \
    ((TCDE_VERSION_MAJOR * 10000) + (TCDE_VERSION_MINOR * 100) + TCDE_VERSION_PATCH)

// ============================================================================
// QUICK START API
// ============================================================================

/**
 * @brief Initialize TCDE system
 * @return true on success
 * 
 * Initializes:
 * - Random number generator
 * - Default parameters
 * - Memory pools (if enabled)
 */
bool TCDE_Initialize(void);

/**
 * @brief Shutdown TCDE system
 * 
 * Cleans up:
 * - Memory pools
 * - Cached data
 * - Resources
 */
void TCDE_Shutdown(void);

/**
 * @brief Create default field configuration
 * @param capacity Number of centers
 * @return Configured field
 * 
 * Creates field with:
 * - Fractal dimension: 2.5
 * - Unified topology (2D ⊂ 6D)
 * - Default projection (XY plane)
 * - Gaussian RBF kernel
 */
TCDE_Field* TCDE_CreateDefaultField(int capacity);

/**
 * @brief Create default evolution parameters
 * @return Default parameters
 * 
 * Returns parameters suitable for most applications:
 * - dt: 0.01
 * - D: 0.1
 * - alpha: 0.05
 * - beta: 0.02
 * - gamma: 0.03
 * - sigma: 0.15
 */
TCDE_Parameters TCDE_GetDefaultParameters(void);

/**
 * @brief Create default modality relations
 * @return Default relations (5 modalities)
 * 
 * Creates relations for:
 * - Visual (m=0.0)
 * - Auditory (m=0.2)
 * - Semantic (m=0.4)
 * - Motor (m=0.6)
 * - Emotional (m=0.8)
 */
TCDE_ModalityRelations* TCDE_CreateDefaultModalities(void);

// ============================================================================
// DOCUMENTATION
// ============================================================================

/**
 * @mainpage TCDE - Topological Cognitive Diffusive Emergence
 * 
 * @section intro_sec Introduction
 * 
 * TCDE is a AI paradigm based on continuous field dynamics
 * on curved manifolds. Intelligence emerges from topological dynamics
 * rather than discrete computation.
 * 
 * @section features_sec Key Features
 * 
 * - **Continuous Representation**: No discrete tokenization, C²-differentiable field
 * - **Bi-Temporal Dimension**: τ ∈ [1,2) enables natural anticipation
 * - **Fractal Geometry**: Scale-invariant processing (df ∈ [2,3))
 * - **Multimodal Unity**: All modalities as resonance modes in unified field
 * - **Topological Dynamics**: Torsion and coupling enable creativity and coherence
 * 
 * @section modules_sec Modules
 * 
 * - @ref tcde_core.h "Core Module": Data structures and basic operations
 * - @ref tcde_geometry.h "Geometry Module": Differential geometry operations
 * - @ref tcde_evolution.h "Evolution Module": TDE integration and dynamics
 * - @ref tcde_multimodal.h "Multimodal Module": Multi-modal processing
 * 
 * @section quickstart_sec Quick Start
 * 
 * @code
 * // Initialize TCDE
 * TCDE_Initialize();
 * 
 * // Create field
 * TCDE_Field* field = TCDE_CreateDefaultField(1000);
 * 
 * // Initialize with fractal structure
 * TCDE_InitializeFractal(field, 1.0f, 0.5f, 5);
 * 
 * // Create modality relations
 * TCDE_ModalityRelations* relations = TCDE_CreateDefaultModalities();
 * 
 * // Process multimodal input
 * TCDE_ProcessText(field, relations, "Hello world", 1.0f);
 * 
 * // Evolve field
 * TCDE_Parameters params = TCDE_GetDefaultParameters();
 * for (int i = 0; i < 100; i++) {
 *     TCDE_EvolveStep(field, &params);
 * }
 * 
 * // Validate
 * TCDE_ValidationResults results = TCDE_Validate(field, relations);
 * TCDE_PrintValidationResults(&results);
 * 
 * // Cleanup
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * TCDE_Shutdown();
 * @endcode
 * 
 * @section theory_sec Theoretical Foundation
 * 
 * TCDE is based on the Topological Diffusion Equation (TDE):
 * 
 * ∂Φ/∂t = D∇²_g Φ - α|Φ|²Φ + β·T(Φ) + γ·C(Φ)
 * 
 * Where:
 * - D∇²_g Φ: Diffusion (spreading on curved manifold)
 * - -α|Φ|²Φ: Nonlinearity (saturation)
 * - β·T(Φ): Topological torsion (intuition/creativity)
 * - γ·C(Φ): Contextual coupling (semantic coherence)
 * 
 * @section references_sec References
 * 
 * See `.kiro/specs/` for complete theoretical documentation:
 * - 02_THEORETICAL_FOUNDATIONS.md
 * - 03_MATHEMATICAL_FORMALISM.md
 * - 04_ALGORITHMIC_SPECIFICATIONS.md
 * - 07_VALIDATION_FRAMEWORK.md
 * 
 * @section license_sec License
 * 
 * [Specify license here]
 */

// ============================================================================
// VALIDATION FUNCTIONS (Advanced)
// ============================================================================

/**
 * @brief Rigorous fractal dimension measurement using box-counting
 * @param field TCDE field
 * @param num_scales Number of scales (minimum 10)
 * @param r_min Minimum radius
 * @param r_max Maximum radius
 * @return Fractal dimension d_f
 * 
 * @complexity O(num_scales * num_centers * grid_size³)
 */
float TCDE_MeasureFractalDimensionRigorous(const TCDE_Field* field,
                                           int num_scales,
                                           float r_min,
                                           float r_max);

/**
 * @brief Measure correlation dimension using Grassberger-Procaccia method
 * @param field TCDE field
 * @return Correlation dimension d_c
 * 
 * @complexity O(N² * num_radii)
 */
float TCDE_MeasureCorrelationDimension(const TCDE_Field* field);

/**
 * @brief Measure temporal anticipation capability
 * @param field TCDE field
 * @param delta_t Time horizon for prediction
 * @return Anticipation score [0,1]
 * 
 * @complexity O(num_samples * evaluation_cost)
 */
float TCDE_MeasureTemporalAnticipation(const TCDE_Field* field, float delta_t);

/**
 * @brief Measure real cross-modal coherence
 * @param field TCDE field
 * @return Cross-modal coherence [0,1]
 * 
 * @complexity O(num_modalities² * num_samples)
 */
float TCDE_MeasureCrossModalCoherenceReal(const TCDE_Field* field);

// ============================================================================
// EVOLUTION FUNCTIONS (Parallel)
// ============================================================================

/**
 * @brief Parallel evolution step using OpenMP
 * @param field TCDE field
 * @param dt Time step
 * 
 * Parallelizes computation of evolution derivatives across centers.
 * Requires OpenMP support at compile time.
 * 
 * @complexity O(num_centers / num_threads)
 */
void TCDE_EvolveStep_Parallel(TCDE_Field* field, float dt);

#ifdef __cplusplus
}
#endif

#endif // TCDE_H
