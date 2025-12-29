/**
 * @file tcde_emergent_dimensions.h
 * @brief TCDE Emergent Dimensions Module - Dynamic dimension expansion
 *
 * Enables ASI-level capability: Dimensions emerge from field complexity
 * rather than being fixed. The system can grow from 6D to N-D based on
 * cognitive demands and field evolution.
 *
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_EMERGENT_DIMENSIONS_H
#define TCDE_EMERGENT_DIMENSIONS_H

#include "tcde_core.h"
#include "../utils/tcde_measures.h"
#include "../utils/tcde_multimodal.h"
#include "tcde_evolution.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// EMERGENT DIMENSION STRUCTURES
// ============================================================================

/**
 * @brief Dimension emergence criteria
 */
typedef struct {
    float complexity_threshold;     // Field complexity triggering emergence
    float energy_threshold;         // Energy density threshold
    float correlation_threshold;    // Cross-modal correlation threshold
    float fractal_dimension_min;    // Minimum fractal dimension for emergence
    int min_centers;               // Minimum centers before emergence
} TCDE_EmergenceCriteria;

/**
 * @brief Emergent dimension properties
 */
typedef struct {
    char* name;                    // Dimension semantic name
    char* description;             // Human-readable description
    float emergence_time;          // When dimension emerged
    float importance_weight;       // Cognitive importance [0,1]
    float stability_score;         // Dimension stability [0,1]
    bool is_active;               // Currently used
} TCDE_DimensionInfo;

/**
 * @brief Dynamic dimension manager
 */
typedef struct {
    int base_dimensions;           // Original dimensions (6)
    int total_dimensions;          // Current total dimensions
    int max_dimensions;            // Maximum allowed dimensions
    TCDE_DimensionInfo* dimensions; // Array of dimension info
    TCDE_EmergenceCriteria criteria; // Emergence triggers
    float* dimension_weights;      // Relative importance weights
} TCDE_DimensionManager;

/**
 * @brief Emergent field with dynamic dimensions
 */
typedef struct {
    TCDE_Field* base_field;        // Original 6D field
    TCDE_DimensionManager* dim_manager; // Dimension manager
    float** emergent_coords;       // Additional coordinates [center][dim]
    TCDE_Complex** emergent_coeffs; // Additional coefficients
    float emergence_energy;        // Energy invested in emergence
    bool emergence_active;         // Emergence process active
} TCDE_EmergentField;

// ============================================================================
// DIMENSION EMERGENCE API
// ============================================================================

/**
 * @brief Create dimension manager
 * @param max_dims Maximum dimensions allowed
 * @return Initialized dimension manager
 */
TCDE_DimensionManager* TCDE_CreateDimensionManager(int max_dims);

/**
 * @brief Destroy dimension manager
 * @param manager Manager to destroy
 */
void TCDE_DestroyDimensionManager(TCDE_DimensionManager* manager);

/**
 * @brief Check if field complexity requires dimension emergence
 * @param field Base field to analyze
 * @param manager Dimension manager
 * @return True if emergence criteria met
 */
bool TCDE_CheckEmergenceCriteria(const TCDE_Field* field,
                                const TCDE_DimensionManager* manager);

/**
 * @brief Emerge new dimension based on field patterns
 * @param manager Dimension manager
 * @param field Base field
 * @param name Semantic name for new dimension
 * @param description Human description
 * @return True on successful emergence
 */
bool TCDE_EmergeDimension(TCDE_DimensionManager* manager,
                         const TCDE_Field* field,
                         const char* name,
                         const char* description);

/**
 * @brief Create emergent field from base field
 * @param base_field Original 6D field
 * @param max_dims Maximum dimensions
 * @return Emergent field with dynamic dimensions
 */
TCDE_EmergentField* TCDE_CreateEmergentField(TCDE_Field* base_field, int max_dims);

/**
 * @brief Destroy emergent field
 * @param field Emergent field to destroy
 */
void TCDE_DestroyEmergentField(TCDE_EmergentField* field);

/**
 * @brief Evaluate emergent field at point
 * @param field Emergent field
 * @param point Evaluation point (base dimensions)
 * @return Complex field value
 */
TCDE_Complex TCDE_EvaluateEmergent(const TCDE_EmergentField* field,
                                  const TCDE_Point* point);

/**
 * @brief Evolve emergent field with dimension emergence
 * @param field Emergent field
 * @param params Evolution parameters
 * @return True if evolution successful
 */
bool TCDE_EvolveEmergent(TCDE_EmergentField* field,
                        const TCDE_Parameters* params);

/**
 * @brief Get current dimension count
 * @param field Emergent field
 * @return Current total dimensions
 */
int TCDE_GetEmergentDimensions(const TCDE_EmergentField* field);

/**
 * @brief Print dimension emergence status
 * @param field Emergent field
 */
void TCDE_PrintEmergentStatus(const TCDE_EmergentField* field);

#ifdef __cplusplus
}
#endif

#endif // TCDE_EMERGENT_DIMENSIONS_H
