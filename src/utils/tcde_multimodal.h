/**
 * @file tcde_multimodal.h
 * @brief TCDE Multimodal Module - Multi-Modal Processing
 * 
 * Multimodal processing pipelines for:
 * - Text (semantic, m=0.4)
 * - Image (visual, m=0.0)
 * - Audio (auditory, m=0.2)
 * - Motor (action, m=0.6)
 * - Emotional (affect, m=0.8)
 * 
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_MULTIMODAL_H
#define TCDE_MULTIMODAL_H

#include "../core/tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MODALITY TYPES
// ============================================================================

/**
 * @brief Modality information
 */
typedef struct {
    char name[32];
    int id;
    float* embedding;
    int embedding_dim;
    float modality_coord;  // m ∈ [0,1]
} TCDE_Modality;

/**
 * @brief Multi-modal relations
 */
typedef struct {
    TCDE_Modality** modalities;
    int num_modalities;
    float** similarity_matrix;
} TCDE_ModalityRelations;

// ============================================================================
// MODALITY RELATIONS
// ============================================================================

/**
 * @brief Initialize modality relations structure
 * 
 * Creates and initializes a modality relations structure for managing
 * multiple modalities and their interactions in the unified 6D field.
 * This structure tracks modality information and computes cross-modal
 * similarity matrices based on field geometry.
 * 
 * The modality relations structure enables:
 * - Managing multiple modalities (visual, auditory, semantic, motor, emotional)
 * - Computing geometric similarity between modalities
 * - Tracking modality-specific embeddings
 * - Cross-modal transformations and coherence
 * 
 * Memory allocation:
 * - Modality array: num_modalities pointers
 * - Similarity matrix: num_modalities × num_modalities floats
 * - Total: O(num_modalities²) memory
 * 
 * Typical usage:
 * - 3 modalities: text, image, audio (basic multimodal)
 * - 5 modalities: visual, auditory, semantic, motor, emotional (full TCDE)
 * 
 * @param num_modalities Number of modalities to manage (typically 3-5)
 *                       - Must be > 0
 *                       - Recommended: 3-5 for practical applications
 *                       - Maximum: limited by memory (num_modalities²)
 * @return Initialized relations structure with allocated memory, or NULL on error
 *         - NULL if num_modalities ≤ 0
 *         - NULL if memory allocation fails
 * 
 * @note Caller must free with TCDE_FreeModalityRelations() when done
 * @note Similarity matrix initialized to zeros (call TCDE_ComputeSimilarityMatrix)
 * @note Modality pointers initialized to NULL (must be set by caller)
 * 
 * @warning Memory grows as O(num_modalities²) - avoid excessive modalities
 * 
 * @complexity O(num_modalities²) for similarity matrix allocation
 * 
 * @see TCDE_CreateModalityRelations() for default 5-modality setup
 * @see TCDE_FreeModalityRelations() for cleanup
 * @see TCDE_ComputeSimilarityMatrix() to populate similarity matrix
 * 
 * @par Example:
 * @code
 * // Create custom 3-modality system (text, image, audio)
 * TCDE_ModalityRelations* relations = TCDE_InitModalityRelations(3);
 * 
 * if (relations) {
 *     printf("Modality relations initialized for %d modalities\n", 
 *            relations->num_modalities);
 *     
 *     // Set up modalities manually
 *     // ... (set modality names, coordinates, etc.) ...
 *     
 *     // Cleanup when done
 *     TCDE_FreeModalityRelations(relations);
 * } else {
 *     printf("Failed to initialize modality relations\n");
 * }
 * @endcode
 */
TCDE_ModalityRelations* TCDE_InitModalityRelations(int num_modalities);

/**
 * @brief Create default modality relations (5 standard modalities)
 * 
 * Creates a fully initialized modality relations structure with TCDE's
 * 5 standard modalities, each positioned at specific coordinates in the
 * modal dimension to maximize cognitive coverage and minimize overlap.
 * 
 * Standard TCDE modalities:
 * - **Visual** (m=0.0): Spatial, concrete, immediate perception
 * - **Auditory** (m=0.2): Temporal, sequential, acoustic patterns
 * - **Semantic** (m=0.4): Abstract, linguistic, conceptual meaning
 * - **Motor** (m=0.6): Action, movement, embodied cognition
 * - **Emotional** (m=0.8): Affective, valence, arousal states
 * 
 * Modal spacing rationale:
 * - 0.2 spacing provides sufficient separation
 * - Allows intermediate modalities if needed
 * - Covers full [0,1] modal range
 * - Empirically validated for cognitive tasks
 * 
 * This is the **recommended** function for most TCDE applications,
 * providing a complete multimodal framework out-of-the-box.
 * 
 * @return Initialized relations with 5 modalities, or NULL on error
 *         - NULL if memory allocation fails
 *         - Otherwise: fully configured 5-modality system
 * 
 * @note Modalities are pre-configured with names and coordinates
 * @note Similarity matrix initialized to zeros (call TCDE_ComputeSimilarityMatrix)
 * @note Caller must free with TCDE_FreeModalityRelations() when done
 * @note This is the standard configuration for TCDE research
 * 
 * @complexity O(1) - constant 5 modalities
 * 
 * @see TCDE_InitModalityRelations() for custom modality count
 * @see TCDE_ComputeSimilarityMatrix() to populate similarity matrix
 * @see TCDE_FreeModalityRelations() for cleanup
 * 
 * @par Example:
 * @code
 * // Create standard 5-modality system
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * if (relations) {
 *     printf("Standard TCDE modalities:\n");
 *     for (int i = 0; i < relations->num_modalities; i++) {
 *         printf("  %d. %s (m=%.1f)\n", i+1,
 *                relations->modalities[i]->name,
 *                relations->modalities[i]->modality_coord);
 *     }
 *     // Output:
 *     //   1. Visual (m=0.0)
 *     //   2. Auditory (m=0.2)
 *     //   3. Semantic (m=0.4)
 *     //   4. Motor (m=0.6)
 *     //   5. Emotional (m=0.8)
 *     
 *     // Use for multimodal processing
 *     TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 *     TCDE_ProcessText(field, relations, "Hello world", 1.0f);
 *     TCDE_ProcessImage(field, relations, image_data, 640, 480, 3, 1.0f);
 *     
 *     // Cleanup
 *     TCDE_DestroyField(field);
 *     TCDE_FreeModalityRelations(relations);
 * }
 * @endcode
 */
TCDE_ModalityRelations* TCDE_CreateModalityRelations(void);

/**
 * @brief Free modality relations and all associated memory
 * 
 * Deallocates all memory associated with a modality relations structure,
 * including modality array, similarity matrix, and individual modality
 * embeddings. This is the proper cleanup function for modality relations.
 * 
 * Memory freed:
 * - Modality array (num_modalities pointers)
 * - Each modality structure
 * - Each modality's embedding (if allocated)
 * - Similarity matrix (num_modalities × num_modalities)
 * - Relations structure itself
 * 
 * Safe to call with NULL pointer (no-op).
 * 
 * @param relations Relations to free (can be NULL, in which case no-op)
 * 
 * @note Safe to call with NULL
 * @note Frees all nested structures recursively
 * @note After calling, relations pointer is invalid
 * @note Always pair with TCDE_InitModalityRelations() or TCDE_CreateModalityRelations()
 * 
 * @complexity O(num_modalities²) for similarity matrix deallocation
 * 
 * @see TCDE_InitModalityRelations() for allocation
 * @see TCDE_CreateModalityRelations() for standard allocation
 * @see TCDE_DestroyModalityRelations() for alias
 * 
 * @par Example:
 * @code
 * // Proper cleanup pattern
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // ... use relations ...
 * 
 * // Cleanup (safe even if relations is NULL)
 * TCDE_FreeModalityRelations(relations);
 * relations = NULL;  // Good practice: nullify after free
 * @endcode
 */
void TCDE_FreeModalityRelations(TCDE_ModalityRelations* relations);

/**
 * @brief Destroy modality relations (alias for Free)
 * 
 * Alias for TCDE_FreeModalityRelations() provided for API consistency
 * with other TCDE destroy functions. Performs identical cleanup.
 * 
 * Use this function if you prefer "Destroy" naming convention for
 * consistency with TCDE_DestroyField(), TCDE_DestroyMetric(), etc.
 * 
 * @param relations Relations to destroy (can be NULL, in which case no-op)
 * 
 * @note Identical to TCDE_FreeModalityRelations()
 * @note Safe to call with NULL
 * @note Provided for API naming consistency
 * 
 * @complexity O(num_modalities²)
 * 
 * @see TCDE_FreeModalityRelations() for detailed documentation
 * 
 * @par Example:
 * @code
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * // ... use relations ...
 * TCDE_DestroyModalityRelations(relations);  // Same as Free
 * @endcode
 */
void TCDE_DestroyModalityRelations(TCDE_ModalityRelations* relations);

