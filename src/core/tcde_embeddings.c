/**
 * @file tcde_embeddings.c
 * @brief TCDE Semantic Embeddings Implementation
 */

#include "tcde_embeddings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

// ============================================================================
// HASH TABLE IMPLEMENTATION
// ============================================================================

#define HASH_TABLE_SIZE 100003  // Prime number for better distribution

/**
 * @brief djb2 hash function
 */
static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

/**
 * @brief Insert word into hash table
 */
static void hash_insert(TCDE_HashEntry** table, int table_size,
                       const char* word, int index) {
    unsigned long hash = hash_string(word) % table_size;
    
    TCDE_HashEntry* entry = (TCDE_HashEntry*)malloc(sizeof(TCDE_HashEntry));
    entry->key = strdup(word);
    entry->value = index;
    entry->next = table[hash];
    table[hash] = entry;
}

/**
 * @brief Lookup word in hash table
 */
static int hash_lookup(TCDE_HashEntry** table, int table_size,
                      const char* word) {
    unsigned long hash = hash_string(word) % table_size;
    
    TCDE_HashEntry* entry = table[hash];
    while (entry) {
        if (strcmp(entry->key, word) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return -1;  // Not found
}

/**
 * @brief Free hash table
 */
static void hash_free(TCDE_HashEntry** table, int table_size) {
    for (int i = 0; i < table_size; i++) {
        TCDE_HashEntry* entry = table[i];
        while (entry) {
            TCDE_HashEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(table);
}


// ============================================================================
// LOADING FUNCTIONS
// ============================================================================

/**
 * @brief Load GloVe format embeddings
 */
static TCDE_Embeddings* load_glove(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filepath);
        return NULL;
    }
    
    // First pass: count lines and determine dimension
    int vocab_size = 0;
    int embedding_dim = 0;
    char line[4096];
    
    while (fgets(line, sizeof(line), file)) {
        if (vocab_size == 0) {
            // Count dimensions from first line
            char* token = strtok(line, " \t\n");
            token = strtok(NULL, " \t\n");  // Skip word
            while (token) {
                embedding_dim++;
                token = strtok(NULL, " \t\n");
            }
        }
        vocab_size++;
    }
    
    if (vocab_size == 0 || embedding_dim == 0) {
        fclose(file);
        return NULL;
    }
    
    // Allocate structure
    TCDE_Embeddings* emb = (TCDE_Embeddings*)calloc(1, sizeof(TCDE_Embeddings));
    emb->vocab_size = vocab_size;
    emb->embedding_dim = embedding_dim;
    emb->words = (char**)malloc(vocab_size * sizeof(char*));
    emb->vectors = (float**)malloc(vocab_size * sizeof(float*));
    emb->hash_table = (TCDE_HashEntry**)calloc(HASH_TABLE_SIZE, sizeof(TCDE_HashEntry*));
    emb->hash_table_size = HASH_TABLE_SIZE;
    
    // Second pass: load data
    rewind(file);
    int idx = 0;
    
    while (fgets(line, sizeof(line), file) && idx < vocab_size) {
        char word[256];
        if (sscanf(line, "%255s", word) != 1) continue;
        
        emb->words[idx] = strdup(word);
        emb->vectors[idx] = (float*)malloc(embedding_dim * sizeof(float));
        
        // Parse vector
        char* ptr = line;
        while (*ptr && !isspace(*ptr)) ptr++;  // Skip word
        
        for (int d = 0; d < embedding_dim; d++) {
            while (*ptr && isspace(*ptr)) ptr++;
            if (sscanf(ptr, "%f", &emb->vectors[idx][d]) != 1) {
                emb->vectors[idx][d] = 0.0f;
            }
            while (*ptr && !isspace(*ptr)) ptr++;
        }
        
        // Add to hash table
        hash_insert(emb->hash_table, emb->hash_table_size, word, idx);
        idx++;
    }
    
    fclose(file);
    return emb;
}

/**
 * @brief Load Word2Vec text format
 */
static TCDE_Embeddings* load_word2vec_text(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filepath);
        return NULL;
    }
    
    int vocab_size, embedding_dim;
    if (fscanf(file, "%d %d", &vocab_size, &embedding_dim) != 2) {
        fclose(file);
        return NULL;
    }
    
    // Allocate structure
    TCDE_Embeddings* emb = (TCDE_Embeddings*)calloc(1, sizeof(TCDE_Embeddings));
    emb->vocab_size = vocab_size;
    emb->embedding_dim = embedding_dim;
    emb->words = (char**)malloc(vocab_size * sizeof(char*));
    emb->vectors = (float**)malloc(vocab_size * sizeof(float*));
    emb->hash_table = (TCDE_HashEntry**)calloc(HASH_TABLE_SIZE, sizeof(TCDE_HashEntry*));
    emb->hash_table_size = HASH_TABLE_SIZE;
    
    // Load data
    for (int i = 0; i < vocab_size; i++) {
        char word[256];
        if (fscanf(file, "%255s", word) != 1) break;
        
        emb->words[i] = strdup(word);
        emb->vectors[i] = (float*)malloc(embedding_dim * sizeof(float));
        
        for (int d = 0; d < embedding_dim; d++) {
            if (fscanf(file, "%f", &emb->vectors[i][d]) != 1) {
                emb->vectors[i][d] = 0.0f;
            }
        }
        
        hash_insert(emb->hash_table, emb->hash_table_size, word, i);
    }
    
    fclose(file);
    return emb;
}

