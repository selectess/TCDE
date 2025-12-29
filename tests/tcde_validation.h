/**
 * @file tcde_validation.h
 * @brief TCDE Validation Module - Comprehensive Testing Framework
 * 
 * Multi-level validation:
 * - Level 1: Mathematical validation (metric properties, geodesics)
 * - Level 2: Numerical validation (convergence, conservation)
 * - Level 3: Implementation validation (unit tests)
 * - Level 4: Scientific validation (benchmark problems)
 * - Level 5: Cognitive validation (reflexivity, prediction, intuition)
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_VALIDATION_H
#define TCDE_VALIDATION_H

#include "tcde_core.h"
#include "tcde_multimodal.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// VALIDATION RESULTS
// ============================================================================

/**
 * @brief Validation results structure
 */
typedef struct {
    // Cognitive validation
    float reflexive_coherence;      // Self-awareness [0,1]
    float predictive_strength;      // Prediction capability [0,2]
    int intuitive_connections;      // Non-obvious associations
    float anticipation_score;       // Future prediction [0,1]
    float cross_modal_coherence;    // Multi-modal consistency [0,1]
    
    // Mathematical validation
    bool metric_positive_definite;  // Metric is positive definite
    bool geodesic_triangle_ineq;    // Triangle inequality holds
    float curvature_bounds;         // Curvature within bounds
    
    // Numerical validation
    float energy_conservation_error; // Relative energy error
    float spatial_convergence_rate;  // Spatial convergence rate
    float temporal_convergence_rate; // Temporal convergence rate
    
    // Overall validation
    bool validated;                 // Overall validation passed
    char message[256];              // Validation message
} TCDE_ValidationResults;

// ============================================================================
// COGNITIVE VALIDATION
// ============================================================================

/**
 * @brief Validate reflexivity - field's ability to reconstruct patterns (Task 6.1)
 * @param field TCDE field to validate
 * @param pattern_type Type of pattern to test (0=gaussian, 1=sine, 2=fractal)
 * @param perturbation_strength Strength of perturbation [0,1]
 * @param evolution_steps Number of evolution steps
 * @return Reflexivity score ∈ [0,1]
 * 
 * **AUTHENTIC TCDE COGNITIVE VALIDATION**
 * 
 * Tests the field's ability to maintain and reconstruct perturbed patterns
 * through evolution. This is a fundamental property of cognitive systems.
 * 
 * Algorithm:
 * 1. Create field with known pattern
 * 2. Perturb field (add noise or distortion)
 * 3. Evolve field for N steps (let it "settle")
 * 4. Measure correlation with original pattern
 * 5. Return reflexivity score ∈ [0,1]
 * 
 * Formula: ρ = ⟨Φ_original, Φ_reconstructed⟩ / (||Φ_original|| ||Φ_reconstructed||)
 * 
 * High score (>0.7) indicates strong reflexivity - field maintains structure
 * 
 * @complexity O(evolution_steps × field_evaluation_cost)
 */
float TCDE_ValidateReflexivity(TCDE_Field* field, int pattern_type,
                                float perturbation_strength, int evolution_steps);

/**
 * @brief Test reflexivity robustness with varying perturbations
 * @param field TCDE field
 * @param num_tests Number of tests to run
 * @param results Output array of reflexivity scores (can be NULL)
 * @return Average reflexivity score
 * 
 * Runs multiple reflexivity tests with varying perturbation strengths
 * to characterize the field's robustness.
 */
float TCDE_TestReflexivityRobustness(TCDE_Field* field, int num_tests, float* results);

