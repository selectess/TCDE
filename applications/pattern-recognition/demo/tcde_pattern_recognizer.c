/**
 * @file tcde_pattern_recognizer.c
 * @brief TCDE Pattern Recognizer - Implementation
 * 
 * Topological pattern recognition using TCDE field dynamics.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#include "tcde_pattern_recognizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// ============================================================================
// INTERNAL HELPERS
// ============================================================================

/**
 * @brief Normalize pattern data to [0,1]
 */
static void normalize_pattern(float* data, int size) {
    float min_val = data[0];
    float max_val = data[0];
    
    for (int i = 1; i < size; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    float range = max_val - min_val;
    if (range > 1e-6f) {
        for (int i = 0; i < size; i++) {
            data[i] = (data[i] - min_val) / range;
        }
    }
}

/**
 * @brief Compute topological signature (simplified persistent homology)
 */
static void compute_topology(const float* data, int width, int height, 
                            float* topology, int topology_dim) {
    // Simplified topological features:
    // - Connected components (H0)
    // - Holes (H1)
    // - Voids (H2)
    // - Statistical moments
    
    int size = width * height;
    
    // Feature 0: Mean intensity
    float mean = 0.0f;
    for (int i = 0; i < size; i++) {
        mean += data[i];
    }
    mean /= size;
    topology[0] = mean;
    
    // Feature 1: Variance
    float variance = 0.0f;
    for (int i = 0; i < size; i++) {
        float diff = data[i] - mean;
        variance += diff * diff;
    }
    variance /= size;
    topology[1] = sqrtf(variance);
    
    // Feature 2: Skewness (asymmetry)
    float skewness = 0.0f;
    if (variance > 1e-6f) {
        for (int i = 0; i < size; i++) {
            float diff = (data[i] - mean) / sqrtf(variance);
            skewness += diff * diff * diff;
        }
        skewness /= size;
    }
    topology[2] = skewness;
    
    // Feature 3: Kurtosis (tailedness)
    float kurtosis = 0.0f;
    if (variance > 1e-6f) {
        for (int i = 0; i < size; i++) {
            float diff = (data[i] - mean) / sqrtf(variance);
            kurtosis += diff * diff * diff * diff;
        }
        kurtosis /= size;
        kurtosis -= 3.0f;  // Excess kurtosis
    }
    topology[3] = kurtosis;
    
    // Feature 4: Gradient magnitude (edge strength)
    float gradient_sum = 0.0f;
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int idx = y * width + x;
            float dx = data[idx + 1] - data[idx];
            float dy = data[idx + width] - data[idx];
            gradient_sum += sqrtf(dx * dx + dy * dy);
        }
    }
    topology[4] = gradient_sum / (width * height);
    
    // Feature 5: Laplacian (curvature)
    float laplacian_sum = 0.0f;
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            float center = data[idx];
            float neighbors = data[idx - 1] + data[idx + 1] + 
                            data[idx - width] + data[idx + width];
            laplacian_sum += fabsf(4.0f * center - neighbors);
        }
    }
    topology[5] = laplacian_sum / (width * height);
    
    // Features 6-9: Quadrant means (spatial distribution)
    int hw = width / 2;
    int hh = height / 2;
    
    float q1 = 0.0f, q2 = 0.0f, q3 = 0.0f, q4 = 0.0f;
    int count = 0;
    
    for (int y = 0; y < hh; y++) {
        for (int x = 0; x < hw; x++) {
            q1 += data[y * width + x];
            q2 += data[y * width + (x + hw)];
            q3 += data[(y + hh) * width + x];
            q4 += data[(y + hh) * width + (x + hw)];
            count++;
        }
    }
    
    if (count > 0) {
        topology[6] = q1 / count;
        topology[7] = q2 / count;
        topology[8] = q3 / count;
        topology[9] = q4 / count;
    }
    
    // Normalize topology features
    normalize_pattern(topology, topology_dim);
}

// ============================================================================
// PATTERN OPERATIONS
// ============================================================================

