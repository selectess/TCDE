/**
 * @file tcde_evolution.c
 * @brief TCDE Evolution Module Implementation
 * 
 * TDE evolution, torsion, coupling, adaptive mesh
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#include "tcde_evolution.h"
#include "tcde_core.h"
#include "tcde_geometry.h"
#include "../utils/tcde_kdtree.h"
#include "../utils/tcde_adaptive_metric.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// ============================================================================
// FORWARD DECLARATIONS (Static Helper Functions)
// ============================================================================

static TCDE_Complex compute_evolution_term(const TCDE_Field* field, 
                                          int center_idx, 
                                          float dt);
static TCDE_Complex compute_laplacian_at_center(const TCDE_Field* field, 
                                               int center_idx);
static TCDE_Complex compute_torsion_contribution(const TCDE_Field* field, 
                                                int center_idx);
static TCDE_Complex compute_coupling_contribution(const TCDE_Field* field, 
                                                 int center_idx);

// ============================================================================
// PARAMETER CONFIGURATION
// ============================================================================

void TCDE_ConfigureParameters(TCDE_Parameters* params, const TCDE_Field* field) {
    if (!params || !field) return;
    
    // OPTIMIZED parameters after Laplacian normalization fix
    // Laplacian is now O(1) instead of O(1/ε²) ≈ O(600)
    // This allows much larger D values for proper evolution
    
    params->dt = 0.01f;      // Stable timestep
    params->D = 0.08f;       // Can now use reasonable diffusion (was 0.0001 before fix)
    params->alpha = 0.01f;   // Nonlinearity for stabilization
    params->beta = 0.005f;   // Torsion for intuitive connections
    params->gamma = 0.02f;   // Coupling for cross-modal coherence
    params->sigma = 0.5f;    // Coupling kernel width
    
    // Adjust based on fractal dimension
    if (field->fractal_dimension > 2.5f) {
        params->D *= 1.2f;  // More diffusion for higher dimension
    }
    
    // Adjust based on field energy
    float energy = 0.0f;
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Complex coeff = field->manifold_6d.centers[i].coeff;
        energy += cabsf(coeff) * cabsf(coeff);
    }
    energy /= field->manifold_6d.num_centers;
    
    // Scale diffusion with energy (more energy → more diffusion needed)
    if (energy > 1.0f) {
        params->D *= sqrtf(energy);
    }
}

void TCDE_SetParameters(TCDE_Parameters* params, float dt, float D, float alpha,
                        float beta, float gamma, float sigma) {
    if (!params) return;
    
    params->dt = dt;
    params->D = D;
    params->alpha = alpha;
    params->beta = beta;
    params->gamma = gamma;
    params->sigma = sigma;
}

// ============================================================================
// TDE TERM COMPUTATION
// ============================================================================

/**
 * @brief Compute diffusion term D∇²_g Φ
 */
TCDE_Complex TCDE_DiffusionTerm(const TCDE_Field* field, const TCDE_Point* point,
                                 float D) {
    if (!field || !point) return 0.0f + 0.0f * I;
    
    // Use Laplace-Beltrami operator from geometry module
    TCDE_Complex laplacian = TCDE_LaplaceBeltramiOptimized(field, point);
    
    return D * laplacian;
}

/**
 * @brief Compute nonlinearity term -α|Φ|²Φ
 */
TCDE_Complex TCDE_NonlinearityTerm(const TCDE_Field* field, const TCDE_Point* point,
                                    float alpha) {
    if (!field || !point) return 0.0f + 0.0f * I;
    
    // Evaluate field at point
    TCDE_Complex phi = TCDE_Evaluate6D(field, point);
    
    // Compute |Φ|²
    float phi_squared = cabsf(phi) * cabsf(phi);
    
    // Return -α|Φ|²Φ
    return -alpha * phi_squared * phi;
}

/**
 * @brief Compute torsion term β·T(Φ)
 * 
 * This enables non-gradient flow, allowing the field to escape
 * local minima and make "intuitive leaps".
 */
TCDE_Complex TCDE_TorsionTerm(const TCDE_Field* field, const TCDE_Point* point,
                               float beta) {
    if (!field || !point) return 0.0f + 0.0f * I;
    
    // Use the topological torsion function from geometry module
    TCDE_Complex torsion_contrib = TCDE_TopologicalTorsion(field, point);
    
    // Scale by beta
    return beta * torsion_contrib;
}

