/**
 * @file tcde_emergent_dimensions.c
 * @brief TCDE Emergent Dimensions Implementation
 *
 * Implements dynamic dimension emergence from field complexity.
 * Dimensions emerge naturally when field complexity exceeds thresholds.
 */

#include "tcde_emergent_dimensions.h"
#include "../utils/tcde_measures.h"
#include "../utils/tcde_multimodal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute field complexity metric
 */
static float compute_field_complexity(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers < 5) return 0.0f;

    // Complexity = fractal_dimension * energy_density * center_interactions
    float fractal_dim = TCDE_MeasureFractalDimension((TCDE_Field*)field);
    float energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    float density = energy / (float)field->manifold_6d.num_centers;

    // Interaction metric: average coefficient magnitude correlations
    float interaction_metric = 0.0f;
    int pair_count = 0;

    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        for (int j = i + 1; j < field->manifold_6d.num_centers; j++) {
            TCDE_Complex c1 = field->manifold_6d.centers[i].coeff;
            TCDE_Complex c2 = field->manifold_6d.centers[j].coeff;
            float correlation = crealf(c1) * crealf(c2) + cimagf(c1) * cimagf(c2);
            interaction_metric += fabsf(correlation);
            pair_count++;
        }
    }

    if (pair_count > 0) {
        interaction_metric /= pair_count;
    }

    return fractal_dim * density * interaction_metric;
}

/**
 * @brief Compute energy density gradient
 */
__attribute__((unused))
static float compute_energy_density_gradient(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers < 3) return 0.0f;

    // Sample energy at different points to compute gradient
    const int samples = 10;
    float energies[10];  // Fixed size to avoid VLA warning
    float max_gradient = 0.0f;

    for (int i = 0; i < samples; i++) {
        float coords[6] = {
            (float)i / (samples - 1), 0.5f, 0.5f, 1.0f, 0.0f, 0.4f
        };
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex value = TCDE_Evaluate6D(field, &point);
        energies[i] = cabsf(value) * cabsf(value);
        TCDE_FreePoint(&point);
    }

    // Compute maximum gradient
    for (int i = 1; i < samples; i++) {
        float gradient = fabsf(energies[i] - energies[i-1]);
        if (gradient > max_gradient) max_gradient = gradient;
    }

    return max_gradient;
}

/**
 * @brief Analyze field patterns for dimension emergence
 */
static void analyze_field_patterns(const TCDE_Field* field,
                                  float* pattern_complexity,
                                  float* symmetry_breaking) {
    *pattern_complexity = 0.0f;
    *symmetry_breaking = 0.0f;

    if (!field || field->manifold_6d.num_centers < 3) return;

    // Analyze spatial distribution patterns
    float spatial_variance[3] = {0.0f};
    float center_coords[3] = {0.0f};

    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        for (int d = 0; d < 3; d++) {
            float coord = field->manifold_6d.centers[i].point.coords[d];
            center_coords[d] += coord;
        }
    }

    for (int d = 0; d < 3; d++) {
        center_coords[d] /= field->manifold_6d.num_centers;
    }

    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        for (int d = 0; d < 3; d++) {
            float diff = field->manifold_6d.centers[i].point.coords[d] - center_coords[d];
            spatial_variance[d] += diff * diff;
        }
    }

    // Pattern complexity from spatial distribution anisotropy
    float total_variance = spatial_variance[0] + spatial_variance[1] + spatial_variance[2];
    if (total_variance > 0) {
        for (int d = 0; d < 3; d++) {
            float anisotropy = spatial_variance[d] / total_variance;
            *pattern_complexity += anisotropy * logf(anisotropy + 1e-10f);
        }
        *pattern_complexity = -*pattern_complexity; // Convert to positive
    }

    // Symmetry breaking from temporal and modal distributions
    float temporal_spread = 0.0f;
    float modal_diversity = 0.0f;

    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float tau1 = field->manifold_6d.centers[i].point.coords[3];
        float tau2 = field->manifold_6d.centers[i].point.coords[4];
        float mod = field->manifold_6d.centers[i].point.coords[5];

        temporal_spread += tau1 * tau1 + tau2 * tau2;
        modal_diversity += mod * mod * (1.0f - mod) * (1.0f - mod); // Peaks at 0.5
    }

    temporal_spread /= field->manifold_6d.num_centers;
    modal_diversity /= field->manifold_6d.num_centers;

    *symmetry_breaking = sqrtf(temporal_spread) + modal_diversity;
}

