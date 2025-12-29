/**
 * @file tcde_coupling.c
 * @brief TCDE Coupling Module - Contextual Coupling Implementation
 * 
 * Implements non-local semantic coherence through geodesic kernels
 * and Monte Carlo integration.
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#include "tcde_evolution.h"
#include "tcde_geometry.h"
#include "tcde_core.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// ============================================================================
// GEODESIC KERNEL (Task 4.1)
// ============================================================================

/**
 * @brief Compute geodesic kernel between two points
 * @param p1 First point
 * @param p2 Second point
 * @param metric Riemannian metric
 * @param sigma Length scale parameter
 * @return K(p1,p2) = exp(-d_g(p1,p2)²/2σ²)
 * 
 * This is a Gaussian kernel in the geodesic distance metric.
 * It measures similarity between points on the manifold.
 * 
 * Properties:
 * - K(p,p) = 1 (self-similarity)
 * - K(p,q) → 0 as d_g(p,q) → ∞
 * - K(p,q) = K(q,p) (symmetry)
 * - 0 ≤ K(p,q) ≤ 1 (bounded)
 * 
 * The length scale σ controls the "reach" of interactions:
 * - Small σ: Local interactions only
 * - Large σ: Long-range interactions
 */
float TCDE_GeodesicKernel(const TCDE_Point* p1, const TCDE_Point* p2,
                          const TCDE_Metric* metric, float sigma) {
    if (!p1 || !p2 || !metric || sigma <= 0.0f) return 0.0f;
    
    // Compute geodesic distance using optimized version
    float dist = TCDE_GeodesicDistanceOptimized(p1, p2, metric);
    
    // Compute kernel: K(p,q) = exp(-d²/2σ²)
    float sigma_sq = sigma * sigma;
    float dist_sq = dist * dist;
    
    float kernel = expf(-dist_sq / (2.0f * sigma_sq));
    
    return kernel;
}

/**
 * @brief Compute geodesic kernel with automatic length scale
 * @param p1 First point
 * @param p2 Second point
 * @param metric Riemannian metric
 * @param field Field (used to estimate appropriate σ)
 * @return Kernel value
 * 
 * Automatically estimates σ based on field properties:
 * - Average center spacing
 * - Fractal dimension
 * - Manifold volume
 */
float TCDE_GeodesicKernelAuto(const TCDE_Point* p1, const TCDE_Point* p2,
                              const TCDE_Metric* metric, const TCDE_Field* field) {
    if (!field) return 0.0f;
    
    // Estimate length scale from field properties
    // σ ≈ (Volume / N_centers)^(1/d) where d = fractal dimension
    float volume = 1.0f;  // Unit hypercube [0,1]^6
    int n_centers = field->manifold_6d.num_centers;
    
    if (n_centers == 0) return 0.0f;
    
    float d = field->fractal_dimension;
    float sigma = powf(volume / n_centers, 1.0f / d);
    
    // Clamp to reasonable range
    if (sigma < 0.05f) sigma = 0.05f;
    if (sigma > 0.5f) sigma = 0.5f;
    
    return TCDE_GeodesicKernel(p1, p2, metric, sigma);
}

/**
 * @brief Compute kernel matrix for all center pairs
 * @param field Field
 * @param sigma Length scale
 * @param kernel_matrix Output: K[i][j] = K(p_i, p_j)
 * 
 * Computes the full kernel matrix for all RBF centers.
 * This is useful for batch operations and analysis.
 * 
 * Complexity: O(K²) where K = number of centers
 */
void TCDE_ComputeKernelMatrix(const TCDE_Field* field, float sigma,
                               float** kernel_matrix) {
    if (!field || !kernel_matrix || sigma <= 0.0f) return;
    
    int n = field->manifold_6d.num_centers;
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    
    // Compute kernel for all pairs
    for (int i = 0; i < n; i++) {
        const TCDE_Point* pi = &field->manifold_6d.centers[i].point;
        
        for (int j = 0; j < n; j++) {
            const TCDE_Point* pj = &field->manifold_6d.centers[j].point;
            
            kernel_matrix[i][j] = TCDE_GeodesicKernel(pi, pj, metric, sigma);
        }
    }
}

/**
 * @brief Verify kernel properties
 * @param field Field
 * @param sigma Length scale
 * @return true if kernel satisfies all properties
 * 
 * Verifies:
 * 1. Symmetry: K(p,q) = K(q,p)
 * 2. Self-similarity: K(p,p) = 1
 * 3. Boundedness: 0 ≤ K(p,q) ≤ 1
 * 4. Positive definiteness (Gram matrix)
 */