/**
 * @brief Compute cross-modal similarity matrix from field geometry
 * 
 * Computes similarity between all modality pairs using geodesic distance
 * in the unified 6D field. This is **authentic TCDE**: geometry determines
 * similarity, not hardcoded assumptions or learned matrices.
 * 
 * **TCDE Principle:** Geometry IS the similarity measure
 * 
 * The similarity matrix S_ij measures how similar modality i is to modality j
 * based on the average geodesic distance between their field centers:
 * 
 * Formula: S_ij = exp(-⟨d_g(m_i, m_j)²⟩ / 2σ_m²)
 * 
 * where:
 * - ⟨d_g²⟩ is the average squared geodesic distance
 * - σ_m is the modal length scale (typically 0.2)
 * - Averaging is over all center pairs in modalities i and j
 * 
 * Properties of similarity matrix:
 * - Symmetric: S_ij = S_ji
 * - Diagonal unity: S_ii = 1 (self-similarity)
 * - Range: S_ij ∈ [0,1]
 * - Geometry-based: emerges from field structure
 * - Dynamic: changes as field evolves
 * 
 * This approach is fundamentally different from traditional AI:
 * - ❌ NOT hardcoded similarity matrices
 * - ❌ NOT learned from data
 * - ✅ Emerges from geometric field structure
 * - ✅ Respects Riemannian manifold geometry
 * - ✅ Updates dynamically with field evolution
 * 
 * Applications:
 * - Cross-modal retrieval (find similar content across modalities)
 * - Modality fusion (weight modalities by similarity)
 * - Transfer learning (leverage similar modalities)
 * - Cognitive modeling (understand modality relationships)
 * 
 * @param field Field containing multimodal centers (must not be NULL)
 *              - Must have centers in multiple modalities
 *              - Centers identified by their m-coordinate
 * @param relations Modality relations (similarity matrix updated in-place)
 *                  - Must not be NULL
 *                  - similarity_matrix must be allocated
 *                  - Matrix updated with computed similarities
 * 
 * @note Requires field to have centers in multiple modalities
 * @note Computationally expensive for large fields
 * @note Results depend on current field state (dynamic)
 * @note Similarity matrix is symmetric
 * 
 * @warning Very expensive for large numbers of centers
 * @warning Requires at least 2 modalities with centers
 * 
 * @complexity O(num_modalities² × K²) where K = average centers per modality
 * 
 * @see TCDE_CrossModalSimilarity() for pairwise similarity
 * @see TCDE_GeodesicDistance() for distance computation
 * @see TCDE_CreateModalityRelations() for relations initialization
 * 
 * @par Example:
 * @code
 * // Create field and add multimodal content
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // Add content in different modalities
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * TCDE_ProcessImage(field, relations, sunset_image, 640, 480, 3, 1.0f);
 * TCDE_ProcessAudio(field, relations, ocean_sounds, 44100, 44100, 1.0f);
 * 
 * // Compute similarity matrix from field geometry
 * TCDE_ComputeSimilarityMatrix(field, relations);
 * 
 * // Analyze cross-modal similarities
 * printf("Cross-modal similarity matrix:\n");
 * printf("           Visual  Auditory Semantic Motor  Emotional\n");
 * for (int i = 0; i < 5; i++) {
 *     printf("%-10s", relations->modalities[i]->name);
 *     for (int j = 0; j < 5; j++) {
 *         printf(" %.3f   ", relations->similarity_matrix[i][j]);
 *     }
 *     printf("\n");
 * }
 * 
 * // Example output:
 * // Visual     1.000   0.234   0.567   0.123   0.089
 * // Auditory   0.234   1.000   0.345   0.156   0.234
 * // Semantic   0.567   0.345   1.000   0.234   0.456
 * // Motor      0.123   0.156   0.234   1.000   0.178
 * // Emotional  0.089   0.234   0.456   0.178   1.000
 * 
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
void TCDE_ComputeSimilarityMatrix(TCDE_Field* field, 
                                   TCDE_ModalityRelations* relations);

// ============================================================================
// SEMANTIC EMBEDDINGS (Authentic)
// ============================================================================

/**
 * @brief Load Word2Vec/GloVe embeddings from file
 * 
 * Loads pre-trained word embeddings (Word2Vec, GloVe, FastText, etc.) from
 * a text file for use in semantic text processing. These embeddings map
 * words to dense vectors that capture semantic relationships.
 * 
 * File format (space-separated):
 * ```
 * word dim1 dim2 dim3 ... dimN
 * ```
 * 
 * Example:
 * ```
 * cat 0.123 0.456 0.789 -0.234 0.567
 * dog 0.145 0.478 0.812 -0.198 0.534
 * king 0.234 -0.123 0.456 0.789 -0.345
 * ```
 * 
 * Supported embedding types:
 * - Word2Vec (Google News, 300D)
 * - GloVe (Wikipedia, 50D-300D)
 * - FastText (Common Crawl, 300D)
 * - Custom embeddings (any dimension)
 * 
 * Fallback behavior:
 * If loading fails (file not found, parse error, etc.), the system
 * automatically falls back to intelligent character-level embeddings
 * that capture:
 * - Character n-gram statistics
 * - Word length and structure
 * - Phonetic patterns
 * - Morphological features
 * 
 * This ensures TCDE always has semantic representations, even without
 * pre-trained embeddings.
 * 
 * @param filepath Path to embeddings file (must not be NULL)
 *                 - Absolute or relative path
 *                 - File must be readable
 *                 - Format: space-separated text
 * @param embedding_dim Dimension of embeddings (must match file)
 *                      - Typical: 50, 100, 200, 300
 *                      - Must be > 0
 *                      - Must match dimension in file
 * @return true on successful load, false on error
 *         - false if filepath is NULL
 *         - false if file cannot be opened
 *         - false if parse error occurs
 *         - false if dimension mismatch
 *         - System falls back to character-level on false
 * 
 * @note Embeddings are stored globally (singleton pattern)
 * @note Subsequent calls replace previous embeddings
 * @note Memory is managed internally
 * @note Fallback ensures system always works
 * 
 * @warning Large embedding files (>1GB) may take time to load
 * @warning Ensure embedding_dim matches file dimension
 * 
 * @complexity O(V × D) where V = vocabulary size, D = embedding dimension
 * 
 * @see TCDE_GetWordEmbedding() to retrieve embeddings
 * @see TCDE_ProcessText() for text processing with embeddings
 * 
 * @par Example:
 * @code
 * // Load GloVe embeddings (300D)
 * bool success = TCDE_LoadEmbeddings("glove.6B.300d.txt", 300);
 * 
 * if (success) {
 *     printf("Embeddings loaded successfully\n");
 *     
 *     // Test embedding retrieval
 *     float embedding[300];
 *     if (TCDE_GetWordEmbedding("cat", embedding, 300)) {
 *         printf("Embedding for 'cat': [%.3f, %.3f, ...]\n",
 *                embedding[0], embedding[1]);
 *     }
 * } else {
 *     printf("Failed to load embeddings, using character-level fallback\n");
 *     // System still works with fallback embeddings
 * }
 * 
 * // Process text (works with or without pre-trained embeddings)
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * TCDE_ProcessText(field, relations, "The cat sat on the mat", 1.0f);
 * @endcode
 */
bool TCDE_LoadEmbeddings(const char* filepath, int embedding_dim);

// ============================================================================
// TEXT PROCESSING
// ============================================================================