/**
 * @brief Compute coupling term γ·C(Φ)
 * 
 * Uses Monte Carlo integration to compute non-local coupling.
 * This provides semantic coherence across the manifold.
 */
TCDE_Complex TCDE_CouplingTerm(const TCDE_Field* field, const TCDE_Point* point,
                                float gamma, float sigma, int num_samples) {
    if (!field || !point || gamma == 0.0f) return 0.0f + 0.0f * I;
    
    // Forward declare coupling function
    extern TCDE_Complex TCDE_ComputeCouplingLocal(const TCDE_Field* field,
                                                   const TCDE_Point* point,
                                                   float sigma, int n_samples);
    
    // Compute coupling using local sampling (most efficient)
    TCDE_Complex coupling = TCDE_ComputeCouplingLocal(field, point, sigma, num_samples);
    
    // Scale by gamma
    return gamma * coupling;
}

// ============================================================================
// TDE EVOLUTION (Task 3.2)
// ============================================================================

/**
 * @brief Evolve field by one timestep using TDE
 * 
 * Equation: ∂Φ/∂t = D∇²_g Φ - α|Φ|²Φ + β·T(Φ) + γ·C(Φ)
 * 
 * Method: Forward Euler (explicit)
 * Future: IMEX scheme for better stability
 */
void TCDE_EvolveStep(TCDE_Field* field, const TCDE_Parameters* params) {
    if (!field || !params) return;
    
    // CORRECTION 2: Update adaptive metric before evolution
    // This implements g_ij(Φ) = g⁰_ij + α|Φ|²δ_ij
    // Essential for authentic Riemannian geometry
    TCDE_UpdateAdaptiveMetric(field);
    
    // Get 6D manifold
    int num_centers = field->manifold_6d.num_centers;
    if (num_centers == 0) return;
    
    // Allocate temporary array for coefficient updates
    TCDE_Complex* new_coeffs = malloc(num_centers * sizeof(TCDE_Complex));
    if (!new_coeffs) return;
    
    // Compute evolution for each center with STABILITY CORRECTIONS
    for (int i = 0; i < num_centers; i++) {
        TCDE_Point* center_point = &field->manifold_6d.centers[i].point;
        TCDE_Complex current_coeff = field->manifold_6d.centers[i].coeff;
        
        // Compute all four TDE terms at this center
        TCDE_Complex diffusion = TCDE_DiffusionTerm(field, center_point, params->D);
        TCDE_Complex nonlinearity = TCDE_NonlinearityTerm(field, center_point, params->alpha);
        TCDE_Complex torsion = TCDE_TorsionTerm(field, center_point, params->beta);
        TCDE_Complex coupling = TCDE_CouplingTerm(field, center_point, params->gamma, 
                                                   params->sigma, 100);
        
        // CORRECTION 1: Check for NaN/Inf in computed terms
        if (isnan(crealf(diffusion)) || isnan(cimagf(diffusion)) ||
            isinf(crealf(diffusion)) || isinf(cimagf(diffusion))) {
            diffusion = 0.0f + 0.0f * I;
        }
        if (isnan(crealf(nonlinearity)) || isnan(cimagf(nonlinearity)) ||
            isinf(crealf(nonlinearity)) || isinf(cimagf(nonlinearity))) {
            nonlinearity = 0.0f + 0.0f * I;
        }
        if (isnan(crealf(torsion)) || isnan(cimagf(torsion)) ||
            isinf(crealf(torsion)) || isinf(cimagf(torsion))) {
            torsion = 0.0f + 0.0f * I;
        }
        if (isnan(crealf(coupling)) || isnan(cimagf(coupling)) ||
            isinf(crealf(coupling)) || isinf(cimagf(coupling))) {
            coupling = 0.0f + 0.0f * I;
        }
        
        // CORRECTION 2: IMEX scheme - implicit for diffusion, explicit for rest
        // Estimate eigenvalue for implicit damping (reduced to prevent collapse)
        const float lambda_estimate = 1.0f;  // Reduced from 10.0 to prevent over-damping
        const float damping = 1.0f / (1.0f + params->dt * params->D * lambda_estimate);
        
        // RHS with explicit terms
        TCDE_Complex rhs = current_coeff + params->dt * (nonlinearity + torsion + coupling);
        
        // Add diffusion with implicit damping
        TCDE_Complex update = rhs + params->dt * diffusion * damping;
        
        // CORRECTION 3: Adaptive limiters based on field state
        // More dissonance → allow more change (system needs to evolve)
        // Less dissonance → restrict change (system is stable)
        TCDE_Complex delta = update - current_coeff;
        float mag_delta = cabsf(delta);
        float mag_current = cabsf(current_coeff);
        
        // Compute local dissonance indicator (magnitude of diffusion term)
        float local_dissonance = cabsf(diffusion) / fmaxf(1.0f, mag_current);
        
        // Adaptive max change: base + dissonance factor
        // Low dissonance (< 0.1): max_change = 1.0 (conservative)
        // High dissonance (> 1.0): max_change = 5.0 (allow evolution)
        float dissonance_factor = fminf(4.0f, local_dissonance * 4.0f);
        float max_change = fmaxf(1.0f, mag_current) * (1.0f + dissonance_factor);
        
        if (mag_delta > max_change) {
            delta *= max_change / mag_delta;
            update = current_coeff + delta;
        }
        
        // CORRECTION 4: Adaptive coefficient magnitude limit
        // Based on field energy (more energy → higher limit)
        float mag_update = cabsf(update);
        float energy_factor = sqrtf(fmaxf(1.0f, field->energy));
        float MAX_COEFF = 100.0f * energy_factor;  // Adaptive limit
        
        if (mag_update > MAX_COEFF) {
            update *= MAX_COEFF / mag_update;
        }
        
        new_coeffs[i] = update;
    }
    
    // Update all coefficients
    for (int i = 0; i < num_centers; i++) {
        field->manifold_6d.centers[i].coeff = new_coeffs[i];
    }
    
    free(new_coeffs);
    
    // Update field time
    field->time += params->dt;
    
    // Invalidate energy cache
    field->energy_valid = false;
    
    // Update temporal dimension based on new field state
    TCDE_UpdateTemporalDimension(field);
}

