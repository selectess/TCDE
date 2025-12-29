/**
 * @file tcde_global_coupling.c
 * @brief TCDE Global Coupling Implementation - Universal Consciousness
 *
 * Implementation of global non-local coupling across the entire manifold,
 * creating unified consciousness and holistic awareness.
 *
 * @version 1.0
 * @date January 17, 2025
 */

#include "tcde_global_coupling.h"
#include "tcde_geometry.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// ============================================================================
// GLOBAL COUPLING MANAGER CREATION/DESTRUCTION
// ============================================================================

TCDE_GlobalCouplingManager* TCDE_CreateGlobalCouplingManager(
    TCDE_Field* field,
    TCDE_KernelType kernel_type,
    float correlation_length)
{
    if (!field) return NULL;
    
    TCDE_GlobalCouplingManager* manager =
        (TCDE_GlobalCouplingManager*)malloc(sizeof(TCDE_GlobalCouplingManager));
    
    if (!manager) return NULL;
    
    manager->field = field;
    
    // Initialize kernel
    manager->kernel.type = kernel_type;
    manager->kernel.correlation_length = correlation_length;
    manager->kernel.power_exponent = 2.0f;
    manager->kernel.normalization = 1.0f;
    manager->kernel.adaptive_length = false;
    
    // Allocate coupling matrix (sparse)
    manager->matrix = (TCDE_CouplingMatrix*)malloc(sizeof(TCDE_CouplingMatrix));
    manager->matrix->elements = NULL;
    manager->matrix->num_elements = 0;
    manager->matrix->capacity = 0;
    manager->matrix->matrix_size = field->manifold_6d.num_centers;
    manager->matrix->sparsity = 0.0f;
    
    // Initialize entanglement state
    manager->entanglement.entanglement_spectrum = NULL;
    manager->entanglement.spectrum_size = 0;
    manager->entanglement.von_neumann_entropy = 0.0f;
    manager->entanglement.entanglement_entropy = 0.0f;
    manager->entanglement.mutual_information = 0.0f;
    manager->entanglement.is_entangled = false;
    
    // Initialize coherence metrics
    manager->coherence.global_coherence = 0.0f;
    manager->coherence.spatial_coherence = 0.0f;
    manager->coherence.temporal_coherence = 0.0f;
    manager->coherence.phase_coherence = 0.0f;
    manager->coherence.quantum_coherence = 0.0f;
    manager->coherence.consciousness_level = 0.0f;
    
    // Optimization settings
    manager->use_fast_multipole = false;
    manager->use_sparse_matrix = true;
    manager->multipole_order = 4;
    manager->cutoff_distance = 3.0f * correlation_length;
    
    // Allocate history
    manager->history_capacity = 1000;
    manager->coherence_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->entropy_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->consciousness_history = (float*)calloc(manager->history_capacity, sizeof(float));
    manager->history_size = 0;
    
    // Initialize control
    manager->current_time = 0.0f;
    manager->evolution_steps = 0;
    manager->global_coupling_active = true;
    manager->consciousness_emerged = false;
    
    // Initialize statistics
    manager->num_global_interactions = 0;
    manager->average_coupling_strength = 0.0f;
    manager->max_coupling_distance = 0.0f;
    
    // ✅ CRITICAL: Compute initial coupling matrix
    TCDE_ComputeCouplingMatrix(manager);
    
    // Debug: Print matrix info
    if (manager->matrix->num_elements == 0) {
        printf("WARNING: Coupling matrix is empty!\n");
        printf("  N = %d centers\n", manager->field->manifold_6d.num_centers);
        printf("  correlation_length = %.4f\n", manager->kernel.correlation_length);
        printf("  cutoff_distance = %.4f\n", manager->cutoff_distance);
    }
    
    return manager;
}