// ============================================================================
// DIMENSION MANAGER IMPLEMENTATION
// ============================================================================

TCDE_DimensionManager* TCDE_CreateDimensionManager(int max_dims) {
    if (max_dims <= 6) return NULL;

    TCDE_DimensionManager* manager = (TCDE_DimensionManager*)malloc(sizeof(TCDE_DimensionManager));
    if (!manager) return NULL;

    manager->base_dimensions = 6;
    manager->total_dimensions = 6;
    manager->max_dimensions = max_dims;

    manager->dimensions = (TCDE_DimensionInfo*)calloc(max_dims, sizeof(TCDE_DimensionInfo));
    if (!manager->dimensions) {
        free(manager);
        return NULL;
    }

    manager->dimension_weights = (float*)calloc(max_dims, sizeof(float));
    if (!manager->dimension_weights) {
        free(manager->dimensions);
        free(manager);
        return NULL;
    }

    // Initialize base dimensions
    const char* base_names[6] = {"spatial_x", "spatial_y", "spatial_z", "temporal_1", "temporal_2", "modal"};
    const char* base_desc[6] = {
        "Spatial X coordinate", "Spatial Y coordinate", "Spatial Z coordinate",
        "Primary temporal dimension", "Secondary temporal dimension", "Modality dimension"
    };

    for (int i = 0; i < 6; i++) {
        manager->dimensions[i].name = strdup(base_names[i]);
        manager->dimensions[i].description = strdup(base_desc[i]);
        manager->dimensions[i].emergence_time = 0.0f; // Base dimensions
        manager->dimensions[i].importance_weight = 1.0f;
        manager->dimensions[i].stability_score = 1.0f;
        manager->dimensions[i].is_active = true;
        manager->dimension_weights[i] = 1.0f;
    }

    // Set default emergence criteria
    manager->criteria.complexity_threshold = 2.5f;
    manager->criteria.energy_threshold = 10.0f;
    manager->criteria.correlation_threshold = 0.3f;
    manager->criteria.fractal_dimension_min = 2.2f;
    manager->criteria.min_centers = 20;

    return manager;
}

void TCDE_DestroyDimensionManager(TCDE_DimensionManager* manager) {
    if (!manager) return;

    if (manager->dimensions) {
        for (int i = 0; i < manager->max_dimensions; i++) {
            free(manager->dimensions[i].name);
            free(manager->dimensions[i].description);
        }
        free(manager->dimensions);
    }

    free(manager->dimension_weights);
    free(manager);
}

bool TCDE_CheckEmergenceCriteria(const TCDE_Field* field,
                                const TCDE_DimensionManager* manager) {
    if (!field || !manager) return false;
    if (manager->total_dimensions >= manager->max_dimensions) return false;

    // Check basic criteria
    if (field->manifold_6d.num_centers < manager->criteria.min_centers) return false;

    float fractal_dim = TCDE_MeasureFractalDimension((TCDE_Field*)field);
    if (fractal_dim < manager->criteria.fractal_dimension_min) return false;

    float energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    if (energy < manager->criteria.energy_threshold) return false;

    // Check complexity
    float complexity = compute_field_complexity(field);
    if (complexity < manager->criteria.complexity_threshold) return false;

    // Check cross-modal coherence as additional criterion
    float coherence = TCDE_MeasureCrossModalCoherence((TCDE_Field*)field);
    if (coherence < manager->criteria.correlation_threshold) return false;

    return true;
}

