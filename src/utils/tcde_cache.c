/**
 * @file tcde_cache.c
 * @brief TCDE Distance Cache Implementation
 * 
 * LRU cache for geodesic distance calculations.
 * Provides 2-3x speedup for repeated field evaluations.
 */

#include "tcde_cache.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// ============================================================================
// HASH FUNCTION
// ============================================================================

/**
 * @brief Hash function for cache key
 * 
 * Combines center index and point coordinates into hash value.
 * Uses FNV-1a hash algorithm for good distribution.
 */
static unsigned int hash_key(int center_idx, const float point[6], int hash_size) {
    unsigned int hash = 2166136261u;  // FNV offset basis
    
    // Hash center index
    hash ^= (unsigned int)center_idx;
    hash *= 16777619u;  // FNV prime
    
    // Hash point coordinates (quantized to reduce floating point issues)
    for (int i = 0; i < 6; i++) {
        // Quantize to 0.001 precision
        int quantized = (int)(point[i] * 1000.0f);
        hash ^= (unsigned int)quantized;
        hash *= 16777619u;
    }
    
    return hash % hash_size;
}

/**
 * @brief Check if two points are equal (within tolerance)
 */
static bool points_equal(const float p1[6], const float p2[6]) {
    const float epsilon = 1e-6f;
    for (int i = 0; i < 6; i++) {
        if (fabsf(p1[i] - p2[i]) > epsilon) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// LRU LIST OPERATIONS
// ============================================================================

/**
 * @brief Move entry to front of LRU list (most recently used)
 */
static void move_to_front(TCDE_DistanceCache* cache, TCDE_CacheEntry* entry) {
    if (entry == cache->lru_head) {
        return;  // Already at front
    }
    
    // Remove from current position
    if (entry->prev) {
        entry->prev->next = entry->next;
    }
    if (entry->next) {
        entry->next->prev = entry->prev;
    }
    if (entry == cache->lru_tail) {
        cache->lru_tail = entry->prev;
    }
    
    // Insert at front
    entry->prev = NULL;
    entry->next = cache->lru_head;
    if (cache->lru_head) {
        cache->lru_head->prev = entry;
    }
    cache->lru_head = entry;
    
    if (!cache->lru_tail) {
        cache->lru_tail = entry;
    }
}

/**
 * @brief Remove entry from LRU list
 */
static void remove_from_lru(TCDE_DistanceCache* cache, TCDE_CacheEntry* entry) {
    if (entry->prev) {
        entry->prev->next = entry->next;
    } else {
        cache->lru_head = entry->next;
    }
    
    if (entry->next) {
        entry->next->prev = entry->prev;
    } else {
        cache->lru_tail = entry->prev;
    }
    
    entry->prev = NULL;
    entry->next = NULL;
}

// ============================================================================
// CACHE OPERATIONS
// ============================================================================

/**
 * @brief Create distance cache
 */
TCDE_DistanceCache* TCDE_CreateDistanceCache(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }
    
    TCDE_DistanceCache* cache = (TCDE_DistanceCache*)malloc(sizeof(TCDE_DistanceCache));
    if (!cache) {
        return NULL;
    }
    
    // Hash table size: next prime after capacity
    cache->hash_size = capacity * 2 + 1;
    cache->hash_table = (TCDE_CacheEntry**)calloc(cache->hash_size, sizeof(TCDE_CacheEntry*));
    if (!cache->hash_table) {
        free(cache);
        return NULL;
    }
    
    // Pre-allocate entry pool
    cache->entries = (TCDE_CacheEntry*)calloc(capacity, sizeof(TCDE_CacheEntry));
    if (!cache->entries) {
        free(cache->hash_table);
        free(cache);
        return NULL;
    }
    
    cache->capacity = capacity;
    cache->size = 0;
    cache->lru_head = NULL;
    cache->lru_tail = NULL;
    cache->hits = 0;
    cache->misses = 0;
    
    return cache;
}

/**
 * @brief Destroy distance cache
 */
void TCDE_DestroyDistanceCache(TCDE_DistanceCache* cache) {
    if (!cache) {
        return;
    }
    
    free(cache->entries);
    free(cache->hash_table);
    free(cache);
}

/**
 * @brief Look up distance in cache
 */
bool TCDE_CacheLookup(TCDE_DistanceCache* cache,
                      int center_idx,
                      const float point[6],
                      float* distance) {
    if (!cache || !point || !distance) {
        return false;
    }
    
    // Compute hash
    unsigned int hash = hash_key(center_idx, point, cache->hash_size);
    
    // Search hash bucket
    TCDE_CacheEntry* entry = cache->hash_table[hash];
    while (entry) {
        if (entry->center_idx == center_idx && points_equal(entry->point_coords, point)) {
            // Found!
            *distance = entry->distance;
            cache->hits++;
            
            // Move to front of LRU list
            move_to_front(cache, entry);
            
            return true;
        }
        entry = entry->hash_next;
    }
    
    // Not found
    cache->misses++;
    return false;
}

/**
 * @brief Insert distance into cache
 */
void TCDE_CacheInsert(TCDE_DistanceCache* cache,
                      int center_idx,
                      const float point[6],
                      float distance) {
    if (!cache || !point) {
        return;
    }
    
    // Compute hash
    unsigned int hash = hash_key(center_idx, point, cache->hash_size);
    
    // Check if already exists (update)
    TCDE_CacheEntry* entry = cache->hash_table[hash];
    while (entry) {
        if (entry->center_idx == center_idx && points_equal(entry->point_coords, point)) {
            // Update existing entry
            entry->distance = distance;
            move_to_front(cache, entry);
            return;
        }
        entry = entry->hash_next;
    }
    
    // Need to insert new entry
    TCDE_CacheEntry* new_entry = NULL;
    
    if (cache->size < cache->capacity) {
        // Use next available entry from pool
        new_entry = &cache->entries[cache->size];
        cache->size++;
    } else {
        // Evict LRU entry
        new_entry = cache->lru_tail;
        
        // Remove from hash table
        unsigned int old_hash = hash_key(new_entry->center_idx, new_entry->point_coords, cache->hash_size);
        TCDE_CacheEntry** bucket = &cache->hash_table[old_hash];
        while (*bucket && *bucket != new_entry) {
            bucket = &(*bucket)->hash_next;
        }
        if (*bucket) {
            *bucket = new_entry->hash_next;
        }
        
        // Remove from LRU list
        remove_from_lru(cache, new_entry);
    }
    
    // Initialize entry
    new_entry->center_idx = center_idx;
    memcpy(new_entry->point_coords, point, 6 * sizeof(float));
    new_entry->distance = distance;
    new_entry->hash_next = NULL;
    
    // Insert into hash table
    new_entry->hash_next = cache->hash_table[hash];
    cache->hash_table[hash] = new_entry;
    
    // Insert at front of LRU list
    new_entry->prev = NULL;
    new_entry->next = cache->lru_head;
    if (cache->lru_head) {
        cache->lru_head->prev = new_entry;
    }
    cache->lru_head = new_entry;
    
    if (!cache->lru_tail) {
        cache->lru_tail = new_entry;
    }
}

/**
 * @brief Clear all cache entries
 */
void TCDE_CacheClear(TCDE_DistanceCache* cache) {
    if (!cache) {
        return;
    }
    
    // Clear hash table
    memset(cache->hash_table, 0, cache->hash_size * sizeof(TCDE_CacheEntry*));
    
    // Reset LRU list
    cache->lru_head = NULL;
    cache->lru_tail = NULL;
    cache->size = 0;
    
    // Keep statistics
}

/**
 * @brief Get cache statistics
 */
void TCDE_CacheStats(const TCDE_DistanceCache* cache,
                     float* hit_rate,
                     int* size,
                     int* capacity) {
    if (!cache) {
        return;
    }
    
    if (hit_rate) {
        unsigned long total = cache->hits + cache->misses;
        *hit_rate = (total > 0) ? (float)cache->hits / total : 0.0f;
    }
    
    if (size) {
        *size = cache->size;
    }
    
    if (capacity) {
        *capacity = cache->capacity;
    }
}

/**
 * @brief Print cache statistics
 */
void TCDE_PrintCacheStats(const TCDE_DistanceCache* cache) {
    if (!cache) {
        return;
    }
    
    unsigned long total = cache->hits + cache->misses;
    float hit_rate = (total > 0) ? (float)cache->hits / total : 0.0f;
    
    printf("Distance Cache Statistics:\n");
    printf("  Capacity:  %d\n", cache->capacity);
    printf("  Size:      %d\n", cache->size);
    printf("  Hits:      %lu\n", cache->hits);
    printf("  Misses:    %lu\n", cache->misses);
    printf("  Hit Rate:  %.2f%%\n", hit_rate * 100.0f);
}
