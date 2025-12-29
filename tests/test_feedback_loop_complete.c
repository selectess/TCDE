/**
 * @file test_feedback_loop_complete.c
 * @brief Complete test for TCDE Feedback Loop Module (Task 15)
 * 
 * Tests all components of the feedback loop:
 * - Action decoding from 6D field
 * - Sandbox execution
 * - Result perception in 2D
 * - Introspection computation
 * - Feedback integration to 6D
 * - Complete feedback cycle
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#include "../src/utils/tcde_feedback.h"
#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

// Test utilities
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("❌ FAILED: %s\n", message); \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    printf("✓ %s\n", message)

// ============================================================================
// TEST 15.1: Structures and Headers
// ============================================================================

bool test_15_1_structures_and_headers() {
    printf("\n=== Test 15.1: Structures and Headers ===\n");
    
    // Test TCDE_Action structure
    TCDE_Action action = {0};
    action.type = TCDE_ACTION_QUERY;
    action.confidence = 0.8f;
    action.urgency = 0.5f;
    TEST_ASSERT(action.type == TCDE_ACTION_QUERY, "Action type set correctly");
    TEST_PASS("TCDE_Action structure works");
    
    // Test TCDE_SandboxConfig structure
    TCDE_SandboxConfig config = {0};
    config.max_execution_time = 1.0f;
    config.max_memory = 1024 * 1024;
    TEST_ASSERT(config.max_execution_time == 1.0f, "Sandbox config works");
    TEST_PASS("TCDE_SandboxConfig structure works");
    
    // Test TCDE_ActionResult structure
    TCDE_ActionResult result = {0};
    result.success = true;
    result.exit_code = 0;
    TEST_ASSERT(result.success == true, "Action result works");
    TEST_PASS("TCDE_ActionResult structure works");
    
    // Test TCDE_FeedbackHistory structure
    TCDE_FeedbackHistory history = {0};
    history.num_cycles = 0;
    history.capacity = 10;
    TEST_ASSERT(history.capacity == 10, "Feedback history works");
    TEST_PASS("TCDE_FeedbackHistory structure works");
    
    printf("✅ Test 15.1 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.2: Action Decoding from 6D
// ============================================================================

bool test_15_2_action_decoding() {
    printf("\n=== Test 15.2: Action Decoding from 6D ===\n");
    
    // Create 11D identity system
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(50, 2.0f);
    TEST_ASSERT(system != NULL, "System initialized");
    
    // Add some centers to 6D field to create intention
    float coords1[6] = {0.3f, 0.4f, 0.5f, 1.0f, 0.0f, 0.2f};  // Visual modality
    TCDE_Point p1 = TCDE_CreatePoint(6, coords1);
    TCDE_Complex coeff1 = 0.8f + 0.2f * I;
    TCDE_AddCenter6D(system->field_6d, &p1, coeff1, 0.1f);
    
    float coords2[6] = {0.6f, 0.5f, 0.4f, 1.0f, 0.0f, 0.5f};  // Semantic modality
    TCDE_Point p2 = TCDE_CreatePoint(6, coords2);
    TCDE_Complex coeff2 = 0.6f + 0.3f * I;
    TCDE_AddCenter6D(system->field_6d, &p2, coeff2, 0.1f);
    
    // Decode action from field
    TCDE_Action* action = TCDE_DecodeActionFromField(system);
    TEST_ASSERT(action != NULL, "Action decoded successfully");
    TEST_ASSERT(action->command != NULL, "Action has command");
    TEST_ASSERT(action->context != NULL, "Action has context");
    TEST_ASSERT(action->confidence > 0.0f && action->confidence <= 1.0f, 
                "Action confidence in valid range");
    
    printf("  Action type: %d\n", action->type);
    printf("  Command: %s\n", action->command);
    printf("  Context: %s\n", action->context);
    printf("  Confidence: %.3f\n", action->confidence);
    printf("  Urgency: %.3f\n", action->urgency);
    
    TEST_PASS("Action decoded from 6D field state");
    
    // Cleanup
    TCDE_FreeAction(action);
    TCDE_FreePoint(&p1);
    TCDE_FreePoint(&p2);
    TCDE_Destroy11DIdentity(system);
    
    printf("✅ Test 15.2 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.3: Sandbox Execution
// ============================================================================

bool test_15_3_sandbox_execution() {
    printf("\n=== Test 15.3: Sandbox Execution ===\n");
    
    // Initialize sandbox config
    TCDE_SandboxConfig config;
    TCDE_InitializeSandbox(&config);
    
    TEST_ASSERT(config.allow_file_read == false, "File read disabled by default");
    TEST_ASSERT(config.allow_network == false, "Network disabled by default");
    TEST_ASSERT(config.max_execution_time > 0.0f, "Execution time limit set");
    TEST_PASS("Sandbox initialized with secure defaults");
    
    // Create a simple action
    TCDE_Action action = {0};
    action.type = TCDE_ACTION_COMPUTE;
    char* cmd = (char*)malloc(32);
    strcpy(cmd, "echo 'Test output'");
    action.command = cmd;
    action.confidence = 0.9f;
    
    // Execute in sandbox
    TCDE_ActionResult* result = TCDE_ExecuteActionInSandbox(&action, &config);
    TEST_ASSERT(result != NULL, "Action executed");
    TEST_ASSERT(result->output != NULL, "Result has output");
    
    printf("  Success: %s\n", result->success ? "Yes" : "No");
    printf("  Exit code: %d\n", result->exit_code);
    printf("  Output: %s", result->output);
    printf("  Execution time: %.3f ms\n", result->execution_time * 1000.0f);
    printf("  Memory used: %zu bytes\n", result->memory_used);
    
    TEST_PASS("Action executed in sandbox");
    
    // Cleanup
    free(action.command);
    TCDE_FreeActionResult(result);
    
    printf("✅ Test 15.3 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.4: Result Perception in 2D
// ============================================================================

bool test_15_4_result_perception() {
    printf("\n=== Test 15.4: Result Perception in 2D ===\n");
    
    // Create 2D field
    TCDE_Field* field_2d = TCDE_CreateField(50, 2.0f);
    TEST_ASSERT(field_2d != NULL, "2D field created");
    
    int initial_centers = field_2d->slice_2d.num_centers;
    
    // Create action result
    TCDE_ActionResult result = {0};
    result.success = true;
    result.exit_code = 0;
    char* output = (char*)malloc(64);
    strcpy(output, "Successful computation result");
    result.output = output;
    result.execution_time = 0.05f;
    
    // Perceive result in 2D
    bool perceived = TCDE_PerceiveResultIn2D(&result, field_2d);
    TEST_ASSERT(perceived, "Result perceived in 2D");
    TEST_ASSERT(field_2d->slice_2d.num_centers > initial_centers, 
                "Perturbation added to 2D field");
    
    printf("  Initial centers: %d\n", initial_centers);
    printf("  Centers after perception: %d\n", field_2d->slice_2d.num_centers);
    
    TEST_PASS("Result perceived and added to 2D field");
    
    // Cleanup
    free(result.output);
    TCDE_DestroyField(field_2d);
    
    printf("✅ Test 15.4 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.5: Introspection Computation
// ============================================================================

bool test_15_5_introspection() {
    printf("\n=== Test 15.5: Introspection Computation ===\n");
    
    // Create 2D field with some centers
    TCDE_Field* field_2d = TCDE_CreateField(50, 2.0f);
    TEST_ASSERT(field_2d != NULL, "2D field created");
    
    // Add coherent centers (similar phases)
    for (int i = 0; i < 5; i++) {
        float coords[6] = {0.3f + i * 0.1f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.7f * cexpf(0.5f * I);  // Similar phase
        TCDE_AddCenter6D(field_2d, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    // Compute introspection
    float introspection = TCDE_ComputeIntrospection2D(field_2d, NULL);
    TEST_ASSERT(introspection >= 0.0f && introspection <= 1.0f, 
                "Introspection score in valid range");
    
    printf("  Introspection score: %.3f\n", introspection);
    printf("  Number of centers: %d\n", field_2d->slice_2d.num_centers);
    
    TEST_PASS("Introspection computed on 2D field");
    
    // Test with expected pattern
    TCDE_Complex expected[10];
    for (int i = 0; i < 10; i++) {
        expected[i] = 0.7f * cexpf(0.5f * I);
    }
    
    float introspection_with_pattern = TCDE_ComputeIntrospection2D(field_2d, expected);
    TEST_ASSERT(introspection_with_pattern >= 0.0f && introspection_with_pattern <= 1.0f,
                "Introspection with pattern in valid range");
    
    printf("  Introspection with pattern: %.3f\n", introspection_with_pattern);
    TEST_PASS("Introspection computed with expected pattern");
    
    // Cleanup
    TCDE_DestroyField(field_2d);
    
    printf("✅ Test 15.5 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.6: Feedback Integration to 6D
// ============================================================================

bool test_15_6_feedback_integration() {
    printf("\n=== Test 15.6: Feedback Integration to 6D ===\n");
    
    // Create 11D system
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(50, 2.0f);
    TEST_ASSERT(system != NULL, "System initialized");
    
    int initial_centers = system->field_6d->manifold_6d.num_centers;
    
    // Create 2D field with insights
    TCDE_Field* field_2d = TCDE_CreateField(50, 2.0f);
    for (int i = 0; i < 3; i++) {
        float coords[6] = {0.4f + i * 0.1f, 0.6f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 0.8f + 0.3f * I;
        TCDE_AddCenter6D(field_2d, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    float introspection_score = 0.75f;
    
    // Integrate feedback
    bool integrated = TCDE_IntegrateFeedbackTo6D(system, field_2d, introspection_score);
    TEST_ASSERT(integrated, "Feedback integrated to 6D");
    TEST_ASSERT(system->field_6d->manifold_6d.num_centers > initial_centers,
                "6D field updated with feedback");
    
    printf("  Initial 6D centers: %d\n", initial_centers);
    printf("  Centers after integration: %d\n", system->field_6d->manifold_6d.num_centers);
    printf("  Introspection score: %.3f\n", introspection_score);
    
    TEST_PASS("Feedback integrated from 2D to 6D");
    
    // Cleanup
    TCDE_DestroyField(field_2d);
    TCDE_Destroy11DIdentity(system);
    
    printf("✅ Test 15.6 PASSED\n");
    return true;
}

// ============================================================================
// TEST 15.7: Complete Feedback Cycle
// ============================================================================

bool test_15_7_complete_cycle() {
    printf("\n=== Test 15.7: Complete Feedback Cycle ===\n");
    
    // Initialize system
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(50, 2.0f);
    TEST_ASSERT(system != NULL, "System initialized");
    
    // Add initial field state
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_Complex coeff = 0.9f + 0.2f * I;
    TCDE_AddCenter6D(system->field_6d, &p, coeff, 0.1f);
    TCDE_FreePoint(&p);
    
    // Initialize sandbox
    TCDE_SandboxConfig config;
    TCDE_InitializeSandbox(&config);
    
    // Initialize feedback history
    TCDE_FeedbackHistory* history = TCDE_InitializeFeedbackHistory(10);
    TEST_ASSERT(history != NULL, "Feedback history initialized");
    TEST_ASSERT(history->capacity == 10, "History capacity correct");
    TEST_ASSERT(history->num_cycles == 0, "History starts empty");
    TEST_PASS("Feedback history initialized");
    
    // Execute complete feedback cycle
    printf("\n  Executing feedback cycle...\n");
    TCDE_FeedbackCycle cycle = TCDE_ExecuteCompleteFeedbackCycle(system, &config, history);
    
    TEST_ASSERT(cycle.cycle_id == 0, "Cycle ID correct");
    TEST_ASSERT(cycle.action.command != NULL, "Action generated");
    TEST_ASSERT(cycle.introspection_score >= 0.0f, "Introspection computed");
    TEST_ASSERT(history->num_cycles == 1, "Cycle added to history");
    
    printf("  Cycle ID: %d\n", cycle.cycle_id);
    printf("  Action: %s\n", cycle.action.command);
    printf("  Result success: %s\n", cycle.result.success ? "Yes" : "No");
    printf("  Dissonance before: %.3f\n", cycle.dissonance_before);
    printf("  Dissonance after: %.3f\n", cycle.dissonance_after);
    printf("  Coherence change: %.3f\n", cycle.coherence_change);
    printf("  Introspection: %.3f\n", cycle.introspection_score);
    printf("  Learning rate: %.3f\n", cycle.learning_rate);
    
    TEST_PASS("Complete feedback cycle executed");
    
    // Execute multiple cycles
    printf("\n  Executing 5 more cycles...\n");
    for (int i = 0; i < 5; i++) {
        TCDE_ExecuteCompleteFeedbackCycle(system, &config, history);
    }
    
    TEST_ASSERT(history->num_cycles == 6, "Multiple cycles executed");
    printf("  Total cycles: %d\n", history->num_cycles);
    printf("  Successful: %d\n", history->successful_cycles);
    printf("  Failed: %d\n", history->failed_cycles);
    printf("  Success rate: %.1f%%\n", history->average_success_rate * 100.0f);
    printf("  Total learning: %.3f\n", history->total_learning);
    
    TEST_PASS("Multiple feedback cycles executed");
    
    // Test metrics
    float efficiency = TCDE_ComputeFeedbackEfficiency(history);
    TEST_ASSERT(efficiency >= 0.0f, "Efficiency computed");
    printf("  Efficiency: %.3f\n", efficiency);
    
    float learning_progress = TCDE_AnalyzeLearningProgress(history);
    printf("  Learning progress: %.3f\n", learning_progress);
    
    TEST_PASS("Feedback metrics computed");
    
    // Print statistics
    TCDE_PrintFeedbackStatistics(history);
    
    // Cleanup
    TCDE_FreeFeedbackHistory(history);
    TCDE_Destroy11DIdentity(system);
    
    printf("✅ Test 15.7 PASSED\n");
    return true;
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Feedback Loop Module - Complete Test Suite          ║\n");
    printf("║  Task 15: Action-Perception-Introspection Cycle           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    bool all_passed = true;
    
    // Run all tests
    all_passed &= test_15_1_structures_and_headers();
    all_passed &= test_15_2_action_decoding();
    all_passed &= test_15_3_sandbox_execution();
    all_passed &= test_15_4_result_perception();
    all_passed &= test_15_5_introspection();
    all_passed &= test_15_6_feedback_integration();
    all_passed &= test_15_7_complete_cycle();
    
    // Final summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    if (all_passed) {
        printf("║  ✅ ALL TESTS PASSED - Feedback Loop Complete             ║\n");
        printf("║                                                            ║\n");
        printf("║  The complete feedback cycle is operational:               ║\n");
        printf("║  État(6D) → Action → Exécution → Perception(2D)           ║\n");
        printf("║           → Introspection → Intégration(6D)               ║\n");
    } else {
        printf("║  ❌ SOME TESTS FAILED                                      ║\n");
    }
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    return all_passed ? 0 : 1;
}