/**
 * @brief Process text input into continuous field representation
 * 
 * Transforms text into a continuous field perturbation in the semantic
 * modality (m=0.4). This is **authentic TCDE**: NO discrete tokenization,
 * but continuous geometric perturbations of the 6D field.
 * 
 * **TCDE Principle: Text → Continuous Field Perturbation (NOT tokens!)**
 * 
 * Processing pipeline:
 * 1. **Tokenize** text (whitespace + punctuation separation)
 * 2. **Embed** words using semantic embeddings (Word2Vec, GloVe, or fallback)
 * 3. **Map** embeddings to 3D spatial coordinates (x,y,z) via dimensionality reduction
 * 4. **Set** temporal coordinate τ₁ = current_time (sequential position)
 * 5. **Set** anticipation τ₂ based on context (forward-looking)
 * 6. **Set** modality coordinate m = 0.4 (semantic modality)
 * 7. **Add** RBF centers as continuous field perturbations
 * 
 * Key differences from traditional NLP:
 * - ❌ NOT discrete token sequences
 * - ❌ NOT symbolic manipulation
 * - ❌ NOT attention mechanisms
 * - ✅ Continuous field dynamics
 * - ✅ Geometric semantic space
 * - ✅ Riemannian manifold processing
 * 
 * Semantic modality (m=0.4):
 * - Abstract, linguistic, conceptual
 * - Between auditory (0.2) and motor (0.6)
 * - Captures meaning, not form
 * - Enables cross-modal semantic transfer
 * 
 * Spatial mapping:
 * - High-dimensional embeddings → 3D coordinates
 * - Preserves semantic relationships
 * - Similar words → nearby locations
 * - Enables geometric similarity
 * 
 * Temporal structure:
 * - τ₁: Sequential position in text
 * - τ₂: Anticipatory context (next word prediction)
 * - Captures temporal flow of language
 * 
 * @param field TCDE field (must not be NULL, will be modified)
 *              - Centers added in semantic modality
 *              - Field structure updated
 * @param relations Modality relations (must not be NULL)
 *                  - Provides modality configuration
 *                  - Used for cross-modal coherence
 * @param text Input text string (must not be NULL)
 *             - UTF-8 encoded
 *             - Any length (limited by memory)
 *             - Punctuation preserved for structure
 * @param intensity Perturbation strength (typically 0.5-2.0)
 *                  - Controls field amplitude
 *                  - Higher = stronger semantic influence
 *                  - Typical: 1.0 for balanced processing
 * 
 * @note Modality coordinate fixed at m=0.4 (semantic)
 * @note Uses pre-trained embeddings if loaded, else character-level
 * @note Temporal coordinates capture sequential structure
 * @note Creates continuous field, not discrete tokens
 * 
 * @warning Long texts may create many centers (memory usage)
 * @warning Requires embeddings for best results (load with TCDE_LoadEmbeddings)
 * 
 * @complexity O(N × D) where N = number of words, D = embedding dimension
 * 
 * @see TCDE_LoadEmbeddings() to load semantic embeddings
 * @see TCDE_TokenizeText() for tokenization details
 * @see TCDE_GetWordEmbedding() for embedding retrieval
 * @see TCDE_ProcessImage() for visual modality
 * @see TCDE_ProcessAudio() for auditory modality
 * 
 * @par Example:
 * @code
 * // Process text into semantic field
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // Load embeddings for better semantic representation
 * TCDE_LoadEmbeddings("glove.6B.300d.txt", 300);
 * 
 * // Process text
 * const char* text = "The quick brown fox jumps over the lazy dog";
 * TCDE_ProcessText(field, relations, text, 1.0f);
 * 
 * printf("Text processed: %d centers added\n", field->num_centers);
 * printf("Modality: Semantic (m=0.4)\n");
 * 
 * // Verify centers are in semantic modality
 * int semantic_count = 0;
 * for (int i = 0; i < field->num_centers; i++) {
 *     if (fabsf(field->centers[i].coords[5] - 0.4f) < 0.01f) {
 *         semantic_count++;
 *     }
 * }
 * printf("Centers in semantic modality: %d/%d\n", 
 *        semantic_count, field->num_centers);
 * 
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
void TCDE_ProcessText(TCDE_Field* field, TCDE_ModalityRelations* relations,
                      const char* text, float intensity);

/**
 * @brief Tokenize text into words for preprocessing
 * 
 * Splits input text into individual tokens (words) using whitespace and
 * punctuation as delimiters. This is a preprocessing step for text
 * processing, though TCDE ultimately creates continuous representations.
 * 
 * **TCDE Context: Tokenization as Preprocessing Only**
 * 
 * Important distinction:
 * - Traditional NLP: Tokens are the fundamental units (discrete)
 * - TCDE: Tokens are temporary preprocessing, final representation is continuous
 * - This function extracts words, but TCDE_ProcessText() creates continuous field
 * - No "token embeddings" - only continuous geometric perturbations
 * 
 * Tokenization rules:
 * - **Split** on whitespace (space, tab, newline, carriage return)
 * - **Split** on punctuation (.,!?;:'"()[]{}-)
 * - **Preserve** punctuation as separate tokens (captures structure)
 * - **Convert** to lowercase (optional, for case-insensitive processing)
 * - **Remove** empty tokens and excessive whitespace
 * - **Preserve** order (sequential structure matters)
 * 
 * Example tokenization:
 * - Input: "Hello, world! How are you?"
 * - Output: ["hello", ",", "world", "!", "how", "are", "you", "?"]
 * - Count: 8 tokens
 * 
 * Why simpler than full NLP tokenization:
 * - ❌ NO stemming (running → run)
 * - ❌ NO lemmatization (better → good)
 * - ❌ NO part-of-speech tagging
 * - ❌ NO named entity recognition
 * - ✅ Simple, fast, sufficient for TCDE
 * - ✅ Continuous field handles variations geometrically
 * 
 * TCDE's continuous representation naturally handles:
 * - Semantic similarity (run, running, ran → nearby in field)
 * - Morphological variations (cat, cats → geometric proximity)
 * - Contextual meaning (bank: river vs money → different field regions)
 * 
 * @param text Input text string (must not be NULL)
 *             - UTF-8 encoded (ASCII subset supported)
 *             - Any length (limited by memory)
 *             - Whitespace and punctuation preserved
 * @param tokens Output token array (must be pre-allocated)
 *               - Array of char* pointers
 *               - Each token is null-terminated string
 *               - Caller must allocate: char* tokens[max_tokens]
 *               - Caller must free each token after use
 * @param max_tokens Maximum number of tokens to extract
 *                   - Prevents buffer overflow
 *                   - Typical: 100-1000 tokens
 *                   - Excess tokens ignored (truncation)
 * @return Number of tokens extracted (≤ max_tokens)
 *         - 0 if text is NULL or empty
 *         - Actual count may be less than max_tokens
 *         - Typical: 10-100 tokens for sentences
 * 
 * @note Tokens are dynamically allocated, caller must free
 * @note Punctuation preserved as separate tokens
 * @note Case conversion optional (default: lowercase)
 * @note UTF-8 support limited to ASCII subset
 * 
 * @warning Caller must free each token with free()
 * @warning max_tokens must match allocated array size
 * 
 * @complexity O(N) where N = text length
 * 
 * @see TCDE_ProcessText() for continuous field creation
 * @see TCDE_GetWordEmbedding() for semantic embeddings
 * 
 * @par Example:
 * @code
 * // Tokenize text
 * const char* text = "The quick brown fox jumps!";
 * char* tokens[100];
 * 
 * int num_tokens = TCDE_TokenizeText(text, tokens, 100);
 * 
 * printf("Tokenized: %d tokens\n", num_tokens);
 * for (int i = 0; i < num_tokens; i++) {
 *     printf("  Token %d: '%s'\n", i, tokens[i]);
 * }
 * 
 * // Expected output:
 * // Token 0: 'the'
 * // Token 1: 'quick'
 * // Token 2: 'brown'
 * // Token 3: 'fox'
 * // Token 4: 'jumps'
 * // Token 5: '!'
 * 
 * // Free tokens
 * for (int i = 0; i < num_tokens; i++) {
 *     free(tokens[i]);
 * }
 * @endcode
 * 
 * @note Tokens are allocated internally (caller must free)
 * @note Punctuation preserved as separate tokens
 * @note Case-sensitive by default
 * @note Simple whitespace/punctuation splitting
 * 
 * @warning Caller must free each token string
 * @warning Does not handle complex Unicode tokenization
 * 
 * @complexity O(N) where N = text length
 * 
 * @see TCDE_ProcessText() for full text processing
 * @see TCDE_GetWordEmbedding() for word embeddings
 * 
 * @par Example:
 * @code
 * // Tokenize text
 * const char* text = "Hello, world! How are you?";
 * char* tokens[100];
 * int num_tokens = TCDE_TokenizeText(text, tokens, 100);
 * 
 * printf("Tokenized into %d tokens:\n", num_tokens);
 * for (int i = 0; i < num_tokens; i++) {
 *     printf("  %d: '%s'\n", i, tokens[i]);
 * }
 * // Output:
 * //   0: 'hello'
 * //   1: ','
 * //   2: 'world'
 * //   3: '!'
 * //   4: 'how'
 * //   5: 'are'
 * //   6: 'you'
 * //   7: '?'
 * 
 * // Free tokens
 * for (int i = 0; i < num_tokens; i++) {
 *     free(tokens[i]);
 * }
 * @endcode
 */
int TCDE_TokenizeText(const char* text, char** tokens, int max_tokens);

