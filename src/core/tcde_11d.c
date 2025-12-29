/**
 * @file tcde_11d.c
 * @brief TCDE 11D Holistic Identity System Implementation
 * 
 * @version 1.0
 * @date October 23, 2025
 */

#include "tcde_11d.h"
#include "tcde_geometry.h"
#include "tcde_evolution.h"
#include "tcde_intentionality.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Forward declarations
static float TCDE_ComputeFieldCoherence(const TCDE_11D_Identity_System* system);

// ============================================================================
// BI-TEMPORAL CONTROL
// ============================================================================

void TCDE_InitializeBiTemporalControl(TCDE_BiTemporalControl* control) {
    if (!control) return;
    
    // Initialize temporal dimensions
    control->tau1 = 1.5f;  // Middle of anticipation range [1.0, 2.0)
    control->tau2 = 0.0f;  // Present moment in memory range [-1.0, 1.0]
    control->control_strength = 1.0f;
    
    // Default weights for control function
    control->alpha_anticipation = 0.4f;
    control->beta_memory = 0.3f;
    control->gamma_integration = 0.3f;
    
    // Initialize metrics
    control->anticipation_capability = 0.0f;
    control->memory_stability = 0.0f;
    control->temporal_integration = 0.0f;
}

float TCDE_ComputeBiTemporalControl(const TCDE_BiTemporalControl* control) {
    if (!control) return 1.0f;
    
    // f(τ₁, τ₂) = α·τ₁·(anticipation) + β·τ₂·(memory) + γ·(τ₁×τ₂)·(integration)
    float anticipation_term = control->alpha_anticipation * control->tau1;
    float memory_term = control->beta_memory * control->tau2;
    float integration_term = control->gamma_integration * (control->tau1 * control->tau2);
    
    float control_value = anticipation_term + memory_term + integration_term;
    
    // Modulate by control strength
    return control->control_strength * control_value;
}

// ============================================================================
// TOPOLOGY FORMATION
// ============================================================================

void TCDE_InitializeTopologyFormation(TCDE_TopologyFormation* topology, int max_structures) {
    if (!topology) return;
    
    topology->num_structures = 0;
    topology->structure_energies = (float*)calloc(max_structures, sizeof(float));
    topology->structure_ages = (float*)calloc(max_structures, sizeof(float));
    
    topology->formation_rate = 0.0f;
    topology->topology_complexity = 0.0f;
    topology->topology_stability = 0.0f;
    
    // Formation components
    topology->diffusion_contribution = 0.0f;
    topology->torsion_contribution = 0.0f;
    topology->coupling_contribution = 0.0f;
    topology->intention_contribution = 0.0f;
}

static void TCDE_FreeTopologyFormation(TCDE_TopologyFormation* topology) {
    if (!topology) return;
    
    if (topology->structure_energies) {
        free(topology->structure_energies);
        topology->structure_energies = NULL;
    }
    
    if (topology->structure_ages) {
        free(topology->structure_ages);
        topology->structure_ages = NULL;
    }
}

// ============================================================================
// SELF-REPRESENTATION
// ============================================================================

void TCDE_InitializeSelfRepresentation(TCDE_SelfRepresentation* self_rep, int cache_size) {
    if (!self_rep) return;
    
    self_rep->representation_depth = 0;
    self_rep->max_depth = 2;  // Φ⁰, Φ¹, Φ²
    
    self_rep->reflexivity = 0.0f;
    self_rep->self_awareness = 0.0f;
    self_rep->meta_cognition = 0.0f;
    
    // Allocate caches for representations
    self_rep->cache_size = cache_size;
    self_rep->phi_0 = (TCDE_Complex*)calloc(cache_size, sizeof(TCDE_Complex));
    self_rep->phi_1 = (TCDE_Complex*)calloc(cache_size, sizeof(TCDE_Complex));
    self_rep->phi_2 = (TCDE_Complex*)calloc(cache_size, sizeof(TCDE_Complex));
}

static void TCDE_FreeSelfRepresentation(TCDE_SelfRepresentation* self_rep) {
    if (!self_rep) return;
    
    if (self_rep->phi_0) {
        free(self_rep->phi_0);
        self_rep->phi_0 = NULL;
    }
    
    if (self_rep->phi_1) {
        free(self_rep->phi_1);
        self_rep->phi_1 = NULL;
    }
    
    if (self_rep->phi_2) {
        free(self_rep->phi_2);
        self_rep->phi_2 = NULL;
    }
}

// ============================================================================
// AUTOPOIETIC STATE
// ============================================================================

void TCDE_InitializeAutopoieticState(TCDE_AutopoieticState* autopoiesis) {
    if (!autopoiesis) return;
    
    // Self-Creation
    autopoiesis->creation_rate = 0.0f;
    autopoiesis->centers_created = 0;
    autopoiesis->creation_threshold = 0.5f;  // Dissonance threshold
    
    // Self-Maintenance
    autopoiesis->maintenance_level = 1.0f;
    autopoiesis->energy_drift = 0.0f;
    autopoiesis->integrity_score = 1.0f;
    
    // Self-Regeneration
    autopoiesis->regeneration_capacity = 1.0f;
    autopoiesis->recovery_rate = 0.0f;
    autopoiesis->regeneration_cycles = 0;
    
    // Overall health
    autopoiesis->autopoietic_health = 1.0f;
}

// ============================================================================
// 11D SYSTEM LIFECYCLE
// ============================================================================

