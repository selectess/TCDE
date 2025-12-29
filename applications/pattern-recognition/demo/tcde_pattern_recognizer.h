/**
 * @file tcde_pattern_recognizer.h
 * @brief TCDE Pattern Recognizer - Topological Pattern Recognition POC
 * 
 * Proof-of-concept implementation demonstrating TCDE's pattern recognition
 * capabilities using topological features and field dynamics.
 * 
 * @date 10 Novembre 2025
 * @version 1.0
 */

#ifndef TCDE_PATTERN_RECOGNIZER_H
#define TCDE_PATTERN_RECOGNIZER_H

#include "../../../src/core/tcde_core.h"
#include "../../../src/core/tcde_geometry.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief Pattern representation
 */
typedef struct {
    int width;              // Pattern width
    int height;             // Pattern height
    float* data;            // Pattern data (normalized 0-1)
    int label;              // Pattern class label
    
    // Topological features
    float* topology;        // Topological signature
    int topology_dim;       // Topology dimension
    
    // TCDE field
    TCDE_Field* field;      // Associated field
    
} Pattern;

/**
 * @brief Pattern database
 */
typedef struct {
    Pattern** patterns;     // Array of patterns
    int count;              // Number of patterns
    int capacity;           // Allocated capacity
    
    // Statistics
    int num_classes;        // Number of classes
    int* class_counts;      // Patterns per class
    
} PatternDatabase;

/**
 * @brief Recognition result
 */
typedef struct {
    int predicted_label;    // Predicted class
    float confidence;       // Confidence score (0-1)
    float* class_scores;    // Scores for all classes
    int num_classes;        // Number of classes
    
    // Timing
    float recognition_time_ms;  // Recognition time
    
} RecognitionResult;

/**
 * @brief TCDE Pattern Recognizer
 */
typedef struct {
    PatternDatabase* database;  // Learned patterns
    
    // Configuration
    int pattern_width;          // Expected pattern width
    int pattern_height;         // Expected pattern height
    int topology_dim;           // Topology feature dimension
    
    // Statistics
    int total_recognitions;     // Total recognitions performed
    int correct_recognitions;   // Correct recognitions
    
} TCDEPatternRecognizer;

// ============================================================================
// PATTERN OPERATIONS
// ============================================================================

/**
 * @brief Create pattern from data
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data (will be copied)
 * @param label Pattern class label
 * @return Pattern instance or NULL on error
 */
Pattern* TCDE_CreatePattern(int width, int height, const float* data, int label);

/**
 * @brief Destroy pattern and free memory
 * @param pattern Pattern to destroy
 */
void TCDE_DestroyPattern(Pattern* pattern);

/**
 * @brief Extract topological features from pattern
 * @param pattern Pattern to analyze
 * @return true on success, false on error
 */
bool TCDE_ExtractTopology(Pattern* pattern);

/**
 * @brief Compute similarity between two patterns
 * @param p1 First pattern
 * @param p2 Second pattern
 * @return Similarity score (0-1, higher = more similar)
 */
float TCDE_PatternSimilarity(const Pattern* p1, const Pattern* p2);

// ============================================================================
// DATABASE OPERATIONS
// ============================================================================

/**
 * @brief Create pattern database
 * @param initial_capacity Initial capacity
 * @return Database instance or NULL on error
 */
PatternDatabase* TCDE_CreateDatabase(int initial_capacity);

/**
 * @brief Destroy database and free memory
 * @param db Database to destroy
 */
void TCDE_DestroyDatabase(PatternDatabase* db);

/**
 * @brief Add pattern to database
 * @param db Database
 * @param pattern Pattern to add (will be copied)
 * @return true on success, false on error
 */
bool TCDE_AddPattern(PatternDatabase* db, const Pattern* pattern);

/**
 * @brief Get patterns by class label
 * @param db Database
 * @param label Class label
 * @param count Output: number of patterns found
 * @return Array of patterns (do not free)
 */
Pattern** TCDE_GetPatternsByClass(PatternDatabase* db, int label, int* count);

// ============================================================================
// RECOGNIZER API
// ============================================================================

/**
 * @brief Create pattern recognizer
 * @param pattern_width Expected pattern width
 * @param pattern_height Expected pattern height
 * @param topology_dim Topology feature dimension
 * @return Recognizer instance or NULL on error
 */
TCDEPatternRecognizer* TCDE_CreateRecognizer(int pattern_width, 
                                             int pattern_height,
                                             int topology_dim);

/**
 * @brief Destroy recognizer and free memory
 * @param recognizer Recognizer to destroy
 */
void TCDE_DestroyRecognizer(TCDEPatternRecognizer* recognizer);

/**
 * @brief Learn pattern (add to database)
 * @param recognizer Recognizer instance
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data
 * @param label Pattern class label
 * @return true on success, false on error
 */
bool TCDE_LearnPattern(TCDEPatternRecognizer* recognizer,
                       int width, int height,
                       const float* data, int label);

/**
 * @brief Recognize pattern
 * @param recognizer Recognizer instance
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data
 * @return Recognition result (caller must free result.class_scores)
 */
RecognitionResult TCDE_RecognizePattern(TCDEPatternRecognizer* recognizer,
                                        int width, int height,
                                        const float* data);

/**
 * @brief Get recognizer accuracy
 * @param recognizer Recognizer instance
 * @return Accuracy (0-1)
 */
float TCDE_GetAccuracy(const TCDEPatternRecognizer* recognizer);

/**
 * @brief Reset recognizer statistics
 * @param recognizer Recognizer instance
 */
void TCDE_ResetStatistics(TCDEPatternRecognizer* recognizer);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Load pattern from file (simple format)
 * @param filename File path
 * @param width Output: pattern width
 * @param height Output: pattern height
 * @param label Output: pattern label
 * @return Pattern data (caller must free) or NULL on error
 */
float* TCDE_LoadPatternFile(const char* filename, int* width, int* height, int* label);

/**
 * @brief Save pattern to file
 * @param filename File path
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data
 * @param label Pattern label
 * @return true on success, false on error
 */
bool TCDE_SavePatternFile(const char* filename, int width, int height, 
                          const float* data, int label);

#ifdef __cplusplus
}
#endif

#endif // TCDE_PATTERN_RECOGNIZER_H
