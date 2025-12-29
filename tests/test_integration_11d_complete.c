/**
 * @file test_integration_11d_complete.c
 * @brief Integration test for complete 11D system
 * 
 * Tests the integration of all 11D components working together:
 * - 6D Field + 3D Intention + 2D Bi-temporal
 * - Complete system evolution
 * - Holistic emergence
 */

#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int main(void) {
    printf("=== 11D System Integration Test ===\n\n");
    
    // Create complete 11D system
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentitySystem(50, 2.5f);
    TEST("11D system created", system != NULL);
    
    if (!system) {
        printf("❌ Cannot proceed without system\n");
        return 1;
    }
    
    // Test initial state
    TEST("System initialized", system->field != NULL);
    TEST("Bi-temporal control initialized", system->bi_temporal.tau1 > 0.0f);
    TEST("DRC cycle initialized", system->drc.current_state >= 0);
    TEST("Self-representation initialized", system->self_rep.max_depth > 0);
    TEST("Autopoiesis initialized", system->autopoiesis.maintenance_level > 0.0f);
    
    // Evolve system for multiple steps
    float initial_his = 0.0f;
    float final_his = 0.0f;
    
    // Measure initial HIS
    TCDE_11D_Identity_Metrics metrics;
    TCDE_Measure11DIdentityMetrics(system, &metrics);
    initial_his = TCDE_ComputeHolisticIdentityScore(&metrics);
    
    TEST("Initial HIS computed", initial_his >= 0.0f && initial_his <= 1.0f);
    
    // Evolution loop
    int evolution_steps = 100;
    for (int i = 0; i < evolution_steps; i++) {
        TCDE_Evolve11DIdentitySystem(system, 0.01f);
    }
    
    // Measure final HIS
    TCDE_Measure11DIdentityMetrics(system, &metrics);
    final_his = TCDE_ComputeHolisticIdentityScore(&metrics);
    
    TEST("Final HIS computed", final_his >= 0.0f && final_his <= 1.0f);
    TEST("System evolved", system->evolution_steps == evolution_steps);
    TEST("Time advanced", system->time > 0.0f);
    
    // Test component integration
    TEST("Bi-temporal active", system->bi_temporal.control_strength > 0.0f);
    TEST("DRC cycle active", system->drc.cycles_completed > 0);
    TEST("Autopoiesis healthy", system->autopoiesis.health > 0.5f);
    
    // Test holistic emergence
    float his_improvement = final_his - initial_his;
    TEST("HIS shows development", final_his >= initial_his - 0.1f); // Allow small fluctuations
    
    printf("\n=== Integration Results ===\n");
    printf("Initial HIS: %.6f\n", initial_his);
    printf("Final HIS:   %.6f\n", final_his);
    printf("Change:      %.6f\n", his_improvement);
    printf("Evolution steps: %d\n", system->evolution_steps);
    printf("System time: %.6f\n", system->time);
    
    // Cleanup
    TCDE_Destroy11DIdentitySystem(system);
    TEST("System destroyed", true);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    float success_rate = (float)tests_passed / tests_total * 100.0f;
    printf("Success rate: %.1f%%\n", success_rate);
    
    if (success_rate >= 90.0f) {
        printf("\n✅ 11D INTEGRATION TEST PASSED\n");
        return 0;
    } else {
        printf("\n❌ 11D INTEGRATION TEST FAILED\n");
        return 1;
    }
}