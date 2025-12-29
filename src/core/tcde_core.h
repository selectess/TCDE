/**
 * @file tcde_core.h
 * @brief TCDE Core Module - Data Structures and Basic Operations
 * 
 * Core data structures (Point, Metric, Field, Center) and basic operations
 * (create, destroy, validate, evaluate).
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_CORE_H
#define TCDE_CORE_H

#include <stdbool.h>
#include <complex.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// CORE TYPES
// ============================================================================

/**
 * @brief Complex number (using C99 complex)
 */
typedef float complex TCDE_Complex;

/**
 * @brief RBF kernel types
 */
typedef enum {
    TCDE_RBF_GAUSSIAN = 0,
    TCDE_RBF_MULTIQUADRIC = 1,
    TCDE_RBF_INVERSE_MULTIQUADRIC = 2,
    TCDE_RBF_THIN_PLATE_SPLINE = 3
} TCDE_RBFType;

/**
 * @brief Point in N-dimensional space
 */
typedef struct {
    float* coords;          // Coordinates (size = dimension)
    int dimension;          // Dimension (2 or 6)
} TCDE_Point;

/**
 * @brief Riemannian metric tensor
 */
typedef struct {
    float** g;              // Metric tensor [dim][dim]
    float** g_inv;          // Inverse metric (cached)
    float det;              // Determinant (cached)
    int dimension;          // Dimension
    bool is_valid;          // Validation flag
} TCDE_Metric;

/**
 * @brief RBF center with coefficient
 */
typedef struct {
    TCDE_Point point;       // Center location
    TCDE_Complex coeff;     // Complex coefficient
    float epsilon;          // Shape parameter
    TCDE_Metric metric;     // Local metric
} TCDE_Center;

/**
 * @brief Projection parameters for 2D slice
 */
typedef struct {
    int axis1;              // First projection axis (0-5)
    int axis2;              // Second projection axis (0-5)
    float slice_coords[4];  // Fixed coordinates for other 4 dimensions
    bool auto_update;       // Auto-update slice based on field dynamics
} TCDE_Projection2D;

/**
 * @brief Main TCDE field structure (Unified Topology)
 */
typedef struct {
    // 6D Full Manifold
    struct {
        TCDE_Center* centers;
        int num_centers;
        int capacity;
        TCDE_Metric metric;
    } manifold_6d;
    
    // 2D Projection (dynamic slice of 6D)
    struct {
        TCDE_Center* centers;  // Projected centers
        int num_centers;
        TCDE_Metric metric;
        TCDE_Projection2D projection;
    } slice_2d;
    
    // Shared properties
    TCDE_RBFType rbf_type;
    float time;
    float energy;
    bool energy_valid;
    
    // Fractal properties
    float fractal_dimension;
    float temporal_dimension;
    
    // Synchronization
    bool sync_enabled;      // Keep 2D and 6D synchronized
    float sync_strength;    // Strength of 2D→6D feedback
} TCDE_Field;

/**
 * @brief Holistic metrics for identity scoring
 */
typedef struct {
    float reflexivity;           // Φ(Φ) self-reflection
    float self_awareness;        // Self-monitoring capability
    float drc_cycle_efficiency;  // Desire-Reason-Choice cycle
    float coherence;             // Field stability
    float emergence;             // Non-programmed behaviors
    float autopoiesis;           // Self-maintenance
    float autonomy;              // Goal generation
    float creativity;            // Novel solution generation
} TCDE_HolisticMetrics;

// ============================================================================
// POINT OPERATIONS
// ============================================================================

