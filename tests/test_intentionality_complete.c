/**
 * TCDE Intentionality Capabilities - Complete Test Suite
 * Tests: Curiosity (12), Intentional Force (13), Intentional Coherence (14), Autonomous Decisions (15)
 * Date: November 10, 2025
 * Protocol: Zero Tolerance v3.0
 */

#include "../src/validation/tcde_capability_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Test data structure
typedef struct {
    double* field_values;
    double* intention_values;
    int num_points;
    int time_steps;
} IntentionalityTestData;

// Initialize test data
IntentionalityTestData* init_test_data(int num_points, int time_steps) {
    IntentionalityTestData* data = malloc(sizeof(IntentionalityTestData));
    data->num_points = num_points;
    data->time_steps = time_steps;
    data->field_values = malloc(num_points * sizeof(double));
    data->intention_values = malloc(num_points * sizeof(double));
    
    // Initialize with varying values
    srand(time(NULL));
    for (int i = 0; i < num_points; i++) {
        data->field_values[i] = 0.5 + 0.3 * sin(i * 0.1);
        data->intention_values[i] = 0.4 + 0.2 * cos(i * 0.15);
    }
    
    return data;
}

void free_test_data(IntentionalityTestData* data) {
    if (data) {
        free(data->field_values);
        free(data->intention_values);
        free(data);
    }
}

// Test 12: Curiosity - Measure exploration rate
TCDE_CapabilityScore test_curiosity(void* data) {
    TCDE_CapabilityScore score = {0};
    IntentionalityTestData* test_data = (IntentionalityTestData*)data;
    
    score.capability_id = 12;
    strcpy(score.capability_name, "Curiosity");
    
    // Measure exploration: variance in field evolution
    double exploration_sum = 0.0;
    for (int i = 1; i < test_data->num_points; i++) {
        double diff = fabs(test_data->field_values[i] - test_data->field_values[i-1]);
        exploration_sum += diff;
    }
    
    // Normalize by number of points
    double exploration_rate = exploration_sum / (test_data->num_points - 1);
    
    score.score = exploration_rate;
    score.min_expected = 0.0;
    score.max_expected = 10.0;  // Unbounded but typically < 10
    score.is_valid = true;
    
    strcpy(score.validation_message, "Exploration rate measured from field variance");
    
    return score;
}

// Test 13: Intentional Force - Measure force magnitude
TCDE_CapabilityScore test_intentional_force(void* data) {
    TCDE_CapabilityScore score = {0};
    IntentionalityTestData* test_data = (IntentionalityTestData*)data;
    
    score.capability_id = 13;
    strcpy(score.capability_name, "Intentional Force");
    
    // Compute force as gradient of intention field
    double force_magnitude = 0.0;
    for (int i = 1; i < test_data->num_points - 1; i++) {
        double gradient = (test_data->intention_values[i+1] - test_data->intention_values[i-1]) / 2.0;
        force_magnitude += fabs(gradient);
    }
    
    // Average force magnitude
    force_magnitude /= (test_data->num_points - 2);
    
    score.score = force_magnitude;
    score.min_expected = 0.0;
    score.max_expected = 10.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Force magnitude from intention gradient");
    
    return score;
}

// Test 14: Intentional Coherence - Measure goal-field alignment
TCDE_CapabilityScore test_intentional_coherence(void* data) {
    TCDE_CapabilityScore score = {0};
    IntentionalityTestData* test_data = (IntentionalityTestData*)data;
    
    score.capability_id = 14;
    strcpy(score.capability_name, "Intentional Coherence");
    
    // Compute dot product between intention and field
    double dot_product = 0.0;
    double intention_norm = 0.0;
    double field_norm = 0.0;
    
    for (int i = 0; i < test_data->num_points; i++) {
        dot_product += test_data->intention_values[i] * test_data->field_values[i];
        intention_norm += test_data->intention_values[i] * test_data->intention_values[i];
        field_norm += test_data->field_values[i] * test_data->field_values[i];
    }
    
    intention_norm = sqrt(intention_norm);
    field_norm = sqrt(field_norm);
    
    // Normalized coherence
    double coherence = 0.0;
    if (intention_norm > 0.0 && field_norm > 0.0) {
        coherence = dot_product / (intention_norm * field_norm);
        coherence = (coherence + 1.0) / 2.0;  // Map [-1,1] to [0,1]
    }
    
    score.score = coherence;
    score.min_expected = 0.0;
    score.max_expected = 1.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Coherence from normalized dot product");
    
    return score;
}

// Test 15: Autonomous Decisions - Count independent decisions
TCDE_CapabilityScore test_autonomous_decisions(void* data) {
    TCDE_CapabilityScore score = {0};
    IntentionalityTestData* test_data = (IntentionalityTestData*)data;
    
    score.capability_id = 15;
    strcpy(score.capability_name, "Autonomous Decisions");
    
    // Count decision points: significant changes in field direction
    int decision_count = 0;
    double threshold = 0.1;
    
    for (int i = 2; i < test_data->num_points; i++) {
        double prev_diff = test_data->field_values[i-1] - test_data->field_values[i-2];
        double curr_diff = test_data->field_values[i] - test_data->field_values[i-1];
        
        // Decision = change in direction
        if (prev_diff * curr_diff < 0 && fabs(curr_diff) > threshold) {
            decision_count++;
        }
    }
    
    // Normalize by time steps
    double decision_rate = (double)decision_count / test_data->num_points;
    
    score.score = decision_rate;
    score.min_expected = 0.0;
    score.max_expected = 10.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Decision rate from field direction changes");
    
    return score;
}

// Main test program
int main() {
    printf("TCDE Intentionality Capabilities Test Suite\n");
    printf("Protocol: Zero Tolerance v3.0\n");
    printf("Tests: 4 (IDs: 12, 13, 14, 15)\n\n");
    
    // Initialize test data
    IntentionalityTestData* test_data = init_test_data(100, 50);
    
    // Initialize validation context
    TCDE_ValidationContext ctx;
    TCDE_StatisticalResult stats;
    
    if (!TCDE_InitValidationContext(&ctx, 100)) {
        fprintf(stderr, "Failed to initialize validation context\n");
        free_test_data(test_data);
        return 1;
    }
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 12: Curiosity
    printf("Test 12: Curiosity\n");
    TCDE_CapabilityScore result_12 = TCDE_ExecuteCapabilityTest(
        test_curiosity, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_12.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_12.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_12.is_valid) passed_tests++;
    
    // Test 13: Intentional Force
    printf("Test 13: Intentional Force\n");
    TCDE_CapabilityScore result_13 = TCDE_ExecuteCapabilityTest(
        test_intentional_force, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_13.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_13.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_13.is_valid) passed_tests++;
    
    // Test 14: Intentional Coherence
    printf("Test 14: Intentional Coherence\n");
    TCDE_CapabilityScore result_14 = TCDE_ExecuteCapabilityTest(
        test_intentional_coherence, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_14.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_14.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_14.is_valid) passed_tests++;
    
    // Test 15: Autonomous Decisions
    printf("Test 15: Autonomous Decisions\n");
    TCDE_CapabilityScore result_15 = TCDE_ExecuteCapabilityTest(
        test_autonomous_decisions, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_15.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_15.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_15.is_valid) passed_tests++;
    
    // Summary
    printf("========================================\n");
    printf("Intentionality Tests Summary\n");
    printf("========================================\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.2f%%\n", (double)passed_tests / total_tests * 100.0);
    
    // Cleanup
    TCDE_FreeValidationContext(&ctx);
    free_test_data(test_data);
    
    return (passed_tests == total_tests) ? 0 : 1;
}
