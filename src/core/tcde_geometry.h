/**
 * @file tcde_geometry.h
 * @brief TCDE Geometry Module - Differential Geometry Operations
 * 
 * Geometric operations on Riemannian manifolds:
 * - Christoffel symbols
 * - Riemann curvature tensor
 * - Covariant derivatives
 * - Laplace-Beltrami operator
 * - Geodesic distance and paths
 * - Topological torsion
 * - Contextual coupling
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_GEOMETRY_H
#define TCDE_GEOMETRY_H

#include "tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// GEODESIC OPERATIONS
// ============================================================================

/**
 * @brief Compute geodesic distance between two points on Riemannian manifold
 * 
 * Computes the length of the shortest path (geodesic) between two points
 * on a Riemannian manifold with the given metric tensor. This is the
 * fundamental distance measure in TCDE, used for similarity computation,
 * coupling kernels, and field evaluation.
 * 
 * The geodesic distance generalizes Euclidean distance to curved spaces:
 * - In flat space (identity metric): reduces to Euclidean distance
 * - In curved space: accounts for metric curvature
 * - Respects the manifold's geometric structure
 * 
 * Formula: d(p,q) = √[(p-q)ᵀ g (p-q)]
 * where g is the metric tensor at the midpoint.
 * 
 * For TCDE's 6D space with block-diagonal metric:
 * d² = d²_spatial(3D) + d²_temporal(2D) + d²_modal(1D)
 * 
 * @param p1 First point (must not be NULL, dimension must match metric)
 * @param p2 Second point (must not be NULL, dimension must match metric)
 * @param metric Riemannian metric tensor (if NULL, uses identity metric)
 * @return Geodesic distance d(p1,p2) ≥ 0, or 0.0 on error
 * 
 * @note Distance is always non-negative: d(p,q) ≥ 0
 * @note Symmetric: d(p,q) = d(q,p)
 * @note Satisfies triangle inequality: d(p,r) ≤ d(p,q) + d(q,r)
 * @note For repeated distance computations, consider using
 *       TCDE_GeodesicDistanceSquared to avoid sqrt overhead
 * 
 * @complexity O(d²) where d = dimension (36 ops for 6D)
 *             O(10) for block-diagonal metric (use Optimized version)
 * 
 * @see TCDE_GeodesicDistanceSquared(), TCDE_GeodesicDistanceOptimized()
 * @see TCDE_CouplingKernel() for usage in field coupling
 * 
 * @par Example:
 * @code
 * // Create two points in 6D space
 * float coords1[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 * float coords2[6] = {0.3f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f};
 * TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
 * TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
 * 
 * // Create metric (or use field's metric)
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Compute geodesic distance
 * float distance = TCDE_GeodesicDistance(&p1, &p2, &metric);
 * printf("Geodesic distance: %.3f\n", distance);
 * // Output: Geodesic distance: 0.500 (for identity metric)
 * 
 * // Cleanup
 * TCDE_FreePoint(&p1);
 * TCDE_FreePoint(&p2);
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
float TCDE_GeodesicDistance(const TCDE_Point* p1, const TCDE_Point* p2, 
                             const TCDE_Metric* metric);

/**
 * @brief Compute squared geodesic distance (faster, avoids sqrt)
 * 
 * Computes d²(p,q) without taking the square root, which is significantly
 * faster when only relative distances matter or when the squared distance
 * is needed directly (e.g., in Gaussian kernels, energy computations).
 * 
 * This function is preferred over TCDE_GeodesicDistance when:
 * - Comparing distances (ordering is preserved: d₁ < d₂ ⟺ d₁² < d₂²)
 * - Computing Gaussian kernels: exp(-d²/2σ²)
 * - Minimizing distance (argmin d = argmin d²)
 * - Performance is critical (avoids expensive sqrt operation)
 * 
 * Formula: d²(p,q) = (p-q)ᵀ g (p-q)
 * where g is the metric tensor.
 * 
 * For block-diagonal metric in 6D:
 * d² = Σᵢ gᵢᵢ(pᵢ-qᵢ)² (no cross terms)
 * 
 * @param p1 First point (must not be NULL)
 * @param p2 Second point (must not be NULL)
 * @param metric Riemannian metric (if NULL, uses identity)
 * @return Squared geodesic distance d²(p1,p2) ≥ 0, or 0.0 on error
 * 
 * @note Always non-negative: d²(p,q) ≥ 0
 * @note Symmetric: d²(p,q) = d²(q,p)
 * @note ~30% faster than TCDE_GeodesicDistance (no sqrt)
 * @note For actual distance, use TCDE_GeodesicDistance
 * 
 * @complexity O(d²) where d = dimension (36 ops for 6D)
 *             O(6) for block-diagonal metric (diagonal-only)
 * 
 * @see TCDE_GeodesicDistance() for actual distance
 * @see TCDE_CouplingKernel() for usage in Gaussian kernels
 * 
 * @par Example:
 * @code
 * // Find nearest neighbor (squared distance is sufficient)
 * TCDE_Point query = TCDE_CreatePoint(6, query_coords);
 * float min_dist_sq = INFINITY;
 * int nearest_idx = -1;
 * 
 * for (int i = 0; i < num_points; i++) {
 *     float dist_sq = TCDE_GeodesicDistanceSquared(&query, &points[i], &metric);
 *     if (dist_sq < min_dist_sq) {
 *         min_dist_sq = dist_sq;
 *         nearest_idx = i;
 *     }
 * }
 * 
 * printf("Nearest neighbor: %d at distance %.3f\n", 
 *        nearest_idx, sqrtf(min_dist_sq));
 * 
 * TCDE_FreePoint(&query);
 * @endcode
 */
float TCDE_GeodesicDistanceSquared(const TCDE_Point* p1, const TCDE_Point* p2,
                                    const TCDE_Metric* metric);