/**
 * @brief Create a point in N-dimensional space
 * 
 * Creates a new point with the specified dimension and coordinates.
 * The coordinate array is copied internally, so the caller retains ownership
 * of the input array.
 * 
 * @param dimension Dimension of the space (2 for 2D slice, 6 for full manifold)
 * @param coords Coordinate array of size [dimension]. Must not be NULL.
 * @return Created point with allocated memory. Must be freed with TCDE_FreePoint().
 * 
 * @note For 6D points, coordinates are: [x, y, z, τ₁, τ₂, modality]
 * @note For 2D points, coordinates are: [x, y]
 * 
 * @complexity O(dimension) for memory allocation and copying
 * 
 * @see TCDE_FreePoint(), TCDE_ValidatePoint()
 * 
 * @par Example:
 * @code
 * // Create a 6D point
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * 
 * // Use the point...
 * 
 * // Free when done
 * TCDE_FreePoint(&p);
 * @endcode
 */
TCDE_Point TCDE_CreatePoint(int dimension, const float* coords);

/**
 * @brief Free point memory
 * 
 * Frees the memory allocated for a point's coordinate array.
 * After calling this function, the point should not be used.
 * 
 * @param point Pointer to point to free. Can be NULL (no-op).
 * 
 * @note This function only frees the internal coordinate array, not the point struct itself
 * @note Safe to call multiple times on the same point (idempotent)
 * @note After freeing, point->coords will be NULL and point->dimension will be 0
 * 
 * @complexity O(1)
 * 
 * @see TCDE_CreatePoint()
 * 
 * @par Example:
 * @code
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * 
 * // Use the point...
 * TCDE_Complex value = TCDE_Evaluate6D(field, &p);
 * 
 * // Free when done
 * TCDE_FreePoint(&p);  // Safe to call
 * TCDE_FreePoint(&p);  // Safe to call again (no-op)
 * @endcode
 */
void TCDE_FreePoint(TCDE_Point* point);

/**
 * @brief Validate point coordinates
 * 
 * Checks if a point has valid coordinates and dimension.
 * A point is valid if it has non-NULL coordinates and a valid dimension (2 or 6).
 * 
 * @param point Pointer to point to validate. Can be NULL.
 * @return true if point is valid (non-NULL, valid dimension, non-NULL coords), false otherwise
 * 
 * @note Returns false if point is NULL
 * @note Returns false if point->coords is NULL
 * @note Returns false if dimension is not 2 or 6
 * @note Does not check if coordinate values are in valid ranges
 * 
 * @complexity O(1)
 * 
 * @see TCDE_CreatePoint(), TCDE_FreePoint()
 * 
 * @par Example:
 * @code
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * 
 * if (TCDE_ValidatePoint(&p)) {
 *     printf("Point is valid\n");
 *     // Safe to use point
 *     TCDE_Complex value = TCDE_Evaluate6D(field, &p);
 * } else {
 *     printf("Point is invalid\n");
 * }
 * 
 * TCDE_FreePoint(&p);
 * 
 * // After freeing, point is no longer valid
 * if (!TCDE_ValidatePoint(&p)) {
 *     printf("Point is now invalid after freeing\n");
 * }
 * @endcode
 */
bool TCDE_ValidatePoint(const TCDE_Point* point);

/**
 * @brief Copy point from source to destination
 * 
 * Copies all data from source point to destination point, including
 * dimension and coordinates. If destination already has allocated coordinates,
 * they are freed first. New memory is allocated for the destination coordinates.
 * 
 * @param dest Pointer to destination point (must not be NULL)
 * @param src Pointer to source point (must not be NULL)
 * 
 * @note Both dest and src must be non-NULL
 * @note Destination's old coordinates are freed if they exist
 * @note New memory is allocated for destination coordinates
 * @note After copy, dest and src have independent memory
 * @note If src is invalid, dest is set to invalid state (dimension=0, coords=NULL)
 * 
 * @complexity O(dimension) for memory allocation and copying
 * 
 * @see TCDE_CreatePoint(), TCDE_FreePoint(), TCDE_ValidatePoint()
 * 
 * @par Example:
 * @code
 * // Create source point
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point src = TCDE_CreatePoint(6, coords);
 * 
 * // Create destination point (uninitialized)
 * TCDE_Point dest;
 * dest.coords = NULL;
 * dest.dimension = 0;
 * 
 * // Copy source to destination
 * TCDE_CopyPoint(&dest, &src);
 * 
 * // Now dest is an independent copy of src
 * printf("Source: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n",
 *        src.coords[0], src.coords[1], src.coords[2],
 *        src.coords[3], src.coords[4], src.coords[5]);
 * printf("Dest:   [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n",
 *        dest.coords[0], dest.coords[1], dest.coords[2],
 *        dest.coords[3], dest.coords[4], dest.coords[5]);
 * 
 * // Modify source - dest is unaffected
 * src.coords[0] = 0.9f;
 * printf("After modifying source, dest[0] is still: %.2f\n", dest.coords[0]);
 * 
 * // Clean up both
 * TCDE_FreePoint(&src);
 * TCDE_FreePoint(&dest);
 * @endcode
 */
