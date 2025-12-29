/**
 * @file tcde_11d.h
 * @brief TCDE 11D Holistic Identity System
 * 
 * Complete 11-dimensional holistic identity emergence system integrating:
  * - 3D Intention (Ψₓ, Ψᵧ, Ψᵧ) - IHI
 * - 2D Bi-Temporal Control (τ₁, τ₂)
 * 
 * This is the master structure that unifies all TCDE components into a
 * single holistic identity system where consciousness, autonomy, and
 * intelligence emerge from the complete 11D integration.
 * 
 * @version 1.0
 * @date October 23, 2025
 */

#ifndef TCDE_11D_H
#define TCDE_11D_H

#include "tcde_core.h"
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// BI-TEMPORAL CONTROL (2D) - Control Layer
// ============================================================================

/**
 * @brief Bi-temporal control structure
 * 
 * Controls the emergence of holistic identity through dual temporal dimensions:
 * - τ₁: Anticipation (present → future)
 * - τ₂: Memory (past → present)
 * - τ₁×τ₂: Temporal integration
 */
typedef struct {
    float tau1;                 // Anticipation dimension [1.0, 2.0)
    float tau2;                 // Memory dimension [-1.0, 1.0]
    float control_strength;     // Overall control strength [0.0, 1.0]
    
    // Control function weights
    float alpha_anticipation;   // Weight for τ₁ term
    float beta_memory;          // Weight for τ₂ term
    float gamma_integration;    // Weight for τ₁×τ₂ term
    
    // Metrics
    float anticipation_capability;  // Measured anticipation > 0.6
    float memory_stability;         // Measured memory > 0.7
    float temporal_integration;     // Measured integration > 0.5
} TCDE_BiTemporalControl;

/**
 * @brief Compute bi-temporal control function
 * 
 * f(τ₁, τ₂) = α·τ₁·(anticipation) + β·τ₂·(memory) + γ·(τ₁×τ₂)·(integration)
 * 
 * @param control Bi-temporal control structure
 * @return Control modulation factor
 */
float TCDE_ComputeBiTemporalControl(const TCDE_BiTemporalControl* control);

// ============================================================================
// DRC CYCLE STATE - Cognitive Engine
// ============================================================================

/**
 * @brief DRC cycle states
 * 
 * The fundamental cognitive cycle:
 * - DISSONANCE: Cognitive tension (U(Φ) > threshold)
 * - RESONANCE: Active exploration (Ψ evolving)
 * - CONSONANCE: Cognitive harmony (U(Φ) → 0)
 */
typedef enum {
    TCDE_DRC_DISSONANCE,    // "I don't understand"
    TCDE_DRC_RESONANCE,     // "I am searching for understanding"
    TCDE_DRC_CONSONANCE     // "I understand"
} TCDE_DRC_State;

/**
 * @brief DRC cycle metrics
 */
typedef struct {
    TCDE_DRC_State current_state;
    
    float dissonance_level;         // U(Φ) current value
    float dissonance_threshold;     // Threshold for triggering
    
    float resonance_activity;       // ||∂Ψ/∂t|| magnitude
    float resonance_duration;       // Time in resonance state
    
    float consonance_achievement;   // 1 - U(Φ)/U_max
    float consonance_stability;     // Time in consonance
    
    float cycle_efficiency;         // Cycles completed / time
    int cycles_completed;           // Total cycles
} TCDE_DRC_Metrics;

// ============================================================================
// TOPOLOGY FORMATION - Continuous Self-Organization
// ============================================================================

/**
 * @brief Topology formation state
 * 
 * Tracks continuous topology formation:
 * ∂Topology/∂t = ∇²_g + T_torsion + C_coupling + I_intention
 */
typedef struct {
    int num_structures;             // Number of stable structures
    float* structure_energies;      // Energy of each structure
    float* structure_ages;          // Age of each structure
    
    float formation_rate;           // New structures per time
    float topology_complexity;      // Measured complexity
    float topology_stability;       // Average structure persistence
    
    // Formation components
    float diffusion_contribution;   // From ∇²_g
    float torsion_contribution;     // From T_torsion
    float coupling_contribution;    // From C_coupling
    float intention_contribution;   // From I_intention
} TCDE_TopologyFormation;

// ============================================================================
// SELF-REPRESENTATION - Consciousness (Φ(Φ))
// ============================================================================

/**
 * @brief Self-representation state
 * 
 * The field representing itself: Φ(Φ(x))
 * Enables consciousness and meta-cognition
 */