/**
 * @brief Compute geodesic distance with block-diagonal optimization
 * 
 * Highly optimized version that exploits the block-diagonal structure
 * of TCDE's metric tensor. This is the **recommended function** for
 * production use in TCDE, providing 83% speedup over the general version.
 * 
 * TCDE's 6D metric has block-diagonal structure:
 * - Spatial block (3×3): coordinates (x, y, z)
 * - Temporal block (2×2): coordinates (τ₁, τ₂)
 * - Modal block (1×1): coordinate (m)
 * 
 * This structure allows decomposition:
 * d²(p,q) = d²_spatial + d²_temporal + d²_modal
 * 
 * where each block is computed independently, eliminating cross-terms
 * and reducing operations from O(d²) to O(d).
 * 
 * Performance comparison (6D):
 * - General version: 36 operations
 * - Optimized version: 6 operations
 * - Speedup: 6× faster (83% reduction)
 * 
 * @param p1 First point in 6D space (must not be NULL)
 * @param p2 Second point in 6D space (must not be NULL)
 * @param metric Block-diagonal metric (if NULL, uses identity)
 * @return Geodesic distance d(p1,p2) ≥ 0, or 0.0 on error
 * 
 * @note Requires block-diagonal metric structure
 * @note Falls back to general computation if metric is not block-diagonal
 * @note Produces identical results to TCDE_GeodesicDistance
 * @note This is the default distance function used in TCDE field operations
 * 
 * @warning Only use with block-diagonal metrics! For general metrics,
 *          use TCDE_GeodesicDistance instead.
 * 
 * @complexity O(10) operations for 6D block-diagonal
 *             vs O(36) for general 6D metric
 * 
 * @see TCDE_GeodesicDistance() for general metric
 * @see TCDE_GeodesicDistanceSquared() for squared distance
 * 
 * @par Example:
 * @code
 * // Create field with block-diagonal metric (default in TCDE)
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Create two points
 * float coords1[6] = {0.2f, 0.3f, 0.5f, 1.0f, 0.0f, 0.4f};
 * float coords2[6] = {0.5f, 0.6f, 0.7f, 1.2f, 0.05f, 0.4f};
 * TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
 * TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
 * 
 * // Use optimized distance (6× faster)
 * float dist = TCDE_GeodesicDistanceOptimized(&p1, &p2, 
 *                                              &field->manifold_6d.metric);
 * printf("Distance: %.3f\n", dist);
 * 
 * // Verify it matches general version
 * float dist_general = TCDE_GeodesicDistance(&p1, &p2, 
 *                                             &field->manifold_6d.metric);
 * printf("Difference: %.6f\n", fabsf(dist - dist_general));
 * // Output: Difference: 0.000000 (identical results)
 * 
 * TCDE_FreePoint(&p1);
 * TCDE_FreePoint(&p2);
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_GeodesicDistanceOptimized(const TCDE_Point* p1, const TCDE_Point* p2,
                                      const TCDE_Metric* metric);

/**
 * @brief Verify triangle inequality for three points
 * 
 * Tests whether the triangle inequality holds for three points on the
 * manifold. This is a fundamental property of metric spaces and must
 * be satisfied for the distance function to be a valid metric.
 * 
 * Triangle inequality: d(p,q) ≤ d(p,r) + d(r,q)
 * 
 * Geometric interpretation: The direct path from p to q cannot be longer
 * than the path that goes through an intermediate point r.
 * 
 * This function is used for:
 * - Validating metric tensor correctness
 * - Testing geodesic distance implementations
 * - Verifying manifold properties
 * - Debugging geometric computations
 * 
 * @param p First point (must not be NULL)
 * @param q Second point (must not be NULL)
 * @param r Third point (intermediate, must not be NULL)
 * @param metric Riemannian metric (if NULL, uses identity)
 * @return true if triangle inequality holds, false otherwise
 * 
 * @note Always true for valid metrics
 * @note Violation indicates metric or distance computation error
 * @note Uses small tolerance (1e-6) for floating-point comparison
 * 
 * @complexity O(d²) - computes 3 distances
 * 
 * @see TCDE_GeodesicDistance() for distance computation
 * 
 * @par Example:
 * @code
 * // Create three points forming a triangle
 * float coords_p[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 * float coords_q[6] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 * float coords_r[6] = {0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f};
 * 
 * TCDE_Point p = TCDE_CreatePoint(6, coords_p);
 * TCDE_Point q = TCDE_CreatePoint(6, coords_q);
 * TCDE_Point r = TCDE_CreatePoint(6, coords_r);
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Verify triangle inequality
 * bool valid = TCDE_VerifyTriangleInequality(&p, &q, &r, &metric);
 * printf("Triangle inequality: %s\n", valid ? "VALID" : "VIOLATED");
 * // Output: Triangle inequality: VALID
 * 
 * TCDE_FreePoint(&p);
 * TCDE_FreePoint(&q);
 * TCDE_FreePoint(&r);
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
bool TCDE_VerifyTriangleInequality(const TCDE_Point* p, const TCDE_Point* q,
                                    const TCDE_Point* r, const TCDE_Metric* metric);

/**
 * @brief Compute geodesic path between two points
 * 
 * Computes the shortest path (geodesic) connecting two points on the
 * Riemannian manifold by solving the geodesic equation. The path is
 * discretized into num_steps intermediate points.
 * 
 * Geodesic equation: d²xⁱ/dt² + Γⁱⱼₖ (dxʲ/dt)(dxᵏ/dt) = 0
 * 
 * where Γⁱⱼₖ are the Christoffel symbols of the metric.
 * 
 * This function uses numerical integration (RK4) to solve the geodesic
 * equation with boundary conditions x(0) = p1 and x(1) = p2.
 * 
 * Applications:
 * - Visualizing field structure
 * - Computing path integrals
 * - Analyzing manifold curvature
 * - Interpolating between field states
 * 
 * @param p1 Start point (must not be NULL)
 * @param p2 End point (must not be NULL)
 * @param metric Riemannian metric (must not be NULL)
 * @param num_steps Number of integration steps (recommended: 50-100)
 * @param path Output array of points (must be pre-allocated, size = num_steps+1)
 * @return true on success, false on error (singular metric, invalid params)
 * 
 * @note path[0] = p1, path[num_steps] = p2
 * @note More steps = higher accuracy but slower computation
 * @note Requires computing Christoffel symbols at each step
 * @note Path array must be allocated by caller
 * 
 * @warning Computationally expensive for large num_steps
 * @warning May fail if metric is singular or nearly singular
 * 
 * @complexity O(num_steps × d³) where d = dimension
 * 
 * @see TCDE_ComputeChristoffel() for Christoffel symbol computation
 * @see TCDE_GeodesicDistance() for distance without path
 * 
 * @par Example:
 * @code
 * // Compute geodesic path between two points
 * float coords1[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 * float coords2[6] = {1.0f, 1.0f, 0.0f, 1.5f, 0.0f, 0.0f};
 * TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
 * TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
 * 
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * 
 * // Allocate path array
 * int num_steps = 50;
 * TCDE_Point* path = (TCDE_Point*)malloc((num_steps + 1) * sizeof(TCDE_Point));
 * 
 * // Compute geodesic
 * bool success = TCDE_ComputeGeodesicPath(&p1, &p2, &metric, num_steps, path);
 * 
 * if (success) {
 *     printf("Geodesic path computed with %d points\n", num_steps + 1);
 *     
 *     // Visualize or analyze path
 *     for (int i = 0; i <= num_steps; i += 10) {
 *         printf("  Point %d: (%.3f, %.3f, %.3f)\n", i,
 *                path[i].coords[0], path[i].coords[1], path[i].coords[2]);
 *     }
 * }
 * 
 * // Cleanup
 * for (int i = 0; i <= num_steps; i++) {
 *     TCDE_FreePoint(&path[i]);
 * }
 * free(path);
 * TCDE_FreePoint(&p1);
 * TCDE_FreePoint(&p2);
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
bool TCDE_ComputeGeodesicPath(const TCDE_Point* p1, const TCDE_Point* p2,
                               const TCDE_Metric* metric, int num_steps,
                               TCDE_Point* path);

// ============================================================================
// CHRISTOFFEL SYMBOLS
// ============================================================================

/**
 * @brief Compute Christoffel symbols of the second kind at a point
 * 
 * Computes the Christoffel symbols Γⁱⱼₖ which encode how the coordinate
 * basis vectors change as you move through the manifold. These are
 * fundamental to differential geometry and are used in:
 * - Geodesic equations (parallel transport)
 * - Covariant derivatives
 * - Riemann curvature tensor
 * - Field evolution (TDE)
 * 
 * The Christoffel symbols are NOT tensors (they don't transform as tensors)
 * but are connection coefficients that define how to differentiate vectors
 * and tensors on curved manifolds.
 * 
 * Formula: Γⁱⱼₖ = (1/2) gⁱˡ (∂ⱼgₗₖ + ∂ₖgⱼₗ - ∂ₗgⱼₖ)
 * 
 * where gⁱˡ is the inverse metric and ∂ⱼgₗₖ are metric derivatives.
 * 
 * Key properties:
 * - Symmetric in lower indices: Γⁱⱼₖ = Γⁱₖⱼ
 * - Vanish in flat space with Cartesian coordinates
 * - Encode metric's local curvature information
 * 
 * For TCDE's 6D block-diagonal metric:
 * - Spatial block (x,y,z): 3×3×3 = 27 components
 * - Temporal block (τ₁,τ₂): 2×2×2 = 8 components
 * - Modal block (m): 1×1×1 = 1 component
 * - Total: 36 non-zero (vs 216 for full metric)
 * - 83% reduction in computation!
 * 
 * @param point Evaluation point (must not be NULL, dimension must be 6)
 * @param metric Riemannian metric (must not be NULL, must be invertible)
 * @param christoffel Output array Γⁱⱼₖ [6][6][6] (must be pre-allocated)
 * 
 * @note christoffel array must be allocated by caller: float christoffel[6][6][6]
 * @note Exploits block-diagonal structure for 83% speedup
 * @note Requires metric to be non-singular (det(g) ≠ 0)
 * 
 * @warning Computationally expensive - cache results when possible
 * @warning Numerical derivatives may be inaccurate for rapidly varying metrics
 * 
 * @complexity O(60) operations for 6D block-diagonal
 *             O(d³) for general d-dimensional metric
 * 
 * @see TCDE_ComputeRiemannTensor() for curvature computation
 * @see TCDE_CovariantDerivative() for usage in derivatives
 * @see TCDE_ComputeGeodesicPath() for usage in geodesics
 * 
 * @par Example:
 * @code
 * // Compute Christoffel symbols at a point
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Use field's metric
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_Metric* metric = &field->manifold_6d.metric;
 * 
 * // Allocate output array
 * float christoffel[6][6][6];
 * 
 * // Compute Christoffel symbols
 * TCDE_ComputeChristoffel(&point, metric, christoffel);
 * 
 * // Access specific component: Γ⁰₁₂ (i=0, j=1, k=2)
 * printf("Γ⁰₁₂ = %.6f\n", christoffel[0][1][2]);
 * 
 * // Verify symmetry in lower indices
 * printf("Γ⁰₁₂ = %.6f, Γ⁰₂₁ = %.6f\n", 
 *        christoffel[0][1][2], christoffel[0][2][1]);
 * // Should be equal (within numerical precision)
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeChristoffel(const TCDE_Point* point, const TCDE_Metric* metric,
                              float christoffel[6][6][6]);

// ============================================================================
// CURVATURE TENSORS
// ============================================================================

/**
 * @brief Compute Riemann curvature tensor - the fundamental measure of curvature
 * 
 * Computes the Riemann curvature tensor R^i_jkl, which is THE fundamental
 * object in differential geometry that measures how curved the manifold is.
 * It quantifies the failure of parallel transport to be path-independent.
 * 
 * Physical/Geometric interpretation:
 * - Measures intrinsic curvature of the manifold
 * - Quantifies how vectors change when parallel transported around loops
 * - Zero Riemann tensor ⟺ flat (Euclidean) space
 * - Non-zero components indicate genuine curvature
 * 
 * In TCDE, the Riemann tensor:
 * - Drives field evolution via curvature coupling
 * - Enables intuitive leaps (non-geodesic connections)
 * - Creates emergent cognitive properties
 * - Distinguishes TCDE from simple RBF networks
 * 
 * Formula: R^i_jkl = ∂_k Γⁱⱼₗ - ∂_l Γⁱⱼₖ + Γⁱₘₖ Γᵐⱼₗ - Γⁱₘₗ Γᵐⱼₖ
 * 
 * where Γⁱⱼₖ are Christoffel symbols and ∂ denotes partial derivatives.
 * 
 * Fundamental properties (algebraic identities):
 * 1. Antisymmetry in last two indices: R^i_jkl = -R^i_jlk
 * 2. Antisymmetry in first pair: R^i_jkl = -R^j_ikl
 * 3. First Bianchi identity: R^i_jkl + R^i_klj + R^i_ljk = 0
 * 4. Symmetry: R^i_jkl = R^k_lij (with index lowering)
 * 
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param metric Riemannian metric (must not be NULL, invertible)
 * @param riemann Output tensor R^i_jkl [6][6][6][6] (must be pre-allocated)
 * 
 * @note riemann array must be allocated: float riemann[6][6][6][6]
 * @note Array size: 6⁴ = 1296 floats = 5.2 KB
 * @note Most components are zero due to symmetries
 * @note Block-diagonal metric reduces non-zero components significantly
 * 
 * @warning Very computationally expensive - use sparingly
 * @warning Requires accurate Christoffel symbols
 * @warning Numerical derivatives amplify errors
 * 
 * @complexity O(d⁴) for general metric (1296 ops for 6D)
 *             O(d³) for block-diagonal (216 ops for 6D)
 * 
 * @see TCDE_ComputeChristoffel() for Christoffel symbols
 * @see TCDE_ComputeRicciTensor() for contracted curvature
 * @see TCDE_ComputeScalarCurvature() for total curvature
 * 
 * @par Example:
 * @code
 * // Compute Riemann tensor to measure field curvature
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_Metric* metric = &field->manifold_6d.metric;
 * 
 * // Allocate Riemann tensor (5.2 KB)
 * float riemann[6][6][6][6];
 * 
 * // Compute curvature
 * TCDE_ComputeRiemannTensor(&point, metric, riemann);
 * 
 * // Check if space is flat (all components ≈ 0)
 * float max_curvature = 0.0f;
 * for (int i = 0; i < 6; i++) {
 *     for (int j = 0; j < 6; j++) {
 *         for (int k = 0; k < 6; k++) {
 *             for (int l = 0; l < 6; l++) {
 *                 float R = fabsf(riemann[i][j][k][l]);
 *                 if (R > max_curvature) max_curvature = R;
 *             }
 *         }
 *     }
 * }
 * 
 * printf("Maximum curvature component: %.6f\n", max_curvature);
 * if (max_curvature < 1e-6f) {
 *     printf("Space is approximately flat\n");
 * } else {
 *     printf("Space has genuine curvature\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeRiemannTensor(const TCDE_Point* point, const TCDE_Metric* metric,
                                float riemann[6][6][6][6]);

/**
 * @brief Compute Ricci curvature tensor - averaged directional curvature
 * 
 * Computes the Ricci tensor R_ij by contracting (summing over) the Riemann
 * tensor. The Ricci tensor represents the average curvature in different
 * directions and is central to Einstein's field equations and geometric flows.
 * 
 * Geometric interpretation:
 * - Measures how volumes change under parallel transport
 * - Positive Ricci curvature: volumes shrink (sphere-like)
 * - Negative Ricci curvature: volumes expand (hyperbolic)
 * - Zero Ricci curvature: volumes preserved (Ricci-flat)
 * 
 * In TCDE:
 * - Drives Ricci flow evolution
 * - Influences field diffusion patterns
 * - Creates attraction/repulsion in field dynamics
 * - Simpler than full Riemann tensor but captures essential curvature
 * 
 * Formula: R_ij = R^k_ikj = Σₖ R^k_ikj
 * 
 * This is a contraction (trace) of the Riemann tensor over the first
 * and third indices.
 * 
 * Properties:
 * - Symmetric: R_ij = R_ji
 * - Dimension: d×d matrix (6×6 for TCDE)
 * - Trace gives scalar curvature: R = g^ij R_ij
 * 
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param metric Riemannian metric (must not be NULL, invertible)
 * @param ricci Output tensor R_ij [6][6] (must be pre-allocated)
 * 
 * @note ricci array must be allocated: float ricci[6][6]
 * @note Symmetric matrix: only 21 independent components
 * @note Much cheaper than full Riemann tensor
 * @note Sufficient for many geometric flow applications
 * 
 * @complexity O(d³) where d = dimension (216 ops for 6D)
 * 
 * @see TCDE_ComputeRiemannTensor() for full curvature
 * @see TCDE_ComputeScalarCurvature() for total curvature
 * 
 * @par Example:
 * @code
 * // Compute Ricci tensor to analyze field curvature
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_Metric* metric = &field->manifold_6d.metric;
 * 
 * // Allocate Ricci tensor
 * float ricci[6][6];
 * 
 * // Compute Ricci curvature
 * TCDE_ComputeRicciTensor(&point, metric, ricci);
 * 
 * // Analyze curvature in spatial directions
 * printf("Spatial curvature:\n");
 * for (int i = 0; i < 3; i++) {
 *     printf("  R_%d%d = %.6f\n", i, i, ricci[i][i]);
 * }
 * 
 * // Check if Ricci-flat (all components ≈ 0)
 * float max_ricci = 0.0f;
 * for (int i = 0; i < 6; i++) {
 *     for (int j = 0; j < 6; j++) {
 *         if (fabsf(ricci[i][j]) > max_ricci) {
 *             max_ricci = fabsf(ricci[i][j]);
 *         }
 *     }
 * }
 * 
 * printf("Maximum Ricci component: %.6f\n", max_ricci);
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeRicciTensor(const TCDE_Point* point, const TCDE_Metric* metric,
                              float ricci[6][6]);

/**
 * @brief Compute scalar curvature - single number measuring total curvature
 * 
 * Computes the scalar curvature R, which is the complete contraction of
 * the Riemann tensor into a single number. This is the simplest measure
 * of curvature and represents the average curvature at a point.
 * 
 * Geometric interpretation:
 * - R > 0: Positive curvature (sphere-like, space curves inward)
 * - R = 0: Flat space (Euclidean, no curvature)
 * - R < 0: Negative curvature (hyperbolic, space curves outward)
 * 
 * Physical significance:
 * - In general relativity: related to energy density
 * - In geometric flows: drives evolution toward constant curvature
 * - In TCDE: influences field concentration and dispersion
 * 
 * Formula: R = g^ij R_ij = Σᵢⱼ g^ij R_ij
 * 
 * where g^ij is the inverse metric and R_ij is the Ricci tensor.
 * 
 * In TCDE:
 * - Positive R: Field concentrates (attractive dynamics)
 * - Negative R: Field disperses (repulsive dynamics)
 * - Used in adaptive metric evolution
 * - Influences fractal dimension
 * 
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param metric Riemannian metric (must not be NULL, invertible)
 * @return Scalar curvature R (can be positive, negative, or zero)
 * 
 * @note Single number summarizing all curvature information
 * @note Loses directional information (use Ricci tensor for that)
 * @note Cheapest curvature measure to compute
 * @note Useful for quick curvature assessment
 * 
 * @complexity O(d³) where d = dimension (216 ops for 6D)
 * 
 * @see TCDE_ComputeRicciTensor() for directional curvature
 * @see TCDE_ComputeRiemannTensor() for complete curvature
 * 
 * @par Example:
 * @code
 * // Compute scalar curvature to assess field geometry
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Sample curvature at multiple points
 * printf("Scalar curvature profile:\n");
 * for (float x = 0.0f; x <= 1.0f; x += 0.2f) {
 *     float coords[6] = {x, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 *     TCDE_Point point = TCDE_CreatePoint(6, coords);
 *     
 *     float R = TCDE_ComputeScalarCurvature(&point, &field->manifold_6d.metric);
 *     printf("  x=%.1f: R = %.6f ", x, R);
 *     
 *     if (R > 0.01f) {
 *         printf("(positive - attractive)\n");
 *     } else if (R < -0.01f) {
 *         printf("(negative - repulsive)\n");
 *     } else {
 *         printf("(approximately flat)\n");
 *     }
 *     
 *     TCDE_FreePoint(&point);
 * }
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_ComputeScalarCurvature(const TCDE_Point* point, const TCDE_Metric* metric);

// ============================================================================
// COVARIANT DERIVATIVES
// ============================================================================

/**
 * @brief Compute covariant derivative of vector field on curved manifold
 * 
 * Computes the covariant derivative ∇_i V^j, which is the proper way to
 * differentiate vector fields on curved manifolds. Unlike ordinary derivatives,
 * covariant derivatives account for how the coordinate basis changes as you
 * move through the manifold.
 * 
 * The covariant derivative is essential for:
 * - Parallel transport of vectors
 * - Defining geodesics (curves with zero acceleration)
 * - Computing curvature tensors
 * - Field evolution in curved space
 * 
 * Formula: ∇_i V^j = ∂_i V^j + Γʲᵢₖ V^k
 * 
 * where:
 * - ∂_i V^j is the ordinary partial derivative
 * - Γʲᵢₖ are the Christoffel symbols
 * - The Christoffel term corrects for basis vector changes
 * 
 * Key property: The covariant derivative of the metric vanishes:
 * ∇_k g_ij = 0 (metric compatibility)
 * 
 * In TCDE:
 * - Used in field evolution equations
 * - Ensures geometric consistency
 * - Preserves tensor transformation properties
 * - Critical for TDE (Tensor Diffusion Evolution)
 * 
 * @param field TCDE field (provides metric and geometry)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param vector_field Vector field V^j at nearby points [N][6]
 * @param covariant_deriv Output: ∇_i V^j [6][6] (must be pre-allocated)
 * 
 * @note vector_field should contain values at points near the evaluation point
 * @note Requires numerical differentiation for ∂_i V^j
 * @note Christoffel symbols computed internally
 * @note Output is a 6×6 matrix: covariant_deriv[i][j] = ∇_i V^j
 * 
 * @warning Accuracy depends on vector_field sampling density
 * @warning Computationally expensive due to Christoffel computation
 * 
 * @complexity O(d³) where d = dimension (216 ops for 6D)
 * 
 * @see TCDE_ComputeChristoffel() for connection coefficients
 * @see TCDE_CovariantDerivativeScalar() for scalar fields
 * 
 * @par Example:
 * @code
 * // Compute covariant derivative of a vector field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Sample vector field at nearby points
 * float vector_field[10][6];
 * // ... fill with vector values at nearby points ...
 * 
 * // Allocate output
 * float covariant_deriv[6][6];
 * 
 * // Compute covariant derivative
 * TCDE_CovariantDerivative(field, &point, vector_field, covariant_deriv);
 * 
 * // Access specific component: ∇₀ V¹
 * printf("∇₀ V¹ = %.6f\n", covariant_deriv[0][1]);
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_CovariantDerivative(const TCDE_Field* field, const TCDE_Point* point,
                               const float vector_field[][6], 
                               float covariant_deriv[6][6]);

/**
 * @brief Compute covariant derivative of scalar field
 * 
 * Computes the covariant derivative of a scalar field Φ in a specified
 * direction. For scalar fields, the covariant derivative reduces to the
 * ordinary partial derivative since scalars don't have components that
 * need basis correction.
 * 
 * Formula: ∇_i Φ = ∂_i Φ
 * 
 * Note: No Christoffel symbols appear because scalars are invariant
 * under coordinate transformations. The Christoffel correction only
 * applies to tensors with indices.
 * 
 * This is the fundamental building block for:
 * - Field gradients
 * - Laplace-Beltrami operator
 * - Field evolution equations
 * - Energy functionals
 * 
 * In TCDE:
 * - Used in TDE diffusion term
 * - Computes field flow directions
 * - Identifies field extrema
 * - Drives field evolution
 * 
 * @param field TCDE field to differentiate (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param direction Direction index i ∈ {0,1,2,3,4,5}
 *                  0-2: spatial (x,y,z)
 *                  3-4: temporal (τ₁,τ₂)
 *                  5: modal (m)
 * @return ∇_i Φ as complex number (magnitude and phase)
 * 
 * @note Returns complex value to preserve field phase information
 * @note Computed via finite differences using RBF centers
 * @note Fast: O(K) where K = number of centers
 * @note No Christoffel symbols needed (scalar invariance)
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_Gradient() for all directional derivatives at once
 * @see TCDE_LaplaceBeltrami() for second derivatives
 * @see TCDE_CovariantDerivative() for vector fields
 * 
 * @par Example:
 * @code
 * // Compute directional derivatives of field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add some centers
 * for (int i = 0; i < 10; i++) {
 *     float coords[6] = {0.5f + 0.1f*i, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 *     TCDE_Point p = TCDE_CreatePoint(6, coords);
 *     TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 *     TCDE_FreePoint(&p);
 * }
 * 
 * // Evaluate derivative at a point
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Compute derivatives in each direction
 * printf("Field derivatives:\n");
 * const char* names[] = {"x", "y", "z", "τ₁", "τ₂", "m"};
 * for (int i = 0; i < 6; i++) {
 *     TCDE_Complex deriv = TCDE_CovariantDerivativeScalar(field, &point, i);
 *     printf("  ∂Φ/∂%s = %.6f + %.6fi\n", names[i], 
 *            crealf(deriv), cimagf(deriv));
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_CovariantDerivativeScalar(const TCDE_Field* field,
                                             const TCDE_Point* point,
                                             int direction);

/**
 * @brief Verify metric compatibility - fundamental Riemannian property
 * 
 * Tests whether the covariant derivative of the metric tensor vanishes:
 * ∇_k g_ij = 0
 * 
 * This is THE fundamental property that defines a Riemannian connection
 * (Levi-Civita connection). It ensures that:
 * - Inner products are preserved under parallel transport
 * - Lengths and angles remain constant along geodesics
 * - The connection is compatible with the metric structure
 * 
 * Mathematical significance:
 * The Christoffel symbols Γⁱⱼₖ are specifically constructed to ensure
 * metric compatibility. This is why we use the formula:
 * Γⁱⱼₖ = (1/2) gⁱˡ (∂ⱼgₗₖ + ∂ₖgⱼₗ - ∂ₗgⱼₖ)
 * 
 * If this test fails, it indicates:
 * - Incorrect Christoffel symbol computation
 * - Numerical errors in metric derivatives
 * - Non-Riemannian connection (torsion present)
 * - Implementation bugs
 * 
 * In TCDE:
 * - Used for validation and testing
 * - Ensures geometric consistency
 * - Verifies correct implementation
 * - Debugging tool for geometry code
 * 
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param metric Riemannian metric (must not be NULL, invertible)
 * @return true if ∇_k g_ij ≈ 0 (within numerical tolerance)
 *         false if compatibility violated
 * 
 * @note Uses tolerance of 1e-6 for floating-point comparison
 * @note Should always return true for correctly implemented Christoffel symbols
 * @note Failure indicates serious implementation error
 * @note Computationally expensive - use for testing only
 * 
 * @warning Do not use in production code (testing/validation only)
 * @warning Requires accurate metric derivatives
 * 
 * @complexity O(d⁴) where d = dimension (1296 ops for 6D)
 * 
 * @see TCDE_ComputeChristoffel() for connection computation
 * @see TCDE_CovariantDerivative() for covariant differentiation
 * 
 * @par Example:
 * @code
 * // Verify metric compatibility for validation
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Test metric compatibility
 * bool compatible = TCDE_VerifyMetricCompatibility(&point, 
 *                                                   &field->manifold_6d.metric);
 * 
 * if (compatible) {
 *     printf("✓ Metric compatibility verified\n");
 *     printf("  Connection is Riemannian (Levi-Civita)\n");
 * } else {
 *     printf("✗ Metric compatibility VIOLATED\n");
 *     printf("  ERROR: Check Christoffel symbol computation\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
bool TCDE_VerifyMetricCompatibility(const TCDE_Point* point, const TCDE_Metric* metric);

// ============================================================================
// LAPLACE-BELTRAMI OPERATOR
// ============================================================================

/**
 * @brief Compute Laplace-Beltrami operator - the geometric Laplacian
 * 
 * Computes the Laplace-Beltrami operator ∇²_g Φ, which is the natural
 * generalization of the Laplacian to curved Riemannian manifolds. This
 * is THE fundamental differential operator in geometric analysis.
 * 
 * Formula: ∇²_g Φ = (1/√|g|) ∂_i(√|g| g^ij ∂_j Φ)
 * 
 * where g is the metric determinant and g^ij is the inverse metric.
 * 
 * Geometric interpretation:
 * - Measures how much Φ differs from its average over nearby points
 * - Drives diffusion on curved manifolds
 * - Generalizes heat equation to curved spaces
 * - Preserves geometric structure
 * 
 * In TCDE, the Laplace-Beltrami operator:
 * - Drives field diffusion in TDE
 * - Respects manifold geometry
 * - Enables geometric flows
 * - Creates scale-invariant evolution
 * 
 * For 6D block-diagonal metric:
 * ∇²_g = ∇²_spatial + ∇²_temporal + ∇²_modal
 * 
 * This decomposition provides 83% speedup!
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @return ∇²_g Φ as complex number
 * 
 * @note Returns complex to preserve phase information
 * @note Uses RBF representation for efficient computation
 * @note Accounts for metric curvature automatically
 * @note Critical for TDE evolution
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_LaplaceBeltramiOptimized() for block-diagonal speedup
 * @see TCDE_Laplacian() for flat-space version
 * @see TCDE_Gradient() for first derivatives
 * 
 * @par Example:
 * @code
 * // Compute Laplace-Beltrami for field evolution
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add Gaussian-like field
 * for (int i = 0; i < 20; i++) {
 *     float coords[6] = {
 *         0.5f + 0.1f * cosf(2*M_PI*i/20),
 *         0.5f + 0.1f * sinf(2*M_PI*i/20),
 *         0.5f, 1.0f, 0.0f, 0.4f
 *     };
 *     TCDE_Point p = TCDE_CreatePoint(6, coords);
 *     TCDE_AddCenter6D(field, &p, 1.0f, 0.05f);
 *     TCDE_FreePoint(&p);
 * }
 * 
 * // Compute Laplace-Beltrami at center
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Complex laplacian = TCDE_LaplaceBeltrami(field, &point);
 * printf("∇²_g Φ = %.6f + %.6fi\n", crealf(laplacian), cimagf(laplacian));
 * 
 * // Negative Laplacian indicates local maximum
 * if (crealf(laplacian) < 0) {
 *     printf("Field has local maximum (diffusion outward)\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_LaplaceBeltrami(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Compute Laplace-Beltrami with block-diagonal optimization
 * 
 * Optimized version exploiting TCDE's block-diagonal metric structure.
 * Provides identical results to TCDE_LaplaceBeltrami but with significant
 * performance improvement.
 * 
 * Decomposition: ∇²_g = ∇²_spatial + ∇²_temporal + ∇²_modal
 * 
 * This is the **recommended function** for production TCDE code.
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL)
 * @return ∇²_g Φ as complex number
 * 
 * @note 83% faster than general version
 * @note Identical results to TCDE_LaplaceBeltrami
 * @note Exploits block-diagonal structure
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_LaplaceBeltrami() for general version
 */