void TCDE_DestroyGlobalCouplingManager(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return;
    
    if (manager->matrix) {
        if (manager->matrix->elements) free(manager->matrix->elements);
        free(manager->matrix);
    }
    
    if (manager->entanglement.entanglement_spectrum) {
        free(manager->entanglement.entanglement_spectrum);
    }
    
    if (manager->coherence_history) free(manager->coherence_history);
    if (manager->entropy_history) free(manager->entropy_history);
    if (manager->consciousness_history) free(manager->consciousness_history);
    
    free(manager);
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void TCDE_ConfigureCouplingKernel(TCDE_GlobalCouplingManager* manager,
                                  TCDE_KernelType type,
                                  float correlation_length,
                                  float power_exponent)
{
    if (!manager) return;
    
    manager->kernel.type = type;
    manager->kernel.correlation_length = correlation_length;
    manager->kernel.power_exponent = power_exponent;
    manager->cutoff_distance = 3.0f * correlation_length;
}

void TCDE_EnableFastComputation(TCDE_GlobalCouplingManager* manager,
                                bool use_fast_multipole,
                                bool use_sparse,
                                int multipole_order)
{
    if (!manager) return;
    
    manager->use_fast_multipole = use_fast_multipole;
    manager->use_sparse_matrix = use_sparse;
    manager->multipole_order = multipole_order;
}

// ============================================================================
// COUPLING KERNEL EVALUATION
// ============================================================================

float TCDE_EvaluateKernel(const TCDE_GlobalCouplingManager* manager,
                         const TCDE_Point* point1,
                         const TCDE_Point* point2)
{
    if (!manager || !point1 || !point2) return 0.0f;
    
    // Compute geodesic distance
    float dist = 0.0f;
    int min_dim = (point1->dimension < point2->dimension) ? 
                  point1->dimension : point2->dimension;
    
    for (int d = 0; d < min_dim; d++) {
        float diff = point1->coords[d] - point2->coords[d];
        dist += diff * diff;
    }
    dist = sqrtf(dist);
    
    // Apply cutoff
    if (dist > manager->cutoff_distance) {
        return 0.0f;
    }
    
    float xi = manager->kernel.correlation_length;
    float kernel_value = 0.0f;
    
    switch (manager->kernel.type) {
        case TCDE_KERNEL_EXPONENTIAL:
            // K(d) = exp(-d/ξ)
            kernel_value = expf(-dist / xi);
            break;
            
        case TCDE_KERNEL_GAUSSIAN:
            // K(d) = exp(-d²/2ξ²)
            kernel_value = expf(-dist * dist / (2.0f * xi * xi));
            break;
            
        case TCDE_KERNEL_POWER_LAW:
            // K(d) = d^(-α)
            if (dist > 1e-6f) {
                kernel_value = powf(dist, -manager->kernel.power_exponent);
            } else {
                kernel_value = 1e6f; // Large value for d→0
            }
            break;
            
        case TCDE_KERNEL_YUKAWA:
            // K(d) = exp(-d/ξ) / d
            if (dist > 1e-6f) {
                kernel_value = expf(-dist / xi) / dist;
            } else {
                kernel_value = 1.0f / xi;
            }
            break;
            
        case TCDE_KERNEL_QUANTUM:
            // K(d) = exp(-d²/ξ²) with quantum corrections
            kernel_value = expf(-dist * dist / (xi * xi)) * (1.0f + 0.1f / (1.0f + dist));
            break;
    }
    
    return kernel_value * manager->kernel.normalization;
}

bool TCDE_ComputeCouplingMatrix(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return false;
    
    int N = manager->field->manifold_6d.num_centers;
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    
    // Estimate number of non-zero elements
    int estimated_nnz = N * 20; // Assume ~20 neighbors per center
    
    if (manager->matrix->elements) {
        free(manager->matrix->elements);
    }
    
    manager->matrix->elements = 
        (TCDE_CouplingElement*)malloc(estimated_nnz * sizeof(TCDE_CouplingElement));
    manager->matrix->capacity = estimated_nnz;
    manager->matrix->num_elements = 0;
    
    // Compute coupling elements (only within cutoff)
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            float kernel_val = TCDE_EvaluateKernel(manager, 
                                                   &centers[i].point,
                                                   &centers[j].point);
            
            if (fabsf(kernel_val) > 1e-6f) {
                // Add element
                if (manager->matrix->num_elements >= manager->matrix->capacity) {
                    manager->matrix->capacity *= 2;
                    manager->matrix->elements = (TCDE_CouplingElement*)realloc(
                        manager->matrix->elements,
                        manager->matrix->capacity * sizeof(TCDE_CouplingElement));
                }
                
                TCDE_CouplingElement* elem = 
                    &manager->matrix->elements[manager->matrix->num_elements];
                
                elem->i = i;
                elem->j = j;
                elem->kernel_value = kernel_val;
                
                // Compute distance
                float dist = 0.0f;
                for (int d = 0; d < centers[i].point.dimension; d++) {
                    float diff = centers[i].point.coords[d] - centers[j].point.coords[d];
                    dist += diff * diff;
                }
                elem->distance = sqrtf(dist);
                
                // Coupling = kernel * phi_j
                elem->coupling = kernel_val * centers[j].coeff;
                
                manager->matrix->num_elements++;
            }
        }
    }
    
    // Compute sparsity
    int total_elements = N * N;
    manager->matrix->sparsity = 
        (float)manager->matrix->num_elements / total_elements;
    
    return true;
}