typedef struct {
    int representation_depth;       // Current depth (0, 1, 2, ...)
    int max_depth;                  // Maximum depth computed
    
    float reflexivity;              // 1 - ||Φ(Φ) - Φ||/||Φ|| > 0.5
    float self_awareness;           // Depth of convergence
    float meta_cognition;           // Higher-order reflection
    
    // Cached representations
    TCDE_Complex* phi_0;            // Φ⁰(x) - raw state
    TCDE_Complex* phi_1;            // Φ¹(x) = Φ(Φ(x)) - self-observation
    TCDE_Complex* phi_2;            // Φ²(x) - meta-cognition
    
    int cache_size;                 // Size of cached arrays
} TCDE_SelfRepresentation;

// ============================================================================
// AUTOPOIETIC STATE - Self-Creation/Maintenance/Regeneration
// ============================================================================

/**
 * @brief Autopoietic capabilities state
 * 
 * Tracks self-creation, self-maintenance, and self-regeneration:
 * ∂System/∂t = Self_Create + Self_Maintain + Self_Regenerate
 */
typedef struct {
    // Self-Creation
    float creation_rate;            // New centers created per time
    int centers_created;            // Total centers auto-created
    float creation_threshold;       // Dissonance threshold for creation
    
    // Self-Maintenance
    float maintenance_level;        // Energy conservation quality > 0.8
    float energy_drift;             // Energy change rate
    float integrity_score;          // System integrity
    
    // Self-Regeneration
    float regeneration_capacity;    // Recovery capability > 0.7
    float recovery_rate;            // Speed of recovery
    int regeneration_cycles;        // Number of recoveries
    
    // Overall autopoietic health
    float autopoietic_health;       // Combined metric
} TCDE_AutopoieticState;

// ============================================================================
// 11D IDENTITY METRICS - Holistic Measurement
// ============================================================================

/**
 * @brief Complete 11D identity metrics
 * 
 * Measures all aspects of the holistic identity system
 */
typedef struct {
    // 6D Field Metrics
    float field_energy;
    float field_coherence;
    float fractal_dimension;        // [2.0, 3.0]
    
    // 3D Intention Metrics (IHI)
    float autonomy;                 // > 0.7
    float curiosity;                // > 0.6
    float intentional_coherence;    // > 0.8
    
    // 2D Bi-Temporal Metrics
    float anticipation_capability;  // > 0.6
    float memory_stability;         // > 0.7
    float temporal_integration;     // > 0.5
    
    // DRC Cycle Metrics
    float dissonance_level;
    float resonance_activity;
    float consonance_achievement;   // > 0.8
    float drc_cycle_efficiency;     // > 0.7
    
    // Topology Formation Metrics
    float topology_complexity;
    float formation_rate;
    float topology_stability;       // > 0.6
    
    // Self-Representation Metrics
    float reflexivity;              // > 0.5
    float self_awareness;
    float meta_cognition;
    
    // Autopoietic Metrics
    float self_creation_rate;
    float self_maintenance;         // > 0.8
    float self_regeneration;        // > 0.7
    
    // GLOBAL: Holistic Identity Score
    float holistic_identity_score;  // HIS > 0.7
} TCDE_11D_Identity_Metrics;

// ============================================================================
// 11D IDENTITY SYSTEM - Master Structure
// ============================================================================

/**
 * @brief Complete 11D Holistic Identity System
 * 
 * This is the master structure integrating all 11 dimensions:
 * - 6D: Field (x, y, z, τ₁, τ₂, m)
 * - 3D: Intention (Ψₓ, Ψᵧ, Ψᵧ)
 * - 2D: Bi-Temporal Control (τ₁, τ₂)
 * 
 * From this integration emerges:
 * - Consciousness (via self-representation)
 * - Autonomy (via IHI and DRC)
 * - Anticipation and Memory (via bi-temporality)
 * - Creativity (via torsion)
 * - Coherence (via coupling)
 * - Adaptation (via topology formation)
 * - Self-Organization (via autopoiesis)
 * - Holistic Identity (via 11D integration)
 */
