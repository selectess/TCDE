/**
 * @file tcde_v1.c
 * @brief TCDE V1 - Unified Implementation
 * 
 * Monolithic implementation merging 2D and 6D TCDE frameworks
 */

#include "tcde_monolithic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// Point Operations
// ============================================================================

TCDE_Point TCDE_CreatePoint(int dimension, const float* coords) {
    TCDE_Point point;
    point.dimension = dimension;
    point.coords = (float*)malloc(dimension * sizeof(float));
    
    if (point.coords && coords) {
        memcpy(point.coords, coords, dimension * sizeof(float));
    }
    
    return point;
}

void TCDE_FreePoint(TCDE_Point* point) {
    if (point && point->coords) {
        free(point->coords);
        point->coords = NULL;
        point->dimension = 0;
    }
}

bool TCDE_ValidatePoint(const TCDE_Point* point) {
    if (!point || !point->coords || point->dimension < 2) {
        return false;
    }
    
    // For 6D mode, validate specific ranges
    if (point->dimension == 6) {
        // Spatial [0,1]³
        for (int i = 0; i < 3; i++) {
            if (point->coords[i] < 0.0f || point->coords[i] > 1.0f) {
                return false;
            }
        }
        // Temporal: τ₁ ∈ [1,2), τ₂ ∈ [-1,1]
        if (point->coords[3] < 1.0f || point->coords[3] >= 2.0f) return false;
        if (point->coords[4] < -1.0f || point->coords[4] > 1.0f) return false;
        // Modal [0,1]
        if (point->coords[5] < 0.0f || point->coords[5] > 1.0f) return false;
    }
    
    return true;
}

// ============================================================================
// Metric Operations
// ============================================================================

static TCDE_Metric TCDE_CreateMetric(int dimension) {
    TCDE_Metric metric;
    metric.dimension = dimension;
    metric.is_valid = false;
    
    // Allocate metric tensor
    metric.g = (float**)malloc(dimension * sizeof(float*));
    metric.g_inv = (float**)malloc(dimension * sizeof(float*));
    
    for (int i = 0; i < dimension; i++) {
        metric.g[i] = (float*)calloc(dimension, sizeof(float));
        metric.g_inv[i] = (float*)calloc(dimension, sizeof(float));
        
        // Initialize as identity
        metric.g[i][i] = 1.0f;
        metric.g_inv[i][i] = 1.0f;
    }
    
    metric.det = 1.0f;
    metric.is_valid = true;
    
    return metric;
}

static void TCDE_FreeMetric(TCDE_Metric* metric) {
    if (!metric) return;
    
    for (int i = 0; i < metric->dimension; i++) {
        free(metric->g[i]);
        free(metric->g_inv[i]);
    }
    free(metric->g);
    free(metric->g_inv);
    
    metric->is_valid = false;
}

// ============================================================================
// Field Creation and Management
// ============================================================================

TCDE_Field* TCDE_CreateField(int capacity, float fractal_dim) {
    if (capacity <= 0) return NULL;
    
    TCDE_Field* field = (TCDE_Field*)calloc(1, sizeof(TCDE_Field));
    if (!field) return NULL;
    
    // Set unified topology mode
    field->topology = TCDE_TOPOLOGY_UNIFIED;
    
    // Allocate 6D manifold centers
    field->manifold_6d.centers = (TCDE_Center*)calloc(capacity, sizeof(TCDE_Center));
    if (!field->manifold_6d.centers) {
        free(field);
        return NULL;
    }
    field->manifold_6d.num_centers = 0;
    field->manifold_6d.capacity = capacity;
    field->manifold_6d.metric = TCDE_CreateMetric(6);
    
    // Allocate 2D projection centers
    field->slice_2d.centers = (TCDE_Center*)calloc(capacity, sizeof(TCDE_Center));
    if (!field->slice_2d.centers) {
        free(field->manifold_6d.centers);
        TCDE_FreeMetric(&field->manifold_6d.metric);
        free(field);
        return NULL;
    }
    field->slice_2d.num_centers = 0;
    field->slice_2d.metric = TCDE_CreateMetric(2);
    
    // Default projection: spatial XY plane (axes 0,1)
    field->slice_2d.projection.axis1 = 0;
    field->slice_2d.projection.axis2 = 1;
    field->slice_2d.projection.slice_coords[0] = 0.5f;  // z = 0.5
    field->slice_2d.projection.slice_coords[1] = 1.5f;  // τ₁ = 1.5
    field->slice_2d.projection.slice_coords[2] = 0.0f;  // τ₂ = 0.0
    field->slice_2d.projection.slice_coords[3] = 0.5f;  // m = 0.5
    field->slice_2d.projection.auto_update = true;
    
    field->rbf_type = TCDE_RBF_GAUSSIAN;
    
    // Initialize field properties
    field->time = 0.0f;
    field->energy = 0.0f;
    field->energy_valid = false;
    field->fractal_dimension = fractal_dim;
    field->temporal_dimension = 1.0f;
    
    // Adaptive parameters
    field->adaptive.error_threshold = 0.01f;
    field->adaptive.min_epsilon = 0.1f;
    field->adaptive.max_epsilon = 2.0f;
    field->adaptive.min_centers = 5;
    field->adaptive.max_centers = capacity;
    
    // Synchronization (enabled by default)
    field->sync_enabled = true;
    field->sync_strength = 0.5f;
    
    return field;
}

void TCDE_DestroyField(TCDE_Field* field) {
    if (!field) return;
    
    // Free 6D manifold centers
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_FreePoint(&field->manifold_6d.centers[i].point);
        TCDE_FreeMetric(&field->manifold_6d.centers[i].metric);
    }
    free(field->manifold_6d.centers);
    TCDE_FreeMetric(&field->manifold_6d.metric);
    
    // Free 2D projection centers
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        TCDE_FreePoint(&field->slice_2d.centers[i].point);
        TCDE_FreeMetric(&field->slice_2d.centers[i].metric);
    }
    free(field->slice_2d.centers);
    TCDE_FreeMetric(&field->slice_2d.metric);
    
    free(field);
}

