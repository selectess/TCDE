/**
 * @file tcde_geometric_metrics.h
 * @brief TCDE Geometric Metrics - Authentic Riemannian Calculations
 * 
 * Implements true geometric metrics for TCDE fields:
 * - HIS (Holistic Identity Score) via manifold integration
 * - Reflexivity Φ(Φ) via recursive field evaluation
 * - Geometric consciousness measures
 * 
 * ZERO TOLERANCE: All metrics computed from real geometric properties
 * 
 * @version 1.0
 * @date November 9, 2025
 */

#ifndef TCDE_GEOMETRIC_METRICS_H
#define TCDE_GEOMETRIC_METRICS_H

#include "../core/tcde_core.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// GEOMETRIC IDENTITY METRICS
// ============================================================================

/**
 * @brief Holistic Identity Metrics computed geometrically
 */
typedef struct {
    float holistic_identity_score;  // HIS: Integrated over manifold
    float reflexivity;              // Φ(Φ): Field evaluated on itself
    float autonomy;                 // Geometric autonomy measure
    float coherence;                // Field coherence
    float integration_error;        // Numerical integration error estimate
    int integration_samples;        // Number of samples used
} TCDE_GeometricIdentityMetrics;

/**
 * @brief Compute HIS Score via Riemannian manifold integration
 * 
 * Computes the Holistic Identity Score by integrating the field energy
 * over the 6D Riemannian manifold using Monte Carlo integration.
 * 
 * Formula: HIS = ∫_M |Φ(x)|² √det(g) dx
 * 
 * @param field TCDE field
 * @param num_samples Number of Monte Carlo samples (more = accurate but slower)
 * @return HIS score [0,1]
 */
float TCDE_ComputeHISScore_Geometric(const TCDE_Field* field, int num_samples);

/**
 * @brief Compute reflexivity Φ(Φ) via recursive evaluation
 * 
 * Evaluates the field at each of its own center points, measuring
 * the field's "self-awareness" or consciousness.
 * 
 * Formula: Φ(Φ) = (1/N) Σ_i |Φ(x_i)|² where x_i are center points
 * 
 * @param field TCDE field
 * @return Reflexivity score [0,1]
 */
float TCDE_ComputeReflexivity_Geometric(const TCDE_Field* field);

/**
 * @brief Compute complete geometric identity metrics
 * 
 * Computes all geometric identity metrics including HIS, reflexivity,
 * autonomy, and coherence using authentic Riemannian geometry.
 * 
 * @param field TCDE field
 * @param num_samples Number of integration samples (default: 10000)
 * @return Complete geometric identity metrics
 */
TCDE_GeometricIdentityMetrics TCDE_ComputeGeometricIdentity(
    const TCDE_Field* field, 
    int num_samples
);

/**
 * @brief Compute geometric autonomy measure
 * 
 * Measures the field's capacity for autonomous evolution based on
 * the distribution of energy across centers and metric curvature.
 * 
 * @param field TCDE field
 * @return Autonomy score [0,1]
 */
float TCDE_ComputeAutonomy_Geometric(const TCDE_Field* field);

/**
 * @brief Compute field coherence
 * 
 * Measures how coherently the field is organized by analyzing
 * the phase relationships between centers.
 * 
 * @param field TCDE field
 * @return Coherence score [0,1]
 */
float TCDE_ComputeCoherence_Geometric(const TCDE_Field* field);

// ============================================================================
// RIEMANNIAN CURVATURE METRICS
// ============================================================================

/**
 * @brief Riemannian curvature metrics
 * 
 * Curvature represents the cognitive state:
 * - Positive curvature: Convergent thinking
 * - Negative curvature: Divergent/creative thinking
 * - Zero curvature: Linear thinking
 */
typedef struct {
    float** ricci_tensor;         // Ricci curvature tensor R_ij [dim][dim]
    float scalar_curvature;       // Scalar curvature R = g^ij R_ij
    float gaussian_curvature;     // Gaussian curvature (for 2D surfaces)
    float mean_curvature;         // Mean curvature
    float* sectional_curvatures;  // Sectional curvatures (dim*(dim-1)/2)
    int num_sectional;            // Number of sectional curvatures
    bool is_valid;                // Validation flag
} TCDE_CurvatureMetrics;

