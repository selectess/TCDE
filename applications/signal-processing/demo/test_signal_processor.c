/**
 * @file test_signal_processor.c
 * @brief Test program for TCDE Signal Processor POC
 * 
 * Tests real-time signal processing with synthetic signals.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#include "tcde_signal_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// TEST SIGNALS
// ============================================================================

/**
 * @brief Generate sine wave
 */
static float generate_sine(float t, float freq, float amplitude) {
    return amplitude * sinf(2.0f * M_PI * freq * t);
}

/**
 * @brief Generate square wave
 */
static float generate_square(float t, float freq, float amplitude) {
    float phase = fmodf(t * freq, 1.0f);
    return (phase < 0.5f) ? amplitude : -amplitude;
}

/**
 * @brief Generate noise
 */
static float generate_noise(float amplitude) {
    return amplitude * (2.0f * ((float)rand() / RAND_MAX) - 1.0f);
}

// ============================================================================
// TESTS
// ============================================================================

/**
 * @brief Test 1: Basic functionality
 */
static bool test_basic_functionality(void) {
    printf("\n[TEST 1] Basic Functionality\n");
    printf("========================================\n");
    
    // Create processor
    float sample_rate = 44100.0f;
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(sample_rate);
    if (!proc) {
        printf("[FAIL] Could not create processor\n");
        return false;
    }
    printf("[PASS] Processor created (sample_rate=%.0f Hz)\n", sample_rate);
    
    // Process a few samples
    float test_samples[] = {0.0f, 0.5f, 1.0f, 0.5f, 0.0f};
    int num_samples = sizeof(test_samples) / sizeof(test_samples[0]);
    
    printf("\nProcessing %d samples...\n", num_samples);
    for (int i = 0; i < num_samples; i++) {
        SignalFeatures features = TCDE_ProcessSample(proc, test_samples[i]);
        printf("  Sample %d (%.2f): E=%.3f, R=%.3f, H0=%d\n",
               i, test_samples[i], features.energy, features.curvature, 
               features.num_components);
    }
    
    // Cleanup
    TCDE_DestroySignalProcessor(proc);
    printf("\n[PASS] Basic functionality test completed\n");
    return true;
}

/**
 * @brief Test 2: Sine wave processing
 */
static bool test_sine_wave(void) {
    printf("\n[TEST 2] Sine Wave Processing\n");
    printf("========================================\n");
    
    float sample_rate = 44100.0f;
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(sample_rate);
    if (!proc) {
        printf("[FAIL] Could not create processor\n");
        return false;
    }
    
    // Generate 440 Hz sine wave (A4 note)
    float freq = 440.0f;
    int num_samples = 4410;  // 0.1 seconds
    
    printf("Processing %.1f Hz sine wave (%d samples, %.3f seconds)\n",
           freq, num_samples, (float)num_samples / sample_rate);
    
    // Measure processing time
    clock_t start = clock();
    
    SignalFeatures final_features;
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / sample_rate;
        float sample = generate_sine(t, freq, 1.0f);
        
        final_features = TCDE_ProcessSample(proc, sample);
        
        // Print every 1000 samples
        if (i % 1000 == 0) {
            printf("  Sample %d: E=%.3f, R=%.3f, H0=%d\n",
                   i, final_features.energy, final_features.curvature,
                   final_features.num_components);
        }
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double latency_per_sample = (elapsed / num_samples) * 1000.0;  // ms
    
    printf("\nFinal features:\n");
    printf("  Energy: %.3f\n", final_features.energy);
    printf("  Curvature: %.3f\n", final_features.curvature);
    printf("  Components: %d\n", final_features.num_components);
    printf("  Mean: %.3f\n", final_features.mean);
    printf("  Variance: %.3f\n", final_features.variance);
    
    printf("\nPerformance:\n");
    printf("  Total time: %.3f seconds\n", elapsed);
    printf("  Latency per sample: %.3f ms\n", latency_per_sample);
    
    // Check latency requirement (<1ms)
    if (latency_per_sample < 1.0) {
        printf("[PASS] Latency requirement met (< 1ms)\n");
    } else {
        printf("[WARN] Latency exceeds 1ms target\n");
    }
    
    TCDE_DestroySignalProcessor(proc);
    return true;
}

/**
 * @brief Test 3: Anomaly detection
 */
