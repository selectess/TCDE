/**
 * @file tcde_kdtree.h
 * @brief TCDE KD-Tree for Spatial Indexing
 * 
 * Accelerates nearest neighbor search from O(K) to O(log K).
 * Critical optimization for field evaluation and coupling.
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#ifndef TCDE_KDTREE_H
#define TCDE_KDTREE_H

#include "../core/tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// KD-TREE STRUCTURES
// ============================================================================

/**
 * @brief KD-Tree node
 */
typedef struct TCDE_KDNode {
    TCDE_Point point;              ///< Point in 6D space
    TCDE_Complex coeff;            ///< Complex coefficient
    float width;                   ///< RBF width
    int index;                     ///< Original index in field
    struct TCDE_KDNode* left;      ///< Left child
    struct TCDE_KDNode* right;     ///< Right child
    int axis;                      ///< Split axis
} TCDE_KDNode;

/**
 * @brief KD-Tree structure
 */
typedef struct {
    TCDE_KDNode* root;             ///< Root node
    int num_nodes;                 ///< Number of nodes
    int dimension;                 ///< Dimension (6 for TCDE)
} TCDE_KDTree;

/**
 * @brief Neighbor result
 */
typedef struct {
    TCDE_Point point;
    TCDE_Complex coeff;
    float width;
    float distance;
    int index;
} TCDE_Neighbor;

// ============================================================================
// KD-TREE CONSTRUCTION
// ============================================================================

/**
 * @brief Build KD-Tree from field centers
 * @param field Field containing centers
 * @return KD-Tree (must be freed with TCDE_FreeKDTree)
 * 
 * Complexity: O(K log K) construction
 * Memory: O(K)
 * 
 * Example:
 * ```c
 * TCDE_KDTree* tree = TCDE_BuildKDTree(field);
 * // Use tree for fast queries
 * TCDE_FreeKDTree(tree);
 * ```
 */
TCDE_KDTree* TCDE_BuildKDTree(const TCDE_Field* field);

/**
 * @brief Free KD-Tree
 * @param tree Tree to free
 */
void TCDE_FreeKDTree(TCDE_KDTree* tree);

// ============================================================================
// KD-TREE QUERIES
// ============================================================================

/**
 * @brief Find K nearest neighbors
 * @param tree KD-Tree
 * @param query Query point
 * @param k Number of neighbors
 * @param neighbors Output neighbors (pre-allocated array of size k)
 * @return Number of neighbors found
 * 
 * Complexity: O(log K) average, O(K) worst case
 * 
 * Example:
 * ```c
 * TCDE_Neighbor neighbors[10];
 * int found = TCDE_KDTreeKNN(tree, &query, 10, neighbors);
 * ```
 */
int TCDE_KDTreeKNN(const TCDE_KDTree* tree,
                   const TCDE_Point* query,
                   int k,
                   TCDE_Neighbor* neighbors);

/**
 * @brief Find all neighbors within radius
 * @param tree KD-Tree
 * @param query Query point
 * @param radius Search radius
 * @param neighbors Output neighbors (pre-allocated)
 * @param max_neighbors Maximum neighbors to return
 * @return Number of neighbors found
 * 
 * Complexity: O(log K + M) where M is number of neighbors
 * 
 * This is the key function for fast RBF evaluation:
 * Only evaluate RBF for centers within 3σ radius.
 * 
 * Example:
 * ```c
 * TCDE_Neighbor neighbors[100];
 * int found = TCDE_KDTreeRadius(tree, &query, 0.5f, neighbors, 100);
 * ```
 */
int TCDE_KDTreeRadius(const TCDE_KDTree* tree,
                      const TCDE_Point* query,
                      float radius,
                      TCDE_Neighbor* neighbors,
                      int max_neighbors);

// ============================================================================
// OPTIMIZED FIELD OPERATIONS
// ============================================================================

/**
 * @brief Evaluate field using KD-Tree (optimized)
 * @param field Field
 * @param tree KD-Tree for field
 * @param point Evaluation point
 * @return Field value
 * 
 * Uses radius search to only evaluate nearby centers.
 * Speedup: 10-100x for K > 1000
 * 
 * Example:
 * ```c
 * TCDE_KDTree* tree = TCDE_BuildKDTree(field);
 * TCDE_Complex value = TCDE_Evaluate6D_Fast(field, tree, &point);
 * ```
 */
TCDE_Complex TCDE_Evaluate6D_Fast(const TCDE_Field* field,
                                   const TCDE_KDTree* tree,
                                   const TCDE_Point* point);

/**
 * @brief Compute coupling using KD-Tree (optimized)
 * @param field Field
 * @param tree KD-Tree for field
 * @param point Evaluation point
 * @param sigma Coupling length scale
 * @return Coupling value
 * 
 * Uses radius search to only couple with nearby centers.
 * Speedup: 10-100x for K > 1000
 */
TCDE_Complex TCDE_ComputeCouplingFast(const TCDE_Field* field,
                                       const TCDE_KDTree* tree,
                                       const TCDE_Point* point,
                                       float sigma);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get KD-Tree statistics
 * @param tree KD-Tree
 * @param max_depth Output: maximum depth
 * @param avg_depth Output: average depth
 * @param balance Output: balance factor [0,1] (1 = perfect)
 * 
 * Useful for debugging and optimization.
 */
void TCDE_KDTreeStats(const TCDE_KDTree* tree,
                      int* max_depth,
                      float* avg_depth,
                      float* balance);

// ============================================================================
// OPTIMIZED KNN (Heap-based)
// ============================================================================

/**
 * @brief K-nearest neighbors search (optimized with max-heap)
 * @param tree KD-Tree
 * @param query Query point
 * @param k Number of neighbors
 * @param indices Output indices (pre-allocated array of size k)
 * @param distances Output distances (pre-allocated array of size k)
 * 
 * Optimized version using max-heap for O(log k) insertions.
 * Speedup: 2-5x over bubble sort for k > 10
 * 
 * @complexity O(log N * log k) average case
 */
void TCDE_KDTree_KNN_Optimized(const TCDE_KDTree* tree,
                               const TCDE_Point* query,
                               int k,
                               int* indices,
                               float* distances);

/**
 * @brief K-nearest neighbors search (original implementation)
 * @param tree KD-Tree
 * @param query Query point
 * @param k Number of neighbors
 * @param indices Output indices
 * @param distances Output distances
 * 
 * Original implementation for comparison/fallback.
 * 
 * @complexity O(log N * k) average case
 */
void TCDE_KDTree_KNN(const TCDE_KDTree* tree,
                     const TCDE_Point* query,
                     int k,
                     int* indices,
                     float* distances);

#ifdef __cplusplus
}
#endif

#endif // TCDE_KDTREE_H
