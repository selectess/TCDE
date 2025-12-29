/**
 * @file test_pattern_recognizer.c
 * @brief Test program for TCDE Pattern Recognizer POC
 * 
 * Tests topological pattern recognition on synthetic patterns.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#include "tcde_pattern_recognizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// SYNTHETIC PATTERN GENERATORS
// ============================================================================

/**
 * @brief Generate horizontal line pattern
 */
static void generate_horizontal_line(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    int y = height / 2;
    for (int x = 0; x < width; x++) {
        data[y * width + x] = 1.0f;
    }
}

/**
 * @brief Generate vertical line pattern
 */
static void generate_vertical_line(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    int x = width / 2;
    for (int y = 0; y < height; y++) {
        data[y * width + x] = 1.0f;
    }
}

/**
 * @brief Generate diagonal line pattern
 */
static void generate_diagonal_line(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    for (int i = 0; i < (width < height ? width : height); i++) {
        data[i * width + i] = 1.0f;
    }
}

/**
 * @brief Generate circle pattern
 */
static void generate_circle(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    int cx = width / 2;
    int cy = height / 2;
    int radius = (width < height ? width : height) / 3;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - cx;
            int dy = y - cy;
            float dist = sqrtf(dx * dx + dy * dy);
            
            if (fabsf(dist - radius) < 1.5f) {
                data[y * width + x] = 1.0f;
            }
        }
    }
}

/**
 * @brief Generate square pattern
 */
static void generate_square(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    int size = (width < height ? width : height) / 2;
    int x0 = (width - size) / 2;
    int y0 = (height - size) / 2;
    
    for (int y = y0; y < y0 + size; y++) {
        for (int x = x0; x < x0 + size; x++) {
            if (x == x0 || x == x0 + size - 1 || 
                y == y0 || y == y0 + size - 1) {
                data[y * width + x] = 1.0f;
            }
        }
    }
}

/**
 * @brief Generate cross pattern
 */
static void generate_cross(float* data, int width, int height) {
    memset(data, 0, width * height * sizeof(float));
    
    // Horizontal line
    int y = height / 2;
    for (int x = 0; x < width; x++) {
        data[y * width + x] = 1.0f;
    }
    
    // Vertical line
    int x = width / 2;
    for (int y = 0; y < height; y++) {
        data[y * width + x] = 1.0f;
    }
}

/**
 * @brief Add noise to pattern
 */
static void add_noise(float* data, int size, float noise_level) {
    for (int i = 0; i < size; i++) {
        float noise = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * noise_level;
        data[i] += noise;
        
        // Clamp to [0,1]
        if (data[i] < 0.0f) data[i] = 0.0f;
        if (data[i] > 1.0f) data[i] = 1.0f;
    }
}

// ============================================================================
// TESTS
// ============================================================================

/**
 * @brief Test 1: Basic pattern learning and recognition
 */
