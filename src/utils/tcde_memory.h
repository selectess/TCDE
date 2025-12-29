/**
 * @file tcde_memory.h
 * @brief TCDE Memory Management - Pools and Arenas
 * 
 * Optimized memory allocation using pools for fixed-size objects
 * and arenas for temporary allocations. Reduces allocation overhead
 * and improves cache locality.
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#ifndef TCDE_MEMORY_H
#define TCDE_MEMORY_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MEMORY POOL (Fixed-size allocations)
// ============================================================================

/**
 * @brief Memory pool for fixed-size allocations
 * 
 * Efficient allocation of many objects of the same size.
 * O(1) allocation and deallocation.
 */
typedef struct {
    void* memory;               ///< Pre-allocated memory block
    void** free_list;           ///< Free list (stack)
    int free_count;             ///< Number of free blocks
    size_t block_size;          ///< Size of each block
    int capacity;               ///< Total blocks
    int allocated;              ///< Currently allocated blocks
} TCDE_MemoryPool;

/**
 * @brief Create memory pool
 * @param block_size Size of each block (bytes)
 * @param capacity Number of blocks
 * @return Pool pointer, or NULL on error
 * 
 * Example:
 * ```c
 * // Pool for 1000 centers
 * TCDE_MemoryPool* pool = TCDE_CreateMemoryPool(sizeof(TCDE_Center), 1000);
 * ```
 */
TCDE_MemoryPool* TCDE_CreateMemoryPool(size_t block_size, int capacity);

/**
 * @brief Destroy memory pool
 * @param pool Pool to destroy
 */
void TCDE_DestroyMemoryPool(TCDE_MemoryPool* pool);

/**
 * @brief Allocate block from pool
 * @param pool Pool
 * @return Pointer to block, or NULL if pool is full
 * 
 * Complexity: O(1)
 */
void* TCDE_PoolAlloc(TCDE_MemoryPool* pool);

/**
 * @brief Free block back to pool
 * @param pool Pool
 * @param ptr Pointer to block (must be from this pool)
 * 
 * Complexity: O(1)
 */
void TCDE_PoolFree(TCDE_MemoryPool* pool, void* ptr);

/**
 * @brief Get pool statistics
 * @param pool Pool
 * @param allocated Output: currently allocated blocks
 * @param capacity Output: total capacity
 * @param fragmentation Output: fragmentation ratio [0,1]
 */
void TCDE_PoolStats(const TCDE_MemoryPool* pool,
                    int* allocated,
                    int* capacity,
                    float* fragmentation);

// ============================================================================
// MEMORY ARENA (Temporary allocations)
// ============================================================================

/**
 * @brief Memory arena for temporary allocations
 * 
 * Fast bump-pointer allocation for temporary objects.
 * All allocations freed at once with arena reset.
 * Perfect for per-frame or per-iteration temporaries.
 */
typedef struct {
    void* memory;               ///< Pre-allocated memory block
    size_t size;                ///< Total size (bytes)
    size_t used;                ///< Currently used (bytes)
    int num_allocations;        ///< Number of allocations
} TCDE_MemoryArena;

/**
 * @brief Create memory arena
 * @param size Total size (bytes)
 * @return Arena pointer, or NULL on error
 * 
 * Example:
 * ```c
 * // 1 MB arena for temporary calculations
 * TCDE_MemoryArena* arena = TCDE_CreateMemoryArena(1024 * 1024);
 * ```
 */
TCDE_MemoryArena* TCDE_CreateMemoryArena(size_t size);

/**
 * @brief Destroy memory arena
 * @param arena Arena to destroy
 */
void TCDE_DestroyMemoryArena(TCDE_MemoryArena* arena);

/**
 * @brief Allocate from arena
 * @param arena Arena
 * @param size Size to allocate (bytes)
 * @return Pointer to memory, or NULL if arena is full
 * 
 * Complexity: O(1) - just bump pointer
 * 
 * Note: Individual allocations cannot be freed.
 * Use TCDE_ArenaReset() to free all at once.
 */
void* TCDE_ArenaAlloc(TCDE_MemoryArena* arena, size_t size);

/**
 * @brief Reset arena (free all allocations)
 * @param arena Arena to reset
 * 
 * Complexity: O(1)
 * 
 * After reset, all previous pointers from this arena are invalid.
 */
void TCDE_ArenaReset(TCDE_MemoryArena* arena);

/**
 * @brief Get arena statistics
 * @param arena Arena
 * @param used Output: bytes used
 * @param size Output: total size
 * @param utilization Output: utilization ratio [0,1]
 */
void TCDE_ArenaStats(const TCDE_MemoryArena* arena,
                     size_t* used,
                     size_t* size,
                     float* utilization);

// ============================================================================
// ALIGNED ALLOCATION (For SIMD)
// ============================================================================

/**
 * @brief Allocate aligned memory
 * @param size Size (bytes)
 * @param alignment Alignment (must be power of 2)
 * @return Aligned pointer, or NULL on error
 * 
 * Use for SIMD data structures (AVX2 requires 32-byte alignment).
 * Must be freed with TCDE_AlignedFree().
 */
void* TCDE_AlignedAlloc(size_t size, size_t alignment);

/**
 * @brief Free aligned memory
 * @param ptr Pointer from TCDE_AlignedAlloc()
 */
void TCDE_AlignedFree(void* ptr);

#ifdef __cplusplus
}
#endif

#endif // TCDE_MEMORY_H