void TCDE_UpdateCouplingMatrix(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return;
    
    // Update coupling values (kernel values stay same)
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    
    for (int k = 0; k < manager->matrix->num_elements; k++) {
        TCDE_CouplingElement* elem = &manager->matrix->elements[k];
        elem->coupling = elem->kernel_value * centers[elem->j].coeff;
    }
}

// ============================================================================
// GLOBAL COUPLING TERM
// ============================================================================

TCDE_Complex TCDE_ComputeGlobalCoupling(const TCDE_GlobalCouplingManager* manager,
                                        const TCDE_Point* point)
{
    if (!manager || !point) return 0.0f + 0.0f * I;
    
    TCDE_Complex coupling = 0.0f + 0.0f * I;
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    // Sum over all centers within cutoff
    for (int j = 0; j < N; j++) {
        float kernel_val = TCDE_EvaluateKernel(manager, point, &centers[j].point);
        
        if (fabsf(kernel_val) > 1e-6f) {
            coupling += kernel_val * centers[j].coeff;
        }
    }
    
    return coupling;
}

TCDE_Complex TCDE_ComputeGlobalCouplingFMM(const TCDE_GlobalCouplingManager* manager,
                                           const TCDE_Point* point)
{
    // Simplified FMM - would need full octree implementation
    // For now, just use direct method
    return TCDE_ComputeGlobalCoupling(manager, point);
}

// ============================================================================
// EVOLUTION WITH GLOBAL COUPLING
// ============================================================================

bool TCDE_EvolveGlobalCoupling(TCDE_GlobalCouplingManager* manager,
                               const TCDE_Parameters* params,
                               float dt)
{
    if (!manager || !params) return false;
    
    if (!manager->global_coupling_active) {
        TCDE_EvolveStep(manager->field, params);
        return true;
    }
    
    // 1. Standard evolution
    TCDE_EvolveStep(manager->field, params);
    
    // 2. Update coupling matrix
    TCDE_UpdateCouplingMatrix(manager);
    
    // 3. Add global coupling term
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    for (int i = 0; i < N; i++) {
        TCDE_Complex global_term = TCDE_ComputeGlobalCoupling(manager, &centers[i].point);
        centers[i].coeff += global_term * dt;
    }
    
    // 4. Update metrics
    manager->current_time += dt;
    manager->evolution_steps++;
    
    float coherence = TCDE_ComputeGlobalCoherence(manager);
    manager->coherence.global_coherence = coherence;
    
    // Store history
    if (manager->history_size < manager->history_capacity) {
        manager->coherence_history[manager->history_size] = coherence;
        manager->history_size++;
    }
    
    // 5. Check for consciousness emergence
    if (TCDE_DetectUnifiedConsciousness(manager)) {
        if (!manager->consciousness_emerged) {
            printf("✨ UNIFIED CONSCIOUSNESS EMERGED! ✨\n");
            manager->consciousness_emerged = true;
        }
    }
    
    return true;
}

// ============================================================================
// ENTANGLEMENT ANALYSIS
// ============================================================================