/**
 * @brief Load embeddings (main function)
 */
TCDE_Embeddings* TCDE_LoadEmbeddings(const char* filepath,
                                     TCDE_EmbeddingFormat format) {
    if (!filepath) return NULL;
    
    switch (format) {
        case TCDE_EMBEDDING_GLOVE:
            return load_glove(filepath);
        case TCDE_EMBEDDING_WORD2VEC_TEXT:
            return load_word2vec_text(filepath);
        case TCDE_EMBEDDING_WORD2VEC_BIN:
            fprintf(stderr, "Word2Vec binary format not yet implemented\n");
            return NULL;
        default:
            return NULL;
    }
}

/**
 * @brief Free embeddings
 */
void TCDE_FreeEmbeddings(TCDE_Embeddings* embeddings) {
    if (!embeddings) return;
    
    for (int i = 0; i < embeddings->vocab_size; i++) {
        free(embeddings->words[i]);
        free(embeddings->vectors[i]);
    }
    
    free(embeddings->words);
    free(embeddings->vectors);
    
    if (embeddings->word_frequencies) {
        free(embeddings->word_frequencies);
    }
    
    hash_free(embeddings->hash_table, embeddings->hash_table_size);
    free(embeddings);
}


// ============================================================================
// LOOKUP FUNCTIONS
// ============================================================================

int TCDE_GetWordIndex(const TCDE_Embeddings* embeddings, const char* word) {
    if (!embeddings || !word) return -1;
    return hash_lookup(embeddings->hash_table, embeddings->hash_table_size, word);
}

const float* TCDE_GetWordEmbedding(const TCDE_Embeddings* embeddings,
                                   const char* word) {
    int idx = TCDE_GetWordIndex(embeddings, word);
    if (idx < 0) return NULL;
    return embeddings->vectors[idx];
}

// ============================================================================
// PROJECTION FUNCTIONS
// ============================================================================

/**
 * @brief Simple PCA projection (first 3 components)
 */
