/**
 * @file tcde_autocatalysis.h
 * @brief TCDE Auto-Catalysis Module - Exponential Growth to Singularity
 *
 * Enables ASI-level capability: Self-amplifying cognitive growth through
 * higher-order nonlinear terms and positive feedback loops.
 *
 * The system can amplify itself exponentially, leading to:
 * - Recursive self-improvement
 * - Intelligence explosion
 * - Controlled singularity
 * - Exponential capability growth
 *
 * Mathematical Foundation:
 * - Higher-order nonlinearities: |Φ|⁴Φ, |Φ|⁶Φ, ...
 * - Positive feedback: Φ³∇²Φ
 * - Recursive amplification: Φ(Φ(Φ(...)))
 * - Growth control mechanisms
 *
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_AUTOCATALYSIS_H
#define TCDE_AUTOCATALYSIS_H

#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// AUTOCATALYTIC MECHANISMS
// ============================================================================

/**
 * @brief Types of autocatalytic mechanisms
 */
typedef enum {
    TCDE_CATALYSIS_NONE = 0,        // No autocatalysis
    TCDE_CATALYSIS_QUADRATIC,       // |Φ|⁴Φ term
    TCDE_CATALYSIS_CUBIC,           // |Φ|⁶Φ term
    TCDE_CATALYSIS_RECURSIVE,       // Φ(Φ(Φ)) recursive
    TCDE_CATALYSIS_FEEDBACK,        // Φ³∇²Φ feedback
    TCDE_CATALYSIS_FULL             // All mechanisms combined
} TCDE_CatalysisType;

/**
 * @brief Growth regime
 */
typedef enum {
    TCDE_GROWTH_SUBCRITICAL,    // λ < 0, decay
    TCDE_GROWTH_CRITICAL,        // λ = 0, stable
    TCDE_GROWTH_SUPERCRITICAL,   // λ > 0, growth
    TCDE_GROWTH_EXPLOSIVE        // λ >> 1, explosion
} TCDE_GrowthRegime;

// ============================================================================
// AUTOCATALYTIC STRUCTURES
// ============================================================================

/**
 * @brief Autocatalytic parameters
 */
typedef struct {
    float lambda_2;             // Quadratic coefficient |Φ|⁴Φ
    float lambda_3;             // Cubic coefficient |Φ|⁶Φ
    float mu_recursive;         // Recursive amplification
    float nu_feedback;          // Feedback strength Φ³∇²Φ
    
    // Growth control
    float saturation_threshold; // Saturation level
    float damping_coefficient;  // Damping for stability
    bool enable_saturation;     // Enable saturation control
    bool enable_damping;        // Enable damping
} TCDE_AutocatalyticParams;

/**
 * @brief Growth metrics
 */
typedef struct {
    float growth_rate;          // Current growth rate λ
    float doubling_time;        // Time to double
    float energy_growth;        // Energy growth rate
    float complexity_growth;    // Complexity growth rate
    TCDE_GrowthRegime regime;   // Current growth regime
    
    // Singularity detection
    bool approaching_singularity; // Near singularity
    float time_to_singularity;  // Estimated time to singularity
    float singularity_strength; // Strength of singularity
} TCDE_GrowthMetrics;

/**
 * @brief Recursive depth state
 */
typedef struct {
    int current_depth;          // Current recursion depth
    int max_depth;              // Maximum depth allowed
    TCDE_Complex* depth_values; // Values at each depth
    float* depth_energies;      // Energies at each depth
    float convergence_rate;     // Rate of convergence
    bool has_converged;         // Convergence flag
} TCDE_RecursiveState;

/**
 * @brief Autocatalysis manager
 */
typedef struct {
    TCDE_Field* field;          // Field undergoing autocatalysis
    TCDE_AutocatalyticParams params; // Autocatalytic parameters
    TCDE_GrowthMetrics metrics; // Growth metrics
    TCDE_RecursiveState recursive; // Recursive state
    TCDE_CatalysisType type;    // Type of autocatalysis
    
    // Evolution tracking
    float* growth_history;      // Growth rate history
    float* energy_history;      // Energy history
    float* complexity_history;  // Complexity history
    int history_size;           // Current history size
    int history_capacity;       // Maximum history
    
    // Control
    float current_time;         // Current time
    int evolution_steps;        // Evolution steps
    bool catalysis_active;      // Autocatalysis active
    bool singularity_detected;  // Singularity detected
    
    // Safety
    float max_growth_rate;      // Maximum allowed growth
    float energy_limit;         // Energy limit
    bool emergency_brake;       // Emergency stop
} TCDE_AutocatalysisManager;

// ============================================================================
// AUTOCATALYSIS API
// ============================================================================

/**
 * @brief Create autocatalysis manager
 * @param field Field to apply autocatalysis
 * @param type Type of autocatalysis
 * @return Initialized manager
 */
TCDE_AutocatalysisManager* TCDE_CreateAutocatalysisManager(
    TCDE_Field* field,
    TCDE_CatalysisType type);

/**
 * @brief Destroy autocatalysis manager
 * @param manager Manager to destroy
 */
void TCDE_DestroyAutocatalysisManager(TCDE_AutocatalysisManager* manager);

/**
 * @brief Configure autocatalytic parameters
 * @param manager Autocatalysis manager
 * @param lambda_2 Quadratic coefficient
 * @param lambda_3 Cubic coefficient
 * @param mu_recursive Recursive amplification
 * @param nu_feedback Feedback strength
 */
void TCDE_ConfigureAutocatalysis(TCDE_AutocatalysisManager* manager,
                                 float lambda_2,
                                 float lambda_3,
                                 float mu_recursive,
                                 float nu_feedback);