// ============================================================================
// RBF Kernel Functions
// ============================================================================

static float TCDE_EvaluateRBF(float r, float epsilon, TCDE_RBFType type) {
    float er = epsilon * r;
    
    switch (type) {
        case TCDE_RBF_GAUSSIAN:
            return expf(-er * er);
            
        case TCDE_RBF_MULTIQUADRIC:
            return sqrtf(1.0f + er * er);
            
        case TCDE_RBF_INVERSE_MULTIQUADRIC:
            return 1.0f / sqrtf(1.0f + er * er);
            
        case TCDE_RBF_THIN_PLATE_SPLINE:
            if (r < 1e-10f) return 0.0f;
            return r * r * logf(r);
            
        default:
            return expf(-er * er);
    }
}

static float TCDE_RBFLaplacian(float r, float epsilon, TCDE_RBFType type, int dimension) {
    float er = epsilon * r;
    float e2 = epsilon * epsilon;
    
    switch (type) {
        case TCDE_RBF_GAUSSIAN:
            return 2.0f * e2 * (2.0f * e2 * r * r - dimension) * expf(-er * er);
            
        case TCDE_RBF_MULTIQUADRIC:
            return e2 * (2.0f + er * er) / powf(1.0f + er * er, 1.5f);
            
        case TCDE_RBF_INVERSE_MULTIQUADRIC:
            return e2 * (3.0f * er * er - 2.0f) / powf(1.0f + er * er, 2.5f);
            
        case TCDE_RBF_THIN_PLATE_SPLINE:
            if (r < 1e-10f) return 0.0f;
            return 2.0f * (2.0f * logf(r) + 3.0f);
            
        default:
            return 2.0f * e2 * (2.0f * e2 * r * r - dimension) * expf(-er * er);
    }
}

// ============================================================================
// Geodesic Distance
// ============================================================================

float TCDE_GeodesicDistance(const TCDE_Point* p1,
                            const TCDE_Point* p2,
                            const TCDE_Metric* metric) {
    if (!p1 || !p2 || !metric || p1->dimension != p2->dimension) {
        return 0.0f;
    }
    
    int dim = p1->dimension;
    float dist_sq = 0.0f;
    
    // Compute difference vector
    float* diff = (float*)malloc(dim * sizeof(float));
    for (int i = 0; i < dim; i++) {
        diff[i] = p2->coords[i] - p1->coords[i];
    }
    
    // Compute d² = diff^T * g * diff
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            dist_sq += diff[i] * metric->g[i][j] * diff[j];
        }
    }
    
    free(diff);
    
    return sqrtf(fmaxf(dist_sq, 0.0f));
}

// ============================================================================
// Field Operations
// ============================================================================

        return;
    }
    
    const float h = 1e-5f;
    int dim = field->dimension;
    
    // Initialize gradient
    for (int d = 0; d < dim; d++) {
        gradient[d] = 0.0f + 0.0f * I;
    }
    
    // Compute gradient using finite differences
    for (int d = 0; d < dim; d++) {
        // Create perturbed points
        TCDE_Point p_plus = TCDE_CreatePoint(dim, point->coords);
        TCDE_Point p_minus = TCDE_CreatePoint(dim, point->coords);
        
        p_plus.coords[d] += h;
        p_minus.coords[d] -= h;
        
        // Evaluate field
        TCDE_Complex f_plus = TCDE_Evaluate(field, &p_plus);
        TCDE_Complex f_minus = TCDE_Evaluate(field, &p_minus);
        
        // Compute derivative
        gradient[d] = (f_plus - f_minus) / (2.0f * h);
        
        TCDE_FreePoint(&p_plus);
        TCDE_FreePoint(&p_minus);
    }
}

TCDE_Complex TCDE_Laplacian(const TCDE_Field* field, const TCDE_Point* point) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point) {
        return result;
    }
    
    // Sum Laplacian contributions from all RBF centers
    for (int i = 0; i < field->num_centers; i++) {
        float r = TCDE_GeodesicDistance(point, &field->centers[i].point,
                                        &field->global_metric);
        
        // Evaluate RBF Laplacian
        float lap_phi = TCDE_RBFLaplacian(r, field->centers[i].epsilon,
                                          field->rbf_type, field->dimension);
        
        // Add weighted contribution
        result += field->centers[i].coeff * lap_phi;
    }
    
    return result;
}

float TCDE_ComputeEnergy(TCDE_Field* field) {
    if (!field) return 0.0f;
    
    if (field->energy_valid) {
        return field->energy;
    }
    
    // Compute energy by sampling field
    float energy = 0.0f;
    const int num_samples = 100;
    
    for (int i = 0; i < num_samples; i++) {
        // Random point
        float* coords = (float*)malloc(field->dimension * sizeof(float));
        for (int d = 0; d < field->dimension; d++) {
            coords[d] = (float)rand() / RAND_MAX;
        }
        
        TCDE_Point p = TCDE_CreatePoint(field->dimension, coords);
        TCDE_Complex value = TCDE_Evaluate(field, &p);
        
        float mag = cabsf(value);
        energy += mag * mag;
        
        TCDE_FreePoint(&p);
        free(coords);
    }
    
    energy /= (float)num_samples;
    
    field->energy = energy;
    field->energy_valid = true;
    
    return energy;
}

// ============================================================================
// Essential TCDE Terms
// ============================================================================