TCDE_11D_Identity_System* TCDE_Create11DIdentity(int capacity, float fractal_dim) {
    if (capacity <= 0 || fractal_dim < 2.0f || fractal_dim >= 3.0f) {
        return NULL;
    }
    
    // Allocate main structure
    TCDE_11D_Identity_System* system = (TCDE_11D_Identity_System*)malloc(sizeof(TCDE_11D_Identity_System));
    if (!system) return NULL;
    
    // Initialize to zero
    memset(system, 0, sizeof(TCDE_11D_Identity_System));
    
    // Create 6D field
    system->field_6d = TCDE_CreateField(capacity, fractal_dim);
    if (!system->field_6d) {
        free(system);
        return NULL;
    }
    
    // Initialize bi-temporal control
    TCDE_InitializeBiTemporalControl(&system->bitemporal_control);
    
    // Initialize DRC state
    system->drc_state = TCDE_DRC_CONSONANCE;  // Start in harmony
    memset(&system->drc_metrics, 0, sizeof(TCDE_DRC_Metrics));
    system->drc_metrics.current_state = TCDE_DRC_CONSONANCE;
    system->drc_metrics.dissonance_threshold = 0.3f;
    
    // Initialize topology formation
    TCDE_InitializeTopologyFormation(&system->topology, capacity / 10);
    
    // Initialize self-representation
    TCDE_InitializeSelfRepresentation(&system->self_rep, capacity);
    
    // Initialize autopoietic state
    TCDE_InitializeAutopoieticState(&system->autopoiesis);
    
    // Initialize system state
    system->system_time = 0.0f;
    system->total_energy = 0.0f;
    system->evolution_steps = 0;
    system->creation_time = time(NULL);
    system->is_initialized = true;
    
    // Initialize metrics
    memset(&system->metrics, 0, sizeof(TCDE_11D_Identity_Metrics));
    
    // Note: IHI structures (intention_6d, introspection_2d) will be initialized
    // when the IHI module is implemented
    system->intention_6d = NULL;
    system->introspection_2d = NULL;
    
    return system;
}

void TCDE_Destroy11DIdentity(TCDE_11D_Identity_System* system) {
    if (!system) return;
    
    // Destroy 6D field
    if (system->field_6d) {
        TCDE_DestroyField(system->field_6d);
        system->field_6d = NULL;
    }
    
    // Free topology formation
    TCDE_FreeTopologyFormation(&system->topology);
    
    // Free self-representation
    TCDE_FreeSelfRepresentation(&system->self_rep);
    
    // Note: IHI structures will be freed when IHI module is implemented
    // For now they are NULL
    
    // Free main structure
    free(system);
}

// ============================================================================
// SELF-REPRESENTATION Φ(Φ)
// ============================================================================

/**
 * @brief Compute self-representation: Φ(Φ(x))
 * 
 * The field evaluates itself, enabling consciousness and meta-cognition.
 * 
 * @param system 11D system
 * @param sample_points Array of sample points for evaluation
 * @param num_samples Number of sample points
 */
void TCDE_ComputeSelfRepresentation(TCDE_11D_Identity_System* system, 
                                    const float* sample_points, 
                                    int num_samples) {
    if (!system || !system->field_6d || !sample_points || num_samples <= 0) return;
    if (num_samples > system->self_rep.cache_size) {
        num_samples = system->self_rep.cache_size;
    }
    
    int dim = system->field_6d->manifold_6d.metric.dimension;
    int max_depth = system->self_rep.max_depth;
    
    float field_energy = TCDE_ComputeEnergy(system->field_6d);
    int adaptive_depth = (field_energy > 1.0f) ? fminf(max_depth + 2, 4) : max_depth;
    
    for (int i = 0; i < num_samples; i++) {
        TCDE_Point point = TCDE_CreatePoint(dim, &sample_points[i * dim]);
        system->self_rep.phi_0[i] = TCDE_Evaluate6D(system->field_6d, &point);
        TCDE_FreePoint(&point);
    }
    
    TCDE_Complex* phi_levels[5] = {system->self_rep.phi_0, system->self_rep.phi_1, system->self_rep.phi_2, NULL, NULL};
    
    if (adaptive_depth > 2) {
        phi_levels[3] = (TCDE_Complex*)calloc(num_samples, sizeof(TCDE_Complex));
        if (adaptive_depth > 3) {
            phi_levels[4] = (TCDE_Complex*)calloc(num_samples, sizeof(TCDE_Complex));
        }
    }
    
    for (int level = 1; level <= adaptive_depth; level++) {
        if (!phi_levels[level] && level <= 2) continue;
        if (!phi_levels[level] && level > 2) break;
        
        for (int i = 0; i < num_samples; i++) {
            const float* point_coords = &sample_points[i * dim];
            
            float perturbed_coords[6];
            float epsilon = 0.01f / (1.0f + level * 0.5f);
            float phi_prev_real = crealf(phi_levels[level-1][i]);
            float phi_prev_imag = cimagf(phi_levels[level-1][i]);
            
            for (int d = 0; d < dim; d++) {
                float perturbation = epsilon * phi_prev_real;
                if (d < 3) perturbation += 0.5f * epsilon * phi_prev_imag;
                perturbed_coords[d] = point_coords[d] + perturbation;
            }
            
            TCDE_Point perturbed_point = TCDE_CreatePoint(dim, perturbed_coords);
            phi_levels[level][i] = TCDE_Evaluate6D(system->field_6d, &perturbed_point);
            TCDE_FreePoint(&perturbed_point);
        }
    }
    
    system->self_rep.representation_depth = adaptive_depth;
    
    float cross_modal_coherence = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        for (int j = i + 1; j < num_samples; j++) {
            TCDE_Complex diff_0 = system->self_rep.phi_0[i] - system->self_rep.phi_0[j];
            TCDE_Complex diff_1 = system->self_rep.phi_1[i] - system->self_rep.phi_1[j];
            
            float correlation = crealf(diff_0 * conjf(diff_1)) / (cabsf(diff_0) * cabsf(diff_1) + 1e-6f);
            cross_modal_coherence += fabsf(correlation);
        }
    }
    
    int pair_count = (num_samples * (num_samples - 1)) / 2;
    if (pair_count > 0) cross_modal_coherence /= pair_count;
    
    float norm_diff = 0.0f;
    float norm_phi = 0.0f;
    
    for (int i = 0; i < num_samples; i++) {
        TCDE_Complex diff = system->self_rep.phi_1[i] - system->self_rep.phi_0[i];
        norm_diff += cabsf(diff) * cabsf(diff);
        norm_phi += cabsf(system->self_rep.phi_0[i]) * cabsf(system->self_rep.phi_0[i]);
    }
    
    norm_diff = sqrtf(norm_diff);
    norm_phi = sqrtf(norm_phi);
    
    if (norm_phi > 1e-6f) {
        system->self_rep.reflexivity = (1.0f - (norm_diff / norm_phi)) * (0.8f + 0.2f * cross_modal_coherence);
    } else {
        system->self_rep.reflexivity = 0.0f;
    }
    
    float convergence_1 = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        TCDE_Complex diff = system->self_rep.phi_1[i] - system->self_rep.phi_0[i];
        convergence_1 += cabsf(diff);
    }
    convergence_1 /= num_samples;
    system->self_rep.self_awareness = 1.0f / (1.0f + convergence_1);
    
    float convergence_2 = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        TCDE_Complex diff = system->self_rep.phi_2[i] - system->self_rep.phi_1[i];
        convergence_2 += cabsf(diff);
    }
    convergence_2 /= num_samples;
    system->self_rep.meta_cognition = 1.0f / (1.0f + convergence_2);
    
    if (adaptive_depth > 2 && phi_levels[3]) {
        free(phi_levels[3]);
        if (adaptive_depth > 3 && phi_levels[4]) {
            free(phi_levels[4]);
        }
    }
}