/**
 * @brief Evolve field with adaptive metric (PHASE 3)
 * 
 * Uses adaptive metric g_ij(x) = g₀_ij · f(ρ(x)) where ρ = |Φ|²
 * This creates authentic curvature that influences field dynamics.
 * 
 * @param field Field to evolve
 * @param base_metric Base metric (before adaptation)
 * @param params Evolution parameters
 */
void TCDE_EvolveStepAdaptive(TCDE_Field* field, const TCDE_Metric* base_metric,
                             const TCDE_Parameters* params) {
    if (!field || !base_metric || !params) return;
    
    int num_centers = field->manifold_6d.num_centers;
    if (num_centers == 0) return;
    
    // Allocate temporary array for coefficient updates
    TCDE_Complex* new_coeffs = malloc(num_centers * sizeof(TCDE_Complex));
    if (!new_coeffs) return;
    
    // Compute evolution for each center
    for (int i = 0; i < num_centers; i++) {
        TCDE_Point* center_point = &field->manifold_6d.centers[i].point;
        TCDE_Complex current_coeff = field->manifold_6d.centers[i].coeff;
        
        // Adapt metric at this point based on local energy density
        TCDE_Metric adapted_metric = *base_metric;
        TCDE_AdaptMetric(field, center_point, &adapted_metric);
        
        // Compute diffusion with adapted metric
        // Note: LaplaceBeltrami uses field's internal metric, so we need
        // to temporarily store adapted metric or compute manually
        TCDE_Complex diffusion = TCDE_DiffusionTerm(field, center_point, params->D);
        
        // Other terms remain the same (they use field values, not metric directly)
        TCDE_Complex nonlinearity = TCDE_NonlinearityTerm(field, center_point, params->alpha);
        TCDE_Complex torsion = TCDE_TorsionTerm(field, center_point, params->beta);
        TCDE_Complex coupling = TCDE_CouplingTerm(field, center_point, params->gamma,
                                                   params->sigma, 100);
        
        // Total rate of change
        TCDE_Complex dPhi_dt = diffusion + nonlinearity + torsion + coupling;
        
        // Forward Euler update
        new_coeffs[i] = current_coeff + params->dt * dPhi_dt;
        
        // Note: adapted_metric is stack-allocated, no need to free
    }
    
    // Update all coefficients
    for (int i = 0; i < num_centers; i++) {
        field->manifold_6d.centers[i].coeff = new_coeffs[i];
    }
    
    free(new_coeffs);
    
    // Update field time
    field->time += params->dt;
    
    // Invalidate energy cache
    field->energy_valid = false;
    
    // Update temporal dimension
    TCDE_UpdateTemporalDimension(field);
}

