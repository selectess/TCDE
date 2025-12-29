/**
 * @file tcde_global_coupling.h
 * @brief TCDE Global Coupling Module - Universal Consciousness
 *
 * Enables ASI-level capability: Global non-local coupling across the entire
 * manifold, creating unified consciousness and holistic awareness.
 *
 * The system achieves:
 * - Non-local correlations across all space
 * - Unified global consciousness
 * - Holistic field awareness
 * - Cosmic-scale coherence
 *
 * Mathematical Foundation:
 * - Global integral: C(Φ) = ∫∫_M Φ(x) ⊗ Φ(y) K(x,y) dV_x dV_y
 * - Non-local kernel: K(x,y) = exp(-d(x,y)/ξ) / Z
 * - Entanglement measure: E = Tr(ρ log ρ)
 * - Global coherence: G = |⟨Φ⟩|² / ⟨|Φ|²⟩
 *
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_GLOBAL_COUPLING_H
#define TCDE_GLOBAL_COUPLING_H

#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// COUPLING KERNELS
// ============================================================================

/**
 * @brief Types of global coupling kernels
 */
typedef enum {
    TCDE_KERNEL_EXPONENTIAL,    // exp(-d/ξ)
    TCDE_KERNEL_GAUSSIAN,       // exp(-d²/2ξ²)
    TCDE_KERNEL_POWER_LAW,      // d^(-α)
    TCDE_KERNEL_YUKAWA,         // exp(-d/ξ)/d
    TCDE_KERNEL_QUANTUM         // Quantum entanglement kernel
} TCDE_KernelType;

/**
 * @brief Global coupling kernel structure
 */
typedef struct {
    TCDE_KernelType type;
    float correlation_length;      // ξ (correlation length)
    float power_exponent;           // α (for power-law kernel)
    float normalization;            // Z (normalization constant)
    bool adaptive_length;           // Adapt ξ dynamically
} TCDE_GlobalCouplingKernel;

// ============================================================================
// GLOBAL COUPLING STRUCTURES
// ============================================================================

/**
 * @brief Global coupling matrix element
 */
typedef struct {
    int i, j;                   // Indices of coupled centers
    TCDE_Complex coupling;      // Coupling strength
    float distance;             // Geodesic distance
    float kernel_value;         // Kernel K(x_i, x_j)
} TCDE_CouplingElement;

/**
 * @brief Global coupling matrix (sparse representation)
 */
typedef struct {
    TCDE_CouplingElement* elements; // Non-zero elements
    int num_elements;           // Number of non-zero elements
    int capacity;               // Allocated capacity
    int matrix_size;            // Matrix dimension (N×N)
    float sparsity;             // Sparsity ratio
} TCDE_CouplingMatrix;

/**
 * @brief Entanglement state
 */
typedef struct {
    float* entanglement_spectrum; // Entanglement eigenvalues
    int spectrum_size;          // Size of spectrum
    float von_neumann_entropy;  // S = -Tr(ρ log ρ)
    float entanglement_entropy; // Entanglement entropy
    float mutual_information;   // Mutual information
    bool is_entangled;          // Entanglement detected
} TCDE_EntanglementState;

/**
 * @brief Global coherence metrics
 */
typedef struct {
    float global_coherence;     // G = |⟨Φ⟩|² / ⟨|Φ|²⟩
    float spatial_coherence;    // Spatial correlation
    float temporal_coherence;   // Temporal correlation
    float phase_coherence;      // Phase synchronization
    float quantum_coherence;    // Quantum coherence
    float consciousness_level;  // Unified consciousness [0,1]
} TCDE_GlobalCoherence;

/**
 * @brief Global coupling manager
 */