/**
 * @brief Get word embedding vector
 * 
 * Retrieves the semantic embedding vector for a given word from the
 * pre-loaded embedding dictionary (Word2Vec, GloVe, FastText, etc.).
 * If the word is not found, falls back to character-level embedding.
 * 
 * Embedding lookup process:
 * 1. **Normalize** word (lowercase, trim)
 * 2. **Lookup** in embedding dictionary
 * 3. **Return** embedding if found
 * 4. **Fallback** to character-level if not found
 * 
 * Character-level fallback:
 * - Computes embedding from character n-grams
 * - Captures morphological structure
 * - Handles out-of-vocabulary words
 * - Ensures system always works
 * 
 * Embedding properties:
 * - Dense vectors (typically 50-300 dimensions)
 * - Semantic similarity: similar words → similar vectors
 * - Arithmetic: king - man + woman ≈ queen
 * - Pre-trained on large corpora
 * 
 * @param word Word to embed (must not be NULL)
 *             - Case-insensitive
 *             - Trimmed of whitespace
 *             - Any UTF-8 string
 * @param embedding Output embedding vector (must be pre-allocated)
 *                  - Array of floats
 *                  - Size must match embedding_dim
 *                  - Filled with embedding values
 * @param embedding_dim Embedding dimension (must match loaded embeddings)
 *                      - Typical: 50, 100, 200, 300
 *                      - Must match TCDE_LoadEmbeddings dimension
 *                      - Must be > 0
 * @return true if word found in dictionary, false if using fallback
 *         - true: embedding from pre-trained dictionary
 *         - false: embedding from character-level fallback
 *         - embedding array always filled (never fails)
 * 
 * @note Always returns an embedding (fallback ensures success)
 * @note Fallback embeddings are less accurate but functional
 * @note Load embeddings with TCDE_LoadEmbeddings() for best results
 * @note Case-insensitive lookup
 * 
 * @complexity O(log V) where V = vocabulary size (hash table lookup)
 *             O(W) for fallback where W = word length
 * 
 * @see TCDE_LoadEmbeddings() to load embedding dictionary
 * @see TCDE_ProcessText() for text processing with embeddings
 * @see TCDE_TokenizeText() for word extraction
 * 
 * @par Example:
 * @code
 * // Load embeddings
 * TCDE_LoadEmbeddings("glove.6B.300d.txt", 300);
 * 
 * // Get embedding for a word
 * float embedding[300];
 * bool found = TCDE_GetWordEmbedding("cat", embedding, 300);
 * 
 * if (found) {
 *     printf("Found 'cat' in dictionary\n");
 *     printf("Embedding: [%.3f, %.3f, %.3f, ...]\n",
 *            embedding[0], embedding[1], embedding[2]);
 * } else {
 *     printf("Using character-level fallback for 'cat'\n");
 * }
 * 
 * // Test semantic similarity
 * float cat_emb[300], dog_emb[300], car_emb[300];
 * TCDE_GetWordEmbedding("cat", cat_emb, 300);
 * TCDE_GetWordEmbedding("dog", dog_emb, 300);
 * TCDE_GetWordEmbedding("car", car_emb, 300);
 * 
 * // Compute cosine similarity
 * float sim_cat_dog = cosine_similarity(cat_emb, dog_emb, 300);
 * float sim_cat_car = cosine_similarity(cat_emb, car_emb, 300);
 * 
 * printf("Similarity(cat, dog): %.3f\n", sim_cat_dog);  // High
 * printf("Similarity(cat, car): %.3f\n", sim_cat_car);  // Low
 * @endcode
 */
bool TCDE_GetWordEmbedding(const char* word, float* embedding, int embedding_dim);

// ============================================================================
// IMAGE PROCESSING
// ============================================================================

/**
 * @brief Process image input into field representation
 * @param field Field
 * @param relations Modality relations
 * @param image_data Image pixel data (RGB or grayscale)
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels (1 or 3)
 * @param intensity Perturbation strength
 * 
 * Pipeline:
 * 1. Extract visual features (SIFT keypoints or CNN features)
 * 2. Normalize spatial coordinates to [0,1]²
 * 3. Map feature scale to z coordinate
 * 4. Set temporal coordinate τ₁ = current_time, τ₂ = 0
 * 5. Set modality coordinate m = 0.0 (visual)
 * 6. Add field centers with coefficients ∝ feature strength
 * 
 * Modality: m = 0.0 (visual)
 */
void TCDE_ProcessImage(TCDE_Field* field, TCDE_ModalityRelations* relations,
                       const unsigned char* image_data, int width, int height,
                       int channels, float intensity);

/**
 * @brief Extract SIFT keypoints from image
 * @param image_data Image pixel data
 * @param width Image width
 * @param height Image height
 * @param keypoints Output keypoint array
 * @param max_keypoints Maximum number of keypoints
 * @return Number of keypoints extracted
 */
int TCDE_ExtractSIFTKeypoints(const unsigned char* image_data, int width, int height,
                               float keypoints[][4], int max_keypoints);

/**
 * @brief Extract CNN features from image
 * @param image_data Image pixel data
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param features Output feature vector
 * @param feature_dim Feature dimension
 * @return true on success
 * 
 * Uses pre-trained CNN (e.g., ResNet, VGG) for feature extraction
 */
bool TCDE_ExtractCNNFeatures(const unsigned char* image_data, int width, int height,
                              int channels, float* features, int feature_dim);

// ============================================================================
// AUDIO PROCESSING
// ============================================================================

/**
 * @brief Process audio input into field representation
 * @param field Field
 * @param relations Modality relations
 * @param audio_samples Audio sample data
 * @param num_samples Number of samples
 * @param sample_rate Sample rate (Hz)
 * @param intensity Perturbation strength
 * 
 * Pipeline:
 * 1. Compute Short-Time Fourier Transform (STFT)
 * 2. Map frequency bins to y coordinate (spatial)
 * 3. Map time frames to τ₁ coordinate (temporal)
 * 4. Map amplitude to z coordinate
 * 5. Set modality coordinate m = 0.2 (auditory)
 * 6. Add field centers with complex coefficients from STFT
 * 
 * Modality: m = 0.2 (auditory)
 */
void TCDE_ProcessAudio(TCDE_Field* field, TCDE_ModalityRelations* relations,
                       const float* audio_samples, int num_samples, int sample_rate,
                       float intensity);

/**
 * @brief Compute Short-Time Fourier Transform
 * @param audio_samples Input audio samples
 * @param num_samples Number of samples
 * @param window_size STFT window size
 * @param hop_size Hop size between windows
 * @param stft Output STFT (complex) [num_frames][num_bins]
 * @param num_frames Output: number of time frames
 * @param num_bins Output: number of frequency bins
 * @return true on success
 */
bool TCDE_ComputeSTFT(const float* audio_samples, int num_samples,
                      int window_size, int hop_size,
                      TCDE_Complex** stft, int* num_frames, int* num_bins);

/**
 * @brief Extract spectral features from audio
 * @param audio_samples Input audio samples
 * @param num_samples Number of samples
 * @param sample_rate Sample rate (Hz)
 * @param features Output feature vector
 * @param feature_dim Feature dimension
 * @return true on success
 * 
 * Extracts: MFCCs, spectral centroid, spectral rolloff, etc.
 */
bool TCDE_ExtractSpectralFeatures(const float* audio_samples, int num_samples,
                                   int sample_rate, float* features, int feature_dim);

// ============================================================================
// MOTOR PROCESSING
// ============================================================================

/**
 * @brief Process motor/action input into field representation
 * @param field Field
 * @param relations Modality relations
 * @param trajectory Trajectory points [num_points][3]
 * @param num_points Number of trajectory points
 * @param intensity Perturbation strength
 * 
 * Pipeline:
 * 1. Map trajectory to spatiotemporal coordinates
 * 2. Set modality coordinate m = 0.6 (motor)
 * 3. Add field centers along trajectory
 * 
 * Modality: m = 0.6 (motor)
 */
void TCDE_ProcessMotor(TCDE_Field* field, TCDE_ModalityRelations* relations,
                       const float trajectory[][3], int num_points, float intensity);

// ============================================================================
// EMOTIONAL PROCESSING
// ============================================================================

/**
 * @brief Process emotional/affective input into field representation
 * @param field Field
 * @param relations Modality relations
 * @param valence Emotional valence [-1,1]
 * @param arousal Emotional arousal [0,1]
 * @param dominance Emotional dominance [0,1]
 * @param intensity Perturbation strength
 * 
 * Pipeline:
 * 1. Map valence-arousal-dominance to 3D coordinates
 * 2. Set modality coordinate m = 0.8 (emotional)
 * 3. Add field perturbation
 * 
 * Modality: m = 0.8 (emotional)
 */
void TCDE_ProcessEmotion(TCDE_Field* field, TCDE_ModalityRelations* relations,
                         float valence, float arousal, float dominance, 
                         float intensity);

// ============================================================================
// CROSS-MODAL OPERATIONS
// ============================================================================