TCDE_Complex TCDE_LaplaceBeltramiOptimized(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Compute field gradient - all directional derivatives
 * 
 * Computes the gradient ∇Φ = (∂Φ/∂x¹, ∂Φ/∂x², ..., ∂Φ/∂x⁶), which
 * points in the direction of steepest field increase.
 * 
 * The gradient is fundamental for:
 * - Field flow visualization
 * - Optimization (gradient descent/ascent)
 * - Finding field extrema
 * - Computing Laplacian
 * 
 * In TCDE:
 * - Drives field evolution
 * - Identifies attraction/repulsion directions
 * - Used in adaptive metric computation
 * - Critical for TDE coupling term
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL)
 * @param gradient Output array ∇Φ [6] (must be pre-allocated)
 * 
 * @note gradient[i] = ∂Φ/∂xⁱ (complex)
 * @note Magnitude ||∇Φ|| indicates field variation rate
 * @note Direction shows steepest increase
 * 
 * @complexity O(K·d) where K = centers, d = dimension
 * 
 * @see TCDE_CovariantDerivativeScalar() for single direction
 * @see TCDE_LaplaceBeltrami() for second derivatives
 * 
 * @par Example:
 * @code
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Complex gradient[6];
 * TCDE_Gradient(field, &point, gradient);
 * 
 * // Compute gradient magnitude
 * float mag = 0.0f;
 * for (int i = 0; i < 6; i++) {
 *     mag += cabsf(gradient[i]) * cabsf(gradient[i]);
 * }
 * mag = sqrtf(mag);
 * printf("||∇Φ|| = %.6f\n", mag);
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_Gradient(const TCDE_Field* field, const TCDE_Point* point,
                   TCDE_Complex gradient[6]);

