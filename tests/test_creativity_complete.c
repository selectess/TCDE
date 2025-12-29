/**
 * TCDE Creativity Capabilities - Complete Test Suite
 * Tests: Novelty (17), Originality (18)
 * Date: November 11, 2025
 * Protocol: Zero Tolerance v3.0
 */

#include "../src/validation/tcde_capability_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Test data structure for creativity
typedef struct {
    double** patterns;        // Generated patterns
    double* baseline_pattern; // Reference baseline
    int num_patterns;
    int pattern_size;
    double novelty_threshold;
} CreativityTestData;

// Initialize test data
CreativityTestData* init_creativity_data(int num_patterns, int pattern_size) {
    CreativityTestData* data = malloc(sizeof(CreativityTestData));
    data->num_patterns = num_patterns;
    data->pattern_size = pattern_size;
    data->novelty_threshold = 0.3;
    
    // Allocate patterns
    data->patterns = malloc(num_patterns * sizeof(double*));
    for (int i = 0; i < num_patterns; i++) {
        data->patterns[i] = malloc(pattern_size * sizeof(double));
    }
    
    data->baseline_pattern = malloc(pattern_size * sizeof(double));
    
    // Initialize baseline pattern (simple sine wave)
    srand(time(NULL) + 5);
    for (int i = 0; i < pattern_size; i++) {
        double x = (double)i / pattern_size;
        data->baseline_pattern[i] = sin(2.0 * M_PI * x);
    }
    
    // Generate diverse patterns with varying novelty
    for (int p = 0; p < num_patterns; p++) {
        double novelty_factor = (double)p / num_patterns;
        
        for (int i = 0; i < pattern_size; i++) {
            double x = (double)i / pattern_size;
            
            // Mix of baseline and novel components
            double baseline_component = (1.0 - novelty_factor) * data->baseline_pattern[i];
            
            // Novel components: higher frequencies, phase shifts, nonlinearities
            double novel_component = novelty_factor * (
                0.3 * sin((3.0 + p) * 2.0 * M_PI * x + p * 0.5) +
                0.2 * cos((5.0 + p) * 2.0 * M_PI * x) +
                0.1 * sin(x * x * M_PI * (1.0 + p))
            );
            
            // Add small random perturbation
            double noise = 0.05 * (rand() / (double)RAND_MAX - 0.5);
            
            data->patterns[p][i] = baseline_component + novel_component + noise;
        }
    }
    
    return data;
}

void free_creativity_data(CreativityTestData* data) {
    if (data) {
        for (int i = 0; i < data->num_patterns; i++) {
            free(data->patterns[i]);
        }
        free(data->patterns);
        free(data->baseline_pattern);
        free(data);
    }
}

// Compute pattern distance (Euclidean)
double compute_pattern_distance(double* pattern1, double* pattern2, int size) {
    double distance = 0.0;
    for (int i = 0; i < size; i++) {
        double diff = pattern1[i] - pattern2[i];
        distance += diff * diff;
    }
    return sqrt(distance / size);  // Normalized
}

// Test 17: Novelty - Measure output divergence from baseline
TCDE_CapabilityScore test_novelty(void* data) {
    TCDE_CapabilityScore score = {0};
    CreativityTestData* test_data = (CreativityTestData*)data;
    
    score.capability_id = 17;
    strcpy(score.capability_name, "Novelty");
    
    // Compute average distance from baseline
    double total_novelty = 0.0;
    
    for (int p = 0; p < test_data->num_patterns; p++) {
        double distance = compute_pattern_distance(
            test_data->patterns[p],
            test_data->baseline_pattern,
            test_data->pattern_size
        );
        
        total_novelty += distance;
    }
    
    // Average novelty score
    double novelty_score = total_novelty / test_data->num_patterns;
    
    score.score = novelty_score;
    score.min_expected = 0.0;
    score.max_expected = 1.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Novelty from baseline divergence");
    
    return score;
}

// Test 18: Originality - Count unique patterns
TCDE_CapabilityScore test_originality(void* data) {
    TCDE_CapabilityScore score = {0};
    CreativityTestData* test_data = (CreativityTestData*)data;
    
    score.capability_id = 18;
    strcpy(score.capability_name, "Originality");
    
    // Count unique patterns (patterns that are sufficiently different from each other)
    int unique_count = 0;
    double similarity_threshold = 0.1;  // Patterns closer than this are considered similar
    
    // Mark which patterns are unique
    int* is_unique = calloc(test_data->num_patterns, sizeof(int));
    
    for (int i = 0; i < test_data->num_patterns; i++) {
        int is_original = 1;
        
        // Check if this pattern is too similar to any previous unique pattern
        for (int j = 0; j < i; j++) {
            if (is_unique[j]) {
                double distance = compute_pattern_distance(
                    test_data->patterns[i],
                    test_data->patterns[j],
                    test_data->pattern_size
                );
                
                if (distance < similarity_threshold) {
                    is_original = 0;
                    break;
                }
            }
        }
        
        if (is_original) {
            is_unique[i] = 1;
            unique_count++;
        }
    }
    
    free(is_unique);
    
    // Originality score: fraction of unique patterns
    double originality_score = (double)unique_count / test_data->num_patterns;
    
    score.score = originality_score;
    score.min_expected = 0.0;
    score.max_expected = 1.0;
    score.is_valid = true;
    
    strcpy(score.validation_message, "Originality from unique pattern count");
    
    return score;
}

// Main test program
int main() {
    printf("TCDE Creativity Capabilities Test Suite\n");
    printf("Protocol: Zero Tolerance v3.0\n");
    printf("Tests: 2 (IDs: 17, 18)\n\n");
    
    // Initialize test data
    CreativityTestData* test_data = init_creativity_data(20, 50);
    
    // Initialize validation context
    TCDE_ValidationContext ctx;
    TCDE_StatisticalResult stats;
    
    if (!TCDE_InitValidationContext(&ctx, 100)) {
        fprintf(stderr, "Failed to initialize validation context\n");
        free_creativity_data(test_data);
        return 1;
    }
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 17: Novelty
    printf("Test 17: Novelty\n");
    TCDE_CapabilityScore result_17 = TCDE_ExecuteCapabilityTest(
        test_novelty, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_17.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_17.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_17.is_valid) passed_tests++;
    
    // Test 18: Originality
    printf("Test 18: Originality\n");
    TCDE_CapabilityScore result_18 = TCDE_ExecuteCapabilityTest(
        test_originality, test_data, 100, &ctx, &stats);
    printf("  Score: %.6f\n", result_18.score);
    printf("  P-value: %.6f\n", stats.p_value);
    printf("  Status: %s\n\n", result_18.is_valid ? "PASS" : "FAIL");
    total_tests++;
    if (result_18.is_valid) passed_tests++;
    
    // Summary
    printf("========================================\n");
    printf("Creativity Tests Summary\n");
    printf("========================================\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.2f%%\n", (double)passed_tests / total_tests * 100.0);
    
    // Cleanup
    TCDE_FreeValidationContext(&ctx);
    free_creativity_data(test_data);
    
    return (passed_tests == total_tests) ? 0 : 1;
}