/**
 * @brief Transform field from one modality to another with geometric preservation
 * 
 * Performs continuous geometric transformation of field representation from
 * one modality to another while preserving semantic content and geometric
 * structure. This is **authentic TCDE**: continuous rotation in modality
 * dimension, not discrete mapping.
 * 
 * **TCDE Principle: Continuous Modality Transformation**
 * 
 * Transformation process:
 * 1. **Identify** centers in source modality
 * 2. **Rotate** continuously in m-dimension: m_source → m_target
 * 3. **Preserve** field structure during transformation (diffeomorphism)
 * 4. **Interpolate** field values continuously during rotation
 * 5. **Maintain** energy and coherence throughout transformation
 * 6. **Apply** modality-specific transformations based on relations
 * 
 * Key properties:
 * - **Continuous:** Smooth rotation, not discrete jumps
 * - **Structure-preserving:** Maintains geometric relationships
 * - **Energy-conserving:** Total field energy preserved
 * - **Reversible:** Transformation can be undone
 * - **Semantic-preserving:** Meaning maintained across modalities
 * 
 * Applications:
 * - **Text → Image:** Semantic concepts to visual representations
 * - **Audio → Text:** Speech/music to textual descriptions
 * - **Image → Audio:** Visual scenes to soundscapes
 * - **Motor → Semantic:** Actions to linguistic descriptions
 * - **Emotion → All:** Affective coloring of any modality
 * 
 * Transformation types:
 * - **Direct:** Simple rotation in m-dimension
 * - **Weighted:** Rotation weighted by modality similarity
 * - **Interpolated:** Gradual transformation over multiple steps
 * 
 * @param field TCDE field (must not be NULL, will be modified)
 *              - Centers in source modality will be transformed
 *              - Field structure preserved during transformation
 * @param relations Modality relations (must not be NULL)
 *                  - Provides similarity matrix for weighted transformation
 *                  - Determines transformation parameters
 * @param source_modality Source modality ID (0-4):
 *                        - 0: Visual (m=0.0)
 *                        - 1: Auditory (m=0.2)
 *                        - 2: Semantic (m=0.4)
 *                        - 3: Motor (m=0.6)
 *                        - 4: Emotional (m=0.8)
 * @param target_modality Target modality ID (0-4, same scale as source)
 *                        - Must be different from source_modality
 *                        - Determines final modality coordinate
 * 
 * @note Transformation is continuous, not discrete
 * @note Field energy preserved to within numerical precision
 * @note Semantic content maintained across transformation
 * @note Centers not in source modality are unaffected
 * 
 * @warning Large transformations (e.g., visual→emotional) may lose fidelity
 * @warning Requires modality relations to be initialized
 * 
 * @complexity O(N) where N = number of centers in source modality
 * 
 * @see TCDE_CrossModalSimilarity() for similarity measurement
 * @see TCDE_CrossModalCoherence() for coherence measurement
 * @see TCDE_CreateModalityRelations() for relations initialization
 * 
 * @par Example:
 * @code
 * // Create field with semantic centers
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // Process text (semantic modality)
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * 
 * printf("Original: %d centers in semantic modality\n", 
 *        count_centers_in_modality(field, 0.4f));
 * 
 * // Transform semantic → visual
 * TCDE_TransformModality(field, relations, 2, 0);  // 2=semantic, 0=visual
 * 
 * printf("After transformation: %d centers in visual modality\n",
 *        count_centers_in_modality(field, 0.0f));
 * 
 * // Verify energy conservation
 * float energy_before = TCDE_ComputeEnergy(field);
 * TCDE_TransformModality(field, relations, 0, 2);  // Back to semantic
 * float energy_after = TCDE_ComputeEnergy(field);
 * 
 * printf("Energy before: %.6f, after: %.6f, diff: %.6f\n",
 *        energy_before, energy_after, fabsf(energy_after - energy_before));
 * 
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
void TCDE_TransformModality(TCDE_Field* field, TCDE_ModalityRelations* relations,
                            int source_modality, int target_modality);

/**
 * @brief Compute cross-modal similarity using geodesic distance (Task 5.4)
 * 
 * Computes the similarity between two modalities by measuring the average
 * geodesic distance between their centers in the unified 6D field.
 * This is a fundamental TCDE principle: **geometry IS the similarity measure**.
 * 
 * @param field TCDE field containing multimodal centers (must not be NULL)
 * @param m1 First modality coordinate ∈ [0,1]:
 *           - 0.0: Visual
 *           - 0.2: Auditory
 *           - 0.4: Semantic
 *           - 0.6: Motor
 *           - 0.8: Emotional
 * @param m2 Second modality coordinate ∈ [0,1] (same scale as m1)
 * @return Similarity score ∈ [0,1] where:
 *         - 1.0: Identical modalities or very close
 *         - 0.5-0.9: Related modalities
 *         - 0.0-0.5: Distant modalities
 * 
 * **TCDE PRINCIPLE: Similarity via Geodesic Distance in Unified 6D Space**
 * 
 * Formula: S(m₁, m₂) = exp(-⟨d_g²⟩ / 2σ²)
 * 
 * where ⟨d_g²⟩ is the average squared geodesic distance between
 * centers in modality m₁ and centers in modality m₂.
 * 
 * **Key Insight:** Geometry IS the similarity measure - no hardcoded matrices!
 * The field's natural geometry determines how modalities relate.
 * 
 * @note Returns 0.0 if field is NULL or has no centers
 * @note Returns 1.0 if m1 == m2 (self-similarity)
 * @note Uses Riemannian geodesic distance, not Euclidean
 * @note Similarity is symmetric: S(m₁,m₂) = S(m₂,m₁)
 * 
 * @complexity O(N₁ × N₂) where N₁, N₂ are centers in each modality
 * 
 * @see TCDE_CrossModalCoherence(), TCDE_AddCenter6D()
 * 
 * @par Example:
 * @code
 * // Create field with multimodal centers
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * 
 * // Add visual centers (m=0.0)
 * for (int i = 0; i < 10; i++) {
 *     float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f};
 *     TCDE_Point p = TCDE_CreatePoint(6, coords);
 *     TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 *     TCDE_FreePoint(&p);
 * }
 * 
 * // Add semantic centers (m=0.4)
 * for (int i = 0; i < 10; i++) {
 *     float coords[6] = {(float)i/10.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
 *     TCDE_Point p = TCDE_CreatePoint(6, coords);
 *     TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
 *     TCDE_FreePoint(&p);
 * }
 * 
 * // Compute visual-semantic similarity
 * float similarity = TCDE_CrossModalSimilarity(field, 0.0f, 0.4f);
 * printf("Visual-Semantic similarity: %.3f\n", similarity);
 * 
 * // Compute self-similarity (should be ~1.0)
 * float self_sim = TCDE_CrossModalSimilarity(field, 0.4f, 0.4f);
 * printf("Semantic self-similarity: %.3f\n", self_sim);
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
float TCDE_CrossModalSimilarity(const TCDE_Field* field, float m1, float m2);

/**
 * @brief Compute cross-modal coherence measuring unified field integration
 * 
 * Computes the coherence of the field across modality boundaries, measuring
 * how well different modalities are integrated in the unified 6D space.
 * High coherence indicates good multimodal integration and semantic alignment.
 * 
 * **TCDE Principle: Coherence via Field Overlap in Unified Space**
 * 
 * Formula: C = ∫∫ K(m_i,m_j) |Φ(m_i)| |Φ(m_j)| dm_i dm_j / (∫|Φ|² dm)
 * 
 * where:
 * - K(m_i,m_j) is the modality similarity kernel
 * - |Φ(m_i)| is the field magnitude at modality m_i
 * - Integration is over all modality pairs
 * - Normalization ensures C ∈ [0,1]
 * 
 * Coherence interpretation:
 * - **C ≈ 1.0:** Perfect coherence - all modalities aligned
 * - **C ≈ 0.7-0.9:** Good coherence - modalities well integrated
 * - **C ≈ 0.4-0.7:** Moderate coherence - some integration
 * - **C ≈ 0.0-0.4:** Low coherence - modalities disconnected
 * 
 * Physical meaning:
 * - Measures "semantic alignment" across modalities
 * - High coherence: visual, auditory, semantic agree
 * - Low coherence: modalities contradict or disconnect
 * - Analogous to quantum coherence in physics
 * 
 * Applications:
 * - **Multimodal validation:** Verify unified processing
 * - **Quality metric:** Assess multimodal integration
 * - **Debugging:** Identify modality misalignment
 * - **Optimization:** Tune processing parameters
 * 
 * @param field TCDE field (must not be NULL)
 *              - Must contain centers in multiple modalities
 *              - Field must be properly initialized
 * @param relations Modality relations (must not be NULL)
 *                  - Provides similarity matrix K(m_i,m_j)
 *                  - Determines coherence weighting
 * @return Coherence score ∈ [0,1] where:
 *         - 1.0: Perfect coherence (all modalities aligned)
 *         - 0.7-0.9: Good coherence (well integrated)
 *         - 0.4-0.7: Moderate coherence (some integration)
 *         - 0.0-0.4: Low coherence (disconnected)
 *         - 0.0: No coherence or error
 * 
 * @note Returns 0.0 if field is NULL or has no centers
 * @note Returns 1.0 if field has only one modality (trivial coherence)
 * @note Computation is O(N²) where N = number of centers
 * @note Uses modality similarity matrix from relations
 * 
 * @warning Expensive for large fields (O(N²) complexity)
 * @warning Requires relations to be properly initialized
 * 
 * @complexity O(N²) where N = number of centers
 * 
 * @see TCDE_CrossModalSimilarity() for pairwise similarity
 * @see TCDE_CreateModalityRelations() for relations initialization
 * @see TCDE_ComputeSimilarityMatrix() for similarity computation
 * 
 * @par Example:
 * @code
 * // Create field with multimodal content
 * TCDE_Field* field = TCDE_CreateField(200, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // Process multiple modalities
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * 
 * // Load and process image
 * unsigned char* image = load_image("sunset.jpg", &width, &height, &channels);
 * TCDE_ProcessImage(field, relations, image, width, height, channels, 1.0f);
 * 
 * // Load and process audio
 * float* audio = load_audio("sunset_sounds.wav", &sample_rate, &num_samples);
 * TCDE_ProcessAudio(field, relations, audio, num_samples, sample_rate, 1.0f);
 * 
 * // Compute cross-modal coherence
 * float coherence = TCDE_CrossModalCoherence(field, relations);
 * 
 * printf("Cross-modal coherence: %.3f\n", coherence);
 * if (coherence > 0.7f) {
 *     printf("Excellent multimodal integration!\n");
 * } else if (coherence > 0.4f) {
 *     printf("Moderate multimodal integration\n");
 * } else {
 *     printf("Poor multimodal integration - check processing\n");
 * }
 * 
 * // Analyze per-modality contributions
 * for (int i = 0; i < 5; i++) {
 *     for (int j = i+1; j < 5; j++) {
 *         float m_i = i * 0.2f;
 *         float m_j = j * 0.2f;
 *         float sim = TCDE_CrossModalSimilarity(field, m_i, m_j);
 *         printf("Similarity(%s, %s): %.3f\n",
 *                TCDE_GetModalityName(m_i),
 *                TCDE_GetModalityName(m_j),
 *                sim);
 *     }
 * }
 * 
 * free(image);
 * free(audio);
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
float TCDE_CrossModalCoherence(const TCDE_Field* field, 
                                const TCDE_ModalityRelations* relations);

/**
 * @brief Generate modality-specific output from unified field representation
 * 
 * Generates output in a specific modality (text, image, audio) from the
 * unified field representation. This is the inverse of multimodal processing:
 * instead of modality → field, this performs field → modality.
 * 
 * **TCDE Principle: Field → Modality Generation**
 * 
 * Generation process:
 * 1. **Extract** centers in target modality
 * 2. **Decode** field structure to modality-specific representation
 * 3. **Synthesize** output using modality-specific generators
 * 4. **Write** to output buffer
 * 
 * Modality-specific generation:
 * - **Text (m=0.4):** Generate text from semantic field
 *   - Decode semantic centers to word embeddings
 *   - Find nearest words in vocabulary
 *   - Construct coherent sentences
 * - **Image (m=0.0):** Render visual field
 *   - Decode visual centers to pixel intensities
 *   - Reconstruct spatial structure
 *   - Generate RGB image
 * - **Audio (m=0.2):** Synthesize audio from auditory field
 *   - Decode spectro-temporal centers to STFT
 *   - Inverse STFT to time-domain signal
 *   - Generate audio samples
 * - **Motor (m=0.6):** Generate action sequence
 *   - Decode motor centers to trajectory
 *   - Reconstruct movement sequence
 * - **Emotional (m=0.8):** Generate affective state
 *   - Decode emotional centers to valence-arousal-dominance
 *   - Reconstruct emotional trajectory
 * 
 * Applications:
 * - **Text generation:** Semantic field → natural language
 * - **Image synthesis:** Visual field → rendered image
 * - **Audio synthesis:** Auditory field → sound
 * - **Cross-modal generation:** Text → Image, Image → Audio, etc.
 * - **Creative AI:** Generate novel multimodal content
 * 
 * @param field TCDE field (must not be NULL)
 *              - Must contain centers in target modality
 *              - Field must be properly initialized
 * @param relations Modality relations (must not be NULL)
 *                  - Provides modality configuration
 *                  - Used for generation parameters
 * @param modality_id Target modality ID (0-4):
 *                    - 0: Visual (generate image)
 *                    - 1: Auditory (generate audio)
 *                    - 2: Semantic (generate text)
 *                    - 3: Motor (generate trajectory)
 *                    - 4: Emotional (generate affective state)
 * @param output Output buffer (must be pre-allocated)
 *               - Format depends on modality:
 *                 - Text: char* (null-terminated string)
 *                 - Image: unsigned char* (RGB pixels)
 *                 - Audio: float* (audio samples)
 *                 - Motor: float* (trajectory points)
 *                 - Emotional: float* (valence, arousal, dominance)
 * @param output_size Output buffer size in bytes
 *                    - Must be sufficient for generated output
 *                    - Typical sizes:
 *                      - Text: 1KB-10KB
 *                      - Image: width×height×3 bytes
 *                      - Audio: num_samples×4 bytes
 *                      - Motor: num_points×12 bytes
 *                      - Emotional: 12 bytes
 * @return Number of bytes written to output buffer
 *         - 0 if generation failed or no centers in modality
 *         - > 0: actual bytes written (≤ output_size)
 *         - -1 on error (NULL pointers, invalid modality_id)
 * 
 * @note Output format depends on modality_id
 * @note Generation quality depends on field richness
 * @note May not use full output_size (returns actual bytes)
 * @note Requires sufficient centers in target modality
 * 
 * @warning Output buffer must be pre-allocated
 * @warning Buffer overflow if output_size too small
 * @warning Generation may fail if field is empty or incoherent
 * 
 * @complexity O(N × G) where N = centers in modality, G = generation cost
 * 
 * @see TCDE_ProcessText() for text → field
 * @see TCDE_ProcessImage() for image → field
 * @see TCDE_ProcessAudio() for audio → field
 * @see TCDE_TransformModality() for cross-modal transformation
 * 
 * @par Example:
 * @code
 * // Create field and process text
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * TCDE_ProcessText(field, relations, "A beautiful sunset over the ocean", 1.0f);
 * 
 * // Generate text from semantic field
 * char generated_text[1024];
 * int bytes_written = TCDE_GenerateOutput(
 *     field, relations, 2,  // 2 = semantic (text)
 *     generated_text, sizeof(generated_text)
 * );
 * 
 * if (bytes_written > 0) {
 *     printf("Generated text (%d bytes): %s\n", bytes_written, generated_text);
 * } else {
 *     printf("Text generation failed\n");
 * }
 * 
 * // Transform to visual and generate image
 * TCDE_TransformModality(field, relations, 2, 0);  // semantic → visual
 * 
 * unsigned char* image = malloc(256 * 256 * 3);  // 256x256 RGB
 * bytes_written = TCDE_GenerateOutput(
 *     field, relations, 0,  // 0 = visual (image)
 *     image, 256 * 256 * 3
 * );
 * 
 * if (bytes_written > 0) {
 *     printf("Generated image: %d bytes\n", bytes_written);
 *     save_image("generated_sunset.png", image, 256, 256, 3);
 * }
 * 
 * free(image);
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
int TCDE_GenerateOutput(const TCDE_Field* field, 
                        const TCDE_ModalityRelations* relations,
                        int modality_id, void* output, int output_size);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get modality coordinate from modality name
 * 
 * Converts a human-readable modality name to its corresponding coordinate
 * in the 6D space. This is a utility function for easier API usage.
 * 
 * **TCDE Modality Mapping:**
 * - "visual" → 0.0 (spatial, concrete, immediate perception)
 * - "auditory" → 0.2 (temporal, sequential, acoustic patterns)
 * - "semantic" → 0.4 (abstract, linguistic, conceptual)
 * - "motor" → 0.6 (embodied, action-oriented, sensorimotor)
 * - "emotional" → 0.8 (affective, valence-arousal, feeling)
 * 
 * Case-insensitive matching:
 * - "Visual", "VISUAL", "visual" all map to 0.0
 * - Partial matches supported: "vis" → 0.0, "aud" → 0.2
 * 
 * @param modality_name Modality name (must not be NULL)
 *                      - Case-insensitive
 *                      - Partial matches supported
 *                      - Examples: "visual", "auditory", "semantic"
 * @return Modality coordinate m ∈ [0,1]
 *         - 0.0: Visual
 *         - 0.2: Auditory
 *         - 0.4: Semantic
 *         - 0.6: Motor
 *         - 0.8: Emotional
 *         - -1.0: Unknown modality (error)
 * 
 * @note Case-insensitive matching
 * @note Returns -1.0 for unknown modalities
 * @note Partial matches supported for convenience
 * 
 * @complexity O(1) - simple string comparison
 * 
 * @see TCDE_GetModalityName() for inverse operation
 * @see TCDE_ProcessText(), TCDE_ProcessImage(), etc. for modality processing
 * 
 * @par Example:
 * @code
 * // Get modality coordinates
 * float m_visual = TCDE_GetModalityCoordinate("visual");
 * float m_audio = TCDE_GetModalityCoordinate("auditory");
 * float m_semantic = TCDE_GetModalityCoordinate("semantic");
 * 
 * printf("Visual: %.1f\n", m_visual);      // 0.0
 * printf("Auditory: %.1f\n", m_audio);     // 0.2
 * printf("Semantic: %.1f\n", m_semantic);  // 0.4
 * 
 * // Case-insensitive
 * float m1 = TCDE_GetModalityCoordinate("VISUAL");
 * float m2 = TCDE_GetModalityCoordinate("Visual");
 * assert(m1 == m2);  // Both return 0.0
 * 
 * // Partial matches
 * float m3 = TCDE_GetModalityCoordinate("vis");
 * assert(m3 == 0.0f);  // Matches "visual"
 * 
 * // Unknown modality
 * float m_unknown = TCDE_GetModalityCoordinate("unknown");
 * if (m_unknown < 0.0f) {
 *     printf("Unknown modality\n");
 * }
 * @endcode
 */
