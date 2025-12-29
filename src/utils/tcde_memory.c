/**
 * @file tcde_memory.c
 * @brief TCDE Memory Management Implementation
 */

#include "tcde_memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// MEMORY POOL IMPLEMENTATION
// ============================================================================

/**
 * @brief Create memory pool
 */
TCDE_MemoryPool* TCDE_CreateMemoryPool(size_t block_size, int capacity) {
    if (block_size == 0 || capacity <= 0) {
        return NULL;
    }
    
    TCDE_MemoryPool* pool = (TCDE_MemoryPool*)malloc(sizeof(TCDE_MemoryPool));
    if (!pool) {
        return NULL;
    }
    
    // Allocate memory block
    pool->memory = malloc(block_size * capacity);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }
    
    // Allocate free list
    pool->free_list = (void**)malloc(capacity * sizeof(void*));
    if (!pool->free_list) {
        free(pool->memory);
        free(pool);
        return NULL;
    }
    
    // Initialize free list (all blocks free)
    char* ptr = (char*)pool->memory;
    for (int i = 0; i < capacity; i++) {
        pool->free_list[i] = ptr + (i * block_size);
    }
    
    pool->block_size = block_size;
    pool->capacity = capacity;
    pool->free_count = capacity;
    pool->allocated = 0;
    
    return pool;
}

/**
 * @brief Destroy memory pool
 */
void TCDE_DestroyMemoryPool(TCDE_MemoryPool* pool) {
    if (!pool) {
        return;
    }
    
    free(pool->free_list);
    free(pool->memory);
    free(pool);
}

/**
 * @brief Allocate block from pool
 */
void* TCDE_PoolAlloc(TCDE_MemoryPool* pool) {
    if (!pool || pool->free_count == 0) {
        return NULL;
    }
    
    // Pop from free list
    pool->free_count--;
    void* ptr = pool->free_list[pool->free_count];
    pool->allocated++;
    
    return ptr;
}

/**
 * @brief Free block back to pool
 */
void TCDE_PoolFree(TCDE_MemoryPool* pool, void* ptr) {
    if (!pool || !ptr) {
        return;
    }
    
    // Validate pointer is from this pool
    char* char_ptr = (char*)ptr;
    char* pool_start = (char*)pool->memory;
    char* pool_end = pool_start + (pool->block_size * pool->capacity);
    
    if (char_ptr < pool_start || char_ptr >= pool_end) {
        // Invalid pointer
        return;
    }
    
    // Check alignment
    size_t offset = char_ptr - pool_start;
    if (offset % pool->block_size != 0) {
        // Invalid pointer (not aligned to block boundary)
        return;
    }
    
    // Push to free list
    if (pool->free_count < pool->capacity) {
        pool->free_list[pool->free_count] = ptr;
        pool->free_count++;
        pool->allocated--;
    }
}

/**
 * @brief Get pool statistics
 */
void TCDE_PoolStats(const TCDE_MemoryPool* pool,
                    int* allocated,
                    int* capacity,
                    float* fragmentation) {
    if (!pool) {
        return;
    }
    
    if (allocated) {
        *allocated = pool->allocated;
    }
    
    if (capacity) {
        *capacity = pool->capacity;
    }
    
    if (fragmentation) {
        // Fragmentation: ratio of free blocks to total capacity
        *fragmentation = (float)pool->free_count / pool->capacity;
    }
}

// ============================================================================
// MEMORY ARENA IMPLEMENTATION
// ============================================================================

/**
 * @brief Create memory arena
 */
TCDE_MemoryArena* TCDE_CreateMemoryArena(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    TCDE_MemoryArena* arena = (TCDE_MemoryArena*)malloc(sizeof(TCDE_MemoryArena));
    if (!arena) {
        return NULL;
    }
    
    arena->memory = malloc(size);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }
    
    arena->size = size;
    arena->used = 0;
    arena->num_allocations = 0;
    
    return arena;
}

/**
 * @brief Destroy memory arena
 */
void TCDE_DestroyMemoryArena(TCDE_MemoryArena* arena) {
    if (!arena) {
        return;
    }
    
    free(arena->memory);
    free(arena);
}

/**
 * @brief Allocate from arena
 */
void* TCDE_ArenaAlloc(TCDE_MemoryArena* arena, size_t size) {
    if (!arena || size == 0) {
        return NULL;
    }
    
    // Align to 8 bytes for better performance
    size_t aligned_size = (size + 7) & ~7;
    
    // Check if enough space
    if (arena->used + aligned_size > arena->size) {
        return NULL;
    }
    
    // Bump pointer allocation
    void* ptr = (char*)arena->memory + arena->used;
    arena->used += aligned_size;
    arena->num_allocations++;
    
    return ptr;
}

/**
 * @brief Reset arena
 */
void TCDE_ArenaReset(TCDE_MemoryArena* arena) {
    if (!arena) {
        return;
    }
    
    arena->used = 0;
    arena->num_allocations = 0;
}

/**
 * @brief Get arena statistics
 */
void TCDE_ArenaStats(const TCDE_MemoryArena* arena,
                     size_t* used,
                     size_t* size,
                     float* utilization) {
    if (!arena) {
        return;
    }
    
    if (used) {
        *used = arena->used;
    }
    
    if (size) {
        *size = arena->size;
    }
    
    if (utilization) {
        *utilization = (float)arena->used / arena->size;
    }
}

// ============================================================================
// ALIGNED ALLOCATION
// ============================================================================

/**
 * @brief Allocate aligned memory
 */
void* TCDE_AlignedAlloc(size_t size, size_t alignment) {
    if (size == 0 || alignment == 0) {
        return NULL;
    }
    
    // Check alignment is power of 2
    if ((alignment & (alignment - 1)) != 0) {
        return NULL;
    }
    
#if defined(_WIN32)
    return _aligned_malloc(size, alignment);
#elif defined(__APPLE__) || defined(__linux__)
    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) == 0) {
        return ptr;
    }
    return NULL;
#else
    // Fallback: manual alignment
    void* raw = malloc(size + alignment + sizeof(void*));
    if (!raw) {
        return NULL;
    }
    
    // Align pointer
    void** aligned = (void**)(((size_t)raw + sizeof(void*) + alignment - 1) & ~(alignment - 1));
    aligned[-1] = raw;  // Store original pointer
    
    return aligned;
#endif
}

/**
 * @brief Free aligned memory
 */
void TCDE_AlignedFree(void* ptr) {
    if (!ptr) {
        return;
    }
    
#if defined(_WIN32)
    _aligned_free(ptr);
#elif defined(__APPLE__) || defined(__linux__)
    free(ptr);
#else
    // Fallback: retrieve original pointer
    void** aligned = (void**)ptr;
    free(aligned[-1]);
#endif
}