Pattern* TCDE_CreatePattern(int width, int height, const float* data, int label) {
    if (width <= 0 || height <= 0 || !data) {
        fprintf(stderr, "[ERROR] Invalid pattern parameters\\n");
        return NULL;
    }
    
    Pattern* pattern = (Pattern*)malloc(sizeof(Pattern));
    if (!pattern) {
        fprintf(stderr, "[ERROR] Failed to allocate pattern\\n");
        return NULL;
    }
    
    int size = width * height;
    
    pattern->width = width;
    pattern->height = height;
    pattern->label = label;
    pattern->topology_dim = 10;  // Fixed topology dimension
    
    // Allocate and copy data
    pattern->data = (float*)malloc(size * sizeof(float));
    if (!pattern->data) {
        fprintf(stderr, "[ERROR] Failed to allocate pattern data\\n");
        free(pattern);
        return NULL;
    }
    memcpy(pattern->data, data, size * sizeof(float));
    normalize_pattern(pattern->data, size);
    
    // Allocate topology
    pattern->topology = (float*)malloc(pattern->topology_dim * sizeof(float));
    if (!pattern->topology) {
        fprintf(stderr, "[ERROR] Failed to allocate topology\\n");
        free(pattern->data);
        free(pattern);
        return NULL;
    }
    
    // Create TCDE field
    pattern->field = TCDE_CreateField(50, 2.5f);
    if (!pattern->field) {
        fprintf(stderr, "[ERROR] Failed to create TCDE field\\n");
        free(pattern->topology);
        free(pattern->data);
        free(pattern);
        return NULL;
    }
    
    return pattern;
}

void TCDE_DestroyPattern(Pattern* pattern) {
    if (!pattern) return;
    
    if (pattern->field) {
        TCDE_DestroyField(pattern->field);
    }
    if (pattern->data) free(pattern->data);
    if (pattern->topology) free(pattern->topology);
    
    free(pattern);
}

bool TCDE_ExtractTopology(Pattern* pattern) {
    if (!pattern || !pattern->data || !pattern->topology) {
        return false;
    }
    
    compute_topology(pattern->data, pattern->width, pattern->height,
                    pattern->topology, pattern->topology_dim);
    
    // Add topology to TCDE field
    for (int i = 0; i < pattern->topology_dim && i < 6; i++) {
        float coords[6] = {0};
        coords[i] = pattern->topology[i];
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = pattern->topology[i] + I * 0.1f;
        
        TCDE_AddCenter6D(pattern->field, &point, coeff, 0.1f);
        TCDE_FreePoint(&point);
    }
    
    return true;
}

float TCDE_PatternSimilarity(const Pattern* p1, const Pattern* p2) {
    if (!p1 || !p2 || !p1->topology || !p2->topology) {
        return 0.0f;
    }
    
    // Compute Euclidean distance in topology space
    float distance = 0.0f;
    for (int i = 0; i < p1->topology_dim; i++) {
        float diff = p1->topology[i] - p2->topology[i];
        distance += diff * diff;
    }
    distance = sqrtf(distance);
    
    // Convert to similarity (0-1, higher = more similar)
    float similarity = expf(-distance);
    
    return similarity;
}

// ============================================================================
// DATABASE OPERATIONS
// ============================================================================

PatternDatabase* TCDE_CreateDatabase(int initial_capacity) {
    if (initial_capacity <= 0) {
        initial_capacity = 100;
    }
    
    PatternDatabase* db = (PatternDatabase*)malloc(sizeof(PatternDatabase));
    if (!db) {
        fprintf(stderr, "[ERROR] Failed to allocate database\\n");
        return NULL;
    }
    
    db->patterns = (Pattern**)malloc(initial_capacity * sizeof(Pattern*));
    if (!db->patterns) {
        fprintf(stderr, "[ERROR] Failed to allocate pattern array\\n");
        free(db);
        return NULL;
    }
    
    db->count = 0;
    db->capacity = initial_capacity;
    db->num_classes = 0;
    db->class_counts = NULL;
    
    return db;
}

void TCDE_DestroyDatabase(PatternDatabase* db) {
    if (!db) return;
    
    for (int i = 0; i < db->count; i++) {
        TCDE_DestroyPattern(db->patterns[i]);
    }
    
    if (db->patterns) free(db->patterns);
    if (db->class_counts) free(db->class_counts);
    
    free(db);
}