static bool test_basic_recognition(void) {
    printf("\\n[TEST 1] Basic Pattern Recognition\\n");
    printf("========================================\\n");
    
    int width = 28;
    int height = 28;
    int size = width * height;
    
    // Create recognizer
    TCDEPatternRecognizer* recognizer = TCDE_CreateRecognizer(width, height, 10);
    if (!recognizer) {
        printf("[FAIL] Could not create recognizer\\n");
        return false;
    }
    
    float* pattern_data = (float*)malloc(size * sizeof(float));
    
    // Learn patterns (5 classes)
    printf("Learning patterns...\\n");
    
    // Class 0: Horizontal lines (10 examples)
    for (int i = 0; i < 10; i++) {
        generate_horizontal_line(pattern_data, width, height);
        add_noise(pattern_data, size, 0.1f);
        TCDE_LearnPattern(recognizer, width, height, pattern_data, 0);
    }
    printf("  Class 0 (Horizontal): 10 patterns learned\\n");
    
    // Class 1: Vertical lines (10 examples)
    for (int i = 0; i < 10; i++) {
        generate_vertical_line(pattern_data, width, height);
        add_noise(pattern_data, size, 0.1f);
        TCDE_LearnPattern(recognizer, width, height, pattern_data, 1);
    }
    printf("  Class 1 (Vertical): 10 patterns learned\\n");
    
    // Class 2: Circles (10 examples)
    for (int i = 0; i < 10; i++) {
        generate_circle(pattern_data, width, height);
        add_noise(pattern_data, size, 0.1f);
        TCDE_LearnPattern(recognizer, width, height, pattern_data, 2);
    }
    printf("  Class 2 (Circle): 10 patterns learned\\n");
    
    // Class 3: Squares (10 examples)
    for (int i = 0; i < 10; i++) {
        generate_square(pattern_data, width, height);
        add_noise(pattern_data, size, 0.1f);
        TCDE_LearnPattern(recognizer, width, height, pattern_data, 3);
    }
    printf("  Class 3 (Square): 10 patterns learned\\n");
    
    // Class 4: Crosses (10 examples)
    for (int i = 0; i < 10; i++) {
        generate_cross(pattern_data, width, height);
        add_noise(pattern_data, size, 0.1f);
        TCDE_LearnPattern(recognizer, width, height, pattern_data, 4);
    }
    printf("  Class 4 (Cross): 10 patterns learned\\n");
    
    printf("\\nTotal patterns learned: %d\\n", recognizer->database->count);
    
    // Test recognition
    printf("\\nTesting recognition...\\n");
    
    int correct = 0;
    int total = 0;
    float total_time = 0.0f;
    
    const char* class_names[] = {"Horizontal", "Vertical", "Circle", "Square", "Cross"};
    
    // Test each class (5 test samples per class)
    for (int class = 0; class < 5; class++) {
        printf("\\n  Testing class %d (%s):\\n", class, class_names[class]);
        
        for (int i = 0; i < 5; i++) {
            // Generate test pattern
            switch (class) {
                case 0: generate_horizontal_line(pattern_data, width, height); break;
                case 1: generate_vertical_line(pattern_data, width, height); break;
                case 2: generate_circle(pattern_data, width, height); break;
                case 3: generate_square(pattern_data, width, height); break;
                case 4: generate_cross(pattern_data, width, height); break;
            }
            add_noise(pattern_data, size, 0.15f);  // More noise for testing
            
            // Recognize
            RecognitionResult result = TCDE_RecognizePattern(recognizer, width, height, pattern_data);
            
            total++;
            if (result.predicted_label == class) {
                correct++;
                printf("    Sample %d: ✓ Correct (confidence: %.2f, time: %.3fms)\\n", 
                       i + 1, result.confidence, result.recognition_time_ms);
            } else {
                printf("    Sample %d: ✗ Wrong (predicted: %s, confidence: %.2f)\\n", 
                       i + 1, class_names[result.predicted_label], result.confidence);
            }
            
            total_time += result.recognition_time_ms;
            
            if (result.class_scores) {
                free(result.class_scores);
            }
        }
    }
    
    float accuracy = (float)correct / total;
    float avg_time = total_time / total;
    
    printf("\\n--- Results ---\\n");
    printf("  Accuracy: %d/%d (%.1f%%)\\n", correct, total, accuracy * 100.0f);
    printf("  Average recognition time: %.3f ms\\n", avg_time);
    
    // Check success criteria
    bool success = (accuracy >= 0.80f);  // 80% accuracy threshold
    
    if (success) {
        printf("  [PASS] Accuracy >= 80%%\\n");
    } else {
        printf("  [FAIL] Accuracy < 80%%\\n");
    }
    
    // Cleanup
    free(pattern_data);
    TCDE_DestroyRecognizer(recognizer);
    
    printf("\\n[%s] Basic recognition test completed\\n", success ? "PASS" : "FAIL");
    return success;
}

/**
 * @brief Test 2: Few-shot learning
 */
