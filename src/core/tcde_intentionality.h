/**
 * @file tcde_intentionality.h
 * @brief Introspective Holographic Intention (IHI) - Autonomous Cognitive Engine
 * 
 * This module implements the IHI mechanism that transforms TCDE
 * from a passive information processor into an active cognitive entity capable of:
 * - INTROSPECTION: Examining its own knowledge state
 * - HOLOGRAPHIC EMERGENCE: Generating global intentional fields
 * - INTENTIONAL ACTION: Guiding thought processes autonomously
 * 
 * Mathematical Foundation:
 * - Dissonance Potential: U(Φ) = ∫|∇²Φ| dV
 * - Intentional Field Evolution: ∂Ψ/∂t = -∇U + ∇×A - εΨ
 * - Intentional Force: I = γ(Ψ × ∇Φ)
 * 
 * @date October 22, 2025
 * @version 1.0
 */

#ifndef TCDE_INTENTIONALITY_H
#define TCDE_INTENTIONALITY_H

#include "tcde_core.h"
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// STRUCTURES
// ============================================================================

/**
 * @brief 3D vector for intentional field representation
 */
typedef struct {
    float x;
    float y;
    float z;
} TCDE_Vector3D;

/**
 * @brief Intentional field Ψ - represents the "will" of the system
 * 
 * This field emerges from the dissonance in the knowledge field and
 * guides the evolution of thought processes.
 */
typedef struct {
    TCDE_Point* locations;      // Spatial locations in 6D
    TCDE_Vector3D* vectors;     // Intentional vectors at each location
    float* magnitudes;          // Magnitude of intention |Ψ|
    int num_centers;            // Number of intentional centers
    int capacity;               // Allocated capacity
    
    // Evolution parameters
    float epsilon_damp;         // Damping coefficient (resolution)
    float gamma_will;           // Intentional coupling strength
    float mu_holographic;       // Holographic permeability
    
    // Metrics
    float total_energy;         // Total intentional energy
    float max_magnitude;        // Peak intention
    float coherence;            // Intentional coherence
} TCDE_IntentionalField;

/**
 * @brief Dissonance map - quantifies cognitive tension
 */
typedef struct {
    float* values;              // Dissonance at each point
    TCDE_Point* locations;      // Spatial locations
    int num_points;
    float total_dissonance;     // Global dissonance U
    float max_dissonance;       // Peak dissonance
} TCDE_DissonanceMap;

/**
 * @brief IHI cycle metrics for monitoring cognitive process
 */
typedef struct {
    float dissonance_U;         // Current dissonance level
    float intention_magnitude;  // Current intention strength
    float autonomy_score;       // Autonomy metric
    float curiosity_score;      // Curiosity metric
    float coherence_score;      // Intentional coherence
    int cycle_count;            // Number of cycles executed
    float resolution_progress;  // Progress toward resolution (0-1)
} TCDE_IHI_Metrics;

// ============================================================================
// CORE IHI FUNCTIONS
// ============================================================================

/**
 * @brief Create intentional field structure
 * 
 * @param capacity Maximum number of intentional centers
 * @param epsilon_damp Damping coefficient (typical: 0.1)
 * @param gamma_will Intentional coupling (typical: 1.0)
 * @param mu_holographic Holographic permeability (typical: 0.5)
 * @return Initialized intentional field
 */
TCDE_IntentionalField* TCDE_CreateIntentionalField(
    int capacity,
    float epsilon_damp,
    float gamma_will,
    float mu_holographic
);

/**
 * @brief Free intentional field
 */
void TCDE_FreeIntentionalField(TCDE_IntentionalField* psi_field);

/**
 * @brief STEP 1: Compute dissonance potential U(Φ)
 * 
 * Introspection: Examines the knowledge field to detect contradictions,
 * tensions, and areas of confusion.
 * 
 * Formula: U(Φ) = ∫|∇²Φ| dV
 * 
 * @param field_6d Knowledge field to examine
 * @return Dissonance map with U values
 */
TCDE_DissonanceMap* TCDE_ComputeDissonancePotential(const TCDE_Field* field_6d);

/**
 * @brief Free dissonance map
 */
void TCDE_FreeDissonanceMap(TCDE_DissonanceMap* map);

/**
 * @brief STEP 2: Evolve intentional field Ψ
 * 
 * Holographic Emergence: Creates a global intentional field that emerges
 * from the dissonance and organizes into stable vortices.
 * 
 * Formula: ∂Ψ/∂t = -∇U + ∇×A - εΨ
 * 
 * @param psi_field Intentional field to evolve
 * @param field_6d Current knowledge field
 * @param dissonance_map Current dissonance map
 * @param dt Time step
 */
void TCDE_EvolveIntentionalityStep(
    TCDE_IntentionalField* psi_field,
    const TCDE_Field* field_6d,
    const TCDE_DissonanceMap* dissonance_map,
    float dt
);