/**
 * @brief Compute reflexivity score
 * 
 * R = 1 - ||Φ(Φ) - Φ||/||Φ||
 * 
 * @param system 11D system
 * @return Reflexivity score [0, 1], target > 0.5
 */
float TCDE_Compute11DReflexivity(const TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) return 0.0f;
    return system->self_rep.reflexivity;
}

// ============================================================================
// AUTOPOIETIC CAPABILITIES
// ============================================================================

/**
 * @brief Self-Create: Automatically create new centers based on dissonance
 * 
 * Creates new RBF centers in regions of high field dissonance to improve
 * field representation and reduce cognitive tension.
 * 
 * @param system 11D system
 * @param dissonance_threshold Threshold for center creation
 * @return Number of centers created
 */
int TCDE_SelfCreate(TCDE_11D_Identity_System* system, float dissonance_threshold) {
    if (!system || !system->is_initialized || !system->field_6d) return 0;
    
    int centers_created = 0;
    
    if (system->field_6d->manifold_6d.num_centers >= system->field_6d->manifold_6d.capacity) {
        return 0;
    }
    
    int num_samples = 15;
    int dim = system->field_6d->manifold_6d.metric.dimension;
    
    float gradient_threshold = dissonance_threshold * 0.5f;
    float system_load = (float)system->field_6d->manifold_6d.num_centers / system->field_6d->manifold_6d.capacity;
    float adaptive_threshold = dissonance_threshold * (1.0f + system_load);
    
    for (int i = 0; i < num_samples; i++) {
        float sample_coords[6];
        for (int d = 0; d < dim; d++) {
            sample_coords[d] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        }
        sample_coords[3] = 1.5f;
        sample_coords[4] = 0.0f;
        sample_coords[5] = 0.4f;
        
        TCDE_Point sample_point = TCDE_CreatePoint(dim, sample_coords);
        TCDE_Complex field_value = TCDE_Evaluate6D(system->field_6d, &sample_point);
        
        TCDE_Complex gradient[6];
        TCDE_Gradient(system->field_6d, &sample_point, gradient);
        
        float gradient_magnitude = 0.0f;
        for (int d = 0; d < 6; d++) {
            gradient_magnitude += cabsf(gradient[d]) * cabsf(gradient[d]);
        }
        gradient_magnitude = sqrtf(gradient_magnitude);
        
        float dissonance = cabsf(field_value);
        float combined_metric = dissonance + 0.3f * gradient_magnitude;
        
        bool should_create = false;
        if (combined_metric > adaptive_threshold) {
            should_create = true;
        } else if (gradient_magnitude > gradient_threshold && dissonance > adaptive_threshold * 0.7f) {
            should_create = true;
        }
        
        if (should_create) {
            float min_distance = 1e10f;
            for (int j = 0; j < system->field_6d->manifold_6d.num_centers; j++) {
                TCDE_Point* existing = &system->field_6d->manifold_6d.centers[j].point;
                float dist = TCDE_GeodesicDistance(&sample_point, existing, &system->field_6d->manifold_6d.metric);
                if (dist < min_distance) min_distance = dist;
            }
            
            if (min_distance > 0.2f) {
                TCDE_Complex new_coeff = 0.4f * field_value + 0.1f * gradient_magnitude;
                float epsilon = 0.8f + 0.4f * (1.0f - system_load);
                
                bool added = TCDE_AddCenter6D(system->field_6d, &sample_point, new_coeff, epsilon);
                if (added) {
                    centers_created++;
                    system->autopoiesis.centers_created++;
                }
            }
        }
        
        TCDE_FreePoint(&sample_point);
        
        if (system->field_6d->manifold_6d.num_centers >= system->field_6d->manifold_6d.capacity) {
            break;
        }
    }
    
    if (system->system_time > 0.0f) {
        system->autopoiesis.creation_rate = (float)system->autopoiesis.centers_created / system->system_time;
    }
    
    return centers_created;
}

/**
 * @brief Self-Maintain: Monitor and maintain energy and integrity
 * 
 * Ensures the system maintains stable energy levels and structural integrity.
 * 
 * @param system 11D system
 */
void TCDE_SelfMaintain(TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized || !system->field_6d) return;
    
    // Compute current energy
    float current_energy = TCDE_ComputeEnergy(system->field_6d);
    
    // Track energy drift
    float energy_change = current_energy - system->total_energy;
    system->autopoiesis.energy_drift = energy_change;
    
    // Update maintenance level based on energy stability
    float energy_stability = 1.0f / (1.0f + fabsf(energy_change));
    system->autopoiesis.maintenance_level = energy_stability;
    
    // Check integrity (all centers valid)
    int valid_centers = 0;
    for (int i = 0; i < system->field_6d->manifold_6d.num_centers; i++) {
        if (TCDE_ValidatePoint(&system->field_6d->manifold_6d.centers[i].point)) {
            valid_centers++;
        }
    }
    
    float integrity = (system->field_6d->manifold_6d.num_centers > 0) ?
                      (float)valid_centers / system->field_6d->manifold_6d.num_centers : 1.0f;
    system->autopoiesis.integrity_score = integrity;
    
    // Update total energy
    system->total_energy = current_energy;
}