static bool test_few_shot_learning(void) {
    printf("\\n[TEST 2] Few-Shot Learning\\n");
    printf("========================================\\n");
    
    int width = 28;
    int height = 28;
    int size = width * height;
    
    TCDEPatternRecognizer* recognizer = TCDE_CreateRecognizer(width, height, 10);
    if (!recognizer) {
        printf("[FAIL] Could not create recognizer\\n");
        return false;
    }
    
    float* pattern_data = (float*)malloc(size * sizeof(float));
    
    // Learn with very few examples (3 per class)
    printf("Learning with only 3 examples per class...\\n");
    
    for (int class = 0; class < 3; class++) {
        for (int i = 0; i < 3; i++) {
            switch (class) {
                case 0: generate_horizontal_line(pattern_data, width, height); break;
                case 1: generate_diagonal_line(pattern_data, width, height); break;
                case 2: generate_circle(pattern_data, width, height); break;
            }
            add_noise(pattern_data, size, 0.1f);
            TCDE_LearnPattern(recognizer, width, height, pattern_data, class);
        }
    }
    
    printf("  Total patterns learned: %d (3 per class)\\n", recognizer->database->count);
    
    // Test recognition
    printf("\\nTesting with 10 samples per class...\\n");
    
    int correct = 0;
    int total = 0;
    
    for (int class = 0; class < 3; class++) {
        for (int i = 0; i < 10; i++) {
            switch (class) {
                case 0: generate_horizontal_line(pattern_data, width, height); break;
                case 1: generate_diagonal_line(pattern_data, width, height); break;
                case 2: generate_circle(pattern_data, width, height); break;
            }
            add_noise(pattern_data, size, 0.15f);
            
            RecognitionResult result = TCDE_RecognizePattern(recognizer, width, height, pattern_data);
            
            total++;
            if (result.predicted_label == class) {
                correct++;
            }
            
            if (result.class_scores) {
                free(result.class_scores);
            }
        }
    }
    
    float accuracy = (float)correct / total;
    
    printf("\\n--- Results ---\\n");
    printf("  Accuracy: %d/%d (%.1f%%)\\n", correct, total, accuracy * 100.0f);
    printf("  Training samples: 9 (3 per class)\\n");
    printf("  Test samples: 30 (10 per class)\\n");
    
    bool success = (accuracy >= 0.70f);  // 70% with only 3 examples
    
    if (success) {
        printf("  [GOOD] Few-shot learning successful (>70%% with 3 examples)\\n");
    } else {
        printf("  [WARN] Few-shot learning needs improvement\\n");
    }
    
    free(pattern_data);
    TCDE_DestroyRecognizer(recognizer);
    
    printf("\\n[%s] Few-shot learning test completed\\n", success ? "PASS" : "FAIL");
    return success;
}

/**
 * @brief Test 3: Noise robustness
 */
static bool test_noise_robustness(void) {
    printf("\\n[TEST 3] Noise Robustness\\n");
    printf("========================================\\n");
    
    int width = 28;
    int height = 28;
    int size = width * height;
    
    TCDEPatternRecognizer* recognizer = TCDE_CreateRecognizer(width, height, 10);
    if (!recognizer) {
        printf("[FAIL] Could not create recognizer\\n");
        return false;
    }
    
    float* pattern_data = (float*)malloc(size * sizeof(float));
    
    // Learn clean patterns
    printf("Learning clean patterns...\\n");
    
    for (int class = 0; class < 3; class++) {
        for (int i = 0; i < 10; i++) {
            switch (class) {
                case 0: generate_horizontal_line(pattern_data, width, height); break;
                case 1: generate_vertical_line(pattern_data, width, height); break;
                case 2: generate_circle(pattern_data, width, height); break;
            }
            add_noise(pattern_data, size, 0.05f);  // Very little noise
            TCDE_LearnPattern(recognizer, width, height, pattern_data, class);
        }
    }
    
    // Test with increasing noise levels
    float noise_levels[] = {0.1f, 0.2f, 0.3f, 0.4f};
    int num_noise_levels = sizeof(noise_levels) / sizeof(noise_levels[0]);
    
    printf("\\nTesting with increasing noise levels...\\n");
    
    for (int n = 0; n < num_noise_levels; n++) {
        float noise = noise_levels[n];
        
        int correct = 0;
        int total = 0;
        
        for (int class = 0; class < 3; class++) {
            for (int i = 0; i < 10; i++) {
                switch (class) {
                    case 0: generate_horizontal_line(pattern_data, width, height); break;
                    case 1: generate_vertical_line(pattern_data, width, height); break;
                    case 2: generate_circle(pattern_data, width, height); break;
                }
                add_noise(pattern_data, size, noise);
                
                RecognitionResult result = TCDE_RecognizePattern(recognizer, width, height, pattern_data);
                
                total++;
                if (result.predicted_label == class) {
                    correct++;
                }
                
                if (result.class_scores) {
                    free(result.class_scores);
                }
            }
        }
        
        float accuracy = (float)correct / total;
        printf("  Noise level %.1f: Accuracy %.1f%% (%d/%d)\\n", 
               noise, accuracy * 100.0f, correct, total);
    }
    
    printf("\\n[PASS] Noise robustness test completed\\n");
    
    free(pattern_data);
    TCDE_DestroyRecognizer(recognizer);
    
    return true;
}