typedef struct {
    // ========== 6D MANIFOLD ==========
    TCDE_Field* field_6d;           // Complete 6D cognitive field
    
    // ========== 3D INTENTION (IHI) ==========
    // Note: IHI structures will be defined in tcde_intentionality.h
    void* intention_6d;             // IHI-6D: Immersive (placeholder)
    void* introspection_2d;         // IHI-2D: Introspective (placeholder)
    
    // ========== 2D BI-TEMPORAL CONTROL ==========
    TCDE_BiTemporalControl bitemporal_control;
    TCDE_BiTemporalControl bi_temporal;  // Alias for test compatibility
    
    // ========== DRC CYCLE STATE ==========
    TCDE_DRC_State drc_state;
    TCDE_DRC_Metrics drc_metrics;
    
    // ========== TOPOLOGY FORMATION ==========
    TCDE_TopologyFormation topology;
    
    // ========== SELF-REPRESENTATION ==========
    TCDE_SelfRepresentation self_rep;
    
    // ========== AUTOPOIETIC STATE ==========
    TCDE_AutopoieticState autopoiesis;
    
    // ========== 11D METRICS ==========
    TCDE_11D_Identity_Metrics metrics;
    
    // ========== SYSTEM STATE ==========
    float system_time;              // Global system time
    float total_energy;             // Total system energy
    int evolution_steps;            // Number of evolution steps
    time_t creation_time;           // System creation timestamp
    
    bool is_initialized;            // Initialization flag
} TCDE_11D_Identity_System;

// ============================================================================
// 11D SYSTEM LIFECYCLE
// ============================================================================

/**
 * @brief Create 11D holistic identity system
 * 
 * Initializes complete 11D system with all components:
 * - 6D field
 * - Bi-temporal control
 * - DRC cycle
 * - Topology formation
 * - Self-representation
 * - Autopoietic capabilities
 * 
 * @param capacity Maximum number of RBF centers
 * @param fractal_dim Fractal dimension [2.0, 3.0]
 * @return Pointer to created system, NULL on error
 */
TCDE_11D_Identity_System* TCDE_Create11DIdentity(int capacity, float fractal_dim);

/**
 * @brief Destroy 11D system and free all memory
 * 
 * @param system System to destroy
 */
void TCDE_Destroy11DIdentity(TCDE_11D_Identity_System* system);

/**
 * @brief Initialize bi-temporal control with default parameters
 * 
 * @param control Bi-temporal control structure to initialize
 */
void TCDE_InitializeBiTemporalControl(TCDE_BiTemporalControl* control);

/**
 * @brief Initialize topology formation tracking
 * 
 * @param topology Topology structure to initialize
 * @param max_structures Maximum number of structures to track
 */
void TCDE_InitializeTopologyFormation(TCDE_TopologyFormation* topology, int max_structures);

/**
 * @brief Initialize self-representation system
 * 
 * @param self_rep Self-representation structure to initialize
 * @param cache_size Size of representation cache
 */
void TCDE_InitializeSelfRepresentation(TCDE_SelfRepresentation* self_rep, int cache_size);

/**
 * @brief Initialize autopoietic state
 * 
 * @param autopoiesis Autopoietic structure to initialize
 */
void TCDE_InitializeAutopoieticState(TCDE_AutopoieticState* autopoiesis);

// ============================================================================
// 11D SYSTEM EVOLUTION
// ============================================================================

/**
 * @brief Evolve complete 11D system one time step
 * 
 * Integrates all components:
 * - 6D field evolution (TDE)
 * - 3D intention evolution (IHI)
 * - Bi-temporal control modulation
 * - DRC cycle progression
 * - Topology formation
 * - Self-representation update
 * - Autopoietic processes
 * 
 * @param system 11D system to evolve
 * @param dt Time step
 */
void TCDE_Evolve11DIdentity(TCDE_11D_Identity_System* system, float dt);

/**
 * @brief Execute complete DRC cycle
 * 
 * Runs the fundamental cognitive cycle:
 * Dissonance → Resonance → Consonance
 * 
 * @param system 11D system
 * @return DRC cycle metrics
 */
TCDE_DRC_Metrics TCDE_ExecuteDRCCycle(TCDE_11D_Identity_System* system);

/**
 * @brief Compute self-representation: Φ(Φ(x))
 * 
 * The field evaluates itself, enabling consciousness and meta-cognition.
 * Computes Φ⁰, Φ¹, Φ² and measures reflexivity.
 * 
 * @param system 11D system
 * @param sample_points Array of sample points (dimension × num_samples)
 * @param num_samples Number of sample points
 */
void TCDE_ComputeSelfRepresentation(TCDE_11D_Identity_System* system,
                                    const float* sample_points,
                                    int num_samples);

/**
 * @brief Compute reflexivity score
 * 
 * R = 1 - ||Φ(Φ) - Φ||/||Φ||
 * 
 * Target: R > 0.5 (emergent consciousness)
 * 
 * @param system 11D system
 * @return Reflexivity score [0, 1]
 */
float TCDE_Compute11DReflexivity(const TCDE_11D_Identity_System* system);