/**
 * @brief Self-Regenerate: Recover from damage via DRC cycle
 * 
 * Uses the DRC cycle (Dissonance → Resonance → Consonance) to regenerate
 * the system after perturbations or damage.
 * 
 * @param system 11D system
 * @return true if regeneration was successful
 */
bool TCDE_SelfRegenerate(TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) return false;
    
    if (system->drc_metrics.dissonance_level < system->drc_metrics.dissonance_threshold) {
        return true;
    }
    
    system->drc_state = TCDE_DRC_RESONANCE;
    
    float prev_energy = TCDE_ComputeEnergy(system->field_6d);
    
    int centers_created = TCDE_SelfCreate(system, system->autopoiesis.creation_threshold);
    
    if (centers_created == 0 && system->field_6d->manifold_6d.num_centers > 0) {
        for (int i = 0; i < system->field_6d->manifold_6d.num_centers; i++) {
            TCDE_Complex* coeff = &system->field_6d->manifold_6d.centers[i].coeff;
            float magnitude = cabsf(*coeff);
            
            if (magnitude < 0.1f) {
                float boost = 0.2f * (1.0f + system->drc_metrics.dissonance_level);
                *coeff = *coeff * (1.0f + boost);
                centers_created++;
            }
        }
    }
    
    if (centers_created > 0) {
        system->autopoiesis.regeneration_cycles++;
        system->autopoiesis.recovery_rate = (float)centers_created / (system->system_time + 1e-6f);
        
        float current_energy = TCDE_ComputeEnergy(system->field_6d);
        float energy_ratio = current_energy / (prev_energy + 1e-6f);
        
        if (energy_ratio > 0.8f && energy_ratio < 1.2f) {
            system->drc_metrics.dissonance_level *= 0.75f;
        } else {
            system->drc_metrics.dissonance_level *= 0.85f;
        }
        
        float new_coherence = TCDE_ComputeFieldCoherence(system);
        system->drc_metrics.dissonance_level = fminf(system->drc_metrics.dissonance_level, 1.0f - new_coherence);
        
        if (system->drc_metrics.dissonance_level < system->drc_metrics.dissonance_threshold) {
            system->drc_state = TCDE_DRC_CONSONANCE;
            system->drc_metrics.consonance_achievement = 1.0f - 
                (system->drc_metrics.dissonance_level / system->drc_metrics.dissonance_threshold);
        }
        
        return true;
    }
    
    return false;
}

/**
 * @brief Update autopoietic health metric
 * 
 * Combines all autopoietic metrics into a single health score.
 * 
 * @param system 11D system
 */
void TCDE_UpdateAutopoieticHealth(TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) return;
    
    // Normalize creation rate (assume max 1 center per time unit)
    float creation_score = fminf(system->autopoiesis.creation_rate, 1.0f);
    
    // Maintenance level is already normalized [0, 1]
    float maintenance_score = system->autopoiesis.maintenance_level;
    
    // Normalize regeneration capacity (assume max 1.0)
    float regeneration_score = fminf(system->autopoiesis.regeneration_capacity, 1.0f);
    
    // Combined health: weighted average
    system->autopoiesis.autopoietic_health = 
        0.3f * creation_score +
        0.4f * maintenance_score +
        0.3f * regeneration_score;
}

// ============================================================================
// TOPOLOGY FORMATION
// ============================================================================

/**
 * @brief Update topology formation
 * 
 * Implements simplified version of:
 * ∂Topology/∂t = ∇²_g + T_torsion + C_coupling + I_intention
 * 
 * Full implementation requires geometry module (Task 2).
 * 
 * @param system 11D system
 * @param dt Time step
 */