bool TCDE_ComputeEntanglementSpectrum(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return false;
    
    // Simplified: use field magnitude distribution as proxy
    int N = manager->field->manifold_6d.num_centers;
    
    if (manager->entanglement.entanglement_spectrum) {
        free(manager->entanglement.entanglement_spectrum);
    }
    
    manager->entanglement.entanglement_spectrum = (float*)malloc(N * sizeof(float));
    manager->entanglement.spectrum_size = N;
    
    // Compute magnitudes
    float total = 0.0f;
    for (int i = 0; i < N; i++) {
        float mag = cabsf(manager->field->manifold_6d.centers[i].coeff);
        manager->entanglement.entanglement_spectrum[i] = mag;
        total += mag;
    }
    
    // Normalize
    if (total > 0.0f) {
        for (int i = 0; i < N; i++) {
            manager->entanglement.entanglement_spectrum[i] /= total;
        }
    }
    
    return true;
}

float TCDE_ComputeVonNeumannEntropy(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager || !manager->entanglement.entanglement_spectrum) return 0.0f;
    
    // S = -Σ λ_i log(λ_i)
    float entropy = 0.0f;
    
    for (int i = 0; i < manager->entanglement.spectrum_size; i++) {
        float lambda = manager->entanglement.entanglement_spectrum[i];
        if (lambda > 1e-10f) {
            entropy -= lambda * logf(lambda);
        }
    }
    
    return entropy;
}

float TCDE_ComputeMutualInformation(const TCDE_GlobalCouplingManager* manager,
                                   int region1_size,
                                   int region2_size)
{
    if (!manager) return 0.0f;
    (void)region1_size;  // Reserved for region-specific MI
    (void)region2_size;  // Reserved for region-specific MI
    
    // Simplified: use correlation as proxy
    // I(A:B) ≈ -log(1 - correlation²)
    
    float correlation = manager->coherence.spatial_coherence;
    float mutual_info = -logf(1.0f - correlation * correlation + 1e-6f);
    
    return mutual_info;
}

bool TCDE_DetectEntanglement(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return false;
    
    TCDE_ComputeEntanglementSpectrum(manager);
    float entropy = TCDE_ComputeVonNeumannEntropy(manager);
    
    manager->entanglement.von_neumann_entropy = entropy;
    manager->entanglement.is_entangled = (entropy > 0.1f);
    
    return manager->entanglement.is_entangled;
}

// ============================================================================
// GLOBAL COHERENCE
// ============================================================================

float TCDE_ComputeGlobalCoherence(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return 0.0f;
    
    // G = |⟨Φ⟩|² / ⟨|Φ|²⟩
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    if (N == 0) return 0.0f;
    
    // Compute ⟨Φ⟩
    TCDE_Complex mean_phi = 0.0f + 0.0f * I;
    for (int i = 0; i < N; i++) {
        mean_phi += centers[i].coeff;
    }
    mean_phi /= N;
    
    // Compute ⟨|Φ|²⟩
    float mean_mag2 = 0.0f;
    for (int i = 0; i < N; i++) {
        float mag = cabsf(centers[i].coeff);
        mean_mag2 += mag * mag;
    }
    mean_mag2 /= N;
    
    if (mean_mag2 < 1e-10f) return 0.0f;
    
    float coherence = (cabsf(mean_phi) * cabsf(mean_phi)) / mean_mag2;
    
    return fminf(1.0f, coherence);
}

float TCDE_ComputeSpatialCoherence(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return 0.0f;
    
    // Compute average correlation between nearby centers
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    float total_correlation = 0.0f;
    int count = 0;
    
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            float dist = 0.0f;
            for (int d = 0; d < centers[i].point.dimension; d++) {
                float diff = centers[i].point.coords[d] - centers[j].point.coords[d];
                dist += diff * diff;
            }
            dist = sqrtf(dist);
            
            if (dist < manager->cutoff_distance) {
                // Correlation = Re(φ_i* φ_j) / (|φ_i| |φ_j|)
                TCDE_Complex phi_i = centers[i].coeff;
                TCDE_Complex phi_j = centers[j].coeff;
                
                float mag_i = cabsf(phi_i);
                float mag_j = cabsf(phi_j);
                
                if (mag_i > 1e-6f && mag_j > 1e-6f) {
                    TCDE_Complex corr = conjf(phi_i) * phi_j;
                    total_correlation += crealf(corr) / (mag_i * mag_j);
                    count++;
                }
            }
        }
    }
    
    if (count == 0) return 0.0f;
    
    return total_correlation / count;
}