/**
 * @brief Validate prediction capability - field's ability to anticipate future (Task 6.2)
 * @param field TCDE field to validate
 * @param pattern_type Type of temporal pattern (0=sine, 1=linear, 2=exponential)
 * @param prediction_horizon Time steps ahead to predict
 * @param evolution_steps Number of evolution steps before prediction
 * @return Prediction accuracy ∈ [0,1]
 * 
 * **AUTHENTIC TCDE COGNITIVE VALIDATION**
 * 
 * Tests the field's ability to predict future states using the τ₂ dimension.
 * This is a fundamental property of intelligent systems.
 * 
 * Algorithm:
 * 1. Initialize field with known temporal pattern
 * 2. Evolve field using TDE
 * 3. Extract anticipated future state from τ₂ dimension
 * 4. Continue evolution to actual future
 * 5. Compare predicted vs actual
 * 6. Return prediction accuracy ∈ [0,1]
 * 
 * Formula: accuracy = 1 - ||Φ_predicted - Φ_actual|| / ||Φ_actual||
 * 
 * High score (>0.7) indicates strong predictive capability
 * 
 * @complexity O(evolution_steps × field_evaluation_cost)
 */
float TCDE_ValidatePrediction(TCDE_Field* field, int pattern_type,
                               int prediction_horizon, int evolution_steps);

/**
 * @brief Test prediction robustness with varying horizons
 * @param field TCDE field
 * @param num_tests Number of tests to run
 * @param results Output array of prediction accuracies (can be NULL)
 * @return Average prediction accuracy
 * 
 * Runs multiple prediction tests with varying prediction horizons
 * to characterize the field's predictive capability.
 */
float TCDE_TestPredictionRobustness(TCDE_Field* field, int num_tests, float* results);



/**
 * @brief Validate intuition - field's ability to make non-obvious connections (Task 6.3)
 * @param field TCDE field to validate
 * @param num_test_points Number of points to test
 * @param torsion_threshold Threshold for "high torsion"
 * @return Intuition score ∈ [0,1]
 * 
 * **AUTHENTIC TCDE COGNITIVE VALIDATION**
 * 
 * Tests the field's ability to discover non-obvious connections using the
 * torsion tensor. Torsion represents "twists" in the field that enable
 * intuitive leaps beyond simple geodesic paths.
 * 
 * Algorithm:
 * 1. Create field with known semantic structure
 * 2. Measure torsion at various points
 * 3. Identify high-torsion regions (intuitive hotspots)
 * 4. Compute path integrals along non-geodesic curves
 * 5. Count semantically meaningful connections via torsion
 * 6. Return intuition score ∈ [0,1]
 * 
 * Formula: intuition = (connections_via_torsion) / (total_connections)
 * 
 * High score (>0.3) indicates strong intuitive capability
 * 
 * @complexity O(num_test_points × torsion_computation_cost)
 */
float TCDE_ValidateIntuition(TCDE_Field* field, int num_test_points,
                              float torsion_threshold);

/**
 * @brief Test intuition robustness with varying thresholds
 * @param field TCDE field
 * @param num_tests Number of tests to run
 * @param results Output array of intuition scores (can be NULL)
 * @return Average intuition score
 * 
 * Runs multiple intuition tests with varying torsion thresholds
 * to characterize the field's intuitive capability.
 */
float TCDE_TestIntuitionRobustness(TCDE_Field* field, int num_tests, float* results);

/**
 * @brief Validate anticipation capability
 * @param field Field
 * @return Anticipation score [0,1]
 * 
 * Measures field's ability to anticipate future states
 * Based on temporal dimension τ and prediction accuracy
 */
float TCDE_ValidateAnticipation(TCDE_Field* field);

/**
 * @brief Validate cross-modal coherence
 * @param field Field
 * @param relations Modality relations
 * @return Coherence score [0,1]
 * 
 * Test procedure:
 * 1. Process inputs from multiple modalities
 * 2. Measure field coherence across modality boundaries
 * 3. Compute coherence: C = ∫∫ K(m_i,m_j) |Φ(m_i)| |Φ(m_j)| dm_i dm_j
 * 4. Verify coherence increases with multimodal training
 * 
 * High score indicates unified multimodal representation
 */
float TCDE_ValidateCrossModalCoherence(TCDE_Field* field, 
                                        TCDE_ModalityRelations* relations);

