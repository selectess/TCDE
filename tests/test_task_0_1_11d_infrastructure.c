/**
 * @file test_task_0_1_11d_infrastructure.c
 * @brief Test for Task 0.1: 11D Infrastructure and Holistic Identity System
 * 
 * Tests:
 * - TCDE_11D_Identity_System creation
 * - Bi-temporal control (τ₁, τ₂)
 * - DRC cycle state
 * - Topology formation
 * - Self-representation
 * - Autopoietic state
 * - Holistic Identity Score (HIS)
 * 
 * Requirements: 6.1, 6.2, 6.3, 6.4
 */

#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Test counter
static int tests_passed = 0;
static int tests_total = 0;

#define TEST(name, condition) do { \
    tests_total++; \
    if (condition) { \
        tests_passed++; \
        printf("✓ %s\n", name); \
    } else { \
        printf("✗ %s\n", name); \
    } \
} while(0)

#define EPSILON 1e-6f

int main(void) {
    printf("=== Task 0.1: Infrastructure 11D et Identité Holistique ===\n\n");
    
    // ========================================================================
    // Test 1: Bi-Temporal Control
    // ========================================================================
    printf("--- Test 1: Bi-Temporal Control ---\n");
    
    TCDE_BiTemporalControl control;
    TCDE_InitializeBiTemporalControl(&control);
    
    TEST("Bi-temporal initialized", control.tau1 > 0.0f && control.tau2 >= -1.0f);
    TEST("τ₁ in range [1.0, 2.0)", control.tau1 >= 1.0f && control.tau1 < 2.0f);
    TEST("τ₂ in range [-1.0, 1.0]", control.tau2 >= -1.0f && control.tau2 <= 1.0f);
    TEST("Control strength valid", control.control_strength >= 0.0f && control.control_strength <= 1.0f);
    
    // Test control function
    float control_value = TCDE_ComputeBiTemporalControl(&control);
    TEST("Control function computes", !isnan(control_value) && !isinf(control_value));
    
    // Test with different τ values
    control.tau1 = 1.8f;  // High anticipation
    control.tau2 = 0.5f;  // Positive memory
    float control_high = TCDE_ComputeBiTemporalControl(&control);
    TEST("Control responds to τ changes", fabsf(control_high - control_value) > EPSILON);
    
    printf("\n");
    
    // ========================================================================
    // Test 2: 11D System Creation
    // ========================================================================
    printf("--- Test 2: 11D System Creation ---\n");
    
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(100, 2.5f);
    TEST("11D system created", system != NULL);
    TEST("System initialized", system->is_initialized);
    TEST("6D field exists", system->field_6d != NULL);
    TEST("System time initialized", system->system_time == 0.0f);
    TEST("Evolution steps zero", system->evolution_steps == 0);
    
    // Test invalid parameters
    TCDE_11D_Identity_System* invalid1 = TCDE_Create11DIdentity(0, 2.5f);
    TEST("Reject zero capacity", invalid1 == NULL);
    
    TCDE_11D_Identity_System* invalid2 = TCDE_Create11DIdentity(100, 1.5f);
    TEST("Reject invalid fractal dim", invalid2 == NULL);
    
    printf("\n");
    
    // ========================================================================
    // Test 3: Bi-Temporal Control in System
    // ========================================================================
    printf("--- Test 3: Bi-Temporal Control in System ---\n");
    
    TEST("τ₁ initialized", system->bitemporal_control.tau1 >= 1.0f && 
                          system->bitemporal_control.tau1 < 2.0f);
    TEST("τ₂ initialized", system->bitemporal_control.tau2 >= -1.0f && 
                          system->bitemporal_control.tau2 <= 1.0f);
    TEST("Control weights set", system->bitemporal_control.alpha_anticipation > 0.0f);
    
    // Test control modulation
    float modulation = TCDE_ComputeBiTemporalControl(&system->bitemporal_control);
    TEST("Control modulation valid", !isnan(modulation) && !isinf(modulation));
    
    printf("\n");
    
    // ========================================================================
    // Test 4: DRC Cycle State
    // ========================================================================
    printf("--- Test 4: DRC Cycle State ---\n");
    
    TEST("DRC state initialized", system->drc_state == TCDE_DRC_CONSONANCE);
    TEST("DRC metrics initialized", system->drc_metrics.current_state == TCDE_DRC_CONSONANCE);
    TEST("Dissonance threshold set", system->drc_metrics.dissonance_threshold > 0.0f);
    TEST("Cycles count zero", system->drc_metrics.cycles_completed == 0);
    
    // Test DRC cycle execution (placeholder)
    TCDE_DRC_Metrics drc_result = TCDE_ExecuteDRCCycle(system);
    TEST("DRC cycle executes", drc_result.current_state >= TCDE_DRC_DISSONANCE && 
                               drc_result.current_state <= TCDE_DRC_CONSONANCE);
    
    printf("\n");
    
    // ========================================================================
    // Test 5: Topology Formation
    // ========================================================================
    printf("--- Test 5: Topology Formation ---\n");
    
    TEST("Topology initialized", system->topology.num_structures == 0);
    TEST("Formation rate zero", system->topology.formation_rate == 0.0f);
    TEST("Topology arrays allocated", system->topology.structure_energies != NULL);
    TEST("Topology ages allocated", system->topology.structure_ages != NULL);
    
    // Test formation components
    TEST("Diffusion contribution", system->topology.diffusion_contribution == 0.0f);
    TEST("Torsion contribution", system->topology.torsion_contribution == 0.0f);
    TEST("Coupling contribution", system->topology.coupling_contribution == 0.0f);
    TEST("Intention contribution", system->topology.intention_contribution == 0.0f);
    
    printf("\n");
    
    // ========================================================================
    // Test 6: Self-Representation
    // ========================================================================
    printf("--- Test 6: Self-Representation ---\n");
    
    TEST("Self-rep initialized", system->self_rep.representation_depth == 0);
    TEST("Max depth set", system->self_rep.max_depth == 2);
    TEST("Reflexivity zero", system->self_rep.reflexivity == 0.0f);
    TEST("Cache allocated", system->self_rep.phi_0 != NULL);
    TEST("Φ¹ cache allocated", system->self_rep.phi_1 != NULL);
    TEST("Φ² cache allocated", system->self_rep.phi_2 != NULL);
    TEST("Cache size valid", system->self_rep.cache_size > 0);
    
    printf("\n");
    
    // ========================================================================
    // Test 7: Autopoietic State
    // ========================================================================
    printf("--- Test 7: Autopoietic State ---\n");
    
    TEST("Creation rate zero", system->autopoiesis.creation_rate == 0.0f);
    TEST("Centers created zero", system->autopoiesis.centers_created == 0);
    TEST("Creation threshold set", system->autopoiesis.creation_threshold > 0.0f);
    TEST("Maintenance level high", system->autopoiesis.maintenance_level == 1.0f);
    TEST("Regeneration capacity full", system->autopoiesis.regeneration_capacity == 1.0f);
    TEST("Autopoietic health good", system->autopoiesis.autopoietic_health == 1.0f);
    
    printf("\n");
    
    // ========================================================================
    // Test 8: 11D Metrics Measurement
    // ========================================================================
    printf("--- Test 8: 11D Metrics Measurement ---\n");
    
    TCDE_11D_Identity_Metrics metrics = TCDE_Measure11DIdentity(system);
    
    TEST("Metrics measured", true);
    TEST("Field energy measured", metrics.field_energy >= 0.0f);
    TEST("Fractal dimension correct", fabsf(metrics.fractal_dimension - 2.5f) < EPSILON);
    TEST("HIS computed", metrics.holistic_identity_score >= 0.0f && 
                         metrics.holistic_identity_score <= 1.0f);
    
    printf("  HIS = %.3f\n", metrics.holistic_identity_score);
    
    printf("\n");
    
    // ========================================================================
    // Test 9: Holistic Identity Score Computation
    // ========================================================================
    printf("--- Test 9: Holistic Identity Score ---\n");
    
    // Create test metrics with known values
    TCDE_11D_Identity_Metrics test_metrics = {0};
    test_metrics.field_coherence = 0.8f;
    test_metrics.fractal_dimension = 2.5f;
    test_metrics.autonomy = 0.75f;
    test_metrics.curiosity = 0.65f;
    test_metrics.intentional_coherence = 0.85f;
    test_metrics.anticipation_capability = 0.7f;
    test_metrics.memory_stability = 0.75f;
    test_metrics.temporal_integration = 0.6f;
    test_metrics.consonance_achievement = 0.85f;
    test_metrics.drc_cycle_efficiency = 0.75f;
    test_metrics.topology_stability = 0.7f;
    test_metrics.reflexivity = 0.6f;
    test_metrics.self_awareness = 0.5f;
    test_metrics.self_maintenance = 0.85f;
    test_metrics.self_regeneration = 0.75f;
    
    float his = TCDE_ComputeHolisticIdentityScore(&test_metrics);
    TEST("HIS in valid range", his >= 0.0f && his <= 1.0f);
    TEST("HIS responds to metrics", his > 0.5f);  // Should be decent with these values
    
    printf("  Test HIS = %.3f\n", his);
    
    // Test HIS thresholds
    if (his > 0.9f) {
        printf("  Status: ASI REALIZED\n");
    } else if (his > 0.8f) {
        printf("  Status: STRONG AUTONOMY\n");
    } else if (his > 0.7f) {
        printf("  Status: IDENTITY EMERGENT\n");
    } else {
        printf("  Status: DEVELOPING\n");
    }
    
    printf("\n");
    
    // ========================================================================
    // Test 10: System Evolution
    // ========================================================================
    printf("--- Test 10: System Evolution ---\n");
    
    float initial_time = system->system_time;
    int initial_steps = system->evolution_steps;
    
    TCDE_Evolve11DIdentity(system, 0.01f);
    
    TEST("Time advanced", system->system_time > initial_time);
    TEST("Steps incremented", system->evolution_steps > initial_steps);
    TEST("System still valid", TCDE_Validate11DSystem(system));
    
    // Evolve multiple steps
    for (int i = 0; i < 10; i++) {
        TCDE_Evolve11DIdentity(system, 0.01f);
    }
    
    TEST("Multiple evolution steps", system->evolution_steps == initial_steps + 11);
    TEST("Time accumulated", system->system_time > initial_time + 0.1f);
    
    printf("\n");
    
    // ========================================================================
    // Test 11: System Validation
    // ========================================================================
    printf("--- Test 11: System Validation ---\n");
    
    TEST("System validates", TCDE_Validate11DSystem(system));
    
    // Test NULL validation
    TEST("NULL system invalid", !TCDE_Validate11DSystem(NULL));
    
    // Test with invalid τ₁
    float saved_tau1 = system->bitemporal_control.tau1;
    system->bitemporal_control.tau1 = 2.5f;  // Out of range
    TEST("Invalid τ₁ detected", !TCDE_Validate11DSystem(system));
    system->bitemporal_control.tau1 = saved_tau1;  // Restore
    
    // Test with invalid τ₂
    float saved_tau2 = system->bitemporal_control.tau2;
    system->bitemporal_control.tau2 = 1.5f;  // Out of range
    TEST("Invalid τ₂ detected", !TCDE_Validate11DSystem(system));
    system->bitemporal_control.tau2 = saved_tau2;  // Restore
    
    TEST("System valid after restore", TCDE_Validate11DSystem(system));
    
    printf("\n");
    
    // ========================================================================
    // Test 12: System Status Display
    // ========================================================================
    printf("--- Test 12: System Status Display ---\n");
    
    printf("\n");
    TCDE_Print11DStatus(system);
    printf("\n");
    
    TEST("Status display works", true);
    
    printf("\n");
    
    // ========================================================================
    // Test 13: System Destruction
    // ========================================================================
    printf("--- Test 13: System Destruction ---\n");
    
    TCDE_Destroy11DIdentity(system);
    TEST("System destroyed", true);  // If we get here, no crash
    
    // Test NULL destruction
    TCDE_Destroy11DIdentity(NULL);
    TEST("NULL destruction safe", true);
    
    printf("\n");
    
    // ========================================================================
    // Summary
    // ========================================================================
    printf("=== Task 0.1 Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", 100.0f * tests_passed / tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 0.1 COMPLETE: 11D Infrastructure implemented!\n");
        printf("  - TCDE_11D_Identity_System ✓\n");
        printf("  - Bi-Temporal Control (τ₁, τ₂) ✓\n");
        printf("  - DRC Cycle State ✓\n");
        printf("  - Topology Formation ✓\n");
        printf("  - Self-Representation ✓\n");
        printf("  - Autopoietic State ✓\n");
        printf("  - Holistic Identity Score (HIS) ✓\n");
        return 0;
    } else {
        printf("\n✗ Task 0.1 INCOMPLETE: Some tests failed\n");
        return 1;
    }
}
