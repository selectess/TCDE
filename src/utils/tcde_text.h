/**
 * @file tcde_text.h
 * @brief TCDE Text Processing - Continuous Field Representation
 * 
 * CRITICAL PRINCIPLE: NO TOKENIZATION!
 * 
 * Text is processed as continuous perturbations of the field,
 * not as discrete token sequences. This preserves information
 * and enables richer semantic representations.
 * 
 * Approach:
 * - Extract continuous features from text (n-grams, statistics)
 * - Map to 6D semantic space (x,y,z,τ₁,τ₂,m)
 * - Create RBF centers as continuous perturbations
 * - Result: Continuous field Φ(x,y,z,τ₁,τ₂,m)
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#ifndef TCDE_TEXT_H
#define TCDE_TEXT_H

#include "../core/tcde_core.h"
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// TEXT FEATURE STRUCTURES
// ============================================================================

/**
 * @brief Continuous text feature (NOT a token!)
 * 
 * Represents a continuous perturbation in semantic space,
 * extracted from text without discrete tokenization.
 */
typedef struct {
    float semantic_position[3];  ///< Position in semantic space (x,y,z)
    float temporal_coord;        ///< Temporal coordinate τ₁
    float anticipation;          ///< Anticipation coordinate τ₂
    float modality;              ///< Modality m (0.4 for semantic)
    float amplitude;             ///< Importance/salience
    float phase;                 ///< Relational structure (radians)
    float width;                 ///< RBF width σ
} TCDE_TextFeature;

/**
 * @brief Text analysis results
 */
typedef struct {
    float importance_center[3];  ///< Center of semantic importance
    float temporal_flow;         ///< Temporal flow direction
    float semantic_density;      ///< Average semantic density
    int num_concepts;            ///< Number of detected concepts
    float coherence;             ///< Internal coherence [0,1]
} TCDE_TextAnalysis;

// ============================================================================
// TEXT TO FIELD CONVERSION
// ============================================================================

/**
 * @brief Convert text to continuous field (NO TOKENIZATION!)
 * @param text Input text (UTF-8)
 * @param field Output field (must be pre-allocated)
 * @return Number of features created
 * 
 * Process:
 * 1. Extract continuous features from text
 * 2. Map to 6D semantic space
 * 3. Create RBF centers
 * 4. Add to field as continuous perturbations
 * 
 * The resulting field is continuous everywhere, not discrete.
 * 
 * Example:
 * ```c
 * TCDE_Field* field = TCDE_CreateField(200, 2.5f);
 * int num_features = TCDE_TextToField("Le chat dort", field);
 * // field now contains continuous representation
 * ```
 */
int TCDE_TextToField(const char* text, TCDE_Field* field);

/**
 * @brief Extract continuous features from text
 * @param text Input text
 * @param features Output features array (pre-allocated)
 * @param max_features Maximum number of features
 * @return Number of features extracted
 * 
 * Extracts continuous features based on:
 * - Character n-grams (continuous statistics)
 * - Local frequency patterns
 * - Positional information
 * - Contextual relationships
 * 
 * NO discrete tokenization is performed.
 */
int TCDE_ExtractTextFeatures(const char* text,
                             TCDE_TextFeature* features,
                             int max_features);

/**
 * @brief Map text features to semantic positions
 * @param features Input features
 * @param num_features Number of features
 * @param positions Output semantic positions [num_features][3]
 * 
 * Maps continuous features to 3D semantic space using:
 * - Continuous hashing (n-gram based)
 * - Statistical properties
 * - Contextual information
 * 
 * Result is continuous, not discrete grid.
 */
void TCDE_MapTextToSemantic(const TCDE_TextFeature* features,
                            int num_features,
                            float (*positions)[3]);

// ============================================================================
// TEXT ANALYSIS
// ============================================================================

/**
 * @brief Analyze text field for semantic properties
 * @param field Field containing text representation
 * @param analysis Output analysis results
 * 
 * Extracts:
 * - Semantic importance regions
 * - Temporal flow patterns
 * - Concept density
 * - Internal coherence
 * 
 * All properties emerge from field structure.
 */
void TCDE_AnalyzeTextField(const TCDE_Field* field,
                           TCDE_TextAnalysis* analysis);

/**
 * @brief Compute semantic similarity between text fields
 * @param field1 First text field
 * @param field2 Second text field
 * @param metric Metric for distance computation
 * @return Similarity score [0,1]
 * 
 * Similarity based on field overlap in 6D space.
 * Uses geodesic distance, not discrete token matching.
 */
float TCDE_TextSimilarity(const TCDE_Field* field1,
                         const TCDE_Field* field2,
                         const TCDE_Metric* metric);

// ============================================================================
// ADVANCED TEXT PROCESSING
// ============================================================================

/**
 * @brief Extract key concepts from text field
 * @param field Text field
 * @param concepts Output concept positions [max_concepts][3]
 * @param max_concepts Maximum concepts to extract
 * @return Number of concepts found
 * 
 * Concepts are regions of high energy density in semantic space.
 * Emerge from field structure, not predefined.
 */
int TCDE_ExtractConcepts(const TCDE_Field* field,
                        float (*concepts)[3],
                        int max_concepts);

/**
 * @brief Compute text field entropy
 * @param field Text field
 * @return Entropy (bits)
 * 
 * Measures information content of continuous representation.
 * Higher entropy = more complex/diverse semantic structure.
 */
float TCDE_TextEntropy(const TCDE_Field* field);

/**
 * @brief Generate text from field (experimental)
 * @param field Text field
 * @param output Output buffer
 * @param max_length Maximum output length
 * @return Length of generated text
 * 
 * Samples field to generate text that reflects
 * the continuous semantic structure.
 * 
 * Note: This is inverse operation, experimental.
 */
int TCDE_FieldToText(const TCDE_Field* field,
                    char* output,
                    int max_length);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Compute continuous hash of string
 * @param str Input string
 * @param position Output position [3]
 * 
 * Maps string to continuous position in [0,1]³
 * using continuous hashing (not discrete hash table).
 */
void TCDE_ContinuousHash(const char* str, float position[3]);

/**
 * @brief Compute n-gram statistics
 * @param text Input text
 * @param n N-gram size
 * @param stats Output statistics (implementation-defined)
 * 
 * Extracts continuous statistical features from n-grams.
 */
void TCDE_NGramStatistics(const char* text, int n, void* stats);

// ============================================================================
// AUTHENTIC TEXT PROCESSING (Non-Simplified)
// ============================================================================

/**
 * @brief Compute TF-IDF amplitude for n-gram (authentic implementation)
 * @param ngram N-gram string
 * @param ngram_size Size of n-gram
 * @param text Full text for context
 * @return TF-IDF score [0,1]
 * 
 * Real TF-IDF computation, not simple heuristic.
 * Replaces simplified "non-space character" counting.
 */
float TCDE_ComputeTFIDF(const char* ngram, int ngram_size, const char* text);

/**
 * @brief Cluster concepts using K-means (authentic implementation)
 * @param field Field to analyze
 * @param k Number of clusters
 * @param centroids Output cluster centroids [k][3]
 * @return Number of clusters found
 * 
 * Real K-means clustering, not simple threshold counting.
 */
int TCDE_ClusterConcepts(const TCDE_Field* field, int k, float (*centroids)[3]);

#ifdef __cplusplus
}
#endif

#endif // TCDE_TEXT_H