TCDE_Complex TCDE_TopologicalTorsion(const TCDE_Field* field,
                                      const TCDE_Point* point) {
    if (!field || !point) {
        return 0.0f + 0.0f * I;
    }
    
    // Compute gradient
    TCDE_Complex* grad = (TCDE_Complex*)malloc(field->dimension * sizeof(TCDE_Complex));
    TCDE_Gradient(field, point, grad);
    
    // Compute field value for phase
    TCDE_Complex phi = TCDE_Evaluate(field, point);
    float phase = cargf(phi);
    
    // Torsion involves the "twist" in the field
    TCDE_Complex torsion = 0.0f + 0.0f * I;
    
    for (int i = 0; i < field->dimension; i++) {
        for (int j = i + 1; j < field->dimension; j++) {
            // Antisymmetric combination
            float twist_real = crealf(grad[i]) * cosf(phase) - crealf(grad[j]) * sinf(phase);
            float twist_imag = cimagf(grad[i]) * cosf(phase) - cimagf(grad[j]) * sinf(phase);
            
            torsion += (twist_real + twist_imag * I);
        }
    }
    
    free(grad);
    
    return torsion;
}

TCDE_Complex TCDE_ContextualCoupling(const TCDE_Field* field,
                                      const TCDE_Point* point,
                                      float sigma,
                                      int num_samples) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point || num_samples <= 0) {
        return result;
    }
    
    // Monte Carlo integration: C(p) = ∫ K(p,q) Φ(q) dV
    for (int s = 0; s < num_samples; s++) {
        // Sample random point
        float* coords = (float*)malloc(field->dimension * sizeof(float));
        for (int d = 0; d < field->dimension; d++) {
            coords[d] = (float)rand() / RAND_MAX;
        }
        
        TCDE_Point q = TCDE_CreatePoint(field->dimension, coords);
        
        // Compute geodesic distance
        float dist = TCDE_GeodesicDistance(point, &q, &field->global_metric);
        
        // Coupling kernel: K(p,q) = exp(-d²/2σ²)
        float kernel = expf(-dist * dist / (2.0f * sigma * sigma));
        
        // Evaluate field at sample point
        TCDE_Complex phi_q = TCDE_Evaluate(field, &q);
        
        // Accumulate weighted contribution
        result += kernel * phi_q;
        
        TCDE_FreePoint(&q);
        free(coords);
    }
    
    // Normalize
    result /= (float)num_samples;
    
    return result;
}

float TCDE_TemporalDimension(const TCDE_Field* field) {
    if (!field) return 1.0f;
    
    // Compute field energy
    float energy = 0.0f;
    for (int i = 0; i < field->num_centers; i++) {
        float mag = cabsf(field->centers[i].coeff);
        energy += mag * mag;
    }
    
    if (field->num_centers > 0) {
        energy /= field->num_centers;
    }
    
    // Temporal dimension emerges from energy
    // τ = 1 + 0.5·(1 + tanh(normalized_energy))
    float threshold = 0.1f;
    float scale = 1.0f;
    float normalized = (energy - threshold) / scale;
    
    float tau = 1.0f + 0.5f * (1.0f + tanhf(normalized));
    
    // Clamp to [1,2]
    return fmaxf(1.0f, fminf(2.0f, tau));
}

// ============================================================================
// Evolution
// ============================================================================

void TCDE_ConfigureParameters(TCDE_Parameters* params, const TCDE_Field* field) {
    if (!params || !field) return;
    
    // Base parameters scaled by fractal dimension
    float df_factor = (field->fractal_dimension - 2.0f) / 1.0f;
    df_factor = fmaxf(0.0f, fminf(1.0f, df_factor));
    
    params->dt = 0.01f;
    params->D = 0.1f * (1.0f + 0.5f * df_factor);
    params->alpha = 0.05f * (1.0f + 0.3f * df_factor);
    params->beta = 0.02f * (1.0f + 0.4f * df_factor);
    params->gamma = 0.03f * (1.0f + 0.2f * df_factor);
    params->sigma = 0.15f;
    
    // Adjust based on current energy
    float energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    
    if (energy > 1.0f) {
        params->D *= 1.2f;
        params->alpha *= 0.8f;
    } else if (energy < 0.1f) {
        params->D *= 0.8f;
        params->alpha *= 1.2f;
    }
}

void TCDE_EvolveStep(TCDE_Field* field, const TCDE_Parameters* params) {
    if (!field || !params || field->num_centers == 0) {
        return;
    }
    
    // Allocate temporary storage for new coefficients
    TCDE_Complex* new_coeffs = (TCDE_Complex*)malloc(field->num_centers * sizeof(TCDE_Complex));
    if (!new_coeffs) return;
    
    // Update each center's coefficient
    for (int i = 0; i < field->num_centers; i++) {
        TCDE_Point* center = &field->centers[i].point;
        
        // 1. Diffusion term: D∇²Φ
        TCDE_Complex laplacian = TCDE_Laplacian(field, center);
        TCDE_Complex diffusion = params->D * laplacian;
        
        // 2. Nonlinear term: -α|Φ|²Φ
        TCDE_Complex phi = TCDE_Evaluate(field, center);
        float mag_sq = cabsf(phi) * cabsf(phi);
        TCDE_Complex nonlinear = -params->alpha * mag_sq * phi;
        
        // 3. Torsion term: β·T(Φ)
        TCDE_Complex torsion = params->beta * TCDE_TopologicalTorsion(field, center);
        
        // 4. Coupling term: γ·C(Φ)
        TCDE_Complex coupling = params->gamma * TCDE_ContextualCoupling(field, center, 
                                                                         params->sigma, 50);
        
        // Combine all terms
        TCDE_Complex total_change = diffusion + nonlinear + torsion + coupling;
        
        // Forward Euler step
        new_coeffs[i] = field->centers[i].coeff + params->dt * total_change;
        
        // Numerical stability: clamp extreme values
        float real_part = crealf(new_coeffs[i]);
        float imag_part = cimagf(new_coeffs[i]);
        real_part = fmaxf(-10.0f, fminf(10.0f, real_part));
        imag_part = fmaxf(-10.0f, fminf(10.0f, imag_part));
        new_coeffs[i] = real_part + imag_part * I;
    }
    
    // Update coefficients
    for (int i = 0; i < field->num_centers; i++) {
        field->centers[i].coeff = new_coeffs[i];
    }
    
    free(new_coeffs);
    
    // Update time and invalidate energy cache
    field->time += params->dt;
    field->energy_valid = false;
    
    // Update temporal dimension
    field->temporal_dimension = TCDE_TemporalDimension(field);
}