/**
 * @brief Compute standard Laplacian (flat-space version)
 * 
 * Computes the standard (Euclidean) Laplacian ∇²Φ = Σᵢ ∂²Φ/∂xⁱ²,
 * which assumes flat space with identity metric.
 * 
 * Formula: ∇²Φ = ∂²Φ/∂x² + ∂²Φ/∂y² + ∂²Φ/∂z² + ∂²Φ/∂τ₁² + ∂²Φ/∂τ₂² + ∂²Φ/∂m²
 * 
 * Difference from Laplace-Beltrami:
 * - Standard: Assumes flat space (identity metric)
 * - Laplace-Beltrami: Accounts for curved metric
 * 
 * Use this when:
 * - Metric is approximately identity
 * - Quick approximation needed
 * - Comparing with flat-space results
 * 
 * Use Laplace-Beltrami when:
 * - Metric has significant curvature
 * - Geometric accuracy required
 * - Full TCDE theory compliance needed
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL)
 * @return ∇²Φ as complex number
 * 
 * @note Faster than Laplace-Beltrami (no metric computation)
 * @note Less accurate for curved metrics
 * @note Useful for debugging and comparison
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_LaplaceBeltrami() for geometric version
 * @see TCDE_Gradient() for first derivatives
 * 
 * @par Example:
 * @code
 * // Compare standard vs geometric Laplacian
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Complex lap_std = TCDE_Laplacian(field, &point);
 * TCDE_Complex lap_geo = TCDE_LaplaceBeltrami(field, &point);
 * 
 * printf("Standard Laplacian:  %.6f\n", crealf(lap_std));
 * printf("Geometric Laplacian: %.6f\n", crealf(lap_geo));
 * printf("Difference:          %.6f\n", 
 *        crealf(lap_geo - lap_std));
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_Laplacian(const TCDE_Field* field, const TCDE_Point* point);

// ============================================================================
// TOPOLOGICAL TORSION
// ============================================================================

/**
 * @brief Compute topological torsion contribution at a point
 * 
 * Computes the topological torsion, which represents the "twisting" or
 * non-commutative nature of the field. Torsion enables non-gradient flow,
 * allowing the field to escape local minima and make intuitive leaps.
 * 
 * Topological torsion is THE mechanism in TCDE that enables:
 * - Creative problem solving (escaping local optima)
 * - Intuitive connections (non-obvious relationships)
 * - Non-gradient flow (exploration beyond steepest descent)
 * - Cognitive flexibility (multiple solution paths)
 * 
 * Formula: T(p) = Σᵢⱼ T_ij where T_ij = ∂_i Φ_j - ∂_j Φ_i
 * 
 * The torsion tensor T_ij is antisymmetric (T_ij = -T_ji), forming a
 * 2-form that measures the curl of the field gradient. High torsion
 * indicates regions where the field has rotational structure.
 * 
 * Geometric interpretation:
 * - Zero torsion: Field is gradient of potential (conservative)
 * - Non-zero torsion: Field has vorticity (non-conservative)
 * - High torsion: Strong rotational dynamics
 * 
 * In TCDE evolution (TDE):
 * ∂Φ/∂t = ... + β·T(Φ) + ...
 * 
 * where β controls torsion strength. This term allows field to:
 * - Rotate around attractors
 * - Explore multiple basins
 * - Make creative leaps
 * - Avoid getting stuck
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @return Torsion contribution T(p) as complex number
 * 
 * @note Returns scalar contraction of full torsion tensor
 * @note Complex value preserves phase information
 * @note Magnitude indicates torsion strength
 * @note Phase indicates rotation direction
 * 
 * @complexity O(K·d²) where K = centers, d = dimension (216 ops for 6D)
 * 
 * @see TCDE_ComputeTorsionTensor() for full tensor
 * @see TCDE_TorsionMagnitude() for magnitude only
 * @see TCDE_VisualizeTorsion() for visualization
 * 
 * @par Example:
 * @code
 * // Compute torsion to identify intuitive regions
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers with rotational structure ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Complex torsion = TCDE_TopologicalTorsion(field, &point);
 * float magnitude = cabsf(torsion);
 * 
 * printf("Torsion magnitude: %.6f\n", magnitude);
 * if (magnitude > 0.1f) {
 *     printf("High torsion - intuitive leap region!\n");
 * } else {
 *     printf("Low torsion - gradient flow region\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_TopologicalTorsion(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Compute complete torsion tensor T_ij at a point
 * 
 * Computes all 36 components of the antisymmetric torsion tensor T_ij,
 * which fully characterizes the non-commutative structure of the field.
 * This provides complete information about rotational dynamics and
 * intuitive connections in all directions.
 * 
 * Formula: T_ij(p) = ∂_i Φ_j(p) - ∂_j Φ_i(p)
 * 
 * where ∂_i denotes partial derivative in direction i.
 * 
 * Properties of torsion tensor:
 * - Antisymmetric: T_ij = -T_ji (only 15 independent components)
 * - Diagonal zero: T_ii = 0 (no self-torsion)
 * - Forms 2-form: can be integrated over surfaces
 * - Measures field curl in each 2D plane
 * 
 * Physical interpretation:
 * - T_xy: Rotation in XY plane (spatial vorticity)
 * - T_τ₁τ₂: Rotation in temporal plane (temporal dynamics)
 * - T_xm: Coupling between space and modality
 * 
 * Applications:
 * - Identifying intuitive leap regions
 * - Analyzing field topology
 * - Computing path integrals
 * - Visualizing rotational structure
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param torsion Output tensor T_ij [6][6] (must be pre-allocated, complex)
 * 
 * @note torsion array must be allocated: TCDE_Complex torsion[6][6]
 * @note Antisymmetric: torsion[i][j] = -torsion[j][i]
 * @note Diagonal zero: torsion[i][i] = 0
 * @note Only 15 independent components (upper triangle)
 * 
 * @complexity O(K·d²) where K = centers, d = dimension (216 ops for 6D)
 * 
 * @see TCDE_TopologicalTorsion() for scalar contraction
 * @see TCDE_TorsionMagnitude() for Frobenius norm
 * @see TCDE_VisualizeTorsion() for visualization
 * 
 * @par Example:
 * @code
 * // Compute full torsion tensor for analysis
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Allocate torsion tensor
 * TCDE_Complex torsion[6][6];
 * 
 * // Compute all components
 * TCDE_ComputeTorsionTensor(field, &point, torsion);
 * 
 * // Analyze spatial vorticity (XY plane)
 * printf("Spatial torsion T_xy = %.6f + %.6fi\n",
 *        crealf(torsion[0][1]), cimagf(torsion[0][1]));
 * 
 * // Verify antisymmetry
 * printf("T_xy = %.6f, T_yx = %.6f\n",
 *        crealf(torsion[0][1]), crealf(torsion[1][0]));
 * printf("Sum (should be 0): %.6f\n",
 *        crealf(torsion[0][1] + torsion[1][0]));
 * 
 * // Find strongest torsion component
 * float max_torsion = 0.0f;
 * int max_i = 0, max_j = 0;
 * for (int i = 0; i < 6; i++) {
 *     for (int j = i+1; j < 6; j++) {
 *         float mag = cabsf(torsion[i][j]);
 *         if (mag > max_torsion) {
 *             max_torsion = mag;
 *             max_i = i;
 *             max_j = j;
 *         }
 *     }
 * }
 * printf("Strongest torsion: T_%d%d = %.6f\n", 
 *        max_i, max_j, max_torsion);
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeTorsionTensor(const TCDE_Field* field, const TCDE_Point* point,
                                TCDE_Complex torsion[6][6]);

/**
 * @brief Compute torsion magnitude using Frobenius norm
 * 
 * Computes the Frobenius norm (total magnitude) of the torsion tensor,
 * which provides a single scalar measure of total rotational strength
 * at a point. This is useful for:
 * - Comparing torsion across different points
 * - Identifying high-torsion regions
 * - Thresholding for intuitive leap detection
 * - Visualization and analysis
 * 
 * Formula: ||T|| = √(Σᵢⱼ |T_ij|²)
 * 
 * This is the Frobenius norm (matrix 2-norm) applied to the complex
 * torsion tensor. It measures the total "amount" of torsion regardless
 * of direction.
 * 
 * Properties:
 * - Always non-negative: ||T|| ≥ 0
 * - Zero iff T = 0 (no torsion)
 * - Scale-invariant under rotations
 * - Additive for orthogonal components
 * 
 * Interpretation:
 * - ||T|| = 0: No rotation (gradient flow)
 * - ||T|| < 0.1: Weak rotation
 * - ||T|| > 0.1: Moderate rotation (intuitive region)
 * - ||T|| > 1.0: Strong rotation (creative leap region)
 * 
 * @param torsion 6×6 complex torsion tensor (must not be NULL)
 * @return Frobenius norm ||T|| ≥ 0
 * 
 * @note Input must be computed via TCDE_ComputeTorsionTensor
 * @note Accounts for both real and imaginary parts
 * @note Fast: O(d²) = O(36) for 6D
 * 
 * @complexity O(d²) where d = dimension (36 ops for 6D)
 * 
 * @see TCDE_ComputeTorsionTensor() to compute torsion tensor
 * @see TCDE_TopologicalTorsion() for scalar contraction
 * @see TCDE_VisualizeTorsion() for spatial visualization
 * 
 * @par Example:
 * @code
 * // Compute torsion magnitude for thresholding
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Compute torsion tensor
 * TCDE_Complex torsion[6][6];
 * TCDE_ComputeTorsionTensor(field, &point, torsion);
 * 
 * // Compute magnitude
 * float magnitude = TCDE_TorsionMagnitude(torsion);
 * 
 * printf("Torsion magnitude: %.6f\n", magnitude);
 * 
 * // Classify region
 * if (magnitude < 0.01f) {
 *     printf("Region type: Gradient flow (no intuition)\n");
 * } else if (magnitude < 0.1f) {
 *     printf("Region type: Weak rotation\n");
 * } else if (magnitude < 1.0f) {
 *     printf("Region type: Intuitive connections\n");
 * } else {
 *     printf("Region type: Creative leaps!\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_TorsionMagnitude(const TCDE_Complex torsion[6][6]);

/**
 * @brief Generate torsion magnitude heatmap and export to PGM image
 * 
 * Creates a 2D visualization of torsion magnitude by slicing through the
 * 6D manifold and computing ||T|| at each grid point. The result is
 * exported as a grayscale PGM image where brightness indicates torsion
 * strength.
 * 
 * This visualization helps identify:
 * - Intuitive leap regions (bright areas)
 * - Gradient flow regions (dark areas)
 * - Rotational structure patterns
 * - Creative connection pathways
 * 
 * The 6D space is sliced by:
 * 1. Selecting two axes for the 2D plane (e.g., X and Y)
 * 2. Fixing the other 4 coordinates at specified values
 * 3. Computing torsion magnitude at grid_size × grid_size points
 * 4. Normalizing to [0, 255] for grayscale image
 * 
 * PGM Format (Portable GrayMap):
 * - Type: P2 (ASCII grayscale)
 * - Values: 0 (black/no torsion) to 255 (white/high torsion)
 * - Easy to view with image viewers
 * - Simple text format for debugging
 * 
 * Common visualizations:
 * - Spatial (XY): axes 0,1 - Shows spatial vorticity
 * - Temporal (τ₁τ₂): axes 3,4 - Shows temporal dynamics
 * - Cross-modal (Xm): axes 0,5 - Shows space-modality coupling
 * 
 * @param field TCDE field (must not be NULL)
 * @param axis1 First axis for 2D slice (0-5: x,y,z,τ₁,τ₂,m)
 * @param axis2 Second axis for 2D slice (0-5, must differ from axis1)
 * @param slice_coords Fixed coordinates for other 4 dimensions [4]
 * @param grid_size Number of grid points per axis (recommended: 50-200)
 * @param filename Output PGM filename (e.g., "torsion_xy.pgm")
 * @return true on success, false on error (invalid params, file I/O)
 * 
 * @note Larger grid_size = higher resolution but slower computation
 * @note File can be viewed with any image viewer supporting PGM
 * @note Bright regions indicate high torsion (intuitive leaps)
 * @note Dark regions indicate low torsion (gradient flow)
 * 
 * @warning Computationally expensive for large grid_size
 * @warning Requires write permission for output file
 * 
 * @complexity O(grid_size² × K × d²) where K = centers, d = dimension
 * 
 * @see TCDE_ComputeTorsionTensor() for torsion computation
 * @see TCDE_TorsionMagnitude() for magnitude calculation
 * 
 * @par Example:
 * @code
 * // Visualize torsion in XY plane
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers with rotational structure ...
 * 
 * // Fix other coordinates: z=0.5, τ₁=1.0, τ₂=0.0, m=0.4
 * float slice_coords[4] = {0.5f, 1.0f, 0.0f, 0.4f};
 * 
 * // Generate 100×100 heatmap of XY plane (axes 0,1)
 * bool success = TCDE_VisualizeTorsion(field,
 *                                       0, 1,  // X and Y axes
 *                                       slice_coords,
 *                                       100,   // 100×100 grid
 *                                       "torsion_xy.pgm");
 * 
 * if (success) {
 *     printf("Torsion heatmap saved to torsion_xy.pgm\n");
 *     printf("View with: display torsion_xy.pgm\n");
 * }
 * 
 * // Visualize temporal dynamics (τ₁τ₂ plane)
 * float slice_coords2[4] = {0.5f, 0.5f, 0.5f, 0.4f};  // x,y,z,m fixed
 * TCDE_VisualizeTorsion(field, 3, 4, slice_coords2, 100,
 *                       "torsion_temporal.pgm");
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
bool TCDE_VisualizeTorsion(const TCDE_Field* field,
                            int axis1, int axis2,
                            const float slice_coords[4],
                            int grid_size,
                            const char* filename);

// ============================================================================
// CONTEXTUAL COUPLING
// ============================================================================

/**
 * @brief Compute contextual coupling - non-local field interaction
 * 
 * Computes the contextual coupling term, which integrates field values
 * over the entire manifold weighted by a geodesic distance kernel. This
 * creates non-local interactions that provide semantic coherence and
 * enable the field to "sense" distant structure.
 * 
 * Contextual coupling is CRITICAL for:
 * - Semantic coherence across the field
 * - Long-range correlations
 * - Holistic field behavior
 * - Preventing fragmentation
 * - Emergent global structure
 * 
 * Formula: C(p) = ∫_M K_g(p,q) Φ(q) dV_g(q)
 * 
 * where:
 * - K_g(p,q) = exp(-d_g(p,q)²/2σ²) is the geodesic Gaussian kernel
 * - d_g(p,q) is the geodesic distance
 * - σ is the coupling length scale
 * - dV_g is the volume element on the manifold
 * 
 * Integration method: Monte Carlo sampling
 * 1. Generate random points on manifold
 * 2. Evaluate kernel K_g(p,q) for each sample
 * 3. Evaluate field Φ(q) at each sample
 * 4. Accumulate weighted sum
 * 5. Normalize by volume
 * 
 * Physical interpretation:
 * - Each point "feels" the entire field
 * - Influence decreases exponentially with distance
 * - σ controls interaction range
 * - Creates field coherence
 * 
 * In TDE evolution:
 * ∂Φ/∂t = ... + γ·C(Φ) + ...
 * 
 * where γ controls coupling strength.
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param sigma Coupling length scale σ > 0 (typical: 0.1-0.5)
 *              - Small σ: local coupling
 *              - Large σ: global coupling
 * @param num_samples Number of Monte Carlo samples (recommended: 100-1000)
 *                    - More samples: higher accuracy, slower
 *                    - Fewer samples: faster, noisier
 * @return Coupling contribution C(p) as complex number
 * 
 * @note Convergence improves as √num_samples
 * @note Larger σ requires more samples for accuracy
 * @note Complex return preserves phase information
 * @note Computationally expensive - use sparingly
 * 
 * @warning Very expensive for large num_samples
 * @warning Requires good random sampling of manifold
 * 
 * @complexity O(num_samples × K) where K = number of centers
 * 
 * @see TCDE_CouplingKernel() for kernel computation
 * @see TCDE_GeodesicDistance() for distance computation
 * 
 * @par Example:
 * @code
 * // Compute contextual coupling for field evolution
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Compute coupling with moderate range
 * float sigma = 0.2f;        // Coupling length scale
 * int num_samples = 500;     // Monte Carlo samples
 * 
 * TCDE_Complex coupling = TCDE_ContextualCoupling(field, &point,
 *                                                  sigma, num_samples);
 * 
 * printf("Coupling magnitude: %.6f\n", cabsf(coupling));
 * printf("Coupling phase: %.6f rad\n", cargf(coupling));
 * 
 * // Compare local vs global coupling
 * TCDE_Complex local = TCDE_ContextualCoupling(field, &point, 0.1f, 500);
 * TCDE_Complex global = TCDE_ContextualCoupling(field, &point, 0.5f, 500);
 * 
 * printf("Local coupling:  %.6f\n", cabsf(local));
 * printf("Global coupling: %.6f\n", cabsf(global));
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
TCDE_Complex TCDE_ContextualCoupling(const TCDE_Field* field, const TCDE_Point* point,
                                      float sigma, int num_samples);

/**
 * @brief Compute geodesic Gaussian coupling kernel
 * 
 * Computes the geodesic Gaussian kernel K(p,q) which determines the
 * strength of interaction between two points based on their geodesic
 * distance. This is the fundamental weighting function for contextual
 * coupling in TCDE.
 * 
 * Formula: K(p,q) = exp(-d_g(p,q)²/2σ²)
 * 
 * where:
 * - d_g(p,q) is the geodesic distance on the Riemannian manifold
 * - σ is the length scale parameter
 * 
 * Properties:
 * - K(p,p) = 1 (maximum at zero distance)
 * - K(p,q) → 0 as d_g(p,q) → ∞ (exponential decay)
 * - Symmetric: K(p,q) = K(q,p)
 * - Smooth and differentiable
 * - Respects manifold geometry
 * 
 * Geometric interpretation:
 * - σ controls interaction range
 * - Small σ: local interactions only
 * - Large σ: long-range interactions
 * - Kernel decays to ~0.01 at distance 3σ
 * 
 * Comparison with Euclidean kernel:
 * - Euclidean: K(p,q) = exp(-||p-q||²/2σ²)
 * - Geodesic: K(p,q) = exp(-d_g(p,q)²/2σ²)
 * - Geodesic respects curved geometry
 * - Geodesic accounts for metric structure
 * 
 * Applications:
 * - Contextual coupling in TDE
 * - Similarity computation
 * - Kernel methods on manifolds
 * - Gaussian processes on curved spaces
 * 
 * @param p1 First point (must not be NULL, dimension must match metric)
 * @param p2 Second point (must not be NULL, dimension must match metric)
 * @param metric Riemannian metric (if NULL, uses identity/Euclidean)
 * @param sigma Length scale σ > 0 (typical: 0.1-0.5)
 * @return Kernel value K(p1,p2) ∈ [0,1]
 * 
 * @note Returns value in [0,1] with K(p,p) = 1
 * @note Symmetric: K(p,q) = K(q,p)
 * @note Fast: O(d²) for general metric, O(d) for block-diagonal
 * @note For repeated calls, consider caching distances
 * 
 * @complexity O(d²) where d = dimension (36 ops for 6D)
 *             O(10) for block-diagonal metric
 * 
 * @see TCDE_GeodesicDistance() for distance computation
 * @see TCDE_ContextualCoupling() for usage in coupling
 * @see TCDE_CrossModalSimilarity() for similarity computation
 * 
 * @par Example:
 * @code
 * // Compute kernel values for different distances
 * TCDE_Metric metric = TCDE_CreateMetric(6);
 * float sigma = 0.2f;
 * 
 * // Point at origin
 * float coords1[6] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 * TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
 * 
 * // Test points at various distances
 * float distances[] = {0.0f, 0.1f, 0.2f, 0.4f, 0.6f};
 * 
 * printf("Kernel values (σ=%.2f):\n", sigma);
 * for (int i = 0; i < 5; i++) {
 *     float coords2[6] = {distances[i], 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
 *     TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
 *     
 *     float kernel = TCDE_CouplingKernel(&p1, &p2, &metric, sigma);
 *     printf("  d=%.2f: K=%.6f\n", distances[i], kernel);
 *     
 *     TCDE_FreePoint(&p2);
 * }
 * // Output:
 * //   d=0.00: K=1.000000 (self-interaction)
 * //   d=0.10: K=0.778801 (strong)
 * //   d=0.20: K=0.606531 (moderate)
 * //   d=0.40: K=0.135335 (weak)
 * //   d=0.60: K=0.011109 (negligible)
 * 
 * TCDE_FreePoint(&p1);
 * TCDE_FreeMetric(&metric);
 * @endcode
 */