void TCDE_CopyPoint(TCDE_Point* dest, const TCDE_Point* src);

// ============================================================================
// METRIC OPERATIONS
// ============================================================================

/**
 * @brief Create identity metric tensor
 * 
 * Creates a Riemannian metric tensor initialized to the identity matrix.
 * The metric tensor g_ij defines distances and angles in the manifold.
 * 
 * @param dimension Dimension of the space (2 or 6)
 * @return Created metric with g_ij = δ_ij (Kronecker delta)
 * 
 * @note The metric is initialized to identity: g_ij = 1 if i==j, 0 otherwise
 * @note The inverse metric g^ij is also computed and cached
 * @note The determinant det(g) = 1 for identity metric
 * @note The metric must be freed with TCDE_FreeMetric()
 * 
 * @complexity O(dimension²) for matrix allocation
 * 
 * @see TCDE_FreeMetric(), TCDE_UpdateMetric(), TCDE_ValidateMetric()
 * 
 * @par Example:
 * @code
 * // Create 6D identity metric
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Metric is identity: g_ij = δ_ij
 * printf("g[0][0] = %.2f (should be 1.0)\n", metric.g[0][0]);
 * printf("g[0][1] = %.2f (should be 0.0)\n", metric.g[0][1]);
 * printf("det(g) = %.2f (should be 1.0)\n", metric.det);
 * 
 * // Validate metric
 * if (TCDE_ValidateMetric(&metric)) {
 *     printf("Metric is valid (positive definite)\n");
 * }
 * 
 * // Clean up
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
TCDE_Metric TCDE_CreateMetric(int dimension);

/**
 * @brief Free metric tensor memory
 * 
 * Frees all memory allocated for a metric tensor, including the metric matrix g_ij
 * and its inverse g^ij.
 * 
 * @param metric Pointer to metric to free. Can be NULL (no-op).
 * 
 * @note Safe to call with NULL pointer
 * @note Safe to call multiple times (idempotent)
 * @note After freeing, metric->g and metric->g_inv will be NULL
 * @note Sets is_valid flag to false
 * 
 * @complexity O(dimension) for freeing matrix rows
 * 
 * @see TCDE_CreateMetric()
 * 
 * @par Example:
 * @code
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Use metric...
 * 
 * // Free when done
 * TCDE_FreeMetric(&metric);
 * TCDE_FreeMetric(&metric);  // Safe to call again
 * @endcode
 */
void TCDE_FreeMetric(TCDE_Metric* metric);