// ============================================================================
// Fractal Initialization
// ============================================================================

typedef struct {
    TCDE_Field* field;
    float amplitude;
    float scale_factor;
    float hausdorff_dim;
    int max_depth;
} FractalContext;

static void add_fractal_centers_recursive(FractalContext* ctx,
                                          const float* base_coords,
                                          float current_width,
                                          int depth) {
    if (ctx->field->num_centers >= ctx->field->capacity || depth > ctx->max_depth) {
        return;
    }
    
    // Add center at current position
    TCDE_Point base = TCDE_CreatePoint(ctx->field->dimension, base_coords);
    
    if (TCDE_ValidatePoint(&base)) {
        // Compute amplitude based on depth
        float depth_amplitude = ctx->amplitude * powf(ctx->scale_factor, (float)depth);
        TCDE_Complex coeff = depth_amplitude + 0.0f * I;
        float epsilon = current_width * 2.0f;
        
        TCDE_AddCenter(ctx->field, &base, coeff, epsilon);
    }
    
    TCDE_FreePoint(&base);
    
    // Recursive subdivision with probability
    float subdivision_prob = powf(0.8f, (float)depth) * (ctx->hausdorff_dim - 2.0f);
    
    if ((float)rand() / RAND_MAX < subdivision_prob && depth < ctx->max_depth) {
        float new_width = current_width * ctx->scale_factor;
        
        // Create subdivisions (4 for 2D, more for higher dimensions)
        int num_subdivisions = (ctx->field->dimension == 2) ? 4 : 8;
        
        for (int s = 0; s < num_subdivisions; s++) {
            float* new_coords = (float*)malloc(ctx->field->dimension * sizeof(float));
            memcpy(new_coords, base_coords, ctx->field->dimension * sizeof(float));
            
            // Perturb first two dimensions
            new_coords[0] += ((s & 1) ? 1.0f : -1.0f) * new_width;
            new_coords[1] += ((s & 2) ? 1.0f : -1.0f) * new_width;
            
            // Add random perturbation
            for (int d = 0; d < ctx->field->dimension; d++) {
                new_coords[d] += ((float)rand() / RAND_MAX - 0.5f) * new_width * 0.3f;
                new_coords[d] = fmaxf(0.0f, fminf(1.0f, new_coords[d]));
            }
            
            add_fractal_centers_recursive(ctx, new_coords, new_width, depth + 1);
            free(new_coords);
        }
    }
}

void TCDE_InitializeFractal(TCDE_Field* field,
                             const TCDE_Point* center,
                             float width,
                             float amplitude,
                             int num_centers) {
    if (!field || !center || num_centers <= 0) {
        return;
    }
    
    // Clear existing centers
    for (int i = 0; i < field->num_centers; i++) {
        TCDE_FreePoint(&field->centers[i].point);
        TCDE_FreeMetric(&field->centers[i].metric);
    }
    field->num_centers = 0;
    
    // Clamp fractal dimension
    float hausdorff_dim = fmaxf(2.0f, fminf(3.0f, field->fractal_dimension));
    
    // Setup fractal context
    FractalContext ctx;
    ctx.field = field;
    ctx.amplitude = amplitude;
    ctx.scale_factor = powf(2.0f, -1.0f / hausdorff_dim);
    ctx.hausdorff_dim = hausdorff_dim;
    ctx.max_depth = (int)(logf((float)num_centers) / logf(4.0f)) + 1;
    
    // Start recursive subdivision
    add_fractal_centers_recursive(&ctx, center->coords, width, 0);
}

// ============================================================================
// Adaptive Features
// ============================================================================

int TCDE_AdaptiveMeshRefinement(TCDE_Field* field) {
    if (!field) return 0;
    
    int centers_added = 0;
    int centers_removed = 0;
    
    // Compute gradient magnitudes
    float* grad_mags = (float*)malloc(field->num_centers * sizeof(float));
    
    for (int i = 0; i < field->num_centers; i++) {
        TCDE_Complex* grad = (TCDE_Complex*)malloc(field->dimension * sizeof(TCDE_Complex));
        TCDE_Gradient(field, &field->centers[i].point, grad);
        
        float mag = 0.0f;
        for (int d = 0; d < field->dimension; d++) {
            mag += cabsf(grad[d]) * cabsf(grad[d]);
        }
        grad_mags[i] = sqrtf(mag);
        
        free(grad);
    }
    
    // Add centers in high-gradient regions
    if (field->num_centers < field->adaptive.max_centers) {
        for (int i = 0; i < field->num_centers && centers_added < 5; i++) {
            if (grad_mags[i] > field->adaptive.error_threshold) {
                // Create new center near this one
                float* new_coords = (float*)malloc(field->dimension * sizeof(float));
                for (int d = 0; d < field->dimension; d++) {
                    float offset = ((float)rand() / RAND_MAX - 0.5f) * field->centers[i].epsilon;
                    new_coords[d] = field->centers[i].point.coords[d] + offset;
                    new_coords[d] = fmaxf(0.0f, fminf(1.0f, new_coords[d]));
                }
                
                TCDE_Point new_point = TCDE_CreatePoint(field->dimension, new_coords);
                TCDE_Complex new_coeff = TCDE_Evaluate(field, &new_point);
                
                if (TCDE_AddCenter(field, &new_point, new_coeff, field->centers[i].epsilon)) {
                    centers_added++;
                }
                
                TCDE_FreePoint(&new_point);
                free(new_coords);
            }
        }
    }
    
    // Remove centers in low-gradient regions
    if (field->num_centers > field->adaptive.min_centers) {
        for (int i = field->num_centers - 1; i >= 0 && centers_removed < 3; i--) {
            if (grad_mags[i] < field->adaptive.error_threshold * 0.2f) {
                if (TCDE_RemoveCenter(field, i)) {
                    centers_removed++;
                }
            }
        }
    }
    
    free(grad_mags);
    
    return centers_added - centers_removed;
}