float TCDE_CouplingKernel(const TCDE_Point* p1, const TCDE_Point* p2,
                          const TCDE_Metric* metric, float sigma);

// ============================================================================
// ADAPTIVE METRIC
// ============================================================================

/**
 * @brief Adapt metric tensor based on field energy distribution
 * 
 * Dynamically adapts the metric tensor based on local field energy density,
 * creating a feedback loop between field structure and geometry. This is
 * THE mechanism that enables self-organization and emergent complexity in TCDE.
 * 
 * Adaptive metric principle:
 * - High energy regions → contract metric → slower dynamics → concentration
 * - Low energy regions → expand metric → faster dynamics → dispersion
 * 
 * This creates a self-regulating system where:
 * - Field attracts to high-energy regions
 * - Dynamics slow near attractors (stability)
 * - Dynamics accelerate in low-energy regions (exploration)
 * - Emergent structure forms naturally
 * 
 * Formula: g_ij(x) = g⁰_ij · f(ρ(x))
 * 
 * where:
 * - g⁰_ij is the base metric
 * - ρ(x) = |Φ(x)|² is the energy density
 * - f(ρ) = 1 + α·tanh(β·ρ) is the adaptation function
 * 
 * Parameters:
 * - α controls adaptation strength (typical: 0.1-0.5)
 * - β controls adaptation sensitivity (typical: 1.0-10.0)
 * 
 * Properties maintained:
 * - Positive definiteness: g_ij remains positive definite
 * - Smoothness: continuous adaptation
 * - Symmetry: g_ij = g_ji preserved
 * - Block-diagonal structure: preserved for efficiency
 * 
 * In TCDE evolution:
 * The adapted metric feeds back into:
 * - Geodesic distances
 * - Laplace-Beltrami operator
 * - Christoffel symbols
 * - Curvature tensors
 * 
 * Creating a complete feedback loop:
 * Φ → ρ → g(ρ) → Γ(g) → ∂Φ/∂t → Φ'
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param metric Output adapted metric (must not be NULL, will be modified)
 * 
 * @note Modifies metric in-place
 * @note Preserves positive definiteness
 * @note Maintains block-diagonal structure
 * @note Smooth adaptation (no discontinuities)
 * 
 * @warning Requires field to have non-zero energy
 * @warning May affect numerical stability if adaptation too strong
 * 
 * @complexity O(K + d²) where K = centers, d = dimension
 * 
 * @see TCDE_ComputeChristoffelAdaptive() for connection with adapted metric
 * @see TCDE_ComputeScalarCurvatureAdaptive() for curvature with adaptation
 * 
 * @par Example:
 * @code
 * // Demonstrate metric adaptation
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers creating energy concentration ...
 * 
 * // Point in high-energy region
 * float coords_high[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p_high = TCDE_CreatePoint(6, coords_high);
 * 
 * // Point in low-energy region
 * float coords_low[6] = {0.9f, 0.9f, 0.9f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point p_low = TCDE_CreatePoint(6, coords_low);
 * 
 * // Base metric
 * TCDE_Metric metric_high = TCDE_CreateMetric(6);
 * TCDE_Metric metric_low = TCDE_CreateMetric(6);
 * 
 * // Adapt metrics
 * TCDE_AdaptMetric(field, &p_high, &metric_high);
 * TCDE_AdaptMetric(field, &p_low, &metric_low);
 * 
 * // Compare metric components
 * printf("Metric adaptation:\n");
 * printf("  High energy: g_00 = %.6f (contracted)\n", 
 *        metric_high.components[0][0]);
 * printf("  Low energy:  g_00 = %.6f (expanded)\n",
 *        metric_low.components[0][0]);
 * 
 * // High energy should have smaller metric components (contraction)
 * // Low energy should have larger metric components (expansion)
 * 
 * TCDE_FreePoint(&p_high);
 * TCDE_FreePoint(&p_low);
 * TCDE_FreeMetric(&metric_high);
 * TCDE_FreeMetric(&metric_low);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_AdaptMetric(const TCDE_Field* field, const TCDE_Point* point, 
                      TCDE_Metric* metric);

/**
 * @brief Compute Christoffel symbols with field-adapted metric
 * 
 * Computes Christoffel symbols Γᵏᵢⱼ using a metric that has been adapted
 * to the field's energy distribution. Unlike static metrics, the adaptive
 * metric varies spatially, leading to non-zero metric derivatives and
 * genuine geometric curvature.
 * 
 * Formula: Γᵏᵢⱼ = (1/2) gᵏˡ [∂ᵢgⱼₗ + ∂ⱼgᵢₗ - ∂ₗgᵢⱼ]
 * 
 * Key difference from static metric:
 * - Static metric: ∂ᵢg_jk ≈ 0 → Γᵏᵢⱼ ≈ 0 (flat space)
 * - Adaptive metric: ∂ᵢg_jk ≠ 0 → Γᵏᵢⱼ ≠ 0 (curved space)
 * 
 * The metric derivatives arise from field energy gradients:
 * ∂ᵢg_jk = g⁰_jk · ∂ᵢf(ρ) = g⁰_jk · f'(ρ) · ∂ᵢρ
 * 
 * This creates genuine curvature that:
 * - Follows field structure
 * - Concentrates near high-energy regions
 * - Guides field evolution
 * - Enables self-organization
 * 
 * Applications:
 * - Adaptive field evolution
 * - Self-organizing dynamics
 * - Emergent structure formation
 * - Cognitive emergence mechanisms
 * 
 * @param field TCDE field (must not be NULL, provides energy distribution)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param base_metric Base metric before adaptation (must not be NULL)
 * @param christoffel Output Christoffel symbols Γᵏᵢⱼ [6][6][6] (pre-allocated)
 * 
 * @note christoffel array must be allocated: float christoffel[6][6][6]
 * @note Accounts for metric spatial variation
 * @note More expensive than static version (requires derivatives)
 * @note Essential for adaptive TDE evolution
 * 
 * @complexity O(K + d³) where K = centers, d = dimension
 * 
 * @see TCDE_AdaptMetric() for metric adaptation
 * @see TCDE_ComputeChristoffel() for static version
 * @see TCDE_ComputeRiemannAdaptive() for curvature
 * 
 * @par Example:
 * @code
 * // Compare static vs adaptive Christoffel symbols
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * TCDE_Metric base_metric = TCDE_CreateMetric(6);
 * 
 * // Static Christoffel (should be ~0 for identity metric)
 * float christoffel_static[6][6][6];
 * TCDE_ComputeChristoffel(&point, &base_metric, christoffel_static);
 * 
 * // Adaptive Christoffel (non-zero due to field)
 * float christoffel_adaptive[6][6][6];
 * TCDE_ComputeChristoffelAdaptive(field, &point, &base_metric,
 *                                  christoffel_adaptive);
 * 
 * // Compare
 * printf("Christoffel Γ⁰₀₀:\n");
 * printf("  Static:   %.6f\n", christoffel_static[0][0][0]);
 * printf("  Adaptive: %.6f\n", christoffel_adaptive[0][0][0]);
 * 
 * TCDE_FreePoint(&point);
 * TCDE_FreeMetric(&base_metric);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeChristoffelAdaptive(const TCDE_Field* field,
                                      const TCDE_Point* point,
                                      const TCDE_Metric* base_metric,
                                      float christoffel[6][6][6]);

/**
 * @brief Compute Riemann curvature tensor with field-adapted metric
 * 
 * Computes the complete Riemann curvature tensor R^i_jkl using an adaptive
 * metric that responds to field energy distribution. This produces genuine
 * geometric curvature that follows and reinforces field structure.
 * 
 * Formula: R^i_jkl = ∂_kΓ^i_jl - ∂_lΓ^i_jk + Γ^i_mk Γ^m_jl - Γ^i_ml Γ^m_jk
 * 
 * Key insight:
 * - Static metric → Flat space → R^i_jkl ≈ 0
 * - Adaptive metric → Curved space → R^i_jkl ≠ 0
 * 
 * The curvature arises from field energy gradients and creates:
 * - Attraction toward high-energy regions
 * - Repulsion from low-energy regions
 * - Geodesic focusing/defocusing
 * - Self-organizing dynamics
 * 
 * This is the geometric mechanism behind:
 * - Emergent structure formation
 * - Attractor dynamics
 * - Cognitive emergence
 * - Self-organization
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param base_metric Base metric before adaptation (must not be NULL)
 * @param riemann Output Riemann tensor R^i_jkl [6][6][6][6] (pre-allocated)
 * 
 * @note riemann array must be allocated: float riemann[6][6][6][6] (5.2 KB)
 * @note Very computationally expensive
 * @note Use sparingly (cache results when possible)
 * @note Essential for understanding emergent dynamics
 * 
 * @warning Extremely expensive computation
 * @warning Requires accurate Christoffel symbols
 * 
 * @complexity O(K + d⁴) where K = centers, d = dimension
 * 
 * @see TCDE_ComputeChristoffelAdaptive() for connection
 * @see TCDE_ComputeScalarCurvatureAdaptive() for simpler measure
 * @see TCDE_AdaptMetric() for metric adaptation
 * 
 * @par Example:
 * @code
 * // Analyze curvature induced by field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers creating energy concentration ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * TCDE_Metric base_metric = TCDE_CreateMetric(6);
 * 
 * // Allocate Riemann tensor
 * float riemann[6][6][6][6];
 * 
 * // Compute adaptive curvature
 * TCDE_ComputeRiemannAdaptive(field, &point, &base_metric, riemann);
 * 
 * // Find maximum curvature component
 * float max_curv = 0.0f;
 * for (int i = 0; i < 6; i++) {
 *     for (int j = 0; j < 6; j++) {
 *         for (int k = 0; k < 6; k++) {
 *             for (int l = 0; l < 6; l++) {
 *                 float R = fabsf(riemann[i][j][k][l]);
 *                 if (R > max_curv) max_curv = R;
 *             }
 *         }
 *     }
 * }
 * 
 * printf("Maximum curvature: %.6f\n", max_curv);
 * if (max_curv > 0.01f) {
 *     printf("Significant field-induced curvature!\n");
 * }
 * 
 * TCDE_FreePoint(&point);
 * TCDE_FreeMetric(&base_metric);
 * TCDE_DestroyField(field);
 * @endcode
 */