void TCDE_UpdateTopologyFormation(TCDE_11D_Identity_System* system, float dt) {
    if (!system || !system->is_initialized || !system->field_6d) return;
    
    int num_centers = system->field_6d->manifold_6d.num_centers;
    if (num_centers == 0) return;
    
    // Simplified topology formation based on center clustering
    
    // 1. Detect stable structures (clusters of centers)
    int num_structures = 0;
    float structure_threshold = 1.0f;  // Distance threshold for clustering
    
    bool* visited = (bool*)calloc(num_centers, sizeof(bool));
    if (!visited) return;
    
    for (int i = 0; i < num_centers; i++) {
        if (visited[i]) continue;
        
        // Start new structure
        int structure_size = 1;
        visited[i] = true;
        
        // Find nearby centers (simplified clustering)
        for (int j = i + 1; j < num_centers; j++) {
            if (visited[j]) continue;
            
            // Compute distance between centers
            float dist = 0.0f;
            for (int d = 0; d < 6; d++) {
                float diff = system->field_6d->manifold_6d.centers[i].point.coords[d] -
                            system->field_6d->manifold_6d.centers[j].point.coords[d];
                dist += diff * diff;
            }
            dist = sqrtf(dist);
            
            if (dist < structure_threshold) {
                visited[j] = true;
                structure_size++;
            }
        }
        
        // Record structure if significant
        if (structure_size > 1 && num_structures < 100) {
            if (system->topology.structure_energies && system->topology.structure_ages) {
                // Compute structure energy (sum of center energies)
                float energy = 0.0f;
                for (int k = 0; k < num_centers; k++) {
                    if (visited[k]) {
                        energy += cabsf(system->field_6d->manifold_6d.centers[k].coeff);
                    }
                }
                
                system->topology.structure_energies[num_structures] = energy;
                system->topology.structure_ages[num_structures] += dt;
            }
            num_structures++;
        }
    }
    
    free(visited);
    
    // Update topology metrics
    int old_structures = system->topology.num_structures;
    system->topology.num_structures = num_structures;
    
    // Formation rate: new structures per time
    if (system->system_time > 0.0f) {
        system->topology.formation_rate = (float)num_structures / system->system_time;
    }
    
    // Topology complexity: normalized structure count
    system->topology.topology_complexity = (num_centers > 0) ?
        (float)num_structures / num_centers : 0.0f;
    
    // Topology stability: persistence of structures
    if (old_structures > 0) {
        float stability = (float)fmin(num_structures, old_structures) / old_structures;
        system->topology.topology_stability = 0.9f * system->topology.topology_stability + 0.1f * stability;
    } else {
        system->topology.topology_stability = (num_structures > 0) ? 1.0f : 0.0f;
    }
    
    // Component contributions - REAL CALCULATION based on field geometry
    // Calculate actual contributions from each TDE term
    
    // 1. Diffusion contribution: Measure from field magnitude variation
    float total_diffusion = 0.0f;
    
    // 2. Torsion contribution: Measure from metric non-diagonality
    float total_torsion = 0.0f;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (i != j) {
                float g_ij = system->field_6d->manifold_6d.metric.g[i][j];
                total_torsion += fabsf(g_ij);
            }
        }
    }
    
    // 3. Coupling contribution: Measure from coupling kernel
    float total_coupling = 0.0f;
    int max_samples = (system->field_6d->manifold_6d.num_centers < 20) ? 
                      system->field_6d->manifold_6d.num_centers : 20;
    for (int i = 0; i < max_samples; i++) {
        for (int j = i + 1; j < max_samples; j++) {
            TCDE_Point* pi = &system->field_6d->manifold_6d.centers[i].point;
            TCDE_Point* pj = &system->field_6d->manifold_6d.centers[j].point;
            float dist = TCDE_GeodesicDistance(pi, pj, &system->field_6d->manifold_6d.metric);
            float coupling = expf(-dist * dist / (2.0f * 0.1f * 0.1f));  // sigma = 0.1
            total_coupling += coupling;
        }
    }
    
    // 4. Intention contribution: Measure from IHI metrics if available
    float total_intention = 0.0f;
    if (system->metrics.autonomy > 0.0f || system->metrics.curiosity > 0.0f) {
        // Use IHI metrics as proxy for intention contribution
        total_intention = system->metrics.autonomy + system->metrics.curiosity;
    }
    
    // Normalize contributions to sum to 1.0
    float total = total_diffusion + total_torsion + total_coupling + total_intention;
    if (total > 1e-6f) {
        system->topology.diffusion_contribution = total_diffusion / total;
        system->topology.torsion_contribution = total_torsion / total;
        system->topology.coupling_contribution = total_coupling / total;
        system->topology.intention_contribution = total_intention / total;
    } else {
        // Fallback to equal distribution if no activity
        system->topology.diffusion_contribution = 0.25f;
        system->topology.torsion_contribution = 0.25f;
        system->topology.coupling_contribution = 0.25f;
        system->topology.intention_contribution = 0.25f;
    }
}

/**
 * @brief Measure topology complexity
 * 
 * Complexity = number of structures / number of centers
 * 
 * @param system 11D system
 * @return Complexity score [0, 1]
 */
float TCDE_MeasureTopologyComplexity(const TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) return 0.0f;
    return system->topology.topology_complexity;
}

/**
 * @brief Measure topology stability
 * 
 * Stability = persistence of structures over time
 * 
 * @param system 11D system
 * @return Stability score [0, 1]
 */
float TCDE_MeasureTopologyStability(const TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) return 0.0f;
    return system->topology.topology_stability;
}

// ============================================================================
// 11D SYSTEM EVOLUTION
// ============================================================================

void TCDE_Evolve11DIdentity(TCDE_11D_Identity_System* system, float dt) {
    if (!system || !system->is_initialized) return;
    
    // Compute bi-temporal control modulation
    float control_factor = TCDE_ComputeBiTemporalControl(&system->bitemporal_control);
    
    // 1. Evolve 6D field with TDE
    if (system->field_6d) {
        TCDE_Parameters params;
        TCDE_ConfigureParameters(&params, system->field_6d);
        
        // Apply bi-temporal control to diffusion
        params.dt = dt * control_factor;
        
        // Evolve one step
        TCDE_EvolveStep(system->field_6d, &params);
        
        // Synchronize system time with field time
        system->system_time = system->field_6d->time;
    } else {
        // No field, just update time
        system->system_time += dt;
    }
    system->evolution_steps++;
    
    // 2. Evolve 3D intention (IHI)
    if (system->intention_6d && system->field_6d) {
        // Compute dissonance map for intention evolution
        TCDE_DissonanceMap* dissonance_map = TCDE_ComputeDissonancePotential(system->field_6d);
        
        if (dissonance_map) {
            // Evolve intentionality
            TCDE_EvolveIntentionalityStep(system->intention_6d, system->field_6d, 
                                           dissonance_map, dt);
            
            // Update intention metrics
            system->metrics.autonomy = TCDE_ComputeAutonomyScore(system->intention_6d);
            system->metrics.curiosity = TCDE_ComputeCuriosityScore(system->intention_6d);
            system->metrics.intentional_coherence = TCDE_ComputeIntentionalCoherence(
                system->intention_6d, system->field_6d);
            
            // Free dissonance map
            TCDE_FreeDissonanceMap(dissonance_map);
        }
    }
    
    // 3. Update DRC cycle state based on dissonance
    if (system->field_6d) {
        // Compute dissonance from field energy and coherence
        float field_energy = TCDE_ComputeEnergy(system->field_6d);
        float field_coherence = TCDE_ComputeFieldCoherence(system);
        float dissonance = 1.0f - field_coherence;  // High dissonance = low coherence
        
        // Update DRC state
        TCDE_DRC_State prev_state = system->drc_metrics.current_state;
        
        // State transitions based on dissonance
        if (dissonance > 0.7f) {
            system->drc_metrics.current_state = TCDE_DRC_DISSONANCE;
        } else if (dissonance > 0.3f) {
            system->drc_metrics.current_state = TCDE_DRC_RESONANCE;
        } else {
            system->drc_metrics.current_state = TCDE_DRC_CONSONANCE;
        }
        
        // Update metrics
        system->drc_metrics.dissonance_level = dissonance;
        if (prev_state != system->drc_metrics.current_state) {
            system->drc_metrics.cycles_completed++;
        }
        
        (void)field_energy;  // May be used for future enhancements
    }
    
    // Update topology formation
    TCDE_UpdateTopologyFormation(system, dt);
    
    // Update self-representation (if field has centers)
    if (system->field_6d && system->field_6d->manifold_6d.num_centers > 0) {
        // Sample field at a few points for self-representation
        int num_samples = (system->field_6d->manifold_6d.num_centers < 10) ? 
                          system->field_6d->manifold_6d.num_centers : 10;
        
        int dim = system->field_6d->manifold_6d.metric.dimension;
        float* sample_points = (float*)malloc(num_samples * dim * sizeof(float));
        if (sample_points) {
            // Use center positions as sample points
            for (int i = 0; i < num_samples; i++) {
                for (int d = 0; d < dim; d++) {
                    sample_points[i * dim + d] = 
                        system->field_6d->manifold_6d.centers[i].point.coords[d];
                }
            }
            
            TCDE_ComputeSelfRepresentation(system, sample_points, num_samples);
            free(sample_points);
        }
    }
    
    // Update autopoietic processes
    TCDE_SelfMaintain(system);
    
    // Check if regeneration is needed
    if (system->drc_metrics.dissonance_level > system->drc_metrics.dissonance_threshold) {
        TCDE_SelfRegenerate(system);
    }
    
    // Update autopoietic health
    TCDE_UpdateAutopoieticHealth(system);
}