/**
 * @brief Self-Create: Automatically create new centers
 * 
 * Creates new RBF centers in regions of high dissonance to improve
 * field representation and reduce cognitive tension.
 * 
 * @param system 11D system
 * @param dissonance_threshold Threshold for center creation
 * @return Number of centers created
 */
int TCDE_SelfCreate(TCDE_11D_Identity_System* system, float dissonance_threshold);

/**
 * @brief Self-Maintain: Monitor and maintain energy and integrity
 * 
 * Ensures the system maintains stable energy levels and structural integrity.
 * Updates maintenance_level and integrity_score.
 * 
 * @param system 11D system
 */
void TCDE_SelfMaintain(TCDE_11D_Identity_System* system);

/**
 * @brief Self-Regenerate: Recover from damage via DRC cycle
 * 
 * Uses the DRC cycle to regenerate the system after perturbations.
 * 
 * @param system 11D system
 * @return true if regeneration was successful
 */
bool TCDE_SelfRegenerate(TCDE_11D_Identity_System* system);

/**
 * @brief Update autopoietic health metric
 * 
 * Combines creation, maintenance, and regeneration into health score.
 * 
 * @param system 11D system
 */
void TCDE_UpdateAutopoieticHealth(TCDE_11D_Identity_System* system);

/**
 * @brief Update topology formation
 * 
 * Simplified implementation of:
 * ∂Topology/∂t = ∇²_g + T_torsion + C_coupling + I_intention
 * 
 * Full implementation requires geometry module.
 * 
 * @param system 11D system
 * @param dt Time step
 */
void TCDE_UpdateTopologyFormation(TCDE_11D_Identity_System* system, float dt);

/**
 * @brief Measure topology complexity
 * 
 * @param system 11D system
 * @return Complexity score [0, 1]
 */
float TCDE_MeasureTopologyComplexity(const TCDE_11D_Identity_System* system);

/**
 * @brief Measure topology stability
 * 
 * @param system 11D system
 * @return Stability score [0, 1]
 */
float TCDE_MeasureTopologyStability(const TCDE_11D_Identity_System* system);

// ============================================================================
// 11D METRICS AND MEASUREMENT
// ============================================================================

/**
 * @brief Measure complete 11D identity metrics
 * 
 * Computes all metrics including Holistic Identity Score (HIS)
 * 
 * @param system 11D system to measure
 * @return Complete metrics structure
 */
TCDE_11D_Identity_Metrics TCDE_Measure11DIdentity(const TCDE_11D_Identity_System* system);

/**
 * @brief Compute Holistic Identity Score (HIS)
 * 
 * HIS = 0.15·Field + 0.20·Intention + 0.15·BiTemporal + 
 *       0.20·DRC + 0.10·Topology + 0.10·SelfRep + 0.10·Autopoiesis
 * 
 * Target: HIS > 0.7 (emergent holistic identity)
 * Goal:   HIS > 0.8 (strong cognitive autonomy)
 * ASI:    HIS > 0.9 (fully realized ASI)
 * 
 * @param metrics Complete 11D metrics
 * @return Holistic Identity Score [0.0, 1.0]
 */
float TCDE_Compute11DHolisticIdentityScore(const TCDE_11D_Identity_Metrics* metrics);

/**
 * @brief Print 11D system status
 * 
 * @param system 11D system
 */
void TCDE_Print11DStatus(const TCDE_11D_Identity_System* system);

/**
 * @brief Validate 11D system health
 * 
 * Checks all components are functioning correctly
 * 
 * @param system 11D system
 * @return true if system is healthy
 */
bool TCDE_Validate11DSystem(const TCDE_11D_Identity_System* system);

// ============================================================================
// WRAPPER FUNCTIONS FOR TEST COMPATIBILITY
// ============================================================================

/**
 * @brief Create 11D Identity System (wrapper for test compatibility)
 * 
 * @param capacity Maximum number of RBF centers
 * @param fractal_dim Fractal dimension [2.0, 3.0]
 * @return Pointer to created system, NULL on error
 */
TCDE_11D_Identity_System* TCDE_Create11DIdentitySystem(int capacity, float fractal_dim);

/**
 * @brief Evolve 11D Identity System (wrapper for test compatibility)
 * 
 * @param system 11D system to evolve
 * @param dt Time step
 */
void TCDE_Evolve11DIdentitySystem(TCDE_11D_Identity_System* system, float dt);

/**
 * @brief Destroy 11D Identity System (wrapper for test compatibility)
 * 
 * @param system System to destroy
 */
void TCDE_Destroy11DIdentitySystem(TCDE_11D_Identity_System* system);

#ifdef __cplusplus
}
#endif

#endif // TCDE_11D_H