void TCDE_ComputeRiemannAdaptive(const TCDE_Field* field,
                                  const TCDE_Point* point,
                                  const TCDE_Metric* base_metric,
                                  float riemann[6][6][6][6]);

/**
 * @brief Compute scalar curvature with field-adapted metric
 * 
 * Computes the scalar curvature R using an adaptive metric, providing
 * a single number that measures total field-induced curvature at a point.
 * This is the simplest measure of how the field curves space.
 * 
 * Formula: R = g^ij R_ij where R_ij = R^k_ikj (Ricci tensor)
 * 
 * Interpretation with adaptive metric:
 * - R > 0: Field creates positive curvature (attractive)
 * - R = 0: Flat space (no field influence)
 * - R < 0: Field creates negative curvature (repulsive)
 * 
 * The scalar curvature directly relates to:
 * - Field energy concentration
 * - Attractor strength
 * - Geodesic focusing
 * - Self-organization tendency
 * 
 * In regions of high field energy:
 * - Metric contracts
 * - Curvature becomes positive
 * - Geodesics focus (attraction)
 * - Dynamics slow (stability)
 * 
 * In regions of low field energy:
 * - Metric expands
 * - Curvature becomes negative
 * - Geodesics defocus (repulsion)
 * - Dynamics accelerate (exploration)
 * 
 * @param field TCDE field (must not be NULL)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param base_metric Base metric before adaptation (must not be NULL)
 * @return Scalar curvature R (can be positive, negative, or zero)
 * 
 * @note Much cheaper than full Riemann tensor
 * @note Sufficient for many applications
 * @note Positive R indicates attraction
 * @note Negative R indicates repulsion
 * 
 * @complexity O(K + d³) where K = centers, d = dimension
 * 
 * @see TCDE_ComputeRiemannAdaptive() for complete curvature
 * @see TCDE_AdaptMetric() for metric adaptation
 * @see TCDE_ComputeScalarCurvature() for static version
 * 
 * @par Example:
 * @code
 * // Map scalar curvature across field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * TCDE_Metric base_metric = TCDE_CreateMetric(6);
 * 
 * printf("Scalar curvature profile:\n");
 * for (float x = 0.0f; x <= 1.0f; x += 0.2f) {
 *     float coords[6] = {x, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 *     TCDE_Point point = TCDE_CreatePoint(6, coords);
 *     
 *     float R = TCDE_ComputeScalarCurvatureAdaptive(field, &point,
 *                                                    &base_metric);
 *     printf("  x=%.1f: R = %+.6f ", x, R);
 *     
 *     if (R > 0.01f) {
 *         printf("(attractive region)\n");
 *     } else if (R < -0.01f) {
 *         printf("(repulsive region)\n");
 *     } else {
 *         printf("(approximately flat)\n");
 *     }
 *     
 *     TCDE_FreePoint(&point);
 * }
 * 
 * TCDE_FreeMetric(&base_metric);
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_ComputeScalarCurvatureAdaptive(const TCDE_Field* field,
                                           const TCDE_Point* point,
                                           const TCDE_Metric* base_metric);

// ============================================================================
// TORSION TENSOR
// ============================================================================

/**
 * @brief Compute complete torsion tensor T^i_jk
 * 
 * Formula: T^i_jk = Γ^i_jk - Γ^i_kj (antisymmetric in lower indices)
 * 
 * The torsion tensor captures non-commutativity of covariant derivatives
 * and represents "twisting" of the manifold.
 * 
 * @param field Field (for computing Christoffel symbols)
 * @param point Point where to compute
 * @param metric Metric
 */