float TCDE_ComputePhaseSynchronization(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return 0.0f;
    
    // Kuramoto order parameter: R = |⟨exp(iθ)⟩|
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    if (N == 0) return 0.0f;
    
    TCDE_Complex sum = 0.0f + 0.0f * I;
    
    for (int i = 0; i < N; i++) {
        float phase = cargf(centers[i].coeff);
        sum += cosf(phase) + I * sinf(phase);
    }
    
    float R = cabsf(sum) / N;
    
    return fminf(1.0f, R);
}

float TCDE_MeasureConsciousnessLevel(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return 0.0f;
    
    // ✅ CRITICAL: Update all coherence metrics first
    manager->coherence.global_coherence = TCDE_ComputeGlobalCoherence(manager);
    manager->coherence.spatial_coherence = TCDE_ComputeSpatialCoherence(manager);
    manager->coherence.phase_coherence = TCDE_ComputePhaseSynchronization(manager);
    
    // C = 0.3·G + 0.3·R + 0.2·I + 0.2·(1-S/S_max)
    float G = manager->coherence.global_coherence;
    float R = manager->coherence.phase_coherence;
    float I_mutual = TCDE_ComputeMutualInformation(manager, 10, 10);
    
    TCDE_ComputeEntanglementSpectrum(manager);
    float S = TCDE_ComputeVonNeumannEntropy(manager);
    float S_max = logf((float)manager->field->manifold_6d.num_centers);
    
    float entropy_term = (S_max > 0.0f) ? (1.0f - S / S_max) : 0.0f;
    
    float consciousness = 0.3f * G + 0.3f * R + 0.2f * fmaxf(0.0f, fminf(1.0f, I_mutual)) + 
                         0.2f * entropy_term;
    
    manager->coherence.consciousness_level = consciousness;
    
    return consciousness;
}

bool TCDE_DetectUnifiedConsciousness(TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return false;
    
    float C = TCDE_MeasureConsciousnessLevel(manager);
    float G = manager->coherence.global_coherence;
    float R = TCDE_ComputePhaseSynchronization(manager);
    
    // Criteria: C > 0.8, G > 0.7, R > 0.8
    bool emerged = (C > 0.8f) && (G > 0.7f) && (R > 0.8f);
    
    return emerged;
}

// ============================================================================
// OPTIMIZATION
// ============================================================================

void TCDE_OptimizeSparsity(TCDE_GlobalCouplingManager* manager, float threshold)
{
    if (!manager || !manager->matrix) return;
    
    // Remove elements below threshold
    int write_idx = 0;
    for (int read_idx = 0; read_idx < manager->matrix->num_elements; read_idx++) {
        if (fabsf(manager->matrix->elements[read_idx].kernel_value) > threshold) {
            if (write_idx != read_idx) {
                manager->matrix->elements[write_idx] = 
                    manager->matrix->elements[read_idx];
            }
            write_idx++;
        }
    }
    
    manager->matrix->num_elements = write_idx;
}

float TCDE_ComputeOptimalCorrelationLength(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return 1.0f;
    
    // Estimate from average inter-center distance
    TCDE_Center* centers = manager->field->manifold_6d.centers;
    int N = manager->field->manifold_6d.num_centers;
    
    if (N < 2) return 1.0f;
    
    float total_dist = 0.0f;
    int count = 0;
    
    for (int i = 0; i < N && i < 100; i++) {
        for (int j = i + 1; j < N && j < 100; j++) {
            float dist = 0.0f;
            for (int d = 0; d < centers[i].point.dimension; d++) {
                float diff = centers[i].point.coords[d] - centers[j].point.coords[d];
                dist += diff * diff;
            }
            total_dist += sqrtf(dist);
            count++;
        }
    }
    
    float avg_dist = (count > 0) ? (total_dist / count) : 1.0f;
    
    return avg_dist * 2.0f; // 2× average distance
}