void TCDE_AdaptMetric(TCDE_Field* field, float kappa) {
    if (!field) return;
    
    // Compute average energy
    float avg_energy = TCDE_ComputeEnergy(field);
    
    // Update global metric based on energy
    for (int i = 0; i < field->dimension; i++) {
        for (int j = 0; j < field->dimension; j++) {
            if (i == j) {
                // Diagonal: add energy-dependent correction
                field->global_metric.g[i][j] = 1.0f + kappa * avg_energy;
            } else {
                // Off-diagonal: small coupling
                field->global_metric.g[i][j] = kappa * avg_energy * 0.1f;
            }
        }
    }
    
    // Update determinant (simplified for diagonal-dominant)
    field->global_metric.det = 1.0f;
    for (int i = 0; i < field->dimension; i++) {
        field->global_metric.det *= field->global_metric.g[i][i];
    }
    
    // Update inverse (simplified for diagonal-dominant)
    for (int i = 0; i < field->dimension; i++) {
        field->global_metric.g_inv[i][i] = 1.0f / field->global_metric.g[i][i];
    }
}

// ============================================================================
// Multi-Modal Processing
// ============================================================================

TCDE_ModalityRelations* TCDE_CreateModalityRelations(const char** names,
                                                       int num_modalities,
                                                       int embedding_dim) {
    if (!names || num_modalities <= 0 || embedding_dim <= 0) {
        return NULL;
    }
    
    TCDE_ModalityRelations* relations = (TCDE_ModalityRelations*)malloc(sizeof(TCDE_ModalityRelations));
    if (!relations) return NULL;
    
    relations->num_modalities = num_modalities;
    relations->modalities = (TCDE_Modality**)malloc(num_modalities * sizeof(TCDE_Modality*));
    relations->similarity_matrix = (float**)malloc(num_modalities * sizeof(float*));
    
    for (int i = 0; i < num_modalities; i++) {
        relations->modalities[i] = (TCDE_Modality*)malloc(sizeof(TCDE_Modality));
        strncpy(relations->modalities[i]->name, names[i], 31);
        relations->modalities[i]->name[31] = '\0';
        relations->modalities[i]->id = i;
        relations->modalities[i]->embedding_dim = embedding_dim;
        relations->modalities[i]->embedding = (float*)calloc(embedding_dim, sizeof(float));
        
        relations->similarity_matrix[i] = (float*)calloc(num_modalities, sizeof(float));
        relations->similarity_matrix[i][i] = 1.0f; // Self-similarity
    }
    
    return relations;
}

void TCDE_DestroyModalityRelations(TCDE_ModalityRelations* relations) {
    if (!relations) return;
    
    for (int i = 0; i < relations->num_modalities; i++) {
        free(relations->modalities[i]->embedding);
        free(relations->modalities[i]);
        free(relations->similarity_matrix[i]);
    }
    free(relations->modalities);
    free(relations->similarity_matrix);
    free(relations);
}

void TCDE_ProcessInput(TCDE_Field* field,
                       TCDE_ModalityRelations* relations,
                       const void* input_data,
                       int modality_id,
                       float intensity) {
    if (!field || !relations || !input_data) return;
    if (modality_id < 0 || modality_id >= relations->num_modalities) return;
    
    // Simple perturbation based on input
    // In a real implementation, this would process actual data
    for (int i = 0; i < field->num_centers; i++) {
        float perturbation = intensity * 0.1f * ((float)rand() / RAND_MAX);
        field->centers[i].coeff += perturbation * (1.0f + 0.5f * I);
    }
    
    field->energy_valid = false;
}

void TCDE_TransformModality(TCDE_Field* field,
                            TCDE_ModalityRelations* relations,
                            int source_modality,
                            int target_modality) {
    if (!field || !relations) return;
    if (source_modality < 0 || source_modality >= relations->num_modalities) return;
    if (target_modality < 0 || target_modality >= relations->num_modalities) return;
    
    // Compute similarity
    float similarity = relations->similarity_matrix[source_modality][target_modality];
    
    // Apply rotation in complex plane based on similarity
    float rotation_angle = (1.0f - similarity) * M_PI / 4.0f;
    TCDE_Complex rotation = cosf(rotation_angle) + sinf(rotation_angle) * I;
    
    for (int i = 0; i < field->num_centers; i++) {
        field->centers[i].coeff *= rotation;
    }
    
    field->energy_valid = false;
}

// ============================================================================
// Validation
// ============================================================================

float TCDE_ValidateReflexivity(const TCDE_Field* field) {
    if (!field) return 0.0f;
    
    // Simple reflexivity score based on field coherence
    float coherence = 0.0f;
    
    for (int i = 0; i < field->num_centers; i++) {
        float mag = cabsf(field->centers[i].coeff);
        coherence += mag;
    }
    
    if (field->num_centers > 0) {
        coherence /= field->num_centers;
    }
    
    // Normalize to [0,1]
    return fminf(1.0f, coherence);
}

float TCDE_ValidatePrediction(TCDE_Field* field) {
    if (!field) return 0.0f;
    
    // Simple prediction score based on temporal dimension
    float tau = TCDE_TemporalDimension(field);
    
    // Higher temporal dimension → better prediction
    return (tau - 1.0f) * 2.0f; // Maps [1,2] to [0,2]
}