/**
 * @brief Invert 6×6 metric tensor using LU decomposition
 * 
 * Computes the inverse metric g^ij from the covariant metric g_ij using
 * LU decomposition with partial pivoting. The inverse metric is essential
 * for raising indices and computing geometric quantities.
 * 
 * The inverse metric satisfies: g^ik g_kj = δ^i_j (Kronecker delta)
 * 
 * Applications:
 * - Computing Christoffel symbols: Γ^k_ij = (1/2) g^kl [...]
 * - Raising tensor indices: V^i = g^ij V_j
 * - Computing scalar curvature: R = g^ij R_ij
 * - Geodesic distance: d² = g^ij dx_i dx_j
 * 
 * Algorithm: LU decomposition with partial pivoting
 * 1. Decompose g = PLU (permutation, lower, upper)
 * 2. Solve Ly = Pb for each column b of identity
 * 3. Solve Ux = y to get inverse columns
 * 
 * Properties:
 * - Numerically stable (partial pivoting)
 * - Handles near-singular matrices
 * - Preserves symmetry of inverse
 * - Efficient: O(d³) = O(216) for 6D
 * 
 * @param g Input covariant metric tensor g_ij [6][6] (must not be NULL)
 * @param g_inv Output contravariant metric g^ij [6][6] (must not be NULL)
 * @return true if inversion successful, false if matrix is singular
 * 
 * @note g must be symmetric and positive definite
 * @note g_inv will also be symmetric
 * @note Returns false if det(g) ≈ 0 (singular matrix)
 * @note Uses tolerance of 1e-10 for singularity detection
 * 
 * @warning Fails if metric is singular or nearly singular
 * @warning Numerical errors accumulate for ill-conditioned matrices
 * 
 * @complexity O(d³) where d = dimension (216 ops for 6D)
 * 
 * @see TCDE_CreateMetric() for metric creation
 * @see TCDE_ComputeChristoffel() for usage in connection
 * 
 * @par Example:
 * @code
 * // Invert metric and verify
 * float g[6][6];
 * // ... initialize metric (e.g., identity or field metric) ...
 * 
 * float g_inv[6][6];
 * bool success = TCDE_InvertMetric6x6(g, g_inv);
 * 
 * if (success) {
 *     printf("Metric inverted successfully\n");
 *     
 *     // Verify: g^ik g_kj should equal δ^i_j
 *     printf("Verification (should be identity):\n");
 *     for (int i = 0; i < 3; i++) {
 *         for (int j = 0; j < 3; j++) {
 *             float sum = 0.0f;
 *             for (int k = 0; k < 6; k++) {
 *                 sum += g_inv[i][k] * g[k][j];
 *             }
 *             printf("  %.3f", sum);
 *         }
 *         printf("\n");
 *     }
 * } else {
 *     printf("ERROR: Metric is singular!\n");
 * }
 * @endcode
 */