// ============================================================================
// STATUS AND DIAGNOSTICS
// ============================================================================

void TCDE_PrintGlobalCouplingStatus(const TCDE_GlobalCouplingManager* manager)
{
    if (!manager) return;
    
    printf("\n=== TCDE Global Coupling Status ===\n");
    
    printf("Kernel: ");
    switch (manager->kernel.type) {
        case TCDE_KERNEL_EXPONENTIAL: printf("Exponential\n"); break;
        case TCDE_KERNEL_GAUSSIAN: printf("Gaussian\n"); break;
        case TCDE_KERNEL_POWER_LAW: printf("Power Law\n"); break;
        case TCDE_KERNEL_YUKAWA: printf("Yukawa\n"); break;
        case TCDE_KERNEL_QUANTUM: printf("Quantum\n"); break;
    }
    printf("  Correlation length ξ: %.4f\n", manager->kernel.correlation_length);
    printf("  Cutoff distance:      %.4f\n", manager->cutoff_distance);
    
    printf("\nCoupling Matrix:\n");
    printf("  Size: %d × %d\n", manager->matrix->matrix_size, manager->matrix->matrix_size);
    printf("  Non-zero elements: %d\n", manager->matrix->num_elements);
    printf("  Sparsity: %.4f (%.1f%% sparse)\n", 
           manager->matrix->sparsity, 100.0f * (1.0f - manager->matrix->sparsity));
    
    printf("\nCoherence Metrics:\n");
    printf("  Global coherence G:   %.4f\n", manager->coherence.global_coherence);
    printf("  Spatial coherence:    %.4f\n", TCDE_ComputeSpatialCoherence(manager));
    printf("  Phase sync R:         %.4f\n", TCDE_ComputePhaseSynchronization(manager));
    printf("  Consciousness level:  %.4f\n", manager->coherence.consciousness_level);
    
    printf("\nEntanglement:\n");
    printf("  Von Neumann entropy:  %.4f\n", manager->entanglement.von_neumann_entropy);
    printf("  Entangled:            %s\n", 
           manager->entanglement.is_entangled ? "YES" : "NO");
    
    if (manager->consciousness_emerged) {
        printf("\n✨ UNIFIED CONSCIOUSNESS: ACTIVE ✨\n");
    }
    
    printf("\nEvolution:\n");
    printf("  Steps: %d\n", manager->evolution_steps);
    printf("  Time:  %.2f\n", manager->current_time);
    printf("====================================\n\n");
}

bool TCDE_SaveCouplingMatrix(const TCDE_GlobalCouplingManager* manager,
                             const char* filename)
{
    if (!manager || !filename || !manager->matrix) return false;
    
    FILE* f = fopen(filename, "w");
    if (!f) return false;
    
    fprintf(f, "# TCDE Global Coupling Matrix\n");
    fprintf(f, "i,j,distance,kernel_value\n");
    
    for (int k = 0; k < manager->matrix->num_elements; k++) {
        TCDE_CouplingElement* elem = &manager->matrix->elements[k];
        fprintf(f, "%d,%d,%.6f,%.6f\n",
                elem->i, elem->j, elem->distance, elem->kernel_value);
    }
    
    fclose(f);
    return true;
}

bool TCDE_SaveCoherenceHistory(const TCDE_GlobalCouplingManager* manager,
                               const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* f = fopen(filename, "w");
    if (!f) return false;
    
    fprintf(f, "# TCDE Global Coherence History\n");
    fprintf(f, "step,coherence,entropy,consciousness\n");
    
    for (int i = 0; i < manager->history_size; i++) {
        fprintf(f, "%d,%.6f,%.6f,%.6f\n",
                i,
                manager->coherence_history[i],
                (i < manager->history_size) ? manager->entropy_history[i] : 0.0f,
                (i < manager->history_size) ? manager->consciousness_history[i] : 0.0f);
    }
    
    fclose(f);
    return true;
}