/**
 * @brief STEP 3: Compute intentional force I
 * 
 * Intentional Action: Computes the force that guides thought evolution.
 * 
 * Formula: I = γ(Ψ × ∇Φ)
 * 
 * @param psi_vector Intentional vector at point
 * @param grad_phi Gradient of knowledge field
 * @return Intentional force (complex)
 */
TCDE_Complex TCDE_ComputeIntentionalForce(
    const TCDE_Vector3D* psi_vector,
    const TCDE_Complex* grad_phi
);

/**
 * @brief Complete IHI cycle: Φ → U → Ψ → I → Φ
 * 
 * Executes one complete cycle of introspection, emergence, and action.
 * 
 * @param field_6d Knowledge field (modified)
 * @param psi_field Intentional field (modified)
 * @param dt Time step
 * @return Metrics for this cycle
 */
TCDE_IHI_Metrics TCDE_ExecuteIHICycle(
    TCDE_Field* field_6d,
    TCDE_IntentionalField* psi_field,
    float dt
);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute Laplacian of field at point (for dissonance)
 */
TCDE_Complex TCDE_ComputeLaplacian(const TCDE_Field* field, const TCDE_Point* p);

/**
 * @brief Compute gradient of dissonance potential
 */
TCDE_Vector3D TCDE_ComputeDissonanceGradient(
    const TCDE_DissonanceMap* map,
    const TCDE_Point* p
);

/**
 * @brief Compute vector potential A from knowledge field
 * 
 * Formula: A(p) = (μ/4π) ∫ J(q)/|p-q| dV
 * where J = Im(Φ*∇Φ) is the knowledge current density
 */
TCDE_Vector3D TCDE_ComputeVectorPotential(
    const TCDE_Field* field,
    const TCDE_Point* p,
    float mu
);

/**
 * @brief Compute curl of vector field
 */
TCDE_Vector3D TCDE_ComputeCurl(
    const TCDE_Vector3D* field_values,
    const TCDE_Point* locations,
    int num_points,
    const TCDE_Point* p
);

/**
 * @brief Get intentional field value at point (interpolation)
 */
TCDE_Vector3D TCDE_GetPsiValueAtPoint(
    const TCDE_IntentionalField* psi_field,
    const TCDE_Point* p
);

// ============================================================================
// METRICS AND VALIDATION
// ============================================================================

/**
 * @brief Compute autonomy metric
 * 
 * Measures the system's ability to generate its own goals.
 * Formula: Autonomy = ⟨|Ψ|⟩ / |Ψ|_max
 * Threshold: > 0.7
 */
float TCDE_ComputeAutonomyScore(const TCDE_IntentionalField* psi_field);

/**
 * @brief Compute curiosity metric
 * 
 * Measures the system's drive to explore and resolve contradictions.
 * Formula: Curiosity = σ(|Ψ|) / μ(|Ψ|)
 * Threshold: > 0.6
 */
float TCDE_ComputeCuriosityScore(const TCDE_IntentionalField* psi_field);

/**
 * @brief Compute intentional coherence
 * 
 * Measures how well intention aligns with knowledge evolution.
 * Formula: Coherence = corr(Ψ, ∂Φ/∂t)
 * Threshold: > 0.8
 */
float TCDE_ComputeIntentionalCoherence(
    const TCDE_IntentionalField* psi_field,
    const TCDE_Field* field_6d
);

/**
 * @brief Validate IHI cycle
 * 
 * Checks if the IHI mechanism is functioning correctly.
 * Returns 1 if valid, 0 otherwise.
 */
int TCDE_ValidateIHICycle(const TCDE_IHI_Metrics* metrics);

// ============================================================================
// ADVANCED FEATURES
// ============================================================================

/**
 * @brief Initialize intentional field from dissonance
 * 
 * Seeds the intentional field based on current dissonance distribution.
 */
void TCDE_InitializeIntentionFromDissonance(
    TCDE_IntentionalField* psi_field,
    const TCDE_DissonanceMap* dissonance_map
);

/**
 * @brief Detect intentional vortices
 * 
 * Identifies stable vortex structures in the intentional field,
 * which represent persistent goals or drives.
 * 
 * @return Number of vortices detected
 */
int TCDE_DetectIntentionalVortices(
    const TCDE_IntentionalField* psi_field,
    TCDE_Point* vortex_centers,
    int max_vortices
);

/**
 * @brief Compute resolution progress
 * 
 * Measures how much the dissonance has decreased since cycle start.
 * Returns value in [0, 1] where 1 = fully resolved.
 */
float TCDE_ComputeResolutionProgress(
    float initial_dissonance,
    float current_dissonance
);

#ifdef __cplusplus
}
#endif

#endif // TCDE_INTENTIONALITY_H