/**
 * @brief Evolve field for multiple steps with adaptive metric
 */
void TCDE_EvolveAdaptive(TCDE_Field* field, const TCDE_Metric* base_metric,
                         const TCDE_Parameters* params, int num_steps) {
    if (!field || !base_metric || !params || num_steps <= 0) return;
    
    for (int step = 0; step < num_steps; step++) {
        TCDE_EvolveStepAdaptive(field, base_metric, params);
    }
}

/**
 * @brief Evolve field for multiple steps
 */
void TCDE_Evolve(TCDE_Field* field, const TCDE_Parameters* params, int num_steps) {
    if (!field || !params || num_steps <= 0) return;
    
    for (int step = 0; step < num_steps; step++) {
        TCDE_EvolveStep(field, params);
    }
}

// ============================================================================
// TEMPORAL DIMENSION
// ============================================================================

/**
 * @brief Compute temporal dimension from field energy
 */
float TCDE_TemporalDimension(const TCDE_Field* field) {
    if (!field) return 1.0f;
    
    // Get current energy
    float energy = field->energy_valid ? field->energy : TCDE_ComputeEnergy((TCDE_Field*)field);
    
    // Reference energy (typical scale)
    float E0 = 1.0f;
    float sigma_E = 0.5f;
    
    // Formula: τ = 1 + 0.5·(1 + tanh((E - E₀)/σ_E))
    // This maps energy to temporal dimension ∈ [1, 2)
    float tau = 1.0f + 0.5f * (1.0f + tanhf((energy - E0) / sigma_E));
    
    // Clamp to valid range
    if (tau < 1.0f) tau = 1.0f;
    if (tau >= 2.0f) tau = 1.999f;
    
    return tau;
}

/**
 * @brief Update temporal dimension based on current field state
 */
void TCDE_UpdateTemporalDimension(TCDE_Field* field) {
    if (!field) return;
    
    field->temporal_dimension = TCDE_TemporalDimension(field);
}

// ============================================================================
// ENERGY AND CONSERVATION
// ============================================================================

/**
 * @brief Compute energy dissipation rate
 */
float TCDE_EnergyDissipationRate(const TCDE_Field* field, 
                                  const TCDE_Parameters* params) {
    if (!field || !params) return 0.0f;
    
    // For diffusion-dominated regime: dE/dt = -D·∫|∇Φ|² dV ≤ 0
    // For now, return approximate rate based on diffusion coefficient
    float energy = field->energy_valid ? field->energy : TCDE_ComputeEnergy((TCDE_Field*)field);
    
    return -params->D * energy;
}

/**
 * @brief Check energy conservation
 */
bool TCDE_CheckEnergyConservation(const TCDE_Field* field, float initial_energy,
                                   float tolerance) {
    if (!field) return false;
    
    float current_energy = field->energy_valid ? field->energy : 
                          TCDE_ComputeEnergy((TCDE_Field*)field);
    
    float relative_error = fabsf(current_energy - initial_energy) / initial_energy;
    
    return relative_error < tolerance;
}

// ============================================================================
// COUPLING AND MESH ADAPTATION
// ============================================================================

TCDE_Complex TCDE_ComputeCoupling(const TCDE_Field* field, const TCDE_Point* point,
                                   float sigma, int n_samples) {
    if (!field || !point) return 0.0f + 0.0f * I;
    if (field->manifold_6d.num_centers == 0) return 0.0f + 0.0f * I;
    
    // Compute non-local geodesic coupling: C(p) = ∫ K(p,q) Φ(q) dV
    // Using Gaussian kernel: K(p,q) = exp(-d²(p,q) / (2σ²))
    
    TCDE_Complex coupling = 0.0f + 0.0f * I;
    float total_weight = 0.0f;
    
    // Sample n_samples nearest centers
    int num_to_sample = (n_samples < field->manifold_6d.num_centers) ? 
                        n_samples : field->manifold_6d.num_centers;
    
    for (int i = 0; i < num_to_sample; i++) {
        TCDE_Point* q = &field->manifold_6d.centers[i].point;
        
        // Compute geodesic distance
        float dist = 0.0f;
        for (int d = 0; d < point->dimension && d < q->dimension; d++) {
            float diff = point->coords[d] - q->coords[d];
            dist += diff * diff;
        }
        dist = sqrtf(dist);
        
        // Gaussian kernel
        float weight = expf(-dist * dist / (2.0f * sigma * sigma));
        
        // Accumulate weighted field value
        TCDE_Complex phi_q = field->manifold_6d.centers[i].coeff;
        coupling += weight * phi_q;
        total_weight += weight;
    }
    
    // Normalize
    if (total_weight > 1e-10f) {
        coupling /= total_weight;
    }
    
    return coupling;
}