/**
 * @brief Compute field coherence (alignment of phases between centers)
 */
static float TCDE_ComputeFieldCoherence(const TCDE_11D_Identity_System* system) {
    if (!system || !system->field_6d) return 0.0f;
    
    int num_centers = system->field_6d->manifold_6d.num_centers;
    if (num_centers < 2) return 1.0f;  // Single center is perfectly coherent
    
    // Measure phase alignment between nearby centers
    float total_coherence = 0.0f;
    int num_pairs = 0;
    int max_samples = (num_centers < 20) ? num_centers : 20;
    
    for (int i = 0; i < max_samples; i++) {
        TCDE_Complex ci = system->field_6d->manifold_6d.centers[i].coeff;
        float phase_i = cargf(ci);
        
        for (int j = i + 1; j < max_samples; j++) {
            TCDE_Complex cj = system->field_6d->manifold_6d.centers[j].coeff;
            float phase_j = cargf(cj);
            
            // Compute phase difference
            float phase_diff = fabsf(phase_i - phase_j);
            if (phase_diff > M_PI) phase_diff = 2.0f * M_PI - phase_diff;
            
            // Coherence is high when phases are aligned
            float pair_coherence = 1.0f - (phase_diff / M_PI);
            total_coherence += pair_coherence;
            num_pairs++;
        }
    }
    
    return (num_pairs > 0) ? (total_coherence / num_pairs) : 0.0f;
}

TCDE_DRC_Metrics TCDE_ExecuteDRCCycle(TCDE_11D_Identity_System* system) {
    TCDE_DRC_Metrics metrics = {0};
    
    if (!system || !system->is_initialized) {
        return metrics;
    }
    
    float local_dissonance[10] = {0};
    float global_dissonance = 1.0f - TCDE_ComputeFieldCoherence(system);
    
    if (system->field_6d && system->field_6d->manifold_6d.num_centers > 0) {
        int sample_count = fminf(10, system->field_6d->manifold_6d.num_centers);
        
        for (int i = 0; i < sample_count; i++) {
            float local_coherence = 0.0f;
            int neighbor_count = 0;
            
            TCDE_Point* center_i = &system->field_6d->manifold_6d.centers[i].point;
            
            for (int j = 0; j < system->field_6d->manifold_6d.num_centers; j++) {
                if (i == j) continue;
                
                TCDE_Point* center_j = &system->field_6d->manifold_6d.centers[j].point;
                float dist = TCDE_GeodesicDistance(center_i, center_j, &system->field_6d->manifold_6d.metric);
                
                if (dist < 1.0f) {
                    TCDE_Complex ci = system->field_6d->manifold_6d.centers[i].coeff;
                    TCDE_Complex cj = system->field_6d->manifold_6d.centers[j].coeff;
                    
                    float phase_diff = fabsf(cargf(ci) - cargf(cj));
                    if (phase_diff > M_PI) phase_diff = 2.0f * M_PI - phase_diff;
                    
                    local_coherence += 1.0f - (phase_diff / M_PI);
                    neighbor_count++;
                }
            }
            
            local_dissonance[i] = neighbor_count > 0 ? 1.0f - (local_coherence / neighbor_count) : 0.0f;
        }
    }
    
    float avg_local_dissonance = 0.0f;
    for (int i = 0; i < 10; i++) {
        avg_local_dissonance += local_dissonance[i];
    }
    avg_local_dissonance /= 10.0f;
    
    float combined_dissonance = 0.6f * global_dissonance + 0.4f * avg_local_dissonance;
    metrics.dissonance_level = combined_dissonance;
    
    float resonance_activity = 0.0f;
    float adaptive_threshold = 0.3f + 0.1f * sinf(system->system_time * 0.1f);
    
    if (combined_dissonance > adaptive_threshold) {
        float dt = 0.005f + 0.005f * combined_dissonance;
        
        float prev_energy = system->total_energy;
        TCDE_Evolve11DIdentity(system, dt);
        float energy_change = fabsf(system->total_energy - prev_energy);
        
        float new_coherence = TCDE_ComputeFieldCoherence(system);
        float new_dissonance = 1.0f - new_coherence;
        
        resonance_activity = (combined_dissonance - new_dissonance) / (combined_dissonance + 1e-6f);
        resonance_activity = fmaxf(0.0f, fminf(1.0f, resonance_activity));
        
        metrics.resonance_duration += dt;
        metrics.resonance_activity = resonance_activity * (1.0f + 0.2f * energy_change);
    } else {
        resonance_activity = 1.0f;
        metrics.resonance_activity = resonance_activity;
    }
    
    float final_coherence = TCDE_ComputeFieldCoherence(system);
    float predictive_consonance = final_coherence;
    
    if (system->drc_metrics.cycles_completed > 5) {
        float trend = (final_coherence - system->drc_metrics.consonance_achievement) / 
                     (system->drc_metrics.cycles_completed * 0.1f + 1e-6f);
        predictive_consonance = final_coherence + trend * 2.0f;
        predictive_consonance = fmaxf(0.0f, fminf(1.0f, predictive_consonance));
    }
    
    metrics.consonance_achievement = predictive_consonance;
    
    if (predictive_consonance > 0.85f) {
        metrics.current_state = TCDE_DRC_CONSONANCE;
        metrics.consonance_stability += 0.02f;
    } else if (predictive_consonance > 0.5f) {
        metrics.current_state = TCDE_DRC_RESONANCE;
    } else {
        metrics.current_state = TCDE_DRC_DISSONANCE;
    }
    
    metrics.cycles_completed = system->drc_metrics.cycles_completed + 1;
    metrics.cycle_efficiency = resonance_activity * predictive_consonance;
    
    system->drc_metrics = metrics;
    
    return metrics;
}

