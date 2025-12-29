/**
 * @file test_limiters.c
 * @brief Test Adaptive Limiters
 */

#include "tcde_core.h"
#include "tcde_limiters.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define TEST(name, cond) do { \
    if (cond) { \
        printf("  ✓ %s\n", name); \
        tests_passed++; \
    } else { \
        printf("  ✗ %s\n", name); \
        tests_failed++; \
    } \
} while(0)

static int tests_passed = 0;
static int tests_failed = 0;

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Adaptive Limiters Test                                   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Test 1: Default Configuration
    printf("--- Test 1: Default Configuration ---\n");
    {
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        
        TEST("Config created", config.max_energy > 0.0f);
        TEST("Auto-adjust enabled", config.auto_adjust == true);
        TEST("Safety factor set", config.safety_factor > 0.0f && config.safety_factor <= 1.0f);
        TEST("Statistics initialized", config.num_energy_clips == 0);
    }
    
    // Test 2: Energy Limiter
    printf("\n--- Test 2: Energy Limiter ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        config.max_energy = 100.0f;
        
        // Add high-energy centers
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_Complex coeff = 20.0f * (1.0f + I);  // High amplitude
            TCDE_AddCenter6D(field, &p, coeff, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Compute initial energy
        float energy_before = 0.0f;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            float amp = cabsf(field->manifold_6d.centers[i].coeff);
            energy_before += amp * amp;
        }
        
        printf("  Energy before: %.2f\n", energy_before);
        TEST("Energy exceeds limit", energy_before > config.max_energy);
        
        // Apply limiter
        bool limited = TCDE_ApplyEnergyLimiter(field, &config);
        TEST("Limiter applied", limited);
        
        // Compute final energy
        float energy_after = 0.0f;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            float amp = cabsf(field->manifold_6d.centers[i].coeff);
            energy_after += amp * amp;
        }
        
        printf("  Energy after: %.2f\n", energy_after);
        TEST("Energy within limit", energy_after <= config.max_energy * 1.01f);
        TEST("Statistics updated", config.num_energy_clips == 1);
        
        TCDE_DestroyField(field);
    }
    
    // Test 3: Amplitude Limiter
    printf("\n--- Test 3: Amplitude Limiter ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        config.max_center_amplitude = 10.0f;
        
        // Add centers with varying amplitudes
        float amplitudes[] = {5.0f, 15.0f, 8.0f, 20.0f, 3.0f};
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_Complex coeff = amplitudes[i] * (1.0f + I);
            TCDE_AddCenter6D(field, &p, coeff, 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Apply limiter
        int num_clipped = TCDE_ApplyAmplitudeLimiter(field, &config);
        printf("  Centers clipped: %d\n", num_clipped);
        TEST("Some centers clipped", num_clipped >= 2);  // At least 15.0 and 20.0
        
        // Verify all amplitudes within limit
        bool all_within = true;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            float amp = cabsf(field->manifold_6d.centers[i].coeff);
            if (amp > config.max_center_amplitude * 1.01f) {
                all_within = false;
                break;
            }
        }
        TEST("All amplitudes within limit", all_within);
        
        TCDE_DestroyField(field);
    }
    
    // Test 4: Epsilon Limiter
    printf("\n--- Test 4: Epsilon Limiter ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        config.min_epsilon = 0.05f;
        config.max_epsilon = 5.0f;
        
        // Add centers with out-of-range epsilons
        float epsilons[] = {0.01f, 0.1f, 10.0f, 0.5f, 0.001f};
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 1.0f, epsilons[i]);
            TCDE_FreePoint(&p);
        }
        
        // Apply limiter
        int num_clipped = TCDE_ApplyEpsilonLimiter(field, &config);
        printf("  Epsilons clipped: %d\n", num_clipped);
        TEST("Some epsilons clipped", num_clipped == 3);  // 0.01, 10.0, 0.001
        
        // Verify all epsilons within range
        bool all_within = true;
        for (int i = 0; i < field->manifold_6d.num_centers; i++) {
            float eps = field->manifold_6d.centers[i].epsilon;
            if (eps < config.min_epsilon || eps > config.max_epsilon) {
                all_within = false;
                break;
            }
        }
        TEST("All epsilons within range", all_within);
        
        TCDE_DestroyField(field);
    }
    
    // Test 5: All Limiters Together
    printf("\n--- Test 5: All Limiters Together ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        config.max_energy = 100.0f;
        config.max_center_amplitude = 10.0f;
        config.min_epsilon = 0.05f;
        config.max_epsilon = 5.0f;
        
        // Add problematic centers
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_Complex coeff = 20.0f * (1.0f + I);  // Too large
            float epsilon = (i % 2 == 0) ? 0.01f : 10.0f;  // Out of range
            TCDE_AddCenter6D(field, &p, coeff, epsilon);
            TCDE_FreePoint(&p);
        }
        
        // Apply all limiters
        bool limited = TCDE_ApplyAllLimiters(field, &config);
        TEST("Limiters applied", limited);
        
        // Check if field is now within limits
        bool within_limits = TCDE_CheckLimits(field, &config);
        TEST("Field within limits", within_limits);
        
        TCDE_DestroyField(field);
    }
    
    // Test 6: Auto-Adjust
    printf("\n--- Test 6: Auto-Adjust ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        
        // Add normal centers
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 2.0f * (1.0f + I), 0.1f);
            TCDE_FreePoint(&p);
        }
        
        float max_energy_before = config.max_energy;
        
        // Auto-adjust based on field
        TCDE_AutoAdjustLimiters(field, &config);
        
        printf("  Max energy before: %.2f\n", max_energy_before);
        printf("  Max energy after: %.2f\n", config.max_energy);
        
        TEST("Limits adjusted", config.max_energy != max_energy_before);
        TEST("Limits reasonable", config.max_energy > 0.0f && config.max_energy < 1e6f);
        
        TCDE_DestroyField(field);
    }
    
    // Test 7: Evolution with Limiters
    printf("\n--- Test 7: Evolution with Limiters ---\n");
    {
        TCDE_Field* field = TCDE_CreateField(10, 2.5f);
        TCDE_LimiterConfig config = TCDE_CreateDefaultLimiters();
        config.max_energy = 500.0f;
        
        // Add centers
        for (int i = 0; i < 5; i++) {
            float coords[6] = {0.1f * i, 0.2f * i, 0.3f, 1.0f, 0.0f, 0.5f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_AddCenter6D(field, &p, 5.0f * (1.0f + I), 0.1f);
            TCDE_FreePoint(&p);
        }
        
        // Evolve with limiters
        TCDE_Parameters params;
        TCDE_ConfigureParameters(&params, field);
        params.dt = 0.01f;
        
        for (int step = 0; step < 100; step++) {
            TCDE_EvolveStep(field, &params);
            TCDE_ApplyAllLimiters(field, &config);
        }
        
        // Check final state
        bool within_limits = TCDE_CheckLimits(field, &config);
        TEST("Field stable after evolution", within_limits);
        
        printf("  Final limiter stats:\n");
        printf("    Energy clips: %d\n", config.num_energy_clips);
        printf("    Gradient clips: %d\n", config.num_gradient_clips);
        
        TCDE_DestroyField(field);
    }
    
    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  SUMMARY: %d/%d tests passed                              ║\n", 
           tests_passed, tests_passed + tests_failed);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("✅ Adaptive Limiters Test COMPLETE!\n\n");
        return 0;
    } else {
        printf("⚠️  Some tests need improvement\n\n");
        return 1;
    }
}