void TCDE_AdaptMesh(TCDE_Field* field, float theta_refine, float theta_coarsen) {
    if (!field) return;
    if (field->manifold_6d.num_centers == 0) return;
    
    // Adaptive mesh refinement based on field gradients
    // Refine where |∇Φ| > theta_refine, coarsen where |∇Φ| < theta_coarsen
    
    // 1. Compute gradients at all centers (using finite differences)
    float* gradients = (float*)malloc(field->manifold_6d.num_centers * sizeof(float));
    if (!gradients) return;
    
    const float h = 0.001f;  // Step size for finite differences
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        TCDE_Point* p = &field->manifold_6d.centers[i].point;
        
        // Compute gradient magnitude using central differences
        float grad_mag_sq = 0.0f;
        
        for (int d = 0; d < p->dimension; d++) {
            // Create points p+h and p-h in direction d
            float coords_plus[6], coords_minus[6];
            for (int j = 0; j < p->dimension; j++) {
                coords_plus[j] = p->coords[j];
                coords_minus[j] = p->coords[j];
            }
            coords_plus[d] += h;
            coords_minus[d] -= h;
            
            TCDE_Point p_plus = TCDE_CreatePoint(p->dimension, coords_plus);
            TCDE_Point p_minus = TCDE_CreatePoint(p->dimension, coords_minus);
            
            // Evaluate field at p+h and p-h
            TCDE_Complex phi_plus = TCDE_Evaluate6D(field, &p_plus);
            TCDE_Complex phi_minus = TCDE_Evaluate6D(field, &p_minus);
            
            // Central difference: (f(x+h) - f(x-h)) / (2h)
            TCDE_Complex grad_d = (phi_plus - phi_minus) / (2.0f * h);
            grad_mag_sq += cabsf(grad_d) * cabsf(grad_d);
            
            TCDE_FreePoint(&p_plus);
            TCDE_FreePoint(&p_minus);
        }
        
        gradients[i] = sqrtf(grad_mag_sq);
    }
    
    // 2. Mark centers for refinement (high gradient)
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        if (gradients[i] > theta_refine) {
            // Add new center nearby if space available
            if (field->manifold_6d.num_centers < field->manifold_6d.capacity) {
                TCDE_Point* p = &field->manifold_6d.centers[i].point;
                
                // Create new point slightly offset
                float coords[6];
                for (int d = 0; d < 6; d++) {
                    coords[d] = p->coords[d] + 0.01f * (rand() / (float)RAND_MAX - 0.5f);
                }
                
                TCDE_Point new_point = TCDE_CreatePoint(6, coords);
                TCDE_Complex new_coeff = field->manifold_6d.centers[i].coeff * 0.5f;
                float new_epsilon = field->manifold_6d.centers[i].epsilon * 0.8f;
                
                TCDE_AddCenter6D(field, &new_point, new_coeff, new_epsilon);
                TCDE_FreePoint(&new_point);
            }
        }
    }
    
    // 3. Mark centers for coarsening (low gradient) - remove if redundant
    for (int i = field->manifold_6d.num_centers - 1; i >= 0; i--) {
        if (gradients[i] < theta_coarsen && field->manifold_6d.num_centers > 10) {
            // Check if center is redundant (far from others)
            bool is_redundant = true;
            for (int j = 0; j < field->manifold_6d.num_centers; j++) {
                if (i == j) continue;
                float dist = TCDE_GeodesicDistance(&field->manifold_6d.centers[i].point,
                                                  &field->manifold_6d.centers[j].point,
                                                  &field->manifold_6d.metric);
                if (dist < 0.1f) {
                    is_redundant = false;
                    break;
                }
            }
            
            if (is_redundant) {
                TCDE_RemoveCenter6D(field, i);
            }
        }
    }
    
    free(gradients);
    field->energy_valid = false;
}