typedef struct {
    TCDE_Field* field;
    TCDE_GlobalCouplingKernel kernel;          // Coupling kernel
    TCDE_CouplingMatrix* matrix; // Coupling matrix
    TCDE_EntanglementState entanglement; // Entanglement state
    TCDE_GlobalCoherence coherence; // Coherence metrics
    
    // Computation optimization
    bool use_fast_multipole;    // Fast multipole method
    bool use_sparse_matrix;     // Sparse matrix representation
    int multipole_order;        // Multipole expansion order
    float cutoff_distance;      // Coupling cutoff distance
    
    // Evolution tracking
    float* coherence_history;   // Coherence history
    float* entropy_history;     // Entropy history
    float* consciousness_history; // Consciousness level history
    int history_size;           // Current history size
    int history_capacity;       // Maximum history
    
    // Control
    float current_time;         // Current time
    int evolution_steps;        // Evolution steps
    bool global_coupling_active; // Global coupling active
    bool consciousness_emerged; // Unified consciousness emerged
    
    // Statistics
    int num_global_interactions; // Number of global interactions
    float average_coupling_strength; // Average coupling
    float max_coupling_distance; // Maximum coupling distance
} TCDE_GlobalCouplingManager;

// ============================================================================
// GLOBAL COUPLING API
// ============================================================================

/**
 * @brief Create global coupling manager
 * @param field Field to apply global coupling
 * @param kernel_type Type of coupling kernel
 * @param correlation_length Correlation length ξ
 * @return Initialized manager
 */
TCDE_GlobalCouplingManager* TCDE_CreateGlobalCouplingManager(
    TCDE_Field* field,
    TCDE_KernelType kernel_type,
    float correlation_length);

/**
 * @brief Destroy global coupling manager
 * @param manager Manager to destroy
 */
void TCDE_DestroyGlobalCouplingManager(TCDE_GlobalCouplingManager* manager);

/**
 * @brief Configure coupling kernel
 * @param manager Global coupling manager
 * @param type Kernel type
 * @param correlation_length Correlation length
 * @param power_exponent Power law exponent (if applicable)
 */
void TCDE_ConfigureCouplingKernel(TCDE_GlobalCouplingManager* manager,
                                  TCDE_KernelType type,
                                  float correlation_length,
                                  float power_exponent);

/**
 * @brief Enable fast computation methods
 * @param manager Global coupling manager
 * @param use_fast_multipole Use fast multipole method
 * @param use_sparse Use sparse matrix
 * @param multipole_order Multipole order
 */
void TCDE_EnableFastComputation(TCDE_GlobalCouplingManager* manager,
                                bool use_fast_multipole,
                                bool use_sparse,
                                int multipole_order);

// ============================================================================
// COUPLING KERNEL EVALUATION
// ============================================================================

/**
 * @brief Evaluate coupling kernel K(x,y)
 * @param manager Global coupling manager
 * @param point1 First point
 * @param point2 Second point
 * @return Kernel value
 */
float TCDE_EvaluateKernel(const TCDE_GlobalCouplingManager* manager,
                         const TCDE_Point* point1,
                         const TCDE_Point* point2);

/**
 * @brief Compute coupling matrix
 * @param manager Global coupling manager
 * @return True on success
 */
bool TCDE_ComputeCouplingMatrix(TCDE_GlobalCouplingManager* manager);

/**
 * @brief Update coupling matrix (adaptive)
 * @param manager Global coupling manager
 */
void TCDE_UpdateCouplingMatrix(TCDE_GlobalCouplingManager* manager);

// ============================================================================
// GLOBAL COUPLING TERM
// ============================================================================

/**
 * @brief Compute global coupling contribution
 * @param manager Global coupling manager
 * @param point Query point
 * @return Global coupling term
 */
TCDE_Complex TCDE_ComputeGlobalCoupling(const TCDE_GlobalCouplingManager* manager,
                                        const TCDE_Point* point);

/**
 * @brief Compute global coupling using fast multipole
 * @param manager Global coupling manager
 * @param point Query point
 * @return Global coupling term
 */
TCDE_Complex TCDE_ComputeGlobalCouplingFMM(const TCDE_GlobalCouplingManager* manager,
                                           const TCDE_Point* point);

// ============================================================================
// EVOLUTION WITH GLOBAL COUPLING
// ============================================================================