float TCDE_GetModalityCoordinate(const char* modality_name);

/**
 * @brief Get modality name from coordinate
 * 
 * Converts a modality coordinate to its human-readable name. This is the
 * inverse of TCDE_GetModalityCoordinate() and useful for debugging and
 * visualization.
 * 
 * **TCDE Modality Mapping:**
 * - 0.0 → "visual" (spatial, concrete, immediate perception)
 * - 0.2 → "auditory" (temporal, sequential, acoustic patterns)
 * - 0.4 → "semantic" (abstract, linguistic, conceptual)
 * - 0.6 → "motor" (embodied, action-oriented, sensorimotor)
 * - 0.8 → "emotional" (affective, valence-arousal, feeling)
 * 
 * Tolerance for floating-point comparison:
 * - Coordinates within ±0.05 of standard values are matched
 * - Example: 0.18-0.22 all map to "auditory"
 * - Allows for numerical imprecision
 * 
 * @param m Modality coordinate ∈ [0,1]
 *          - 0.0 ± 0.05: Visual
 *          - 0.2 ± 0.05: Auditory
 *          - 0.4 ± 0.05: Semantic
 *          - 0.6 ± 0.05: Motor
 *          - 0.8 ± 0.05: Emotional
 * @return Modality name (const char*)
 *         - "visual", "auditory", "semantic", "motor", "emotional"
 *         - "unknown" if m doesn't match any standard modality
 *         - String is statically allocated (do not free)
 * 
 * @note Returned string is statically allocated (do not free)
 * @note Tolerance of ±0.05 for floating-point comparison
 * @note Returns "unknown" for non-standard coordinates
 * 
 * @complexity O(1) - simple range checking
 * 
 * @see TCDE_GetModalityCoordinate() for inverse operation
 * @see TCDE_ProcessText(), TCDE_ProcessImage(), etc. for modality processing
 * 
 * @par Example:
 * @code
 * // Get modality names
 * const char* name1 = TCDE_GetModalityName(0.0f);
 * const char* name2 = TCDE_GetModalityName(0.2f);
 * const char* name3 = TCDE_GetModalityName(0.4f);
 * 
 * printf("m=0.0: %s\n", name1);  // "visual"
 * printf("m=0.2: %s\n", name2);  // "auditory"
 * printf("m=0.4: %s\n", name3);  // "semantic"
 * 
 * // Tolerance for floating-point
 * const char* name4 = TCDE_GetModalityName(0.19f);  // Close to 0.2
 * printf("m=0.19: %s\n", name4);  // "auditory" (within tolerance)
 * 
 * // Unknown modality
 * const char* name5 = TCDE_GetModalityName(0.5f);  // Between semantic and motor
 * printf("m=0.5: %s\n", name5);  // "unknown"
 * 
 * // Iterate over field centers and print modalities
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * // ... add centers ...
 * 
 * for (int i = 0; i < field->num_centers; i++) {
 *     float m = field->centers[i].coords[5];
 *     const char* modality = TCDE_GetModalityName(m);
 *     printf("Center %d: modality=%s (m=%.2f)\n", i, modality, m);
 * }
 * 
 * TCDE_DestroyField(field);
 * @endcode
 */