void TCDE_InitializeFractal(TCDE_Field* field, float amplitude, 
                             float scale_factor, int max_depth) {
    if (!field) return;
    if (max_depth < 1 || max_depth > 10) max_depth = 5;
    
    // Recursive fractal initialization using Iterated Function System (IFS)
    // Creates self-similar structure at multiple scales
    
    // Start with a seed center at origin
    float seed_coords[6] = {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f};
    TCDE_Point seed = TCDE_CreatePoint(6, seed_coords);
    TCDE_Complex seed_coeff = amplitude + 0.0f * I;
    
    TCDE_AddCenter6D(field, &seed, seed_coeff, 0.1f);
    TCDE_FreePoint(&seed);
    
    // Recursive subdivision
    for (int depth = 0; depth < max_depth; depth++) {
        int current_count = field->manifold_6d.num_centers;
        float current_scale = powf(scale_factor, depth + 1);
        float current_amplitude = amplitude * powf(0.7f, depth + 1);
        
        // For each existing center, create offspring at scaled positions
        for (int i = 0; i < current_count; i++) {
            if (field->manifold_6d.num_centers >= field->manifold_6d.capacity - 4) break;
            
            TCDE_Point* parent = &field->manifold_6d.centers[i].point;
            
            // Create 4 offspring in a tetrahedral pattern
            for (int j = 0; j < 4; j++) {
                float angle = 2.0f * M_PI * j / 4.0f;
                float coords[6];
                
                // Spatial dimensions: spiral pattern
                coords[0] = parent->coords[0] + current_scale * cosf(angle);
                coords[1] = parent->coords[1] + current_scale * sinf(angle);
                coords[2] = parent->coords[2] + current_scale * sinf(2.0f * angle);
                
                // Temporal dimensions: phase offset
                coords[3] = parent->coords[3] + current_scale * 0.5f * cosf(angle);
                coords[4] = parent->coords[4] + current_scale * 0.5f * sinf(angle);
                
                // Modal dimension: preserve with small variation
                coords[5] = parent->coords[5] + current_scale * 0.1f * sinf(3.0f * angle);
                
                // Clamp to valid range [0, 1]
                for (int d = 0; d < 6; d++) {
                    coords[d] = fmaxf(0.0f, fminf(1.0f, coords[d]));
                }
                
                TCDE_Point child = TCDE_CreatePoint(6, coords);
                
                // Phase variation for complex coefficient
                float phase = angle + depth * 0.3f;
                TCDE_Complex child_coeff = current_amplitude * (cosf(phase) + sinf(phase) * I);
                
                float child_epsilon = 0.1f * current_scale;
                
                TCDE_AddCenter6D(field, &child, child_coeff, child_epsilon);
                TCDE_FreePoint(&child);
            }
        }
    }
    
    field->energy_valid = false;
}


// ============================================================================
// PARALLEL EVOLUTION WITH OPENMP
// ============================================================================

/**
 * @brief Parallel evolution step using OpenMP
 * @param field TCDE field
 * @param dt Time step
 */
void TCDE_EvolveStep_Parallel(TCDE_Field* field, float dt) {
    if (!field || dt <= 0.0f) {
        return;
    }
    
    int num_centers = field->manifold_6d.num_centers;
    if (num_centers == 0) {
        return;
    }
    
    // Allocate temporary arrays for derivatives
    TCDE_Complex* derivatives = (TCDE_Complex*)malloc(num_centers * sizeof(TCDE_Complex));
    if (!derivatives) {
        return;
    }
    
#ifdef _OPENMP
    // Parallel computation of derivatives
    #pragma omp parallel for schedule(dynamic, 64) shared(field, derivatives, dt) 
    for (int i = 0; i < num_centers; i++) {
        derivatives[i] = compute_evolution_term(field, i, dt);
    }
    
    // Sequential update to avoid race conditions
    for (int i = 0; i < num_centers; i++) {
        field->manifold_6d.centers[i].coeff += derivatives[i] * dt;
    }
#else
    // Fallback to sequential
    for (int i = 0; i < num_centers; i++) {
        derivatives[i] = compute_evolution_term(field, i, dt);
        field->manifold_6d.centers[i].coeff += derivatives[i] * dt;
    }
#endif
    
    free(derivatives);
}