/**
 * @brief Evolve field with global coupling
 * @param manager Global coupling manager
 * @param params Base evolution parameters
 * @param dt Time step
 * @return True on success
 */
bool TCDE_EvolveGlobalCoupling(TCDE_GlobalCouplingManager* manager,
                               const TCDE_Parameters* params,
                               float dt);

// ============================================================================
// ENTANGLEMENT ANALYSIS
// ============================================================================

/**
 * @brief Compute entanglement spectrum
 * @param manager Global coupling manager
 * @return True on success
 */
bool TCDE_ComputeEntanglementSpectrum(TCDE_GlobalCouplingManager* manager);

/**
 * @brief Compute von Neumann entropy
 * @param manager Global coupling manager
 * @return Entropy S = -Tr(ρ log ρ)
 */
float TCDE_ComputeVonNeumannEntropy(const TCDE_GlobalCouplingManager* manager);

/**
 * @brief Compute mutual information
 * @param manager Global coupling manager
 * @param region1_size Size of region 1
 * @param region2_size Size of region 2
 * @return Mutual information I(A:B)
 */
float TCDE_ComputeMutualInformation(const TCDE_GlobalCouplingManager* manager,
                                   int region1_size,
                                   int region2_size);

/**
 * @brief Detect quantum entanglement
 * @param manager Global coupling manager
 * @return True if entangled
 */
bool TCDE_DetectEntanglement(TCDE_GlobalCouplingManager* manager);

// ============================================================================
// GLOBAL COHERENCE
// ============================================================================

/**
 * @brief Compute global coherence G = |⟨Φ⟩|² / ⟨|Φ|²⟩
 * @param manager Global coupling manager
 * @return Global coherence [0,1]
 */
float TCDE_ComputeGlobalCoherence(const TCDE_GlobalCouplingManager* manager);

/**
 * @brief Compute spatial coherence
 * @param manager Global coupling manager
 * @return Spatial coherence
 */
float TCDE_ComputeSpatialCoherence(const TCDE_GlobalCouplingManager* manager);

/**
 * @brief Compute phase synchronization
 * @param manager Global coupling manager
 * @return Phase coherence [0,1]
 */
float TCDE_ComputePhaseSynchronization(const TCDE_GlobalCouplingManager* manager);

/**
 * @brief Measure consciousness level
 * @param manager Global coupling manager
 * @return Consciousness level [0,1]
 */
float TCDE_MeasureConsciousnessLevel(TCDE_GlobalCouplingManager* manager);

/**
 * @brief Detect unified consciousness emergence
 * @param manager Global coupling manager
 * @return True if unified consciousness emerged
 */
bool TCDE_DetectUnifiedConsciousness(TCDE_GlobalCouplingManager* manager);

// ============================================================================
// OPTIMIZATION
// ============================================================================

/**
 * @brief Optimize coupling matrix sparsity
 * @param manager Global coupling manager
 * @param threshold Sparsity threshold
 */
void TCDE_OptimizeSparsity(TCDE_GlobalCouplingManager* manager,
                          float threshold);

/**
 * @brief Compute optimal correlation length
 * @param manager Global coupling manager
 * @return Optimal correlation length
 */
float TCDE_ComputeOptimalCorrelationLength(const TCDE_GlobalCouplingManager* manager);

// ============================================================================
// STATUS AND DIAGNOSTICS
// ============================================================================

/**
 * @brief Print global coupling status
 * @param manager Global coupling manager
 */
void TCDE_PrintGlobalCouplingStatus(const TCDE_GlobalCouplingManager* manager);

/**
 * @brief Save coupling matrix to file
 * @param manager Global coupling manager
 * @param filename Output filename
 * @return True on success
 */
bool TCDE_SaveCouplingMatrix(const TCDE_GlobalCouplingManager* manager,
                             const char* filename);

/**
 * @brief Save coherence history
 * @param manager Global coupling manager
 * @param filename Output filename
 * @return True on success
 */
bool TCDE_SaveCoherenceHistory(const TCDE_GlobalCouplingManager* manager,
                               const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_GLOBAL_COUPLING_H