/**
 * @brief Test 4: Performance benchmark
 */
static bool test_performance(void) {
    printf("\\n[TEST 4] Performance Benchmark\\n");
    printf("========================================\\n");
    
    int width = 28;
    int height = 28;
    int size = width * height;
    
    TCDEPatternRecognizer* recognizer = TCDE_CreateRecognizer(width, height, 10);
    if (!recognizer) {
        printf("[FAIL] Could not create recognizer\\n");
        return false;
    }
    
    float* pattern_data = (float*)malloc(size * sizeof(float));
    
    // Learn patterns
    printf("Learning 100 patterns...\\n");
    
    clock_t learn_start = clock();
    
    for (int class = 0; class < 5; class++) {
        for (int i = 0; i < 20; i++) {
            switch (class % 5) {
                case 0: generate_horizontal_line(pattern_data, width, height); break;
                case 1: generate_vertical_line(pattern_data, width, height); break;
                case 2: generate_circle(pattern_data, width, height); break;
                case 3: generate_square(pattern_data, width, height); break;
                case 4: generate_cross(pattern_data, width, height); break;
            }
            add_noise(pattern_data, size, 0.1f);
            TCDE_LearnPattern(recognizer, width, height, pattern_data, class);
        }
    }
    
    clock_t learn_end = clock();
    float learn_time = (double)(learn_end - learn_start) / CLOCKS_PER_SEC;
    
    printf("  Learning time: %.3f seconds (%.2f ms per pattern)\\n", 
           learn_time, learn_time * 1000.0f / 100);
    
    // Benchmark recognition
    printf("\\nBenchmarking recognition (100 samples)...\\n");
    
    float total_time = 0.0f;
    int num_samples = 100;
    
    for (int i = 0; i < num_samples; i++) {
        int class = i % 5;
        
        switch (class) {
            case 0: generate_horizontal_line(pattern_data, width, height); break;
            case 1: generate_vertical_line(pattern_data, width, height); break;
            case 2: generate_circle(pattern_data, width, height); break;
            case 3: generate_square(pattern_data, width, height); break;
            case 4: generate_cross(pattern_data, width, height); break;
        }
        add_noise(pattern_data, size, 0.15f);
        
        RecognitionResult result = TCDE_RecognizePattern(recognizer, width, height, pattern_data);
        total_time += result.recognition_time_ms;
        
        if (result.class_scores) {
            free(result.class_scores);
        }
    }
    
    float avg_time = total_time / num_samples;
    
    printf("  Average recognition time: %.3f ms\\n", avg_time);
    printf("  Throughput: %.0f patterns/second\\n", 1000.0f / avg_time);
    
    bool success = (avg_time < 10.0f);  // Should be < 10ms per pattern
    
    if (success) {
        printf("  [GOOD] Fast recognition (< 10ms per pattern)\\n");
    } else {
        printf("  [WARN] Recognition slower than expected\\n");
    }
    
    free(pattern_data);
    TCDE_DestroyRecognizer(recognizer);
    
    printf("\\n[%s] Performance benchmark completed\\n", success ? "PASS" : "FAIL");
    return success;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    
    printf("╔════════════════════════════════════════════════════════════╗\\n");
    printf("║     TCDE Pattern Recognizer - POC Test Suite              ║\\n");
    printf("║     Topological Pattern Recognition                        ║\\n");
    printf("╚════════════════════════════════════════════════════════════╝\\n");
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Run tests
    int passed = 0;
    int total = 4;
    
    if (test_basic_recognition()) passed++;
    if (test_few_shot_learning()) passed++;
    if (test_noise_robustness()) passed++;
    if (test_performance()) passed++;
    
    // Summary
    printf("\\n╔════════════════════════════════════════════════════════════╗\\n");
    printf("║                      TEST SUMMARY                          ║\\n");
    printf("╠════════════════════════════════════════════════════════════╣\\n");
    printf("║  Tests passed: %d/%d                                        ║\\n", passed, total);
    if (passed == total) {
        printf("║  Status: ✅ ALL TESTS PASSED                              ║\\n");
    } else {
        printf("║  Status: ⚠️  SOME TESTS FAILED                            ║\\n");
    }
    printf("╚════════════════════════════════════════════════════════════╝\\n");
    
    return (passed == total) ? 0 : 1;
}