const char* TCDE_GetModalityName(float m);

#ifdef __cplusplus
}
#endif

#endif // TCDE_MULTIMODAL_H


// ============================================================================
// MODALITY TRANSFORMATION (Task 5.5)
// ============================================================================

/**
 * @brief Rotate field in modality dimension with geometric preservation (Task 5.5)
 * 
 * Implements continuous rotation in the m-dimension to transform field
 * from one modality to another. This is a **diffeomorphism** that preserves
 * field structure while enabling smooth cross-modal transitions.
 * 
 * **TCDE Principle: Continuous Geometric Transformation**
 * 
 * This is NOT discrete mapping - it's continuous rotation in modality space:
 * - ❌ NOT: Discrete modality switching
 * - ❌ NOT: Hardcoded transformation rules
 * - ✅ YES: Continuous rotation in m-dimension
 * - ✅ YES: Structure-preserving diffeomorphism
 * - ✅ YES: Smooth geometric transformation
 * 
 * Transformation formula:
 * - m → m' = m + Δm where Δm = target_modality - source_modality
 * - Wrapping at boundaries: m' = mod(m', 1.0)
 * - All centers rotated uniformly
 * 
 * Diffeomorphism properties:
 * - **Smooth:** Infinitely differentiable
 * - **Bijective:** One-to-one and onto
 * - **Invertible:** Can be reversed
 * - **Structure-preserving:** Maintains geometric relationships
 * 
 * Energy preservation:
 * - If preserve_energy=true: Normalize coefficients to maintain ∫|Φ|²dV
 * - If preserve_energy=false: Coefficients unchanged (energy may vary)
 * - Useful for maintaining field intensity across transformations
 * 
 * Applications:
 * - **Text → Image:** Semantic concepts to visual representations
 * - **Audio → Visual:** Sound to visual patterns (synesthesia)
 * - **Cross-modal generation:** Generate one modality from another
 * - **Modality interpolation:** Smooth transitions between modalities
 * - **Creative AI:** Novel cross-modal content generation
 * 
 * @param field Field to transform (must not be NULL, modified in place)
 *              - All centers will be rotated in m-dimension
 *              - Field structure preserved during transformation
 * @param source_modality Source modality coordinate ∈ [0,1]
 *                        - Only centers near this modality are affected
 *                        - Tolerance: ±0.1 around source_modality
 * @param target_modality Target modality coordinate ∈ [0,1]
 *                        - Final modality after rotation
 *                        - Can be any value in [0,1]
 * @param preserve_energy If true, normalize to preserve total energy
 *                        - true: ∫|Φ'|²dV = ∫|Φ|²dV (energy conserved)
 *                        - false: Coefficients unchanged (energy may vary)
 * @return true on successful transformation, false on error
 *         - false if field is NULL
 *         - false if source_modality or target_modality out of range
 *         - true if transformation completed successfully
 * 
 * @note Transformation is continuous and smooth (diffeomorphism)
 * @note Field structure is preserved (geometric relationships maintained)
 * @note Energy can optionally be preserved (useful for intensity matching)
 * @note Only centers near source_modality are affected (tolerance ±0.1)
 * 
 * @warning Modifies field in place (original modality lost)
 * @warning Large rotations may reduce semantic fidelity
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_ModalityInterpolate() for gradual transitions
 * @see TCDE_ModalityVerifyTransform() for validation
 * @see TCDE_TransformModality() for high-level transformation
 * 
 * @par Example:
 * @code
 * // Create field with semantic content
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * // Process text (semantic modality, m=0.4)
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * 
 * // Measure energy before transformation
 * float energy_before = TCDE_ComputeEnergy(field);
 * printf("Energy before: %.6f\n", energy_before);
 * 
 * // Rotate semantic → visual (0.4 → 0.0)
 * bool success = TCDE_ModalityRotate(field, 0.4f, 0.0f, true);
 * 
 * if (success) {
 *     printf("Transformation successful!\n");
 *     
 *     // Verify energy preservation
 *     float energy_after = TCDE_ComputeEnergy(field);
 *     printf("Energy after: %.6f\n", energy_after);
 *     printf("Energy difference: %.6f\n", fabsf(energy_after - energy_before));
 *     
 *     // Count centers in visual modality
 *     int visual_count = 0;
 *     for (int i = 0; i < field->num_centers; i++) {
 *         if (fabsf(field->centers[i].coords[5] - 0.0f) < 0.1f) {
 *             visual_count++;
 *         }
 *     }
 *     printf("Centers in visual modality: %d/%d\n", 
 *            visual_count, field->num_centers);
 * } else {
 *     printf("Transformation failed\n");
 * }
 * 
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
bool TCDE_ModalityRotate(TCDE_Field* field, float source_modality,
                         float target_modality, bool preserve_energy);

/**
 * @brief Interpolate field between two modalities for smooth transitions
 * 
 * Creates a smooth interpolation between source and target modalities
 * using parameter t ∈ [0,1]. This enables gradual, continuous transitions
 * between modalities rather than abrupt jumps.
 * 
 * **TCDE Principle: Smooth Modality Interpolation**
 * 
 * Interpolation formula:
 * - m(t) = (1-t)·m_source + t·m_target
 * - At t=0: m = m_source (fully in source modality)
 * - At t=0.5: m = (m_source + m_target)/2 (halfway between)
 * - At t=1: m = m_target (fully in target modality)
 * 
 * This is linear interpolation (lerp) in modality space, creating
 * smooth transitions that preserve field continuity.
 * 
 * Use cases:
 * - **Gradual transformation:** Smooth transition over multiple steps
 * - **Animation:** Animate modality changes over time
 * - **Exploration:** Explore intermediate modality states
 * - **Debugging:** Visualize transformation process
 * - **Creative effects:** Blend modalities for artistic purposes
 * 
 * Comparison with TCDE_ModalityRotate:
 * - **ModalityRotate:** Single-step transformation (t=1)
 * - **ModalityInterpolate:** Multi-step transformation (any t)
 * - **ModalityInterpolate:** More control over transition speed
 * - **ModalityRotate:** Simpler API for complete transformations
 * 
 * @param field Field to interpolate (must not be NULL, modified in place)
 *              - Centers near source_modality will be interpolated
 *              - Field structure preserved during interpolation
 * @param source_modality Source modality coordinate ∈ [0,1]
 *                        - Starting point of interpolation
 *                        - Only centers near this modality affected
 * @param target_modality Target modality coordinate ∈ [0,1]
 *                        - Ending point of interpolation
 *                        - Final modality when t=1
 * @param t Interpolation parameter ∈ [0,1]
 *          - 0.0: Fully in source modality
 *          - 0.5: Halfway between source and target
 *          - 1.0: Fully in target modality
 *          - Values outside [0,1] are clamped
 * @return true on successful interpolation, false on error
 *         - false if field is NULL
 *         - false if source_modality or target_modality out of range
 *         - true if interpolation completed successfully
 * 
 * @note Interpolation is linear in modality coordinate
 * @note Field structure preserved during interpolation
 * @note Only centers near source_modality are affected
 * @note t is clamped to [0,1] if outside range
 * 
 * @warning Modifies field in place
 * @warning Multiple calls with different t values will compound
 * 
 * @complexity O(K) where K = number of centers
 * 
 * @see TCDE_ModalityRotate() for single-step transformation
 * @see TCDE_ModalityVerifyTransform() for validation
 * 
 * @par Example:
 * @code
 * // Create field with semantic content
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * 
 * // Animate transformation from semantic (0.4) to visual (0.0)
 * printf("Animating semantic → visual transformation:\n");
 * 
 * for (int frame = 0; frame <= 10; frame++) {
 *     float t = frame / 10.0f;  // 0.0 to 1.0
 *     
 *     // Reset field to original state (in practice, save/restore)
 *     // ... restore original field ...
 *     
 *     // Interpolate to current frame
 *     TCDE_ModalityInterpolate(field, 0.4f, 0.0f, t);
 *     
 *     // Compute current modality
 *     float m_current = (1.0f - t) * 0.4f + t * 0.0f;
 *     
 *     printf("Frame %2d: t=%.2f, m=%.2f (%s)\n",
 *            frame, t, m_current, TCDE_GetModalityName(m_current));
 *     
 *     // Render or save frame
 *     // ... visualization code ...
 * }
 * 
 * // Example output:
 * // Frame  0: t=0.00, m=0.40 (semantic)
 * // Frame  1: t=0.10, m=0.36 (semantic)
 * // Frame  2: t=0.20, m=0.32 (semantic)
 * // ...
 * // Frame  8: t=0.80, m=0.08 (visual)
 * // Frame  9: t=0.90, m=0.04 (visual)
 * // Frame 10: t=1.00, m=0.00 (visual)
 * 
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
bool TCDE_ModalityInterpolate(TCDE_Field* field, float source_modality,
                               float target_modality, float t);

/**
 * @brief Verify modality transformation preserves field properties
 * 
 * Tests that a modality transformation preserves essential field properties,
 * ensuring the transformation is a valid diffeomorphism that maintains
 * geometric structure and physical constraints.
 * 
 * **TCDE Principle: Transformation Validation**
 * 
 * Verified properties:
 * 1. **Energy preservation:** ∫|Φ'|²dV ≈ ∫|Φ|²dV (if requested)
 * 2. **Field coherence:** Coherence maintained across transformation
 * 3. **Geometric structure:** Relative distances preserved
 * 4. **Center count:** Number of centers unchanged
 * 5. **Coefficient magnitudes:** Overall magnitude distribution preserved
 * 
 * Energy preservation test:
 * - Compute E_before = Σ|c_i|²
 * - Compute E_after = Σ|c'_i|²
 * - Verify |E_after - E_before| / E_before < tolerance
 * 
 * Coherence preservation test:
 * - Compute coherence before and after
 * - Verify relative change < tolerance
 * 
 * Structure preservation test:
 * - Sample pairwise distances before and after
 * - Verify distances preserved (modulo modality rotation)
 * 
 * Use cases:
 * - **Testing:** Validate transformation implementations
 * - **Debugging:** Identify transformation errors
 * - **Quality assurance:** Ensure transformations are valid
 * - **Research:** Verify theoretical properties experimentally
 * 
 * @param field_before Field before transformation (must not be NULL)
 *                     - Original field state
 *                     - Used as reference for comparison
 * @param field_after Field after transformation (must not be NULL)
 *                    - Transformed field state
 *                    - Compared against field_before
 * @param tolerance Tolerance for comparison (typically 1e-3 to 1e-6)
 *                  - Relative error threshold
 *                  - Smaller = stricter validation
 *                  - Typical: 1e-4 for single precision
 * @return true if all properties preserved within tolerance, false otherwise
 *         - true: Transformation is valid diffeomorphism
 *         - false: Transformation violated some property
 *         - false if either field is NULL
 * 
 * @note Checks multiple properties (energy, coherence, structure)
 * @note Tolerance is relative error, not absolute
 * @note Useful for testing and validation
 * @note Does not modify either field
 * 
 * @warning Expensive operation (O(K²) for structure check)
 * @warning Use only for testing/validation, not production
 * 
 * @complexity O(K²) where K = number of centers (structure check dominates)
 * 
 * @see TCDE_ModalityRotate() for transformation
 * @see TCDE_ModalityInterpolate() for gradual transformation
 * @see TCDE_ComputeEnergy() for energy computation
 * 
 * @par Example:
 * @code
 * // Create field and save original state
 * TCDE_Field* field = TCDE_CreateField(100, 2.5f);
 * TCDE_ModalityRelations* relations = TCDE_CreateModalityRelations();
 * 
 * TCDE_ProcessText(field, relations, "A beautiful sunset", 1.0f);
 * 
 * // Save original state
 * TCDE_Field* field_original = TCDE_CloneField(field);
 * 
 * // Perform transformation
 * TCDE_ModalityRotate(field, 0.4f, 0.0f, true);  // semantic → visual
 * 
 * // Verify transformation preserves properties
 * bool valid = TCDE_ModalityVerifyTransform(field_original, field, 1e-4f);
 * 
 * if (valid) {
 *     printf("✓ Transformation is valid diffeomorphism\n");
 *     printf("  - Energy preserved\n");
 *     printf("  - Coherence maintained\n");
 *     printf("  - Structure preserved\n");
 * } else {
 *     printf("✗ Transformation violated some property\n");
 *     
 *     // Detailed diagnostics
 *     float E_before = TCDE_ComputeEnergy(field_original);
 *     float E_after = TCDE_ComputeEnergy(field);
 *     float energy_error = fabsf(E_after - E_before) / E_before;
 *     
 *     printf("  Energy before: %.6f\n", E_before);
 *     printf("  Energy after:  %.6f\n", E_after);
 *     printf("  Relative error: %.6f (tolerance: %.6f)\n", 
 *            energy_error, 1e-4f);
 * }
 * 
 * // Test round-trip transformation
 * TCDE_ModalityRotate(field, 0.0f, 0.4f, true);  // visual → semantic (back)
 * 
 * bool round_trip_valid = TCDE_ModalityVerifyTransform(
 *     field_original, field, 1e-3f  // Slightly looser tolerance
 * );
 * 
 * if (round_trip_valid) {
 *     printf("✓ Round-trip transformation successful\n");
 * } else {
 *     printf("✗ Round-trip transformation accumulated errors\n");
 * }
 * 
 * TCDE_DestroyField(field_original);
 * TCDE_DestroyField(field);
 * TCDE_FreeModalityRelations(relations);
 * @endcode
 */
bool TCDE_ModalityVerifyTransform(const TCDE_Field* field_before,
                                   const TCDE_Field* field_after,
                                   float tolerance);

