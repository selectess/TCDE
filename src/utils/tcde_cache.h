/**
 * @file tcde_cache.h
 * @brief TCDE Distance Cache - LRU Cache for Geodesic Distances
 * 
 * Implements an LRU (Least Recently Used) cache for geodesic distance calculations.
 * Significantly speeds up field evaluation by avoiding repeated distance computations.
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#ifndef TCDE_CACHE_H
#define TCDE_CACHE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// CACHE STRUCTURES
// ============================================================================

/**
 * @brief Cache entry for distance calculation
 */
typedef struct TCDE_CacheEntry {
    int center_idx;                 ///< Center index
    float point_coords[6];          ///< Query point coordinates
    float distance;                 ///< Cached distance
    struct TCDE_CacheEntry* prev;   ///< Previous in LRU list
    struct TCDE_CacheEntry* next;   ///< Next in LRU list
    struct TCDE_CacheEntry* hash_next; ///< Next in hash bucket
} TCDE_CacheEntry;

/**
 * @brief LRU cache for geodesic distances
 */
typedef struct {
    TCDE_CacheEntry** hash_table;   ///< Hash table for O(1) lookup
    int hash_size;                  ///< Hash table size
    TCDE_CacheEntry* lru_head;      ///< Most recently used
    TCDE_CacheEntry* lru_tail;      ///< Least recently used
    TCDE_CacheEntry* entries;       ///< Pre-allocated entry pool
    int capacity;                   ///< Maximum entries
    int size;                       ///< Current entries
    
    // Statistics
    unsigned long hits;             ///< Cache hits
    unsigned long misses;           ///< Cache misses
} TCDE_DistanceCache;

// ============================================================================
// CACHE OPERATIONS
// ============================================================================

/**
 * @brief Create distance cache
 * @param capacity Maximum number of cached distances
 * @return Cache pointer, or NULL on error
 * 
 * Typical capacity: 1000-10000 entries
 * Memory usage: ~100 bytes per entry
 */
TCDE_DistanceCache* TCDE_CreateDistanceCache(int capacity);

/**
 * @brief Destroy distance cache
 * @param cache Cache to destroy
 */
void TCDE_DestroyDistanceCache(TCDE_DistanceCache* cache);

/**
 * @brief Look up distance in cache
 * @param cache Cache
 * @param center_idx Center index
 * @param point Query point coordinates (6D)
 * @param distance Output distance (if found)
 * @return true if found, false if not in cache
 * 
 * Complexity: O(1) average case
 */
bool TCDE_CacheLookup(TCDE_DistanceCache* cache,
                      int center_idx,
                      const float point[6],
                      float* distance);

/**
 * @brief Insert distance into cache
 * @param cache Cache
 * @param center_idx Center index
 * @param point Query point coordinates (6D)
 * @param distance Distance to cache
 * 
 * If cache is full, evicts least recently used entry.
 * Complexity: O(1)
 */
void TCDE_CacheInsert(TCDE_DistanceCache* cache,
                      int center_idx,
                      const float point[6],
                      float distance);

/**
 * @brief Clear all cache entries
 * @param cache Cache to clear
 */
void TCDE_CacheClear(TCDE_DistanceCache* cache);

/**
 * @brief Get cache statistics
 * @param cache Cache
 * @param hit_rate Output hit rate [0,1]
 * @param size Output current size
 * @param capacity Output capacity
 */
void TCDE_CacheStats(const TCDE_DistanceCache* cache,
                     float* hit_rate,
                     int* size,
                     int* capacity);

/**
 * @brief Print cache statistics
 * @param cache Cache
 */
void TCDE_PrintCacheStats(const TCDE_DistanceCache* cache);

#ifdef __cplusplus
}
#endif

#endif // TCDE_CACHE_H