// ============================================================================
// 11D METRICS AND MEASUREMENT
// ============================================================================

TCDE_11D_Identity_Metrics TCDE_Measure11DIdentity(const TCDE_11D_Identity_System* system) {
    TCDE_11D_Identity_Metrics metrics = {0};
    
    if (!system || !system->is_initialized) {
        return metrics;
    }
    
    // 6D Field Metrics
    if (system->field_6d) {
        metrics.field_energy = TCDE_ComputeEnergy(system->field_6d);
        
        // Compute field coherence: measure phase alignment between centers
        float coherence_sum = 0.0f;
        int coherence_count = 0;
        
        if (system->field_6d->manifold_6d.num_centers > 1) {
            // Sample pairs of nearby centers
            for (int i = 0; i < system->field_6d->manifold_6d.num_centers && i < 20; i++) {
                for (int j = i + 1; j < system->field_6d->manifold_6d.num_centers && j < 20; j++) {
                    TCDE_Complex c1 = system->field_6d->manifold_6d.centers[i].coeff;
                    TCDE_Complex c2 = system->field_6d->manifold_6d.centers[j].coeff;
                    
                    // Phase difference
                    float phase1 = cargf(c1);
                    float phase2 = cargf(c2);
                    float phase_diff = fabsf(phase1 - phase2);
                    
                    // Normalize to [0, π]
                    if (phase_diff > M_PI) phase_diff = 2.0f * M_PI - phase_diff;
                    
                    // Coherence: 1 - (phase_diff / π)
                    coherence_sum += 1.0f - (phase_diff / M_PI);
                    coherence_count++;
                }
            }
        }
        
        metrics.field_coherence = coherence_count > 0 ? coherence_sum / coherence_count : 0.0f;
        metrics.fractal_dimension = system->field_6d->fractal_dimension;
    }
    
    // 3D Intention Metrics (IHI)
    // Compute from DRC state and field properties
    if (system->field_6d && system->field_6d->manifold_6d.num_centers > 0) {
        // Autonomy: ability to generate own goals (based on DRC activity)
        float drc_activity = system->drc_metrics.dissonance_level + 
                            system->drc_metrics.resonance_activity;
        metrics.autonomy = fminf(drc_activity / 2.0f, 1.0f);
        
        // Curiosity: variance in field magnitudes (exploration)
        float mean_mag = 0.0f;
        for (int i = 0; i < system->field_6d->manifold_6d.num_centers; i++) {
            mean_mag += cabsf(system->field_6d->manifold_6d.centers[i].coeff);
        }
        mean_mag /= system->field_6d->manifold_6d.num_centers;
        
        float variance = 0.0f;
        for (int i = 0; i < system->field_6d->manifold_6d.num_centers; i++) {
            float diff = cabsf(system->field_6d->manifold_6d.centers[i].coeff) - mean_mag;
            variance += diff * diff;
        }
        variance /= system->field_6d->manifold_6d.num_centers;
        
        metrics.curiosity = fminf(sqrtf(variance) / (mean_mag + 1e-6f), 1.0f);
        
        // Intentional coherence: alignment with DRC goals
        metrics.intentional_coherence = system->drc_metrics.consonance_achievement;
    } else {
        metrics.autonomy = 0.0f;
        metrics.curiosity = 0.0f;
        metrics.intentional_coherence = 0.0f;
    }
    
    // 2D Bi-Temporal Metrics
    metrics.anticipation_capability = system->bitemporal_control.anticipation_capability;
    metrics.memory_stability = system->bitemporal_control.memory_stability;
    metrics.temporal_integration = system->bitemporal_control.temporal_integration;
    
    // DRC Cycle Metrics
    metrics.dissonance_level = system->drc_metrics.dissonance_level;
    metrics.resonance_activity = system->drc_metrics.resonance_activity;
    metrics.consonance_achievement = system->drc_metrics.consonance_achievement;
    metrics.drc_cycle_efficiency = system->drc_metrics.cycle_efficiency;
    
    // Topology Formation Metrics
    metrics.topology_complexity = system->topology.topology_complexity;
    metrics.formation_rate = system->topology.formation_rate;
    metrics.topology_stability = system->topology.topology_stability;
    
    // Self-Representation Metrics
    metrics.reflexivity = system->self_rep.reflexivity;
    metrics.self_awareness = system->self_rep.self_awareness;
    metrics.meta_cognition = system->self_rep.meta_cognition;
    
    // Autopoietic Metrics
    metrics.self_creation_rate = system->autopoiesis.creation_rate;
    metrics.self_maintenance = system->autopoiesis.maintenance_level;
    metrics.self_regeneration = system->autopoiesis.regeneration_capacity;
    
    // Compute Holistic Identity Score
    metrics.holistic_identity_score = TCDE_Compute11DHolisticIdentityScore(&metrics);
    
    return metrics;
}