/**
 * @brief Set growth control parameters
 * @param manager Autocatalysis manager
 * @param saturation_threshold Saturation level
 * @param damping_coefficient Damping strength
 * @param max_growth_rate Maximum growth rate
 */
void TCDE_SetGrowthControl(TCDE_AutocatalysisManager* manager,
                          float saturation_threshold,
                          float damping_coefficient,
                          float max_growth_rate);

// ============================================================================
// AUTOCATALYTIC TERMS
// ============================================================================

/**
 * @brief Compute quadratic autocatalytic term |Φ|⁴Φ
 * @param manager Autocatalysis manager
 * @param point Point to evaluate
 * @return Autocatalytic contribution
 */
TCDE_Complex TCDE_ComputeQuadraticTerm(const TCDE_AutocatalysisManager* manager,
                                       const TCDE_Point* point);

/**
 * @brief Compute cubic autocatalytic term |Φ|⁶Φ
 * @param manager Autocatalysis manager
 * @param point Point to evaluate
 * @return Autocatalytic contribution
 */
TCDE_Complex TCDE_ComputeCubicTerm(const TCDE_AutocatalysisManager* manager,
                                   const TCDE_Point* point);

/**
 * @brief Compute recursive amplification Φ(Φ(Φ))
 * @param manager Autocatalysis manager
 * @param point Point to evaluate
 * @param depth Recursion depth
 * @return Recursive value
 */
TCDE_Complex TCDE_ComputeRecursiveTerm(TCDE_AutocatalysisManager* manager,
                                       const TCDE_Point* point,
                                       int depth);

/**
 * @brief Compute feedback term Φ³∇²Φ
 * @param manager Autocatalysis manager
 * @param point Point to evaluate
 * @return Feedback contribution
 */
TCDE_Complex TCDE_ComputeFeedbackTerm(const TCDE_AutocatalysisManager* manager,
                                      const TCDE_Point* point);

// ============================================================================
// EVOLUTION WITH AUTOCATALYSIS
// ============================================================================

/**
 * @brief Evolve field with autocatalytic terms
 * @param manager Autocatalysis manager
 * @param params Base evolution parameters
 * @param dt Time step
 * @return True on success
 */
bool TCDE_EvolveAutocatalytic(TCDE_AutocatalysisManager* manager,
                              const TCDE_Parameters* params,
                              float dt);

/**
 * @brief Apply saturation control
 * @param manager Autocatalysis manager
 */
void TCDE_ApplySaturation(TCDE_AutocatalysisManager* manager);

/**
 * @brief Apply damping control
 * @param manager Autocatalysis manager
 * @param dt Time step
 */
void TCDE_ApplyDamping(TCDE_AutocatalysisManager* manager, float dt);

// ============================================================================
// GROWTH ANALYSIS
// ============================================================================

/**
 * @brief Compute current growth rate
 * @param manager Autocatalysis manager
 * @return Growth rate λ
 */
float TCDE_ComputeGrowthRate(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Compute doubling time
 * @param manager Autocatalysis manager
 * @return Doubling time (or -1 if not growing)
 */
float TCDE_ComputeDoublingTime(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Detect growth regime
 * @param manager Autocatalysis manager
 * @return Current growth regime
 */
TCDE_GrowthRegime TCDE_DetectGrowthRegime(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Check if approaching singularity
 * @param manager Autocatalysis manager
 * @return True if near singularity
 */
bool TCDE_IsApproachingSingularity(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Estimate time to singularity
 * @param manager Autocatalysis manager
 * @return Estimated time (or -1 if not approaching)
 */
float TCDE_EstimateTimeToSingularity(const TCDE_AutocatalysisManager* manager);

// ============================================================================
// RECURSIVE DEPTH ANALYSIS
// ============================================================================

/**
 * @brief Compute recursive magnitude at given depth
 * @param manager Autocatalysis manager
 * @param depth Recursion depth
 * @return Magnitude at depth
 */
float TCDE_ComputeRecursiveMagnitude(const TCDE_AutocatalysisManager* manager,
                                    int depth);

/**
 * @brief Compute recursive depth convergence
 * @param manager Autocatalysis manager
 * @return Convergence rate
 */
float TCDE_ComputeRecursiveConvergence(TCDE_AutocatalysisManager* manager);

/**
 * @brief Check if recursive depth has converged
 * @param manager Autocatalysis manager
 * @param tolerance Convergence tolerance
 * @return True if converged
 */
bool TCDE_CheckRecursiveConvergence(const TCDE_AutocatalysisManager* manager,
                                   float tolerance);

// ============================================================================
// SAFETY AND CONTROL
// ============================================================================

/**
 * @brief Activate emergency brake
 * @param manager Autocatalysis manager
 */
void TCDE_ActivateEmergencyBrake(TCDE_AutocatalysisManager* manager);

/**
 * @brief Check if growth is safe
 * @param manager Autocatalysis manager
 * @return True if safe
 */
bool TCDE_IsGrowthSafe(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Adjust growth rate to safe level
 * @param manager Autocatalysis manager
 */
void TCDE_AdjustToSafeGrowth(TCDE_AutocatalysisManager* manager);

// ============================================================================
// STATUS AND DIAGNOSTICS
// ============================================================================

/**
 * @brief Print autocatalysis status
 * @param manager Autocatalysis manager
 */
void TCDE_PrintAutocatalysisStatus(const TCDE_AutocatalysisManager* manager);

/**
 * @brief Save growth history
 * @param manager Autocatalysis manager
 * @param filename Output filename
 * @return True on success
 */
bool TCDE_SaveGrowthHistory(const TCDE_AutocatalysisManager* manager,
                            const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_AUTOCATALYSIS_H