bool TCDE_AddPattern(PatternDatabase* db, const Pattern* pattern) {
    if (!db || !pattern) {
        return false;
    }
    
    // Resize if needed
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        Pattern** new_patterns = (Pattern**)realloc(db->patterns, 
                                                    new_capacity * sizeof(Pattern*));
        if (!new_patterns) {
            fprintf(stderr, "[ERROR] Failed to resize database\\n");
            return false;
        }
        db->patterns = new_patterns;
        db->capacity = new_capacity;
    }
    
    // Copy pattern
    Pattern* new_pattern = TCDE_CreatePattern(pattern->width, pattern->height,
                                              pattern->data, pattern->label);
    if (!new_pattern) {
        return false;
    }
    
    // Copy topology
    memcpy(new_pattern->topology, pattern->topology, 
           pattern->topology_dim * sizeof(float));
    
    db->patterns[db->count++] = new_pattern;
    
    // Update class statistics
    if (pattern->label >= db->num_classes) {
        int new_num_classes = pattern->label + 1;
        int* new_counts = (int*)realloc(db->class_counts, 
                                       new_num_classes * sizeof(int));
        if (!new_counts) {
            return false;
        }
        
        // Initialize new class counts to 0
        for (int i = db->num_classes; i < new_num_classes; i++) {
            new_counts[i] = 0;
        }
        
        db->class_counts = new_counts;
        db->num_classes = new_num_classes;
    }
    
    db->class_counts[pattern->label]++;
    
    return true;
}

Pattern** TCDE_GetPatternsByClass(PatternDatabase* db, int label, int* count) {
    if (!db || label < 0 || label >= db->num_classes || !count) {
        if (count) *count = 0;
        return NULL;
    }
    
    *count = db->class_counts[label];
    
    if (*count == 0) {
        return NULL;
    }
    
    Pattern** result = (Pattern**)malloc(*count * sizeof(Pattern*));
    if (!result) {
        *count = 0;
        return NULL;
    }
    
    int idx = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->patterns[i]->label == label) {
            result[idx++] = db->patterns[i];
        }
    }
    
    return result;
}

// ============================================================================
// RECOGNIZER API
// ============================================================================

TCDEPatternRecognizer* TCDE_CreateRecognizer(int pattern_width, 
                                             int pattern_height,
                                             int topology_dim) {
    if (pattern_width <= 0 || pattern_height <= 0 || topology_dim <= 0) {
        fprintf(stderr, "[ERROR] Invalid recognizer parameters\\n");
        return NULL;
    }
    
    TCDEPatternRecognizer* recognizer = (TCDEPatternRecognizer*)malloc(sizeof(TCDEPatternRecognizer));
    if (!recognizer) {
        fprintf(stderr, "[ERROR] Failed to allocate recognizer\\n");
        return NULL;
    }
    
    recognizer->database = TCDE_CreateDatabase(100);
    if (!recognizer->database) {
        fprintf(stderr, "[ERROR] Failed to create database\\n");
        free(recognizer);
        return NULL;
    }
    
    recognizer->pattern_width = pattern_width;
    recognizer->pattern_height = pattern_height;
    recognizer->topology_dim = topology_dim;
    recognizer->total_recognitions = 0;
    recognizer->correct_recognitions = 0;
    
    return recognizer;
}

void TCDE_DestroyRecognizer(TCDEPatternRecognizer* recognizer) {
    if (!recognizer) return;
    
    if (recognizer->database) {
        TCDE_DestroyDatabase(recognizer->database);
    }
    
    free(recognizer);
}

bool TCDE_LearnPattern(TCDEPatternRecognizer* recognizer,
                       int width, int height,
                       const float* data, int label) {
    if (!recognizer || width != recognizer->pattern_width || 
        height != recognizer->pattern_height || !data) {
        return false;
    }
    
    Pattern* pattern = TCDE_CreatePattern(width, height, data, label);
    if (!pattern) {
        return false;
    }
    
    if (!TCDE_ExtractTopology(pattern)) {
        TCDE_DestroyPattern(pattern);
        return false;
    }
    
    bool success = TCDE_AddPattern(recognizer->database, pattern);
    
    TCDE_DestroyPattern(pattern);
    
    return success;
}