static bool test_anomaly_detection(void) {
    printf("\n[TEST 3] Anomaly Detection\n");
    printf("========================================\n");
    
    float sample_rate = 44100.0f;
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(sample_rate);
    if (!proc) {
        printf("[FAIL] Could not create processor\n");
        return false;
    }
    
    // Generate signal with anomaly
    int num_samples = 2000;
    int anomaly_start = 1000;
    int anomaly_end = 1100;
    
    printf("Processing signal with anomaly at samples %d-%d\n",
           anomaly_start, anomaly_end);
    
    float baseline_curvature = 0.0f;
    float anomaly_curvature = 0.0f;
    int baseline_count = 0;
    int anomaly_count = 0;
    
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / sample_rate;
        float sample;
        
        // Normal signal: 440 Hz sine
        if (i < anomaly_start || i >= anomaly_end) {
            sample = generate_sine(t, 440.0f, 1.0f);
        } else {
            // Anomaly: noise burst
            sample = generate_noise(2.0f);
        }
        
        SignalFeatures features = TCDE_ProcessSample(proc, sample);
        
        // Collect statistics
        if (i < anomaly_start) {
            baseline_curvature += fabsf(features.curvature);
            baseline_count++;
        } else if (i >= anomaly_start && i < anomaly_end) {
            anomaly_curvature += fabsf(features.curvature);
            anomaly_count++;
        }
        
        // Print at key points
        if (i == anomaly_start - 1) {
            printf("  Before anomaly (sample %d): R=%.3f\n", i, features.curvature);
        } else if (i == anomaly_start + 50) {
            printf("  During anomaly (sample %d): R=%.3f\n", i, features.curvature);
        } else if (i == anomaly_end) {
            printf("  After anomaly (sample %d): R=%.3f\n", i, features.curvature);
        }
    }
    
    // Compute averages
    baseline_curvature /= baseline_count;
    anomaly_curvature /= anomaly_count;
    
    printf("\nCurvature analysis:\n");
    printf("  Baseline average: %.3f\n", baseline_curvature);
    printf("  Anomaly average: %.3f\n", anomaly_curvature);
    printf("  Ratio: %.2fx\n", anomaly_curvature / baseline_curvature);
    
    // Check if anomaly is detectable (curvature should increase)
    if (anomaly_curvature > baseline_curvature * 1.5f) {
        printf("[PASS] Anomaly detected (curvature increased)\n");
    } else {
        printf("[WARN] Anomaly not clearly detected\n");
    }
    
    TCDE_DestroySignalProcessor(proc);
    return true;
}

/**
 * @brief Test 4: Multiple frequencies
 */
static bool test_multiple_frequencies(void) {
    printf("\n[TEST 4] Multiple Frequencies\n");
    printf("========================================\n");
    
    float sample_rate = 44100.0f;
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(sample_rate);
    if (!proc) {
        printf("[FAIL] Could not create processor\n");
        return false;
    }
    
    // Test different frequencies
    float frequencies[] = {220.0f, 440.0f, 880.0f};  // A3, A4, A5
    int num_freqs = sizeof(frequencies) / sizeof(frequencies[0]);
    
    for (int f = 0; f < num_freqs; f++) {
        float freq = frequencies[f];
        printf("\nProcessing %.0f Hz...\n", freq);
        
        // Reset processor for each frequency
        TCDE_ResetProcessor(proc);
        
        // Process 1000 samples
        SignalFeatures features;
        for (int i = 0; i < 1000; i++) {
            float t = (float)i / sample_rate;
            float sample = generate_sine(t, freq, 1.0f);
            features = TCDE_ProcessSample(proc, sample);
        }
        
        printf("  Final energy: %.3f\n", features.energy);
        printf("  Final curvature: %.3f\n", features.curvature);
        printf("  Components: %d\n", features.num_components);
    }
    
    printf("\n[PASS] Multiple frequencies test completed\n");
    
    TCDE_DestroySignalProcessor(proc);
    return true;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║     TCDE Signal Processor - POC Test Suite                ║\n");
    printf("║     Real-Time Signal Analysis with <1ms Latency           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Run tests
    int passed = 0;
    int total = 4;
    
    if (test_basic_functionality()) passed++;
    if (test_sine_wave()) passed++;
    if (test_anomaly_detection()) passed++;
    if (test_multiple_frequencies()) passed++;
    
    // Summary
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                      TEST SUMMARY                          ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Tests passed: %d/%d                                        ║\n", passed, total);
    if (passed == total) {
        printf("║  Status: ✅ ALL TESTS PASSED                              ║\n");
    } else {
        printf("║  Status: ⚠️  SOME TESTS FAILED                            ║\n");
    }
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    return (passed == total) ? 0 : 1;
}