// ============================================================================
// MATHEMATICAL VALIDATION
// ============================================================================

/**
 * @brief Validate metric properties
 * @param metric Metric to validate
 * @return true if metric is valid (positive definite, symmetric)
 * 
 * Checks:
 * - Symmetry: g_ij = g_ji
 * - Positive definiteness: all eigenvalues > 0
 * - Determinant > 0
 */
bool TCDE_ValidateMetric(const TCDE_Metric* metric);

/**
 * @brief Validate geodesic distance properties
 * @param field Field
 * @param num_tests Number of random tests
 * @return true if all tests pass
 * 
 * Checks:
 * - Non-negativity: d(p,q) ≥ 0
 * - Identity: d(p,p) = 0
 * - Symmetry: d(p,q) = d(q,p)
 * - Triangle inequality: d(p,r) ≤ d(p,q) + d(q,r)
 */
bool TCDE_ValidateGeodesicDistance(const TCDE_Field* field, int num_tests);

/**
 * @brief Validate Christoffel symbol properties
 * @param field Field
 * @param point Evaluation point
 * @return true if properties hold
 * 
 * Checks:
 * - Symmetry in lower indices: Γⁱⱼₖ = Γⁱₖⱼ
 * - Covariant derivative of metric is zero: ∇_k g_ij = 0
 */
