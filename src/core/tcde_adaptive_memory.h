/**
 * @file tcde_adaptive_memory.h
 * @brief TCDE Adaptive Memory Module - Unlimited Scalability
 *
 * Enables ASI-level capability: Unlimited memory through intelligent compression,
 * hierarchical organization, selective forgetting, and associative consolidation.
 *
 * The system can handle arbitrarily large amounts of information by:
 * - Compressing redundant information
 * - Organizing memory hierarchically
 * - Forgetting selectively based on importance
 * - Consolidating related memories
 *
 * Memory Hierarchy:
 * - Level 0: Working Memory (short-term, volatile)
 * - Level 1: Episodic Memory (medium-term, events)
 * - Level 2: Semantic Memory (long-term, concepts)
 * - Level 3: Procedural Memory (permanent, skills)
 *
 * @version 1.0
 * @date January 17, 2025
 */

#ifndef TCDE_ADAPTIVE_MEMORY_H
#define TCDE_ADAPTIVE_MEMORY_H

#include "tcde_core.h"
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MEMORY HIERARCHY LEVELS
// ============================================================================

/**
 * @brief Memory hierarchy levels
 */
typedef enum {
    TCDE_MEMORY_WORKING = 0,    // Short-term, volatile (seconds to minutes)
    TCDE_MEMORY_EPISODIC = 1,   // Medium-term, events (hours to days)
    TCDE_MEMORY_SEMANTIC = 2,   // Long-term, concepts (weeks to years)
    TCDE_MEMORY_PROCEDURAL = 3  // Permanent, skills (lifetime)
} TCDE_MemoryLevel;

// ============================================================================
// MEMORY STRUCTURES
// ============================================================================

/**
 * @brief Memory trace - individual memory unit
 */
typedef struct {
    TCDE_Point location;        // Location in cognitive space
    TCDE_Complex content;       // Memory content
    float strength;             // Memory strength [0,1]
    float importance;           // Importance score [0,1]
    float access_count;         // Number of accesses
    time_t creation_time;       // When created
    time_t last_access;         // Last access time
    TCDE_MemoryLevel level;     // Current hierarchy level
    int cluster_id;             // Cluster membership
    bool is_consolidated;       // Consolidated flag
} TCDE_MemoryTrace;

/**
 * @brief Memory cluster - group of related memories
 */
typedef struct {
    int cluster_id;             // Unique cluster ID
    TCDE_Point centroid;        // Cluster center
    int* trace_indices;         // Indices of traces in cluster
    int num_traces;             // Number of traces
    int capacity;               // Allocated capacity
    float coherence;            // Cluster coherence
    float importance;           // Cluster importance
    TCDE_MemoryLevel level;     // Hierarchy level
} TCDE_MemoryCluster;

/**
 * @brief Memory compression state
 */
typedef struct {
    float compression_ratio;    // Current compression ratio
    int original_size;          // Original memory size
    int compressed_size;        // Compressed size
    int num_compressions;       // Number of compressions performed
    float information_loss;     // Information loss [0,1]
} TCDE_CompressionState;

/**
 * @brief Forgetting policy
 */
typedef enum {
    TCDE_FORGET_NONE,           // No forgetting
    TCDE_FORGET_LRU,            // Least Recently Used
    TCDE_FORGET_LFU,            // Least Frequently Used
    TCDE_FORGET_IMPORTANCE,     // Lowest importance
    TCDE_FORGET_ADAPTIVE        // Adaptive based on multiple factors
} TCDE_ForgettingPolicy;

/**
 * @brief Adaptive memory manager
 */
typedef struct {
    // Memory traces
    TCDE_MemoryTrace* traces;   // Array of memory traces
    int num_traces;             // Current number of traces
    int capacity;               // Maximum capacity
    
    // Memory clusters
    TCDE_MemoryCluster* clusters; // Array of clusters
    int num_clusters;           // Current number of clusters
    int max_clusters;           // Maximum clusters
    
    // Hierarchy levels
    int traces_per_level[4];    // Traces at each level
    float level_thresholds[4];  // Promotion thresholds
    
    // Compression
    TCDE_CompressionState compression;
    bool compression_enabled;   // Compression active
    float compression_threshold; // Threshold for compression
    
    // Forgetting
    TCDE_ForgettingPolicy forgetting_policy;
    float forgetting_rate;      // Rate of forgetting [0,1]
    float importance_threshold; // Minimum importance to keep
    
    // Consolidation
    bool consolidation_enabled; // Consolidation active
    float consolidation_threshold; // Similarity threshold
    int consolidation_cycles;   // Number of consolidations
    
    // Statistics
    int total_accesses;         // Total memory accesses
    int cache_hits;             // Cache hits
    int cache_misses;           // Cache misses
    float average_retrieval_time; // Average retrieval time
    
    // Control
    float current_time;         // Current system time
    bool adaptive_mode;         // Adaptive memory management
} TCDE_AdaptiveMemoryManager;

// ============================================================================
// MEMORY MANAGEMENT API
// ============================================================================

/**
 * @brief Create adaptive memory manager
 * @param initial_capacity Initial memory capacity
 * @param max_clusters Maximum number of clusters
 * @return Initialized memory manager
 */
TCDE_AdaptiveMemoryManager* TCDE_CreateMemoryManager(
    int initial_capacity,
    int max_clusters);

/**
 * @brief Destroy memory manager
 * @param manager Manager to destroy
 */
void TCDE_DestroyMemoryManager(TCDE_AdaptiveMemoryManager* manager);