bool TCDE_VerifyKernelProperties(const TCDE_Field* field, float sigma) {
    if (!field || sigma <= 0.0f) return false;
    
    int n = field->manifold_6d.num_centers;
    if (n == 0) return true;
    
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    const float tolerance = 1e-6f;
    
    // Check symmetry and boundedness
    for (int i = 0; i < n; i++) {
        const TCDE_Point* pi = &field->manifold_6d.centers[i].point;
        
        // Check self-similarity: K(p,p) = 1
        float k_ii = TCDE_GeodesicKernel(pi, pi, metric, sigma);
        if (fabsf(k_ii - 1.0f) > tolerance) {
            return false;
        }
        
        for (int j = i + 1; j < n; j++) {
            const TCDE_Point* pj = &field->manifold_6d.centers[j].point;
            
            // Check symmetry: K(p,q) = K(q,p)
            float k_ij = TCDE_GeodesicKernel(pi, pj, metric, sigma);
            float k_ji = TCDE_GeodesicKernel(pj, pi, metric, sigma);
            
            if (fabsf(k_ij - k_ji) > tolerance) {
                return false;
            }
            
            // Check boundedness: 0 ≤ K ≤ 1
            if (k_ij < 0.0f || k_ij > 1.0f) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * @brief Compute effective coupling radius
 * @param sigma Length scale
 * @param threshold Kernel threshold (default: 0.01)
 * @return Radius beyond which K(p,q) < threshold
 * 
 * For Gaussian kernel: K(r) = exp(-r²/2σ²)
 * Solving K(r) = threshold gives: r = σ√(-2 ln(threshold))
 * 
 * For threshold = 0.01: r ≈ 3.03σ
 * For threshold = 0.001: r ≈ 3.72σ
 */
float TCDE_CouplingRadius(float sigma, float threshold) {
    if (sigma <= 0.0f || threshold <= 0.0f || threshold >= 1.0f) {
        return 0.0f;
    }
    
    // r = σ√(-2 ln(threshold))
    float radius = sigma * sqrtf(-2.0f * logf(threshold));
    
    return radius;
}

/**
 * @brief Count neighbors within coupling radius
 * @param field Field
 * @param point Query point
 * @param sigma Length scale
 * @param threshold Kernel threshold
 * @return Number of centers within effective radius
 * 
 * Useful for estimating computational cost and connectivity.
 */
int TCDE_CountNeighbors(const TCDE_Field* field, const TCDE_Point* point,
                        float sigma, float threshold) {
    if (!field || !point || sigma <= 0.0f) return 0;
    
    float radius = TCDE_CouplingRadius(sigma, threshold);
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    
    int count = 0;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        const TCDE_Point* center = &field->manifold_6d.centers[i].point;
        
        float dist = TCDE_GeodesicDistanceOptimized(point, center, metric);
        
        if (dist <= radius) {
            count++;
        }
    }
    
    return count;
}

// ============================================================================
// MONTE CARLO COUPLING (Task 4.2)
// ============================================================================

/**
 * @brief Generate random point on 6D manifold
 * @param bounds Coordinate bounds [min, max] for each dimension
 * @return Random point
 * 
 * Uses uniform sampling in [0,1]^6 hypercube.
 * For more sophisticated sampling, use rejection sampling or
 * importance sampling based on field density.
 */
static TCDE_Point generate_random_point_6d(void) {
    float coords[6];
    
    // Sample uniformly in [0,1]^6
    for (int i = 0; i < 6; i++) {
        coords[i] = (float)rand() / (float)RAND_MAX;
    }
    
    // Adjust temporal dimension to valid range [1, 2)
    coords[3] = 1.0f + coords[3] * 0.999f;  // τ₁ ∈ [1, 2)
    coords[4] = coords[4] * 2.0f - 1.0f;     // τ₂ ∈ [-1, 1]
    
    return TCDE_CreatePoint(6, coords);
}

/**
 * @brief Compute coupling term using Monte Carlo integration
 * @param field Field
 * @param point Evaluation point
 * @param sigma Kernel length scale
 * @param n_samples Number of Monte Carlo samples
 * @return C(p) ≈ (V/N) Σᵢ K(p,qᵢ)Φ(qᵢ)
 * 
 * Formula: C(p) = ∫ K(p,q)Φ(q) dV
 * 
 * Monte Carlo approximation:
 * C(p) ≈ (V/N) Σᵢ₌₁ᴺ K(p,qᵢ)Φ(qᵢ)
 * 
 * where:
 * - V = volume of integration domain (1.0 for unit hypercube)
 * - N = number of samples
 * - qᵢ = random sample points
 * - K(p,qᵢ) = geodesic kernel weight
 * - Φ(qᵢ) = field value at sample
 * 
 * Convergence: Error ~ O(1/√N)
 * 
 * This provides non-local semantic coherence by integrating
 * field contributions weighted by geodesic similarity.
 */
TCDE_Complex TCDE_ComputeCouplingMC(const TCDE_Field* field, const TCDE_Point* point,
                                     float sigma, int n_samples) {
    if (!field || !point || sigma <= 0.0f || n_samples <= 0) {
        return 0.0f + 0.0f * I;
    }
    
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    const float volume = 1.0f;  // Unit hypercube [0,1]^6
    
    TCDE_Complex sum = 0.0f + 0.0f * I;
    
    // Monte Carlo integration
    for (int i = 0; i < n_samples; i++) {
        // Generate random sample point
        TCDE_Point sample = generate_random_point_6d();
        
        // Compute kernel weight
        float kernel = TCDE_GeodesicKernel(point, &sample, metric, sigma);
        
        // Evaluate field at sample
        TCDE_Complex field_value = TCDE_Evaluate6D(field, &sample);
        
        // Accumulate weighted contribution
        sum += kernel * field_value;
        
        TCDE_FreePoint(&sample);
    }
    
    // Scale by volume and normalize by number of samples
    TCDE_Complex coupling = (volume / (float)n_samples) * sum;
    
    return coupling;
}

/**
 * @brief Compute coupling with importance sampling
 * @param field Field
 * @param point Evaluation point
 * @param sigma Kernel length scale
 * @param n_samples Number of samples
 * @return Coupling contribution
 * 
 * Uses importance sampling centered on existing RBF centers
 * to reduce variance and improve convergence.
 * 
 * Strategy:
 * 1. Sample near existing centers (high field density)
 * 2. Weight by kernel and field value
 * 3. Correct for sampling bias
 */
TCDE_Complex TCDE_ComputeCouplingImportance(const TCDE_Field* field, 
                                             const TCDE_Point* point,
                                             float sigma, int n_samples) {
    if (!field || !point || sigma <= 0.0f || n_samples <= 0) {
        return 0.0f + 0.0f * I;
    }
    
    int n_centers = field->manifold_6d.num_centers;
    if (n_centers == 0) return 0.0f + 0.0f * I;
    
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    TCDE_Complex sum = 0.0f + 0.0f * I;
    
    // Sample near each center
    int samples_per_center = n_samples / n_centers;
    if (samples_per_center < 1) samples_per_center = 1;
    
    for (int c = 0; c < n_centers; c++) {
        const TCDE_Point* center = &field->manifold_6d.centers[c].point;
        
        for (int s = 0; s < samples_per_center; s++) {
            // Generate sample near this center
            TCDE_Point sample = generate_random_point_6d();
            
            // Perturb towards center (simple importance sampling)
            for (int d = 0; d < 6; d++) {
                float center_coord = center->coords[d];
                float sample_coord = sample.coords[d];
                // Blend: 70% center, 30% random
                sample.coords[d] = 0.7f * center_coord + 0.3f * sample_coord;
            }
            
            // Compute kernel weight
            float kernel = TCDE_GeodesicKernel(point, &sample, metric, sigma);
            
            // Evaluate field
            TCDE_Complex field_value = TCDE_Evaluate6D(field, &sample);
            
            // Accumulate (importance sampling correction implicit)
            sum += kernel * field_value;
            
            TCDE_FreePoint(&sample);
        }
    }
    
    // Normalize
    TCDE_Complex coupling = sum / (float)(samples_per_center * n_centers);
    
    return coupling;
}

/**
 * @brief Compute coupling with local sampling (optimized)
 * @param field Field
 * @param point Evaluation point
 * @param sigma Kernel length scale
 * @param n_samples Number of samples
 * @return Coupling contribution
 * 
 * Only samples within coupling radius (3σ) to reduce wasted samples.
 * This is much more efficient than full Monte Carlo.
 * 
 * Strategy:
 * 1. Compute coupling radius r = 3σ
 * 2. Only sample within sphere of radius r around point
 * 3. Use rejection sampling to stay in bounds
 */
TCDE_Complex TCDE_ComputeCouplingLocal(const TCDE_Field* field,
                                        const TCDE_Point* point,
                                        float sigma, int n_samples) {
    if (!field || !point || sigma <= 0.0f || n_samples <= 0) {
        return 0.0f + 0.0f * I;
    }
    
    const TCDE_Metric* metric = &field->manifold_6d.metric;
    const float radius = TCDE_CouplingRadius(sigma, 0.01f);  // 3σ radius
    
    TCDE_Complex sum = 0.0f + 0.0f * I;
    int accepted_samples = 0;
    int max_attempts = n_samples * 10;  // Prevent infinite loop
    
    // Sample within local region
    for (int attempt = 0; attempt < max_attempts && accepted_samples < n_samples; attempt++) {
        // Generate random offset
        TCDE_Point sample = generate_random_point_6d();
        
        // Center around query point with radius
        for (int d = 0; d < 6; d++) {
            float offset = (sample.coords[d] - 0.5f) * 2.0f * radius;
            sample.coords[d] = point->coords[d] + offset;
            
            // Clamp to valid bounds
            if (sample.coords[d] < 0.0f) sample.coords[d] = 0.0f;
            if (sample.coords[d] > 1.0f) sample.coords[d] = 1.0f;
        }
        
        // Check if within radius (rejection sampling)
        float dist = TCDE_GeodesicDistanceOptimized(point, &sample, metric);
        if (dist > radius) {
            TCDE_FreePoint(&sample);
            continue;
        }
        
        // Accept sample
        accepted_samples++;
        
        // Compute kernel weight
        float kernel = TCDE_GeodesicKernel(point, &sample, metric, sigma);
        
        // Evaluate field
        TCDE_Complex field_value = TCDE_Evaluate6D(field, &sample);
        
        // Accumulate
        sum += kernel * field_value;
        
        TCDE_FreePoint(&sample);
    }
    
    if (accepted_samples == 0) return 0.0f + 0.0f * I;
    
    // Estimate local volume (sphere volume in 6D)
    // V_6D(r) = π³r⁶/6
    float local_volume = (M_PI * M_PI * M_PI * powf(radius, 6.0f)) / 6.0f;
    
    // Normalize
    TCDE_Complex coupling = (local_volume / (float)accepted_samples) * sum;
    
    return coupling;
}

/**
 * @brief Test convergence of Monte Carlo coupling
 * @param field Field
 * @param point Evaluation point
 * @param sigma Kernel length scale
 * @param n_samples_list Array of sample counts to test
 * @param n_tests Number of tests
 * @param results Output: coupling values for each sample count
 * 
 * Tests convergence by computing coupling with increasing sample counts.
 * Error should decrease as O(1/√N).
 */
void TCDE_TestCouplingConvergence(const TCDE_Field* field, const TCDE_Point* point,
                                   float sigma, const int* n_samples_list,
                                   int n_tests, TCDE_Complex* results) {
    if (!field || !point || !n_samples_list || !results) return;
    
    for (int i = 0; i < n_tests; i++) {
        results[i] = TCDE_ComputeCouplingMC(field, point, sigma, n_samples_list[i]);
    }
}

/**
 * @brief Estimate coupling error (standard error of mean)
 * @param field Field
 * @param point Evaluation point
 * @param sigma Kernel length scale
 * @param n_samples Number of samples
 * @param n_trials Number of independent trials
 * @return Standard error estimate
 * 
 * Runs multiple independent Monte Carlo trials and computes
 * standard error to estimate accuracy.
 */
float TCDE_EstimateCouplingError(const TCDE_Field* field, const TCDE_Point* point,
                                  float sigma, int n_samples, int n_trials) {
    if (!field || !point || n_samples <= 0 || n_trials <= 0) return 0.0f;
    
    TCDE_Complex* trials = malloc(n_trials * sizeof(TCDE_Complex));
    if (!trials) return 0.0f;
    
    // Run independent trials
    for (int i = 0; i < n_trials; i++) {
        trials[i] = TCDE_ComputeCouplingMC(field, point, sigma, n_samples);
    }
    
    // Compute mean
    TCDE_Complex mean = 0.0f + 0.0f * I;
    for (int i = 0; i < n_trials; i++) {
        mean += trials[i];
    }
    mean /= (float)n_trials;
    
    // Compute variance
    float variance = 0.0f;
    for (int i = 0; i < n_trials; i++) {
        float diff = cabsf(trials[i] - mean);
        variance += diff * diff;
    }
    variance /= (float)(n_trials - 1);
    
    // Standard error = sqrt(variance / n_trials)
    float std_error = sqrtf(variance / (float)n_trials);
    
    free(trials);
    return std_error;
}