bool TCDE_ValidateChristoffel(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Validate Riemann tensor properties
 * @param field Field
 * @param point Evaluation point
 * @return true if properties hold
 * 
 * Checks:
 * - Antisymmetry: R^i_jkl = -R^i_jlk = -R^i_kjl
 * - First Bianchi identity: R^i_jkl + R^i_klj + R^i_ljk = 0
 * - Second Bianchi identity: ∇_m R^i_jkl + ∇_k R^i_jlm + ∇_l R^i_jmk = 0
 */
bool TCDE_ValidateRiemannTensor(const TCDE_Field* field, const TCDE_Point* point);

/**
 * @brief Validate torsion antisymmetry
 * @param field Field
 * @param point Evaluation point
 * @return true if T_ij = -T_ji
 */
bool TCDE_ValidateTorsionAntisymmetry(const TCDE_Field* field, 
                                       const TCDE_Point* point);

// ============================================================================
// NUMERICAL VALIDATION
// ============================================================================

/**
 * @brief Validate energy conservation
 * @param field Field
 * @param initial_energy Initial energy
 * @param num_steps Number of evolution steps
 * @param tolerance Relative error tolerance
 * @return true if energy conserved within tolerance
 * 
 * For diffusion-only regime (α=β=γ=0):
 * Verifies: |E_final - E_initial| / E_initial < tolerance
 */
bool TCDE_ValidateEnergyConservation(TCDE_Field* field, float initial_energy,
                                      int num_steps, float tolerance);

/**
 * @brief Validate spatial convergence
 * @param field Field
 * @param test_function Known test function
 * @param num_refinements Number of mesh refinements
 * @return Convergence rate
 * 
 * Measures convergence rate as mesh is refined:
 * error ∝ h^p where h = mesh spacing, p = convergence rate
 * 
 * Expected: p > 1.5 for RBF methods
 */
float TCDE_ValidateSpatialConvergence(TCDE_Field* field, 
                                       TCDE_Complex (*test_function)(const TCDE_Point*),
                                       int num_refinements);

/**
 * @brief Validate temporal convergence
 * @param field Field
 * @param num_refinements Number of timestep refinements
 * @return Convergence rate
 * 
 * Measures convergence rate as timestep is refined:
 * error ∝ dt^p where dt = timestep, p = convergence rate
 * 
 * Expected: p > 0.8 for IMEX scheme
 */
float TCDE_ValidateTemporalConvergence(TCDE_Field* field, int num_refinements);

/**
 * @brief Validate numerical stability
 * @param field Field
 * @param num_steps Number of long-term evolution steps
 * @return true if stable (no NaN/Inf, bounded growth)
 * 
 * Evolves field for many steps and checks for:
 * - NaN or Inf values
 * - Unbounded growth
 * - Oscillations
 */
bool TCDE_ValidateNumericalStability(TCDE_Field* field, int num_steps);

// ============================================================================
// FRACTAL VALIDATION
// ============================================================================

/**
 * @brief Verify fractal dimension
 * @param field Field
 * @return Measured fractal dimension
 * 
 * Uses box-counting method to measure Hausdorff dimension
 * Should match field->fractal_dimension within ±0.1
 */
float TCDE_VerifyFractalDimension(const TCDE_Field* field);

/**
 * @brief Validate scale invariance
 * @param field Field
 * @param num_scales Number of scales to test
 * @return true if scale-invariant
 * 
 * Checks if field statistics are scale-invariant:
 * ⟨|Φ(λx)|²⟩ ∝ λ^(-df) for various λ
 */
bool TCDE_ValidateScaleInvariance(const TCDE_Field* field, int num_scales);

// ============================================================================
// BENCHMARK PROBLEMS
// ============================================================================

/**
 * @brief Test pure diffusion
 * @param field Field
 * @return true if diffusion behaves correctly
 * 
 * Tests:
 * - Gaussian spreading
 * - Energy dissipation
 * - Maximum principle
 */
bool TCDE_TestPureDiffusion(TCDE_Field* field);

/**
 * @brief Test wave propagation
 * @param field Field
 * @return true if waves propagate correctly
 * 
 * Tests:
 * - Wave speed
 * - Dispersion relation
 * - Energy conservation
 */
bool TCDE_TestWavePropagation(TCDE_Field* field);

/**
 * @brief Test pattern formation
 * @param field Field
 * @return true if patterns form correctly
 * 
 * Tests:
 * - Turing patterns
 * - Spiral waves
 * - Spatiotemporal chaos
 */
bool TCDE_TestPatternFormation(TCDE_Field* field);

// ============================================================================
// COMPREHENSIVE VALIDATION
// ============================================================================

// ============================================================================
// AUTHENTIC COGNITIVE VALIDATION (Non-Simplified)
// ============================================================================

/**
 * @brief Authentic reflexivity validation
 * @param field Field to validate
 * @return Reflexivity score [0,1]
 * 
 * Real test: Creates known pattern, evolves field, measures reconstruction
 * Not simplified: Uses actual pattern reconstruction, not just coherence
 */
float TCDE_ValidateReflexivityAuthentic(TCDE_Field* field);

/**
 * @brief Authentic prediction validation
 * @param field Field to validate
 * @return Prediction accuracy [0,1]
 * 
 * Real test: Creates temporal pattern, extracts τ₂ prediction, compares with evolution
 * Not simplified: Uses actual temporal prediction, not just dimension check
 */
float TCDE_ValidatePredictionAuthentic(TCDE_Field* field);

/**
 * @brief Authentic intuition validation
 * @param field Field to validate
 * @return Number of intuitive connections
 * 
 * Real test: Measures non-obvious connections via torsion along paths
 * Not simplified: Uses actual torsion computation, not just similarity matrix
 */
int TCDE_ValidateIntuitionAuthentic(TCDE_Field* field);

/**
 * @brief Run comprehensive validation suite
 * @param field Field
 * @param relations Modality relations
 * @return Validation results
 * 
 * Runs all validation tests and returns comprehensive results
 */
TCDE_ValidationResults TCDE_Validate(TCDE_Field* field, 
                                      TCDE_ModalityRelations* relations);

/**
 * @brief Print validation results
 * @param results Validation results
 */
void TCDE_PrintValidationResults(const TCDE_ValidationResults* results);

/**
 * @brief Save validation results to file
 * @param results Validation results
 * @param filename Output filename
 * @return true on success
 */
bool TCDE_SaveValidationResults(const TCDE_ValidationResults* results, 
                                 const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_VALIDATION_H