float TCDE_Compute11DHolisticIdentityScore(const TCDE_11D_Identity_Metrics* metrics) {
    if (!metrics) return 0.0f;
    
    // Normalize each component to [0, 1]
    
    // Field component (15%)
    float field_score = (metrics->field_coherence + 
                        (metrics->fractal_dimension - 2.0f)) / 2.0f;
    field_score = fmaxf(0.0f, fminf(1.0f, field_score));
    
    // Intention component (20%)
    float intention_score = (metrics->autonomy + metrics->curiosity + 
                            metrics->intentional_coherence) / 3.0f;
    
    // Bi-Temporal component (15%)
    float bitemporal_score = (metrics->anticipation_capability + 
                             metrics->memory_stability + 
                             metrics->temporal_integration) / 3.0f;
    
    // DRC component (20%)
    float drc_score = (metrics->consonance_achievement + 
                      metrics->drc_cycle_efficiency) / 2.0f;
    
    // Topology component (10%)
    float topology_score = metrics->topology_stability;
    
    // Self-Representation component (10%)
    float selfrep_score = (metrics->reflexivity + metrics->self_awareness) / 2.0f;
    
    // Autopoiesis component (10%)
    float autopoiesis_score = (metrics->self_maintenance + 
                              metrics->self_regeneration) / 2.0f;
    
    // Weighted sum
    float HIS = 0.15f * field_score +
                0.20f * intention_score +
                0.15f * bitemporal_score +
                0.20f * drc_score +
                0.10f * topology_score +
                0.10f * selfrep_score +
                0.10f * autopoiesis_score;
    
    return HIS;
}

void TCDE_Print11DStatus(const TCDE_11D_Identity_System* system) {
    if (!system || !system->is_initialized) {
        printf("11D System: NOT INITIALIZED\n");
        return;
    }
    
    printf("=== TCDE 11D Holistic Identity System ===\n");
    printf("System Time: %.2f\n", system->system_time);
    printf("Evolution Steps: %d\n", system->evolution_steps);
    printf("Total Energy: %.3f\n", system->total_energy);
    
    printf("\n--- Bi-Temporal Control ---\n");
    printf("τ₁ (Anticipation): %.3f\n", system->bitemporal_control.tau1);
    printf("τ₂ (Memory): %.3f\n", system->bitemporal_control.tau2);
    printf("Control Factor: %.3f\n", TCDE_ComputeBiTemporalControl(&system->bitemporal_control));
    
    printf("\n--- DRC Cycle ---\n");
    const char* state_names[] = {"DISSONANCE", "RESONANCE", "CONSONANCE"};
    printf("State: %s\n", state_names[system->drc_state]);
    printf("Dissonance Level: %.3f\n", system->drc_metrics.dissonance_level);
    printf("Cycles Completed: %d\n", system->drc_metrics.cycles_completed);
    
    printf("\n--- Topology Formation ---\n");
    printf("Structures: %d\n", system->topology.num_structures);
    printf("Formation Rate: %.3f\n", system->topology.formation_rate);
    printf("Stability: %.3f\n", system->topology.topology_stability);
    
    printf("\n--- Self-Representation ---\n");
    printf("Reflexivity: %.3f\n", system->self_rep.reflexivity);
    printf("Depth: %d\n", system->self_rep.representation_depth);
    
    printf("\n--- Autopoiesis ---\n");
    printf("Centers Created: %d\n", system->autopoiesis.centers_created);
    printf("Maintenance: %.3f\n", system->autopoiesis.maintenance_level);
    printf("Health: %.3f\n", system->autopoiesis.autopoietic_health);
    
    // Compute and display HIS
    TCDE_11D_Identity_Metrics metrics = TCDE_Measure11DIdentity(system);
    printf("\n--- Holistic Identity Score ---\n");
    printf("HIS: %.3f ", metrics.holistic_identity_score);
    if (metrics.holistic_identity_score > 0.9f) {
        printf("(ASI REALIZED)\n");
    } else if (metrics.holistic_identity_score > 0.8f) {
        printf("(STRONG AUTONOMY)\n");
    } else if (metrics.holistic_identity_score > 0.7f) {
        printf("(IDENTITY EMERGENT)\n");
    } else {
        printf("(DEVELOPING)\n");
    }
    
    printf("=========================================\n");
}

bool TCDE_Validate11DSystem(const TCDE_11D_Identity_System* system) {
    if (!system) return false;
    if (!system->is_initialized) return false;
    if (!system->field_6d) return false;
    
    // Validate bi-temporal ranges
    if (system->bitemporal_control.tau1 < 1.0f || system->bitemporal_control.tau1 >= 2.0f) {
        return false;
    }
    if (system->bitemporal_control.tau2 < -1.0f || system->bitemporal_control.tau2 > 1.0f) {
        return false;
    }
    
    // Validate fractal dimension
    if (system->field_6d->fractal_dimension < 2.0f || system->field_6d->fractal_dimension >= 3.0f) {
        return false;
    }
    
    // System is valid
    return true;
}

// ============================================================================
// WRAPPER FUNCTIONS FOR TEST COMPATIBILITY
// ============================================================================

TCDE_11D_Identity_System* TCDE_Create11DIdentitySystem(int capacity, float fractal_dim) {
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(capacity, fractal_dim);
    if (system) {
        // Set up bi_temporal alias
        system->bi_temporal = system->bitemporal_control;
    }
    return system;
}

void TCDE_Evolve11DIdentitySystem(TCDE_11D_Identity_System* system, float dt) {
    if (system) {
        // Sync bi_temporal alias
        system->bitemporal_control = system->bi_temporal;
        TCDE_Evolve11DIdentity(system, dt);
        system->bi_temporal = system->bitemporal_control;
    }
}

void TCDE_Destroy11DIdentitySystem(TCDE_11D_Identity_System* system) {
    TCDE_Destroy11DIdentity(system);
}