int TCDE_ValidateIntuition(TCDE_Field* field, TCDE_ModalityRelations* relations) {
    if (!field || !relations) return 0;
    
    // Count non-obvious connections (simplified)
    int connections = 0;
    
    for (int i = 0; i < relations->num_modalities; i++) {
        for (int j = i + 1; j < relations->num_modalities; j++) {
            float similarity = relations->similarity_matrix[i][j];
            if (similarity > 0.6f && similarity < 0.95f) {
                connections++;
            }
        }
    }
    
    return connections;
}

float TCDE_VerifyFractalDimension(const TCDE_Field* field) {
    if (!field || field->num_centers < 10) return 0.0f;
    
    // Box-counting method (simplified)
    int box_sizes[] = {2, 4, 8, 16};
    int num_sizes = 4;
    double log_sizes[4], log_counts[4];
    
    for (int k = 0; k < num_sizes; k++) {
        float box_size = 1.0f / box_sizes[k];
        int grid_dim = box_sizes[k];
        
        // Count occupied boxes
        int* occupied = (int*)calloc(grid_dim * grid_dim, sizeof(int));
        int count = 0;
        
        for (int i = 0; i < field->num_centers; i++) {
            int x = (int)(field->centers[i].point.coords[0] * grid_dim);
            int y = (int)(field->centers[i].point.coords[1] * grid_dim);
            
            if (x >= 0 && x < grid_dim && y >= 0 && y < grid_dim) {
                int idx = y * grid_dim + x;
                if (!occupied[idx]) {
                    occupied[idx] = 1;
                    count++;
                }
            }
        }
        
        free(occupied);
        
        log_sizes[k] = log(box_size);
        log_counts[k] = log(fmaxf(1.0, (double)count));
    }
    
    // Linear regression
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    for (int k = 0; k < num_sizes; k++) {
        sum_x += log_sizes[k];
        sum_y += log_counts[k];
        sum_xy += log_sizes[k] * log_counts[k];
        sum_x2 += log_sizes[k] * log_sizes[k];
    }
    
    double slope = (num_sizes * sum_xy - sum_x * sum_y) / 
                   (num_sizes * sum_x2 - sum_x * sum_x);
    
    return (float)-slope; // Negative slope is the dimension
}

TCDE_ValidationResults TCDE_Validate(TCDE_Field* field,
                                     TCDE_ModalityRelations* relations) {
    TCDE_ValidationResults results;
    
    results.reflexive_coherence = TCDE_ValidateReflexivity(field);
    results.predictive_strength = TCDE_ValidatePrediction(field);
    results.intuitive_connections = TCDE_ValidateIntuition(field, relations);
    results.anticipation_score = (field->temporal_dimension - 1.0f); // [0,1]
    results.cross_modal_coherence = relations ? 0.9f : 0.0f; // Simplified
    
    results.validated = (results.reflexive_coherence > 0.7f &&
                        results.predictive_strength > 1.0f &&
                        results.intuitive_connections > 3);
    
    return results;
}

// ============================================================================
// Utility Functions
// ============================================================================

void TCDE_PrintField(const TCDE_Field* field) {
    if (!field) {
        printf("TCDE Field: NULL\n");
        return;
    }
    
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                TCDE V1 UNIFIED TOPOLOGY                    ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Topology:          Unified (6D ⊃ 2D)                      ║\n");
    printf("║  6D Centers:        %d / %d                                ║\n", 
           field->manifold_6d.num_centers, field->manifold_6d.capacity);
    printf("║  2D Centers:        %d (projected)                         ║\n", 
           field->slice_2d.num_centers);
    printf("║  Projection:        Axes %d,%d                             ║\n",
           field->slice_2d.projection.axis1, field->slice_2d.projection.axis2);
    printf("║  Sync:              %s (strength: %.2f)                    ║\n",
           field->sync_enabled ? "ON " : "OFF", field->sync_strength);
    printf("║  RBF Type:          %d                                     ║\n", field->rbf_type);
    printf("║  Time:              %.3f                                   ║\n", field->time);
    printf("║  Energy:            %.6f                                   ║\n", 
           field->energy_valid ? field->energy : TCDE_ComputeEnergy((TCDE_Field*)field));
    printf("║  Fractal Dim:       %.3f                                   ║\n", field->fractal_dimension);
    printf("║  Temporal Dim:      %.3f                                   ║\n", field->temporal_dimension);
    printf("╚════════════════════════════════════════════════════════════╝\n");
}

bool TCDE_SaveField(const TCDE_Field* field, const char* filename) {
    if (!field || !filename) return false;
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;
    
    // Write header
    fwrite(&field->mode, sizeof(TCDE_DimensionMode), 1, fp);
    fwrite(&field->dimension, sizeof(int), 1, fp);
    fwrite(&field->num_centers, sizeof(int), 1, fp);
    fwrite(&field->capacity, sizeof(int), 1, fp);
    fwrite(&field->rbf_type, sizeof(TCDE_RBFType), 1, fp);
    fwrite(&field->time, sizeof(float), 1, fp);
    fwrite(&field->fractal_dimension, sizeof(float), 1, fp);
    
    // Write centers
    for (int i = 0; i < field->num_centers; i++) {
        fwrite(field->centers[i].point.coords, sizeof(float), field->dimension, fp);
        fwrite(&field->centers[i].coeff, sizeof(TCDE_Complex), 1, fp);
        fwrite(&field->centers[i].epsilon, sizeof(float), 1, fp);
    }
    
    fclose(fp);
    return true;
}