/**
 * @brief Update metric inverse and determinant
 * 
 * Recomputes the inverse metric tensor g^ij and determinant det(g) after
 * the metric tensor g_ij has been modified. This is necessary whenever
 * the metric components are changed.
 * 
 * @param metric Pointer to metric to update (must not be NULL)
 * 
 * @note Must be called after modifying metric->g components
 * @note Computes g^ij using matrix inversion
 * @note Computes det(g) using LU decomposition
 * @note Sets is_valid flag based on positive definiteness
 * @note If metric is not positive definite, is_valid is set to false
 * 
 * @complexity O(dimension³) for matrix inversion
 * 
 * @see TCDE_CreateMetric(), TCDE_ValidateMetric()
 * 
 * @par Example:
 * @code
 * // Create identity metric
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Modify metric components (e.g., add curvature)
 * metric.g[0][0] = 1.1f;  // Slightly curved in x direction
 * metric.g[1][1] = 0.9f;  // Slightly curved in y direction
 * 
 * // Update inverse and determinant
 * TCDE_UpdateMetric(&metric);
 * 
 * // Check if still valid
 * if (TCDE_ValidateMetric(&metric)) {
 *     printf("Modified metric is valid\n");
 *     printf("det(g) = %.3f\n", metric.det);
 * }
 * 
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
void TCDE_UpdateMetric(TCDE_Metric* metric);

/**
 * @brief Validate metric tensor properties
 * 
 * Checks if a metric tensor is valid for use in Riemannian geometry.
 * A valid metric must be symmetric and positive definite.
 * 
 * @param metric Pointer to metric to validate. Can be NULL.
 * @return true if metric is valid (symmetric, positive definite), false otherwise
 * 
 * @note Returns false if metric is NULL
 * @note Returns false if metric->g is NULL
 * @note Checks symmetry: g_ij = g_ji
 * @note Checks positive definiteness: all eigenvalues > 0
 * @note Uses Sylvester's criterion (leading principal minors > 0)
 * 
 * @complexity O(dimension³) for determinant computations
 * 
 * @see TCDE_CreateMetric(), TCDE_UpdateMetric()
 * 
 * @par Example:
 * @code
 * // Create metric
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Identity metric should be valid
 * if (TCDE_ValidateMetric(&metric)) {
 *     printf("Identity metric is valid\n");
 * }
 * 
 * // Try to create invalid metric
 * metric.g[0][0] = -1.0f;  // Negative diagonal element
 * TCDE_UpdateMetric(&metric);
 * 
 * if (!TCDE_ValidateMetric(&metric)) {
 *     printf("Metric with negative component is invalid\n");
 * }
 * 
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
bool TCDE_ValidateMetric(const TCDE_Metric* metric);

// ============================================================================
// FIELD OPERATIONS
// ============================================================================

/**
 * @brief Create TCDE field with unified topology
 * 
 * Creates a new TCDE field with both 6D manifold and 2D projection capabilities.
 * The field uses a unified topology where the 2D slice is a dynamic projection
 * of the 6D manifold, enabling bidirectional synchronization.
 * 
 * @param capacity Maximum number of centers the field can hold (must be > 0)
 * @param fractal_dim Fractal dimension ∈ [2,3). Controls field complexity:
 *                    - 2.0: Smooth, simple field
 *                    - 2.5: Moderate complexity (recommended)
 *                    - 2.9: High complexity, fractal-like
 * @return Pointer to created field, or NULL on error (invalid parameters or allocation failure)
 * 
 * @note The field must be destroyed with TCDE_DestroyField() to free memory
 * @note Initial field has no centers - use TCDE_AddCenter6D() to populate
 * @note Fractal dimension affects field behavior and cognitive properties
 * 
 * @complexity O(capacity) for memory allocation
 * 
 * @see TCDE_DestroyField(), TCDE_AddCenter6D(), TCDE_Evaluate6D()
 * 
 * @par Example:
 * @code
 * // Create field with capacity for 100 centers
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * if (!field) {
 *     fprintf(stderr, "Failed to create field\n");
 *     return -1;
 * }
 * 
 * // Add centers to the field
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 * TCDE_FreePoint(&p);
 * 
 * // Use the field...
 * 
 * // Clean up
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Field* TCDE_CreateField(int capacity, float fractal_dim);

/**
 * @brief Destroy field and free all memory
 * 
 * Destroys a TCDE field and frees all associated memory including:
 * - All 6D manifold centers and their metrics
 * - All 2D projection centers and their metrics
 * - Field metrics
 * - The field structure itself
 * 
 * @param field Pointer to field to destroy. Can be NULL (no-op).
 * 
 * @note After calling this function, the field pointer is invalid and must not be used
 * @note This function properly cleans up all internal structures
 * @note Safe to call with NULL pointer
 * 
 * @complexity O(N) where N is the total number of centers
 * 
 * @see TCDE_CreateField()
 * 
 * @par Example:
 * @code
 * // Create and use field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add centers and use field...
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 * TCDE_FreePoint(&p);
 * 
 * // Evaluate field...
 * 
 * // Clean up - frees everything
 * TCDE_DestroyField(field);
 * field = NULL;  // Good practice
 * @endcode
 */