bool TCDE_EmergeDimension(TCDE_DimensionManager* manager,
                         const TCDE_Field* field,
                         const char* name,
                         const char* description) {
    if (!manager || !field || !name || !description) return false;
    if (manager->total_dimensions >= manager->max_dimensions) return false;

    // Analyze field patterns to determine dimension properties
    float pattern_complexity, symmetry_breaking;
    analyze_field_patterns(field, &pattern_complexity, &symmetry_breaking);

    // Compute importance weight based on field analysis
    float importance = 0.5f + 0.3f * pattern_complexity + 0.2f * symmetry_breaking;
    importance = fminf(1.0f, importance);

    // Compute stability score (emergent dimensions start less stable)
    float stability = 0.3f + 0.4f * importance;

    int new_dim_idx = manager->total_dimensions;
    manager->dimensions[new_dim_idx].name = strdup(name);
    manager->dimensions[new_dim_idx].description = strdup(description);
    manager->dimensions[new_dim_idx].emergence_time = field->time;
    manager->dimensions[new_dim_idx].importance_weight = importance;
    manager->dimensions[new_dim_idx].stability_score = stability;
    manager->dimensions[new_dim_idx].is_active = true;

    manager->dimension_weights[new_dim_idx] = importance;
    manager->total_dimensions++;

    printf("EMERGENT DIMENSION: '%s' emerged at time %.2f\n", name, field->time);
    printf("  Importance: %.2f, Stability: %.2f\n", importance, stability);
    printf("  Total dimensions: %d/%d\n", manager->total_dimensions, manager->max_dimensions);

    return true;
}

// ============================================================================
// EMERGENT FIELD IMPLEMENTATION
// ============================================================================

TCDE_EmergentField* TCDE_CreateEmergentField(TCDE_Field* base_field, int max_dims) {
    if (!base_field || max_dims <= 6) return NULL;

    TCDE_EmergentField* emergent = (TCDE_EmergentField*)malloc(sizeof(TCDE_EmergentField));
    if (!emergent) return NULL;

    emergent->base_field = base_field;
    emergent->dim_manager = TCDE_CreateDimensionManager(max_dims);
    if (!emergent->dim_manager) {
        free(emergent);
        return NULL;
    }

    int capacity = base_field->manifold_6d.capacity;
    (void)max_dims;  // Reserved for future use

    // Allocate emergent coordinates and coefficients
    emergent->emergent_coords = (float**)malloc(capacity * sizeof(float*));
    emergent->emergent_coeffs = (TCDE_Complex**)malloc(capacity * sizeof(TCDE_Complex*));

    if (!emergent->emergent_coords || !emergent->emergent_coeffs) {
        TCDE_DestroyDimensionManager(emergent->dim_manager);
        free(emergent->emergent_coords);
        free(emergent->emergent_coeffs);
        free(emergent);
        return NULL;
    }

    // Initialize to NULL (dimensions emerge as needed)
    for (int i = 0; i < capacity; i++) {
        emergent->emergent_coords[i] = NULL;
        emergent->emergent_coeffs[i] = NULL;
    }

    emergent->emergence_energy = 0.0f;
    emergent->emergence_active = false;

    return emergent;
}

void TCDE_DestroyEmergentField(TCDE_EmergentField* field) {
    if (!field) return;

    if (field->emergent_coords) {
        for (int i = 0; i < field->base_field->manifold_6d.capacity; i++) {
            free(field->emergent_coords[i]);
        }
        free(field->emergent_coords);
    }

    if (field->emergent_coeffs) {
        for (int i = 0; i < field->base_field->manifold_6d.capacity; i++) {
            free(field->emergent_coeffs[i]);
        }
        free(field->emergent_coeffs);
    }

    TCDE_DestroyDimensionManager(field->dim_manager);
    free(field);
}

TCDE_Complex TCDE_EvaluateEmergent(const TCDE_EmergentField* field,
                                  const TCDE_Point* point) {
    if (!field || !point) return 0.0f + 0.0f * I;

    // Base evaluation from 6D field
    TCDE_Complex base_value = TCDE_Evaluate6D(field->base_field, point);

    // Add emergent dimension contributions
    TCDE_Complex emergent_contrib = 0.0f + 0.0f * I;

    int num_centers = field->base_field->manifold_6d.num_centers;
    for (int i = 0; i < num_centers; i++) {
        if (field->emergent_coords[i] && field->emergent_coeffs[i]) {
            // Compute emergent dimension contribution
            float emergent_weight = 0.0f;
            int emergent_dims = field->dim_manager->total_dimensions - 6;

            for (int d = 0; d < emergent_dims; d++) {
                float coord_diff = field->emergent_coords[i][d] - point->coords[d % point->dimension];
                emergent_weight += field->dim_manager->dimension_weights[6 + d] *
                                 expf(-coord_diff * coord_diff);
            }

            emergent_contrib += emergent_weight * field->emergent_coeffs[i][0];
        }
    }

    return base_value + emergent_contrib;
}

