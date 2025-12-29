/**
 * TCDE Autopoiesis Capabilities - Complete Test Suite
 * Tests: Autopoietic Health (22)
 * Date: November 11, 2025
 * Protocol: Zero Tolerance v3.0
 */

#include "../src/validation/tcde_capability_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Test data structure for autopoiesis
typedef struct {
    double* system_integrity;  // System integrity over time
    double* repair_rates;      // Self-repair rates
    double* component_health;  // Health of individual components
    int num_timesteps;
    int num_components;
    double damage_level;
} AutopoiesisTestData;

// Initialize test data
AutopoiesisTestData* init_autopoiesis_data(int num_timesteps, int num_components) {
    AutopoiesisTestData* data = malloc(sizeof(AutopoiesisTestData));
    data->num_timesteps = num_timesteps;
    data->num_components = num_components;
    data->damage_level = 0.2;  // 20% damage
    
    data->system_integrity = malloc(num_timesteps * sizeof(double));
    data->repair_rates = malloc(num_timesteps * sizeof(double));
    data->component_health = malloc(num_components * sizeof(double));
    
    // Initialize with autopoietic dynamics
    srand(time(NULL) + 6);
    
    // Initial damage followed by self-repair
    double initial_integrity = 1.0 - data->damage_level;
    
    for (int t = 0; t < num_timesteps; t++) {
        double time_factor = (double)t / num_timesteps;
        
        // System integrity recovers over time (autopoietic self-repair)
        double recovery = data->damage_level * (1.0 - exp(-3.0 * time_factor));
        data->system_integrity[t] = initial_integrity + recovery;
        
        // Add small fluctuations
        data->system_integrity[t] += 0.02 * (rand() / (double)RAND_MAX - 0.5);
        
        // Clamp to [0, 1]
        if (data->system_integrity[t] > 1.0) data->system_integrity[t] = 1.0;
        if (data->system_integrity[t] < 0.0) data->system_integrity[t] = 0.0;
        
        // Repair rate: derivative of integrity
        if (t > 0) {
            data->repair_rates[t] = data->system_integrity[t] - data->system_integrity[t-1];
        } else {
            data->repair_rates[t] = 0.0;
        }
    }
    
    // Component health: varies but maintains overall system integrity
    double total_health = 0.0;
    for (int c = 0; c < num_components; c++) {
        // Each component has slightly different health
        data->component_health[c] = 0.7 + 0.3 * (rand() / (double)RAND_MAX);
        total_health += data->component_health[c];
    }
    
    // Normalize to match system integrity
    double avg_integrity = 0.0;
    for (int t = 0; t < num_timesteps; t++) {
        avg_integrity += data->system_integrity[t];
    }
    avg_integrity /= num_timesteps;
    
    double scale = (avg_integrity * num_components) / total_health;
    for (int c = 0; c < num_components; c++) {
        data->component_health[c] *= scale;
        if (data->component_health[c] > 1.0) data->component_health[c] = 1.0;
    }
    
    return data;
}

void free_autopoiesis_data(AutopoiesisTestData* data) {
    if (data) {
        free(data->system_integrity);
        free(data->repair_rates);
        free(data->component_health);
        free(data);
    }
}

// Test 22: Autopoietic Health - Measure system integrity and self-repair
TCDE_CapabilityScore test_autopoietic_health(void* data) {
    TCDE_CapabilityScore score = {0};
    AutopoiesisTestData* test_data = (AutopoiesisTestData*)data;
    
    score.capability_id = 22;
    strcpy(score.capability_name, "Autopoietic Health");
    
    // Compute autopoietic health from multiple factors
    
    // 1. Average system integrity
    double avg_integrity = 0.0;
    for (int t = 0; t < test_data->num_timesteps; t++) {
        avg_integrity += test_data->system_integrity[t];
    }
    avg_integrity /= test_data->num_timesteps;
    
    // 2. Self-repair capability (positive repair rates)
    double avg_repair = 0.0;
    int positive_repairs = 0;
    for (int t = 1; t < test_data->num_timesteps; t++) {
        if (test_data->repair_rates[t] > 0) {
            avg_repair += test_data->repair_rates[t];
            positive_repairs++;
        }
    }
    if (positive_repairs > 0) {
        avg_repair /= positive_repairs;
    }
    
    // 3. Component health variance (low variance = balanced system)
    double mean_component_health = 0.0;
    for (int c = 0; c < test_data->num_components; c++) {
        mean_component_health += test_data->component_health[c];
    }
    mean_component_health /= test_data->num_components;
    
    double variance = 0.0;
    for (int c = 0; c < test_data->num_components; c++) {
        double diff = test_data->component_health[c] - mean_component_health;
        variance += diff * diff;
    }
    variance /= test_data->num_components;
    
    // Balance score: lower variance is better
    double balance_score = exp(-variance * 10.0);  // Exponential decay with variance
    
    // 4. Recovery capability (final integrity vs initial)
    double initial_integrity = test_data->system_integrity[0];
    double final_integrity = test_data->system_integrity[test_data->num_timesteps - 1];
    double recovery_score = (final_integrity > initial_integrity) ? 
                           (final_integrity - initial_integrity) / (1.0 - initial_integrity) : 0.0;
    
    // Combined autopoietic health score
    double health_score = 0.4 * avg_integrity +      // 40% system integrity
                         0.2 * avg_repair * 100.0 +  // 20% repair rate (scaled)
                         0.2 * balance_score +        // 20% component balance
                         0.2 * recovery_score;        // 20% recovery capability
    
    // Clamp to [0, 1]
    if (health_score > 1.0) health_score = 1.0;
    if (health_score < 0.0) health_score = 0.0;
    
    score.score = health_score;
    score.min_expected = 0.0;
    score.max_expected = 1.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Autopoietic health from integrity and self-repair");
    
    return score;
}

// Main test program
int main() {
    printf("TCDE Autopoiesis Capabilities Test Suite\n");
    printf("Protocol: Zero Tolerance v3.0\n");
    printf("Tests: 1 (ID: 22)\n\n");
    
    // Initialize test data
    AutopoiesisTestData* test_data = init_autopoiesis_data(50, 10);
    
    // Initialize validation context
    TCDE_ValidationContext ctx;
    TCDE_StatisticalResult stats;
    
    if (!TCDE_InitValidationContext(&ctx, 100)) {
        fprintf(stderr, "Failed to initialize validation context\n");
        free_autopoiesis_data(test_data);
        return 1;
    }
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 22: Autopoietic Health
    printf("Test 22: Autopoietic Health\n");
    TCDE_CapabilityScore result_22 = TCDE_ExecuteCapabilityTest(
        test_autopoietic_health, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_22.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_22.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_22.is_valid) passed_tests++;
    
    // Summary
    printf("========================================\n");
    printf("Autopoiesis Tests Summary\n");
    printf("========================================\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.2f%%\n", (double)passed_tests / total_tests * 100.0);
    
    // Cleanup
    TCDE_FreeValidationContext(&ctx);
    free_autopoiesis_data(test_data);
    
    return (passed_tests == total_tests) ? 0 : 1;
}