void TCDE_DestroyField(TCDE_Field* field);

/**
 * @brief Add RBF center to 6D manifold
 * 
 * Adds a new Radial Basis Function (RBF) center to the field's 6D manifold.
 * The center contributes to the field value at all points based on its
 * distance from those points.
 * 
 * @param field Field to add center to (must not be NULL)
 * @param point Center location in 6D space (must be 6-dimensional)
 * @param coeff Complex coefficient determining center's amplitude and phase
 * @param epsilon Shape parameter (ε > 0) controlling RBF width:
 *                - Small ε (0.01-0.05): Narrow, localized influence
 *                - Medium ε (0.1-0.2): Moderate influence (recommended)
 *                - Large ε (0.5-1.0): Wide, global influence
 * @return true on success, false on error (NULL field, invalid point, capacity exceeded)
 * 
 * @note If field is at capacity, the function returns false
 * @note The point is copied internally - caller retains ownership
 * @note Each center has its own local metric tensor
 * @note If sync is enabled, the 2D projection is automatically updated
 * 
 * @complexity O(1) for adding the center, O(N) if sync is enabled
 * 
 * @see TCDE_RemoveCenter6D(), TCDE_Evaluate6D(), TCDE_CreatePoint()
 * 
 * @par Example:
 * @code
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add a semantic concept center
 * float coords[6] = {
 *     0.5f,  // x: spatial position
 *     0.5f,  // y: spatial position
 *     0.5f,  // z: spatial position
 *     1.0f,  // τ₁: present time
 *     0.0f,  // τ₂: no anticipation
 *     0.4f   // modality: semantic (0.0=visual, 0.2=auditory, 0.4=semantic)
 * };
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * 
 * // Add with amplitude 1.0 and moderate width
 * bool success = TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
 * if (!success) {
 *     fprintf(stderr, "Failed to add center\n");
 * }
 * 
 * TCDE_FreePoint(&p);
 * TCDE_DestroyField(field);
 * @endcode
 */
bool TCDE_AddCenter6D(TCDE_Field* field, const TCDE_Point* point, 
                      TCDE_Complex coeff, float epsilon);