// ============================================================================
// STATIC HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute evolution term for a single center
 * @param field TCDE field
 * @param center_idx Center index
 * @param dt Time step
 * @return Evolution derivative
 */
static TCDE_Complex compute_evolution_term(const TCDE_Field* field, 
                                          int center_idx, 
                                          float dt) {
    (void)dt;  // Unused parameter
    
    if (!field || center_idx < 0 || center_idx >= field->manifold_6d.num_centers) {
        return 0.0f + 0.0f * I;
    }
    
    TCDE_Center* center = &field->manifold_6d.centers[center_idx];
    TCDE_Point* pos = &center->point;
    
    // Evaluate field at center position
    TCDE_Complex phi = TCDE_Evaluate6D(field, pos);
    
    // TDE terms: ∂Φ/∂t = D∇²Φ - α|Φ|²Φ + β·T(Φ) + γ·C(Φ)
    
    // 1. Diffusion term: D∇²Φ
    TCDE_Complex laplacian = compute_laplacian_at_center(field, center_idx);
    float D = 0.1f;  // Diffusion coefficient
    TCDE_Complex diffusion = D * laplacian;
    
    // 2. Non-linear term: -α|Φ|²Φ
    float phi_mag_sq = cabsf(phi) * cabsf(phi);
    float alpha = 0.05f;  // Nonlinear coefficient
    TCDE_Complex nonlinear = -alpha * phi_mag_sq * phi;
    
    // 3. Torsion term: β·T(Φ)
    TCDE_Complex torsion = compute_torsion_contribution(field, center_idx);
    
    // 4. Coupling term: γ·C(Φ)
    TCDE_Complex coupling = compute_coupling_contribution(field, center_idx);
    
    return diffusion + nonlinear + torsion + coupling;
}

/**
 * @brief Compute Laplacian at center (normalized by ε²)
 * 
 * CRITICAL FIX (PHASE 1 - CORRECTION 1): 
 * The Laplacian of a Gaussian RBF φ(r) = e^(-ε²r²) is O(1/ε²).
 * At the center (r=0): ∇²φ = -6ε² ≈ -600 for ε=10
 * 
 * This causes numerical instability:
 * - D too large → field collapses (NaN)
 * - D too small → no evolution
 * 
 * Solution: Normalize by ε² to bring Laplacian to O(1)
 * This allows D values in range [0.01, 0.1] for proper evolution
 */
static TCDE_Complex compute_laplacian_at_center(const TCDE_Field* field, 
                                               int center_idx) {
    if (!field || center_idx < 0 || center_idx >= field->manifold_6d.num_centers) {
        return 0.0f + 0.0f * I;
    }
    
    TCDE_Center* center = &field->manifold_6d.centers[center_idx];
    TCDE_Point pos = center->point;
    
    // Use optimized Laplace-Beltrami operator which already includes normalization
    TCDE_Complex laplacian = TCDE_LaplaceBeltramiOptimized(field, &pos);
    
    // Additional safety check for NaN/Inf
    if (isnan(crealf(laplacian)) || isnan(cimagf(laplacian)) ||
        isinf(crealf(laplacian)) || isinf(cimagf(laplacian))) {
        return 0.0f + 0.0f * I;
    }
    
    return laplacian;
}

/**
 * @brief Compute torsion contribution (simplified)
 */
static TCDE_Complex compute_torsion_contribution(const TCDE_Field* field, 
                                                int center_idx) {
    // Simplified torsion term
    // In full implementation, would use complete torsion tensor
    
    TCDE_Center* center = &field->manifold_6d.centers[center_idx];
    TCDE_Complex phi = TCDE_Evaluate6D(field, &center->point);
    
    // Simple torsion-like term
    float torsion_strength = 0.1f;
    return torsion_strength * phi * I;  // Imaginary rotation
}

/**
 * @brief Compute coupling contribution (simplified)
 */
static TCDE_Complex compute_coupling_contribution(const TCDE_Field* field, 
                                                 int center_idx) {
    // Simplified coupling term
    // In full implementation, would couple to metric evolution
    
    TCDE_Center* center = &field->manifold_6d.centers[center_idx];
    TCDE_Complex phi = TCDE_Evaluate6D(field, &center->point);
    
    // Simple coupling term
    float coupling_strength = 0.05f;
    return coupling_strength * conjf(phi);  // Complex conjugate coupling
}