/**
 * @brief Store new memory trace
 * @param manager Memory manager
 * @param location Location in cognitive space
 * @param content Memory content
 * @param importance Initial importance
 * @return True on success
 */
bool TCDE_StoreMemory(TCDE_AdaptiveMemoryManager* manager,
                     const TCDE_Point* location,
                     TCDE_Complex content,
                     float importance);

/**
 * @brief Retrieve memory at location
 * @param manager Memory manager
 * @param location Query location
 * @param radius Search radius
 * @return Retrieved memory content
 */
TCDE_Complex TCDE_RetrieveMemory(TCDE_AdaptiveMemoryManager* manager,
                                 const TCDE_Point* location,
                                 float radius);

/**
 * @brief Associative memory retrieval
 * @param manager Memory manager
 * @param query Query pattern
 * @param results Output array for results
 * @param max_results Maximum results to return
 * @return Number of results found
 */
int TCDE_AssociativeRetrieval(TCDE_AdaptiveMemoryManager* manager,
                              const TCDE_Point* query,
                              TCDE_MemoryTrace* results,
                              int max_results);

// ============================================================================
// MEMORY COMPRESSION
// ============================================================================

/**
 * @brief Compress memory by merging similar traces
 * @param manager Memory manager
 * @param similarity_threshold Threshold for merging
 * @return Number of traces compressed
 */
int TCDE_CompressMemory(TCDE_AdaptiveMemoryManager* manager,
                       float similarity_threshold);

/**
 * @brief Compute compression ratio
 * @param manager Memory manager
 * @return Compression ratio
 */
float TCDE_ComputeCompressionRatio(const TCDE_AdaptiveMemoryManager* manager);

// ============================================================================
// HIERARCHICAL ORGANIZATION
// ============================================================================

/**
 * @brief Promote memory to higher level
 * @param manager Memory manager
 * @param trace_index Index of trace to promote
 * @return True if promoted
 */
bool TCDE_PromoteMemory(TCDE_AdaptiveMemoryManager* manager,
                       int trace_index);

/**
 * @brief Demote memory to lower level
 * @param manager Memory manager
 * @param trace_index Index of trace to demote
 * @return True if demoted
 */
bool TCDE_DemoteMemory(TCDE_AdaptiveMemoryManager* manager,
                      int trace_index);

/**
 * @brief Update memory hierarchy based on access patterns
 * @param manager Memory manager
 */
void TCDE_UpdateMemoryHierarchy(TCDE_AdaptiveMemoryManager* manager);

// ============================================================================
// SELECTIVE FORGETTING
// ============================================================================

/**
 * @brief Apply forgetting policy
 * @param manager Memory manager
 * @param num_to_forget Number of traces to forget
 * @return Number of traces actually forgotten
 */
int TCDE_ApplyForgetting(TCDE_AdaptiveMemoryManager* manager,
                        int num_to_forget);

/**
 * @brief Compute forgetting probability for trace
 * @param manager Memory manager
 * @param trace_index Index of trace
 * @return Forgetting probability [0,1]
 */
float TCDE_ComputeForgettingProbability(const TCDE_AdaptiveMemoryManager* manager,
                                       int trace_index);

/**
 * @brief Set forgetting policy
 * @param manager Memory manager
 * @param policy Forgetting policy
 * @param rate Forgetting rate
 */
void TCDE_SetForgettingPolicy(TCDE_AdaptiveMemoryManager* manager,
                             TCDE_ForgettingPolicy policy,
                             float rate);

// ============================================================================
// MEMORY CONSOLIDATION
// ============================================================================

/**
 * @brief Consolidate related memories into clusters
 * @param manager Memory manager
 * @return Number of clusters formed
 */
int TCDE_ConsolidateMemories(TCDE_AdaptiveMemoryManager* manager);

/**
 * @brief Compute similarity between two memory traces
 * @param trace1 First trace
 * @param trace2 Second trace
 * @return Similarity score [0,1]
 */
float TCDE_ComputeMemorySimilarity(const TCDE_MemoryTrace* trace1,
                                   const TCDE_MemoryTrace* trace2);

/**
 * @brief Update memory clusters
 * @param manager Memory manager
 */
void TCDE_UpdateMemoryClusters(TCDE_AdaptiveMemoryManager* manager);

// ============================================================================
// MEMORY STATISTICS
// ============================================================================

/**
 * @brief Compute memory efficiency
 * @param manager Memory manager
 * @return Efficiency score [0,1]
 */
float TCDE_ComputeMemoryEfficiency(const TCDE_AdaptiveMemoryManager* manager);

/**
 * @brief Get memory usage statistics
 * @param manager Memory manager
 * @param level Memory level (or -1 for all)
 * @return Number of traces at level
 */
int TCDE_GetMemoryUsage(const TCDE_AdaptiveMemoryManager* manager,
                       int level);

/**
 * @brief Print memory status
 * @param manager Memory manager
 */
void TCDE_PrintMemoryStatus(const TCDE_AdaptiveMemoryManager* manager);

/**
 * @brief Save memory state to file
 * @param manager Memory manager
 * @param filename Output filename
 * @return True on success
 */
bool TCDE_SaveMemoryState(const TCDE_AdaptiveMemoryManager* manager,
                         const char* filename);

/**
 * @brief Load memory state from file
 * @param manager Memory manager
 * @param filename Input filename
 * @return True on success
 */
bool TCDE_LoadMemoryState(TCDE_AdaptiveMemoryManager* manager,
                         const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_ADAPTIVE_MEMORY_H