/**
 * @brief Remove RBF center from 6D manifold
 * 
 * Removes a center from the field's 6D manifold by index.
 * Centers after the removed one are shifted down to fill the gap.
 * 
 * @param field Field to remove center from (must not be NULL)
 * @param index Index of center to remove (0 ≤ index < num_centers)
 * @return true on success, false on error (NULL field, invalid index)
 * 
 * @note Index must be valid: 0 ≤ index < field->manifold_6d.num_centers
 * @note Centers after removed one are shifted: center[i] becomes center[i-1]
 * @note The removed center's memory is freed
 * @note If sync is enabled, 2D projection is updated
 * @note Field energy cache is invalidated
 * 
 * @complexity O(N) where N is the number of centers (for shifting)
 * 
 * @see TCDE_AddCenter6D(), TCDE_DestroyField()
 * 
 * @par Example:
 * @code
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add 3 centers
 * for (int i = 0; i < 3; i++) {
 *     float coords[6] = {(float)i/3.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 *     TCDE_Point p = TCDE_CreatePoint(6, coords);
 *     TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 *     TCDE_FreePoint(&p);
 * }
 * 
 * printf("Centers before: %d\n", field->manifold_6d.num_centers);  // 3
 * 
 * // Remove middle center (index 1)
 * if (TCDE_RemoveCenter6D(field, 1)) {
 *     printf("Centers after: %d\n", field->manifold_6d.num_centers);  // 2
 * }
 * 
 * // Try to remove invalid index
 * if (!TCDE_RemoveCenter6D(field, 10)) {
 *     printf("Cannot remove invalid index\n");
 * }
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
bool TCDE_RemoveCenter6D(TCDE_Field* field, int index);

/**
 * @brief Evaluate field at a point in 6D manifold
 * 
 * Computes the field value at a given point by summing contributions from all
 * RBF centers using the formula:
 * 
 * Φ(x) = Σᵢ cᵢ · φ(||x - xᵢ||_g)
 * 
 * where cᵢ are complex coefficients, φ is the RBF kernel, and ||·||_g is the
 * Riemannian distance using the local metric tensor.
 * 
 * @param field Field to evaluate (must not be NULL)
 * @param point Evaluation point in 6D space (must be 6-dimensional)
 * @return Complex field value at the point
 * 
 * @note Point coordinates: [x, y, z, τ₁, τ₂, modality]
 * @note Returns 0+0i if field is NULL, point is invalid, or field has no centers
 * @note The evaluation uses the field's RBF type (Gaussian by default)
 * 
 * @complexity O(N) where N is the number of centers in the field
 * 
 * @see TCDE_Evaluate2D(), TCDE_AddCenter6D(), TCDE_CreatePoint()
 * 
 * @par Example:
 * @code
 * // Create and populate field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add a center
 * float center_coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point center = TCDE_CreatePoint(6, center_coords);
 * TCDE_AddCenter6D(field, &center, 1.0f + 0.0f*I, 0.1f);
 * TCDE_FreePoint(&center);
 * 
 * // Evaluate at a point
 * float eval_coords[6] = {0.6f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point eval_point = TCDE_CreatePoint(6, eval_coords);
 * TCDE_Complex value = TCDE_Evaluate6D(field, &eval_point);
 * 
 * printf("Field value: %.3f + %.3fi\n", crealf(value), cimagf(value));
 * printf("Magnitude: %.3f\n", cabsf(value));
 * 
 * TCDE_FreePoint(&eval_point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_Evaluate6D(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Evaluate field at a point in 2D projection
 * 
 * Evaluates the field in the 2D projection slice. The 2D slice is a dynamic
 * projection of the 6D manifold onto a 2D plane.
 * 
 * @param field Field to evaluate (must not be NULL)
 * @param point Evaluation point in 2D space (must be 2-dimensional)
 * @return Complex field value at the point in 2D projection
 * 
 * @note Point coordinates: [x, y] in 2D projection space
 * @note Returns 0+0i if field is NULL, point is invalid, or 2D slice has no centers
 * @note Uses field->slice_2d centers, not manifold_6d centers
 * @note The 2D projection is defined by field->slice_2d.projection parameters
 * 
 * @complexity O(N) where N is the number of centers in 2D slice
 * 
 * @see TCDE_Evaluate6D(), TCDE_AddCenter6D()
 * 
 * @par Example:
 * @code
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add centers to 6D manifold
 * float coords_6d[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p_6d = TCDE_CreatePoint(6, coords_6d);
 * TCDE_AddCenter6D(field, &p_6d, 1.0f, 0.1f);
 * TCDE_FreePoint(&p_6d);
 * 
 * // Evaluate in 2D projection
 * float coords_2d[2] = {0.5f, 0.5f};
 * TCDE_Point p_2d = TCDE_CreatePoint(2, coords_2d);
 * TCDE_Complex value_2d = TCDE_Evaluate2D(field, &p_2d);
 * 
 * printf("2D projection value: %.3f + %.3fi\n", 
 *        crealf(value_2d), cimagf(value_2d));
 * 
 * TCDE_FreePoint(&p_2d);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_Evaluate2D(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Compute total field energy
 * 
 * Computes the total energy of the field by integrating |Φ(x)|² over the manifold.
 * The energy is a measure of the field's overall strength and is used in
 * evolution dynamics and validation.
 * 
 * @param field Field to compute energy for (must not be NULL)
 * @return Total energy E = ∫|Φ(x)|² dx, or 0.0 if field is NULL/empty
 * 
 * @note Energy is computed using Monte Carlo integration
 * @note Result is cached in field->energy if field->energy_valid is true
 * @note Cache is invalidated when centers are added/removed or field evolves
 * @note Energy is always non-negative
 * @note Higher energy indicates stronger field
 * 
 * @complexity O(N × M) where N = centers, M = integration samples
 * 
 * @see TCDE_AddCenter6D(), TCDE_RemoveCenter6D(), TCDE_Evaluate6D()
 * 
 * @par Example:
 * @code
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Empty field has zero energy
 * float energy0 = TCDE_ComputeEnergy(field);
 * printf("Empty field energy: %.3f\n", energy0);  // 0.000
 * 
 * // Add a center
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p = TCDE_CreatePoint(6, coords);
 * TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 * TCDE_FreePoint(&p);
 * 
 * // Field now has energy
 * float energy1 = TCDE_ComputeEnergy(field);
 * printf("Field energy with 1 center: %.3f\n", energy1);
 * 
 * // Add another center - energy increases
 * float coords2[6] = {0.7f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
 * TCDE_AddCenter6D(field, &p2, 1.0f, 0.1f);
 * TCDE_FreePoint(&p2);
 * 
 * float energy2 = TCDE_ComputeEnergy(field);
 * printf("Field energy with 2 centers: %.3f\n", energy2);
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_ComputeEnergy(const TCDE_Field* field);

// ============================================================================
// RBF KERNEL OPERATIONS
// ============================================================================

/**
 * @brief Evaluate RBF kernel
 * @param r Distance
 * @param epsilon Shape parameter
 * @param type Kernel type
 * @return Kernel value
 */