bool TCDE_InvertMetric6x6(const float g[6][6], float g_inv[6][6]);

/**
 * @brief Compute metric derivative using finite differences
 * 
 * Computes the partial derivative ∂g_ij/∂x^k of the metric tensor
 * component g_ij with respect to coordinate x^k using finite differences.
 * These derivatives are essential for computing Christoffel symbols and
 * curvature tensors.
 * 
 * Formula: ∂g_ij/∂x^k ≈ [g_ij(x + h·e_k) - g_ij(x - h·e_k)] / (2h)
 * 
 * where:
 * - h is the finite difference step size (typically 1e-5)
 * - e_k is the unit vector in direction k
 * 
 * For adaptive metrics:
 * ∂g_ij/∂x^k = g⁰_ij · ∂f(ρ)/∂x^k = g⁰_ij · f'(ρ) · ∂ρ/∂x^k
 * 
 * where ρ = |Φ|² is the field energy density.
 * 
 * Applications:
 * - Computing Christoffel symbols
 * - Computing Riemann curvature
 * - Analyzing metric variation
 * - Adaptive metric evolution
 * 
 * @param field TCDE field (for field-dependent metric, can be NULL for static)
 * @param point Evaluation point (must not be NULL, dimension 6)
 * @param i First metric index (0-5)
 * @param j Second metric index (0-5)
 * @param k Derivative direction (0-5: x,y,z,τ₁,τ₂,m)
 * @return ∂g_ij/∂x^k
 * 
 * @note For static metrics: returns 0 (no spatial variation)
 * @note For adaptive metrics: returns non-zero (field-dependent)
 * @note Uses central differences for accuracy
 * @note Step size h = 1e-5 (balance accuracy vs numerical error)
 * 
 * @complexity O(K) where K = number of centers (if field-dependent)
 *             O(1) for static metrics
 * 
 * @see TCDE_ComputeChristoffel() for usage
 * @see TCDE_AdaptMetric() for adaptive metrics
 * 
 * @par Example:
 * @code
 * // Compute metric derivatives for adaptive metric
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 * TCDE_Point point = TCDE_CreatePoint(6, coords);
 * 
 * // Compute ∂g_00/∂x (derivative of g_xx in x-direction)
 * float deriv_x = TCDE_ComputeMetricDerivative(field, &point, 0, 0, 0);
 * printf("∂g_00/∂x = %.6f\n", deriv_x);
 * 
 * // Compute all derivatives of g_00
 * printf("Derivatives of g_00:\n");
 * const char* names[] = {"x", "y", "z", "τ₁", "τ₂", "m"};
 * for (int k = 0; k < 6; k++) {
 *     float deriv = TCDE_ComputeMetricDerivative(field, &point, 0, 0, k);
 *     printf("  ∂g_00/∂%s = %.6f\n", names[k], deriv);
 * }
 * 
 * // For static metric, all should be ~0
 * // For adaptive metric, derivatives follow field gradients
 * 
 * TCDE_FreePoint(&point);
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_ComputeMetricDerivative(const TCDE_Field* field,
                                   const TCDE_Point* point,
                                   int i, int j, int k);

#ifdef __cplusplus
}
#endif

#endif // TCDE_GEOMETRY_H
