/**
 * @file tcde_embeddings.h
 * @brief TCDE Semantic Embeddings Module
 * 
 * Provides Word2Vec/GloVe embedding support for authentic semantic representations.
 * Falls back to hashing if embeddings unavailable.
 * 
 * @version 1.0
 * @date October 21, 2025
 */

#ifndef TCDE_EMBEDDINGS_H
#define TCDE_EMBEDDINGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// TYPES AND STRUCTURES
// ============================================================================

/**
 * @brief Embedding file formats
 */
typedef enum {
    TCDE_EMBEDDING_WORD2VEC_BIN,   ///< Word2Vec binary format
    TCDE_EMBEDDING_WORD2VEC_TEXT,  ///< Word2Vec text format
    TCDE_EMBEDDING_GLOVE           ///< GloVe text format
} TCDE_EmbeddingFormat;

/**
 * @brief Hash table entry for word lookup
 */
typedef struct TCDE_HashEntry {
    char* key;
    int value;
    struct TCDE_HashEntry* next;
} TCDE_HashEntry;

/**
 * @brief Embeddings structure
 */
typedef struct {
    char** words;              ///< Vocabulary
    float** vectors;           ///< Embedding vectors [vocab_size][embedding_dim]
    int vocab_size;            ///< Number of words
    int embedding_dim;         ///< Dimension of embeddings
    
    // Hash table for O(1) lookup
    TCDE_HashEntry** hash_table;
    int hash_table_size;
    
    // Statistics (optional)
    float* word_frequencies;   ///< Word frequencies (can be NULL)
} TCDE_Embeddings;

// ============================================================================
// LOADING FUNCTIONS
// ============================================================================

/**
 * @brief Load embeddings from file
 * @param filepath Path to embedding file
 * @param format Format of the file
 * @return Embeddings structure or NULL on error
 * 
 * Supported formats:
 * - Word2Vec binary: .bin files
 * - Word2Vec text: .txt files with format "word v1 v2 ... vn"
 * - GloVe: .txt files with format "word v1 v2 ... vn"
 * 
 * Example:
 * ```c
 * TCDE_Embeddings* emb = TCDE_LoadEmbeddings("glove.6B.50d.txt", TCDE_EMBEDDING_GLOVE);
 * if (!emb) {
 *     fprintf(stderr, "Failed to load embeddings\n");
 * }
 * ```
 */
TCDE_Embeddings* TCDE_LoadEmbeddings(const char* filepath,
                                     TCDE_EmbeddingFormat format);

/**
 * @brief Free embeddings structure
 * @param embeddings Embeddings to free
 */
void TCDE_FreeEmbeddings(TCDE_Embeddings* embeddings);

// ============================================================================
// LOOKUP FUNCTIONS
// ============================================================================

/**
 * @brief Get word index in vocabulary
 * @param embeddings Embeddings structure
 * @param word Word to look up
 * @return Index in vocabulary or -1 if not found
 * 
 * Complexity: O(1) average via hash table
 */
int TCDE_GetWordIndex(const TCDE_Embeddings* embeddings, const char* word);

/**
 * @brief Get embedding vector for word
 * @param embeddings Embeddings structure
 * @param word Word to look up
 * @return Pointer to embedding vector or NULL if not found
 * 
 * The returned pointer is valid until embeddings are freed.
 */
const float* TCDE_GetWordEmbedding(const TCDE_Embeddings* embeddings,
                                   const char* word);

// ============================================================================
// PROJECTION FUNCTIONS
// ============================================================================

/**
 * @brief Project embedding to 6D space
 * @param embedding Embedding vector (300D typically)
 * @param embedding_dim Dimension of embedding
 * @param position_6d Output 6D position [x,y,z,τ₁,τ₂,m]
 * @param preserve_distances If true, use MDS to preserve semantic distances
 * 
 * Projection strategy:
 * - PCA/t-SNE to reduce to 3D (x,y,z)
 * - Normalize to [0,1]³
 * - τ₁ = 0 (present time)
 * - τ₂ = 0 (no anticipation for isolated words)
 * - m = 0.4 (semantic modality)
 */
void TCDE_ProjectEmbeddingTo6D(const float* embedding,
                               int embedding_dim,
                               float position_6d[6],
                               bool preserve_distances);

/**
 * @brief Get contextual embedding (average of context words)
 * @param embeddings Embeddings structure
 * @param words Array of words in context
 * @param num_words Number of words
 * @param output Output embedding vector (must be pre-allocated)
 * @return true if successful, false otherwise
 * 
 * Computes weighted average of word embeddings in context.
 * Useful for capturing polysemy and context-dependent meanings.
 */
bool TCDE_GetContextualEmbedding(const TCDE_Embeddings* embeddings,
                                 const char** words,
                                 int num_words,
                                 float* output);

// ============================================================================
// SIMILARITY FUNCTIONS
// ============================================================================

/**
 * @brief Compute cosine similarity between two embeddings
 * @param vec1 First embedding vector
 * @param vec2 Second embedding vector
 * @param dim Dimension of vectors
 * @return Cosine similarity [-1, 1]
 */
float TCDE_CosineSimilarity(const float* vec1, const float* vec2, int dim);

/**
 * @brief Find k most similar words to query
 * @param embeddings Embeddings structure
 * @param query Query word
 * @param k Number of similar words to find
 * @param similar_words Output array of word indices (pre-allocated, size k)
 * @param similarities Output array of similarities (pre-allocated, size k)
 * @return Number of similar words found
 */
int TCDE_FindSimilarWords(const TCDE_Embeddings* embeddings,
                          const char* query,
                          int k,
                          int* similar_words,
                          float* similarities);

#ifdef __cplusplus
}
#endif

#endif // TCDE_EMBEDDINGS_H