float TCDE_EvaluateRBF(float r, float epsilon, TCDE_RBFType type);

/**
 * @brief Evaluate RBF Laplacian
 * @param r Distance
 * @param epsilon Shape parameter
 * @param type Kernel type
 * @param dimension Space dimension
 * @return Laplacian value
 */
float TCDE_RBFLaplacian(float r, float epsilon, TCDE_RBFType type, int dimension);

// ============================================================================
// PROJECTION OPERATIONS
// ============================================================================

/**
 * @brief Configure 2D projection
 * @param field Field
 * @param axis1 First projection axis (0-5)
 * @param axis2 Second projection axis (0-5)
 * @param slice_coords Fixed coordinates for other dimensions
 */
void TCDE_ConfigureProjection(TCDE_Field* field, int axis1, int axis2, 
                               const float slice_coords[4]);

/**
 * @brief Update 2D projection from 6D manifold
 * @param field Field
 */
void TCDE_UpdateProjection(TCDE_Field* field);

/**
 * @brief Project 6D point to 2D
 * @param field Field
 * @param point_6d 6D point
 * @return 2D point
 */
TCDE_Point TCDE_Project6Dto2D(const TCDE_Field* field, const TCDE_Point* point_6d);

/**
 * @brief Lift 2D point to 6D
 * @param field Field
 * @param point_2d 2D point
 * @return 6D point
 */
TCDE_Point TCDE_Lift2Dto6D(const TCDE_Field* field, const TCDE_Point* point_2d);

/**
 * @brief Enable/disable 2D-6D synchronization
 * @param field Field
 * @param enabled Enable synchronization
 * @param strength Synchronization strength [0,1]
 */
void TCDE_SetSynchronization(TCDE_Field* field, bool enabled, float strength);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Print field information
 * @param field Field
 */
void TCDE_PrintField(const TCDE_Field* field);

/**
 * @brief Print version information
 */
void TCDE_PrintVersion(void);

/**
 * @brief Save field to file
 * @param field Field
 * @param filename Output filename
 * @return true on success, false on error
 */
bool TCDE_SaveField(const TCDE_Field* field, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_CORE_H