RecognitionResult TCDE_RecognizePattern(TCDEPatternRecognizer* recognizer,
                                        int width, int height,
                                        const float* data) {
    RecognitionResult result = {0};
    
    if (!recognizer || width != recognizer->pattern_width || 
        height != recognizer->pattern_height || !data) {
        result.predicted_label = -1;
        result.confidence = 0.0f;
        return result;
    }
    
    clock_t start_time = clock();
    
    // Create query pattern
    Pattern* query = TCDE_CreatePattern(width, height, data, -1);
    if (!query) {
        result.predicted_label = -1;
        return result;
    }
    
    if (!TCDE_ExtractTopology(query)) {
        TCDE_DestroyPattern(query);
        result.predicted_label = -1;
        return result;
    }
    
    // Allocate class scores
    int num_classes = recognizer->database->num_classes;
    result.num_classes = num_classes;
    result.class_scores = (float*)calloc(num_classes, sizeof(float));
    
    if (!result.class_scores) {
        TCDE_DestroyPattern(query);
        result.predicted_label = -1;
        return result;
    }
    
    // Compute similarity to all patterns (k-NN with k=5)
    int k = 5;
    if (k > recognizer->database->count) {
        k = recognizer->database->count;
    }
    
    float* similarities = (float*)malloc(recognizer->database->count * sizeof(float));
    int* indices = (int*)malloc(recognizer->database->count * sizeof(int));
    
    // Compute all similarities
    for (int i = 0; i < recognizer->database->count; i++) {
        similarities[i] = TCDE_PatternSimilarity(query, recognizer->database->patterns[i]);
        indices[i] = i;
    }
    
    // Find k nearest neighbors (simple selection sort for top k)
    for (int i = 0; i < k; i++) {
        int max_idx = i;
        for (int j = i + 1; j < recognizer->database->count; j++) {
            if (similarities[j] > similarities[max_idx]) {
                max_idx = j;
            }
        }
        
        // Swap
        float temp_sim = similarities[i];
        similarities[i] = similarities[max_idx];
        similarities[max_idx] = temp_sim;
        
        int temp_idx = indices[i];
        indices[i] = indices[max_idx];
        indices[max_idx] = temp_idx;
    }
    
    // Vote among k nearest neighbors
    for (int i = 0; i < k; i++) {
        int pattern_idx = indices[i];
        int label = recognizer->database->patterns[pattern_idx]->label;
        result.class_scores[label] += similarities[i];
    }
    
    // Find class with highest score
    int best_class = 0;
    float best_score = result.class_scores[0];
    
    for (int i = 1; i < num_classes; i++) {
        if (result.class_scores[i] > best_score) {
            best_score = result.class_scores[i];
            best_class = i;
        }
    }
    
    result.predicted_label = best_class;
    result.confidence = best_score / k;  // Normalize by k
    
    clock_t end_time = clock();
    result.recognition_time_ms = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
    
    // Cleanup
    free(similarities);
    free(indices);
    TCDE_DestroyPattern(query);
    
    return result;
}

float TCDE_GetAccuracy(const TCDEPatternRecognizer* recognizer) {
    if (!recognizer || recognizer->total_recognitions == 0) {
        return 0.0f;
    }
    
    return (float)recognizer->correct_recognitions / recognizer->total_recognitions;
}

void TCDE_ResetStatistics(TCDEPatternRecognizer* recognizer) {
    if (!recognizer) return;
    
    recognizer->total_recognitions = 0;
    recognizer->correct_recognitions = 0;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

float* TCDE_LoadPatternFile(const char* filename, int* width, int* height, int* label) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERROR] Cannot open file: %s\\n", filename);
        return NULL;
    }
    
    // Read header: width height label
    if (fscanf(file, "%d %d %d", width, height, label) != 3) {
        fprintf(stderr, "[ERROR] Invalid file format\\n");
        fclose(file);
        return NULL;
    }
    
    int size = (*width) * (*height);
    float* data = (float*)malloc(size * sizeof(float));
    if (!data) {
        fprintf(stderr, "[ERROR] Failed to allocate data\\n");
        fclose(file);
        return NULL;
    }
    
    // Read data
    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%f", &data[i]) != 1) {
            fprintf(stderr, "[ERROR] Failed to read data\\n");
            free(data);
            fclose(file);
            return NULL;
        }
    }
    
    fclose(file);
    return data;
}

bool TCDE_SavePatternFile(const char* filename, int width, int height, 
                          const float* data, int label) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "[ERROR] Cannot create file: %s\\n", filename);
        return false;
    }
    
    // Write header
    fprintf(file, "%d %d %d\\n", width, height, label);
    
    // Write data
    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%.6f ", data[i]);
        if ((i + 1) % width == 0) {
            fprintf(file, "\\n");
        }
    }
    
    fclose(file);
    return true;
}