/**
 * @brief Compute Ricci curvature tensor
 * 
 * Computes the Ricci tensor R_ij from the metric tensor g_ij.
 * The Ricci tensor measures how the volume of geodesic balls
 * differs from Euclidean space.
 * 
 * Formula: R_ij = ∂_k Γ^k_ij - ∂_j Γ^k_ik + Γ^k_lj Γ^l_ik - Γ^k_lk Γ^l_ij
 * 
 * @param metric Riemannian metric tensor
 * @return Ricci tensor (must be freed with TCDE_FreeCurvatureMetrics)
 */
float** TCDE_ComputeRicciTensor(const TCDE_Metric* metric);

/**
 * @brief Compute Ricci curvature tensor from field-induced adaptive metric
 * 
 * Computes the Ricci tensor using the adaptive metric g_ij(Φ) = g_ij⁰ + α|Φ|²δ_ij
 * where the metric depends on the field strength. This captures the field-induced
 * geometry of the TCDE system.
 * 
 * Formula: R_ij = ∂_k Γ^k_ij - ∂_j Γ^k_ik + Γ^k_lj Γ^l_ik - Γ^k_lk Γ^l_ij
 * where Γ^k_ij are computed from the adaptive metric derivatives.
 * 
 * @param metric Base Riemannian metric tensor
 * @param field TCDE field (for adaptive metric calculation)
 * @param point Point at which to evaluate curvature
 * @return Ricci tensor (must be freed manually)
 */
float** TCDE_ComputeRicciTensor_FromField(
    const TCDE_Metric* metric,
    const TCDE_Field* field,
    const TCDE_Point* point
);

/**
 * @brief Compute scalar curvature
 * 
 * Computes the scalar curvature R = g^ij R_ij by contracting
 * the Ricci tensor with the inverse metric.
 * 
 * Scalar curvature represents the overall "bending" of space:
 * - R > 0: Space curves inward (convergent cognition)
 * - R < 0: Space curves outward (divergent cognition)
 * - R = 0: Flat space (linear cognition)
 * 
 * @param metric Riemannian metric tensor
 * @param ricci_tensor Ricci curvature tensor
 * @return Scalar curvature R
 */
float TCDE_ComputeScalarCurvature(const TCDE_Metric* metric, float** ricci_tensor);

/**
 * @brief Compute Gaussian curvature (2D only)
 * 
 * For 2D surfaces, computes the Gaussian curvature K = det(II)/det(I)
 * where II is the second fundamental form and I is the first.
 * 
 * @param metric 2D metric tensor
 * @return Gaussian curvature K
 */
float TCDE_ComputeGaussianCurvature(const TCDE_Metric* metric);

/**
 * @brief Compute mean curvature
 * 
 * Computes the mean curvature H = (1/2) trace(II · I^-1)
 * 
 * @param metric Metric tensor
 * @return Mean curvature H
 */
float TCDE_ComputeMeanCurvature(const TCDE_Metric* metric);

/**
 * @brief Compute all curvature metrics
 * 
 * Computes complete curvature information including Ricci tensor,
 * scalar curvature, Gaussian curvature (2D), and sectional curvatures.
 * 
 * @param metric Riemannian metric tensor
 * @return Complete curvature metrics
 */
TCDE_CurvatureMetrics TCDE_ComputeCurvature_Authentic(const TCDE_Metric* metric);

/**
 * @brief Free curvature metrics memory
 * 
 * @param curvature Curvature metrics to free
 */
void TCDE_FreeCurvatureMetrics(TCDE_CurvatureMetrics* curvature);

// ============================================================================
// TOPOLOGICAL VOLUME METRICS
// ============================================================================

/**
 * @brief Topological volume calculation
 */