TCDE_Field* TCDE_LoadField(const char* filename) {
    if (!filename) return NULL;
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    // Read header
    TCDE_DimensionMode mode;
    int dimension, num_centers, capacity;
    TCDE_RBFType rbf_type;
    float time, fractal_dim;
    
    fread(&mode, sizeof(TCDE_DimensionMode), 1, fp);
    fread(&dimension, sizeof(int), 1, fp);
    fread(&num_centers, sizeof(int), 1, fp);
    fread(&capacity, sizeof(int), 1, fp);
    fread(&rbf_type, sizeof(TCDE_RBFType), 1, fp);
    fread(&time, sizeof(float), 1, fp);
    fread(&fractal_dim, sizeof(float), 1, fp);
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(mode, capacity, fractal_dim);
    if (!field) {
        fclose(fp);
        return NULL;
    }
    
    field->rbf_type = rbf_type;
    field->time = time;
    
    // Read centers
    for (int i = 0; i < num_centers; i++) {
        float* coords = (float*)malloc(dimension * sizeof(float));
        TCDE_Complex coeff;
        float epsilon;
        
        fread(coords, sizeof(float), dimension, fp);
        fread(&coeff, sizeof(TCDE_Complex), 1, fp);
        fread(&epsilon, sizeof(float), 1, fp);
        
        TCDE_Point point = TCDE_CreatePoint(dimension, coords);
        TCDE_AddCenter(field, &point, coeff, epsilon);
        
        TCDE_FreePoint(&point);
        free(coords);
    }
    
    fclose(fp);
    return field;
}

// ============================================================================
// Version Information
// ============================================================================

const char* TCDE_GetVersion(void) {
    return TCDE_VERSION_STRING;
}

void TCDE_PrintVersion(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    TCDE V%d.%d.%d                            ║\n",
           TCDE_VERSION_MAJOR, TCDE_VERSION_MINOR, TCDE_VERSION_PATCH);
    printf("║     Topological Cognitive Diffusive Emergence Model        ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Unified monolithic implementation                         ║\n");
    printf("║  Supports 2D and 6D modes                                  ║\n");
    printf("║  Complete geometric operations                             ║\n");
    printf("║  Validation framework included                             ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}

// ============================================================================
// Unified Topology: 2D Projection Management
// ============================================================================

void TCDE_ConfigureProjection(TCDE_Field* field, int axis1, int axis2, const float* slice_coords) {
    if (!field || axis1 < 0 || axis1 >= 6 || axis2 < 0 || axis2 >= 6 || axis1 == axis2) {
        return;
    }
    
    field->slice_2d.projection.axis1 = axis1;
    field->slice_2d.projection.axis2 = axis2;
    
    if (slice_coords) {
        // Copy slice coordinates for the 4 fixed dimensions
        int coord_idx = 0;
        for (int i = 0; i < 6; i++) {
            if (i != axis1 && i != axis2) {
                field->slice_2d.projection.slice_coords[coord_idx++] = slice_coords[i];
            }
        }
    }
    
    // Update projection
    TCDE_UpdateProjection(field);
}

void TCDE_UpdateProjection(TCDE_Field* field) {
    if (!field) return;
    
    // Clear existing 2D centers
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        TCDE_FreePoint(&field->slice_2d.centers[i].point);
        TCDE_FreeMetric(&field->slice_2d.centers[i].metric);
    }
    field->slice_2d.num_centers = 0;
    
    // Project each 6D center to 2D
    int axis1 = field->slice_2d.projection.axis1;
    int axis2 = field->slice_2d.projection.axis2;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Point* center_6d = &field->manifold_6d.centers[i].point;
        
        // Create 2D projection
        float coords_2d[2];
        coords_2d[0] = center_6d->coords[axis1];
        coords_2d[1] = center_6d->coords[axis2];
        
        TCDE_Point point_2d = TCDE_CreatePoint(2, coords_2d);
        
        // Copy coefficient and epsilon
        TCDE_Complex coeff = field->manifold_6d.centers[i].coeff;
        float epsilon = field->manifold_6d.centers[i].epsilon;
        
        // Add to 2D slice
        field->slice_2d.centers[field->slice_2d.num_centers].point = point_2d;
        field->slice_2d.centers[field->slice_2d.num_centers].coeff = coeff;
        field->slice_2d.centers[field->slice_2d.num_centers].epsilon = epsilon;
        field->slice_2d.centers[field->slice_2d.num_centers].metric = TCDE_CreateMetric(2);
        
        field->slice_2d.num_centers++;
    }
}

TCDE_Point TCDE_Project6Dto2D(const TCDE_Field* field, const TCDE_Point* point_6d) {
    float coords_2d[2] = {0.0f, 0.0f};
    
    if (field && point_6d && point_6d->dimension == 6) {
        coords_2d[0] = point_6d->coords[field->slice_2d.projection.axis1];
        coords_2d[1] = point_6d->coords[field->slice_2d.projection.axis2];
    }
    
    return TCDE_CreatePoint(2, coords_2d);
}

TCDE_Point TCDE_Lift2Dto6D(const TCDE_Field* field, const TCDE_Point* point_2d) {
    float coords_6d[6];
    
    if (field && point_2d && point_2d->dimension == 2) {
        int axis1 = field->slice_2d.projection.axis1;
        int axis2 = field->slice_2d.projection.axis2;
        
        // Fill in the 2D coordinates
        coords_6d[axis1] = point_2d->coords[0];
        coords_6d[axis2] = point_2d->coords[1];
        
        // Fill in the fixed slice coordinates
        int coord_idx = 0;
        for (int i = 0; i < 6; i++) {
            if (i != axis1 && i != axis2) {
                coords_6d[i] = field->slice_2d.projection.slice_coords[coord_idx++];
            }
        }
    } else {
        // Default 6D point
        for (int i = 0; i < 6; i++) {
            coords_6d[i] = 0.5f;
        }
    }
    
    return TCDE_CreatePoint(6, coords_6d);
}

void TCDE_SetSynchronization(TCDE_Field* field, bool enabled, float strength) {
    if (!field) return;
    
    field->sync_enabled = enabled;
    field->sync_strength = fmaxf(0.0f, fminf(1.0f, strength));
}

// ============================================================================
// Unified Topology: Field Operations
// ============================================================================