bool TCDE_EvolveEmergent(TCDE_EmergentField* field,
                        const TCDE_Parameters* params) {
    if (!field || !params) return false;

    // First, evolve the base field
    TCDE_EvolveStep(field->base_field, params);

    // Check for dimension emergence
    if (TCDE_CheckEmergenceCriteria(field->base_field, field->dim_manager)) {
        // Try to emerge a new dimension
        char dim_name[64];
        char dim_desc[256];

        // Generate dimension name based on field analysis
        float pattern_complexity, symmetry_breaking;
        analyze_field_patterns(field->base_field, &pattern_complexity, &symmetry_breaking);

        if (symmetry_breaking > pattern_complexity) {
            sprintf(dim_name, "temporal_%d", field->dim_manager->total_dimensions);
            sprintf(dim_desc, "Emergent temporal dimension from symmetry breaking (t=%.2f)",
                   field->base_field->time);
        } else {
            sprintf(dim_name, "cognitive_%d", field->dim_manager->total_dimensions);
            sprintf(dim_desc, "Emergent cognitive dimension from pattern complexity (t=%.2f)",
                   field->base_field->time);
        }

        TCDE_EmergeDimension(field->dim_manager, field->base_field, dim_name, dim_desc);

        // Allocate storage for new dimension if emergence successful
        int new_dim_idx = field->dim_manager->total_dimensions - 1;
        if (new_dim_idx > 6) {
            int capacity = field->base_field->manifold_6d.capacity;
            int extra_dims = new_dim_idx - 6 + 1;

            for (int i = 0; i < capacity; i++) {
                // Reallocate emergent coordinates
                float* new_coords = (float*)realloc(field->emergent_coords[i],
                                                   extra_dims * sizeof(float));
                if (new_coords) {
                    field->emergent_coords[i] = new_coords;
                    // Initialize new coordinate with some noise
                    new_coords[extra_dims - 1] = 0.5f + 0.1f * ((float)rand() / RAND_MAX - 0.5f);
                }

                // Reallocate emergent coefficients
                TCDE_Complex* new_coeffs = (TCDE_Complex*)realloc(field->emergent_coeffs[i],
                                                                extra_dims * sizeof(TCDE_Complex));
                if (new_coeffs) {
                    field->emergent_coeffs[i] = new_coeffs;
                    // Initialize new coefficient
                    new_coeffs[extra_dims - 1] = 0.1f * cexpf(I * 2.0f * M_PI * (float)rand() / RAND_MAX);
                }
            }
        }
    }

    return true;
}

int TCDE_GetEmergentDimensions(const TCDE_EmergentField* field) {
    return field ? field->dim_manager->total_dimensions : 6;
}

void TCDE_PrintEmergentStatus(const TCDE_EmergentField* field) {
    if (!field) return;

    printf("\n=== EMERGENT FIELD STATUS ===\n");
    printf("Base dimensions: %d\n", field->dim_manager->base_dimensions);
    printf("Total dimensions: %d/%d\n", field->dim_manager->total_dimensions,
           field->dim_manager->max_dimensions);
    printf("Emergence active: %s\n", field->emergence_active ? "YES" : "NO");
    printf("Emergence energy: %.2f\n", field->emergence_energy);

    printf("\nDimension List:\n");
    for (int i = 0; i < field->dim_manager->total_dimensions; i++) {
        TCDE_DimensionInfo* dim = &field->dim_manager->dimensions[i];
        printf("  %d: %-15s | Weight: %.2f | Stability: %.2f | %s\n",
               i, dim->name, dim->importance_weight, dim->stability_score,
               dim->is_active ? "ACTIVE" : "INACTIVE");
        if (i >= 6) {
            printf("      Emerged at t=%.2f: %s\n", dim->emergence_time, dim->description);
        }
    }
    printf("\n");
}