void TCDE_ProjectEmbeddingTo6D(const float* embedding,
                               int embedding_dim,
                               float position_6d[6],
                               bool preserve_distances) {
    if (!embedding || !position_6d) return;
    
    // Simplified projection: use first 3 dimensions and normalize
    // In production, should use proper PCA or t-SNE
    
    float sum_sq = 0.0f;
    for (int i = 0; i < 3 && i < embedding_dim; i++) {
        sum_sq += embedding[i] * embedding[i];
    }
    
    float norm = sqrtf(sum_sq);
    if (norm < 1e-6f) norm = 1.0f;
    
    // x, y, z: normalized first 3 dimensions
    for (int i = 0; i < 3; i++) {
        if (i < embedding_dim) {
            position_6d[i] = 0.5f + 0.5f * (embedding[i] / norm);
        } else {
            position_6d[i] = 0.5f;
        }
    }
    
    // τ₁: present time
    position_6d[3] = 1.0f;
    
    // τ₂: no anticipation for isolated words
    position_6d[4] = 0.0f;
    
    // m: semantic modality
    position_6d[5] = 0.4f;
    
    // Apply MDS if requested to preserve distances
    // Classical MDS: scale coordinates to preserve distance ratios
    if (preserve_distances && embedding_dim > 3) {
        // Compute norm in full embedding space
        float full_norm = 0.0f;
        for (int i = 0; i < embedding_dim; i++) {
            full_norm += embedding[i] * embedding[i];
        }
        full_norm = sqrtf(full_norm);
        
        // Compute norm in projected 3D space
        float proj_norm = sqrtf((position_6d[0] - 0.5f) * (position_6d[0] - 0.5f) +
                               (position_6d[1] - 0.5f) * (position_6d[1] - 0.5f) +
                               (position_6d[2] - 0.5f) * (position_6d[2] - 0.5f));
        
        // Scale to preserve distance: scale factor = full_norm / proj_norm
        if (proj_norm > 1e-6f && full_norm > 1e-6f) {
            float scale = full_norm / (proj_norm * 2.0f); // Normalize to [0,1] range
            scale = fminf(scale, 2.0f); // Limit scaling
            scale = fmaxf(scale, 0.5f);
            
            // Apply scaling around center (0.5, 0.5, 0.5)
            position_6d[0] = 0.5f + (position_6d[0] - 0.5f) * scale;
            position_6d[1] = 0.5f + (position_6d[1] - 0.5f) * scale;
            position_6d[2] = 0.5f + (position_6d[2] - 0.5f) * scale;
        }
    }
}

bool TCDE_GetContextualEmbedding(const TCDE_Embeddings* embeddings,
                                 const char** words,
                                 int num_words,
                                 float* output) {
    if (!embeddings || !words || !output || num_words <= 0) return false;
    
    // Initialize output to zero
    for (int d = 0; d < embeddings->embedding_dim; d++) {
        output[d] = 0.0f;
    }
    
    // Average embeddings
    int count = 0;
    for (int w = 0; w < num_words; w++) {
        const float* emb = TCDE_GetWordEmbedding(embeddings, words[w]);
        if (emb) {
            for (int d = 0; d < embeddings->embedding_dim; d++) {
                output[d] += emb[d];
            }
            count++;
        }
    }
    
    if (count == 0) return false;
    
    // Normalize
    for (int d = 0; d < embeddings->embedding_dim; d++) {
        output[d] /= count;
    }
    
    return true;
}

// ============================================================================
// SIMILARITY FUNCTIONS
// ============================================================================

float TCDE_CosineSimilarity(const float* vec1, const float* vec2, int dim) {
    if (!vec1 || !vec2) return 0.0f;
    
    float dot = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int i = 0; i < dim; i++) {
        dot += vec1[i] * vec2[i];
        norm1 += vec1[i] * vec1[i];
        norm2 += vec2[i] * vec2[i];
    }
    
    if (norm1 < 1e-6f || norm2 < 1e-6f) return 0.0f;
    
    return dot / (sqrtf(norm1) * sqrtf(norm2));
}

int TCDE_FindSimilarWords(const TCDE_Embeddings* embeddings,
                          const char* query,
                          int k,
                          int* similar_words,
                          float* similarities) {
    if (!embeddings || !query || !similar_words || !similarities || k <= 0) {
        return 0;
    }
    
    const float* query_vec = TCDE_GetWordEmbedding(embeddings, query);
    if (!query_vec) return 0;
    
    // Initialize with worst similarities
    for (int i = 0; i < k; i++) {
        similar_words[i] = -1;
        similarities[i] = -2.0f;
    }
    
    // Find k most similar
    for (int w = 0; w < embeddings->vocab_size; w++) {
        if (strcmp(embeddings->words[w], query) == 0) continue;  // Skip self
        
        float sim = TCDE_CosineSimilarity(query_vec, embeddings->vectors[w],
                                         embeddings->embedding_dim);
        
        // Insert if better than worst
        if (sim > similarities[k-1]) {
            // Find insertion point
            int pos = k - 1;
            while (pos > 0 && sim > similarities[pos-1]) {
                pos--;
            }
            
            // Shift and insert
            for (int i = k - 1; i > pos; i--) {
                similar_words[i] = similar_words[i-1];
                similarities[i] = similarities[i-1];
            }
            similar_words[pos] = w;
            similarities[pos] = sim;
        }
    }
    
    // Count valid results
    int count = 0;
    for (int i = 0; i < k; i++) {
        if (similar_words[i] >= 0) count++;
    }
    
    return count;
}