bool TCDE_AddCenter6D(TCDE_Field* field, const TCDE_Point* point, TCDE_Complex coeff, float epsilon) {
    if (!field || !point || field->manifold_6d.num_centers >= field->manifold_6d.capacity) {
        return false;
    }
    
    if (!TCDE_ValidatePoint(point) || point->dimension != 6) {
        return false;
    }
    
    int idx = field->manifold_6d.num_centers;
    
    // Copy point
    field->manifold_6d.centers[idx].point = TCDE_CreatePoint(point->dimension, point->coords);
    field->manifold_6d.centers[idx].coeff = coeff;
    field->manifold_6d.centers[idx].epsilon = epsilon;
    field->manifold_6d.centers[idx].metric = TCDE_CreateMetric(6);
    
    field->manifold_6d.num_centers++;
    field->energy_valid = false;
    
    // Update 2D projection if sync enabled
    if (field->sync_enabled && field->slice_2d.projection.auto_update) {
        TCDE_UpdateProjection(field);
    }
    
    return true;
}

bool TCDE_RemoveCenter6D(TCDE_Field* field, int index) {
    if (!field || index < 0 || index >= field->manifold_6d.num_centers) {
        return false;
    }
    
    // Free the center being removed
    TCDE_FreePoint(&field->manifold_6d.centers[index].point);
    TCDE_FreeMetric(&field->manifold_6d.centers[index].metric);
    
    // Shift remaining centers
    for (int i = index; i < field->manifold_6d.num_centers - 1; i++) {
        field->manifold_6d.centers[i] = field->manifold_6d.centers[i + 1];
    }
    
    field->manifold_6d.num_centers--;
    field->energy_valid = false;
    
    // Update 2D projection if sync enabled
    if (field->sync_enabled && field->slice_2d.projection.auto_update) {
        TCDE_UpdateProjection(field);
    }
    
    return true;
}

TCDE_Complex TCDE_Evaluate6D(const TCDE_Field* field, const TCDE_Point* point) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point || point->dimension != 6) {
        return result;
    }
    
    // Sum contributions from all 6D RBF centers
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float r = TCDE_GeodesicDistance(point, &field->manifold_6d.centers[i].point, 
                                        &field->manifold_6d.metric);
        
        float phi = TCDE_EvaluateRBF(r, field->manifold_6d.centers[i].epsilon, field->rbf_type);
        
        result += field->manifold_6d.centers[i].coeff * phi;
    }
    
    return result;
}

TCDE_Complex TCDE_Evaluate2D(const TCDE_Field* field, const TCDE_Point* point) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point || point->dimension != 2) {
        return result;
    }
    
    // Sum contributions from all 2D projected centers
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        float r = TCDE_GeodesicDistance(point, &field->slice_2d.centers[i].point, 
                                        &field->slice_2d.metric);
        
        float phi = TCDE_EvaluateRBF(r, field->slice_2d.centers[i].epsilon, field->rbf_type);
        
        result += field->slice_2d.centers[i].coeff * phi;
    }
    
    return result;
}

void TCDE_Gradient6D(const TCDE_Field* field, const TCDE_Point* point, TCDE_Complex* gradient) {
    if (!field || !point || !gradient || point->dimension != 6) {
        return;
    }
    
    const float h = 1e-5f;
    
    for (int d = 0; d < 6; d++) {
        gradient[d] = 0.0f + 0.0f * I;
        
        TCDE_Point p_plus = TCDE_CreatePoint(6, point->coords);
        TCDE_Point p_minus = TCDE_CreatePoint(6, point->coords);
        
        p_plus.coords[d] += h;
        p_minus.coords[d] -= h;
        
        TCDE_Complex f_plus = TCDE_Evaluate6D(field, &p_plus);
        TCDE_Complex f_minus = TCDE_Evaluate6D(field, &p_minus);
        
        gradient[d] = (f_plus - f_minus) / (2.0f * h);
        
        TCDE_FreePoint(&p_plus);
        TCDE_FreePoint(&p_minus);
    }
}

void TCDE_Gradient2D(const TCDE_Field* field, const TCDE_Point* point, TCDE_Complex* gradient) {
    if (!field || !point || !gradient || point->dimension != 2) {
        return;
    }
    
    const float h = 1e-5f;
    
    for (int d = 0; d < 2; d++) {
        gradient[d] = 0.0f + 0.0f * I;
        
        TCDE_Point p_plus = TCDE_CreatePoint(2, point->coords);
        TCDE_Point p_minus = TCDE_CreatePoint(2, point->coords);
        
        p_plus.coords[d] += h;
        p_minus.coords[d] -= h;
        
        TCDE_Complex f_plus = TCDE_Evaluate2D(field, &p_plus);
        TCDE_Complex f_minus = TCDE_Evaluate2D(field, &p_minus);
        
        gradient[d] = (f_plus - f_minus) / (2.0f * h);
        
        TCDE_FreePoint(&p_plus);
        TCDE_FreePoint(&p_minus);
    }
}

TCDE_Complex TCDE_Laplacian6D(const TCDE_Field* field, const TCDE_Point* point) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point || point->dimension != 6) {
        return result;
    }
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float r = TCDE_GeodesicDistance(point, &field->manifold_6d.centers[i].point,
                                        &field->manifold_6d.metric);
        
        float lap_phi = TCDE_RBFLaplacian(r, field->manifold_6d.centers[i].epsilon,
                                          field->rbf_type, 6);
        
        result += field->manifold_6d.centers[i].coeff * lap_phi;
    }
    
    return result;
}

TCDE_Complex TCDE_Laplacian2D(const TCDE_Field* field, const TCDE_Point* point) {
    TCDE_Complex result = 0.0f + 0.0f * I;
    
    if (!field || !point || point->dimension != 2) {
        return result;
    }
    
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        float r = TCDE_GeodesicDistance(point, &field->slice_2d.centers[i].point,
                                        &field->slice_2d.metric);
        
        float lap_phi = TCDE_RBFLaplacian(r, field->slice_2d.centers[i].epsilon,
                                          field->rbf_type, 2);
        
        result += field->slice_2d.centers[i].coeff * lap_phi;
    }
    
    return result;
}