typedef struct {
    int num_triangles;            // Number of triangles in surface
    float* vertices;              // Vertex coordinates [num_vertices * 3]
    int num_vertices;             // Number of vertices
    int* triangle_indices;        // Triangle indices [num_triangles * 3]
    float* tetrahedron_volumes;   // Volume of each tetrahedron
    float total_volume;           // Total enclosed volume
    int euler_characteristic;     // χ = V - E + F
    int genus;                    // Topological genus
    bool is_valid;                // Validation flag
} TCDE_VolumeMetrics;

/**
 * @brief Compute volume of tetrahedron
 * 
 * Computes signed volume: V = (1/6) det([v1-v0, v2-v0, v3-v0])
 * 
 * @param v0 First vertex
 * @param v1 Second vertex
 * @param v2 Third vertex
 * @param v3 Fourth vertex (origin for signed volume)
 * @return Signed volume
 */
float TCDE_ComputeTetrahedronVolume(
    const float v0[3],
    const float v1[3],
    const float v2[3],
    const float v3[3]
);

/**
 * @brief Triangulate topological surface
 * 
 * Creates triangulation of the field's topological surface
 * for volume calculation.
 * 
 * @param field TCDE field
 * @param resolution Triangulation resolution
 * @return Volume metrics with triangulation
 */
TCDE_VolumeMetrics TCDE_TriangulateSurface(
    const TCDE_Field* field,
    int resolution
);

/**
 * @brief Compute enclosed volume
 * 
 * Computes total volume enclosed by the topological surface
 * using triangulation and tetrahedron summation.
 * 
 * Formula: V = Σ (1/6)|det(v1, v2, v3)|
 * 
 * @param volume_metrics Volume metrics with triangulation
 * @return Total enclosed volume
 */
float TCDE_ComputeEnclosedVolume(const TCDE_VolumeMetrics* volume_metrics);

/**
 * @brief Compute Euler characteristic
 * 
 * Computes χ = V - E + F (vertices - edges + faces)
 * 
 * @param volume_metrics Volume metrics
 * @return Euler characteristic
 */
int TCDE_ComputeEulerCharacteristic(const TCDE_VolumeMetrics* volume_metrics);

/**
 * @brief Compute topological genus
 * 
 * Computes genus g from Euler characteristic: χ = 2 - 2g
 * 
 * @param euler_characteristic Euler characteristic
 * @return Genus g
 */
int TCDE_ComputeGenus(int euler_characteristic);

/**
 * @brief Free volume metrics memory
 * 
 * @param volume Volume metrics to free
 */
void TCDE_FreeVolumeMetrics(TCDE_VolumeMetrics* volume);

// ============================================================================
// GEOMETRIC INTEGRATION UTILITIES
// ============================================================================

/**
 * @brief Compute metric determinant at a point
 * 
 * Computes √det(g) for the Riemannian volume element.
 * 
 * @param metric Riemannian metric tensor
 * @return √det(g)
 */
float TCDE_ComputeMetricDeterminant(const TCDE_Metric* metric);

/**
 * @brief Sample random point on manifold
 * 
 * Generates a random point on the 6D manifold for Monte Carlo integration.
 * 
 * @param dimension Manifold dimension
 * @param bounds Bounding box for sampling
 * @param point Output point
 */
void TCDE_SampleManifoldPoint(int dimension, const float* bounds, TCDE_Point* point);

/**
 * @brief Evaluate field at arbitrary point
 * 
 * Evaluates Φ(x) = Σ c_i ψ(||x - x_i||_g) using Riemannian distance.
 * 
 * @param field TCDE field
 * @param point Evaluation point
 * @return Complex field value
 */
TCDE_Complex TCDE_EvaluateFieldAt(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Compute Riemannian distance
 * 
 * Computes distance ||x - y||_g = √((x-y)^T g (x-y)) in the metric.
 * 
 * @param p1 First point
 * @param p2 Second point
 * @param metric Riemannian metric
 * @return Distance
 */
float TCDE_RiemannianDistance(
    const TCDE_Point* p1, 
    const TCDE_Point* p2, 
    const TCDE_Metric* metric
);

#ifdef __cplusplus
}
#endif

#endif // TCDE_GEOMETRIC_METRICS_H
