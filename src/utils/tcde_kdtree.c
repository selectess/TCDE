/**
 * @file tcde_kdtree.c
 * @brief KD-Tree Implementation for Spatial Indexing
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#include "tcde_kdtree.h"
#include "../core/tcde_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

// ============================================================================
// HELPER STRUCTURES
// ============================================================================

typedef struct {
    TCDE_Point point;
    TCDE_Complex coeff;
    float width;
    int index;
} KDTreeData;

// Forward declarations for heap-based KNN
typedef struct MaxHeap MaxHeap;
static void knn_search_heap(TCDE_KDNode* node,
                           const TCDE_Point* query,
                           MaxHeap* heap,
                           int depth);

// Helper function for distance calculation
static float point_distance(const TCDE_Point* p1, const TCDE_Point* p2) {
    if (!p1 || !p2) return FLT_MAX;
    
    int dim = (p1->dimension < p2->dimension) ? p1->dimension : p2->dimension;
    float dist_sq = 0.0f;
    
    for (int i = 0; i < dim; i++) {
        float diff = p1->coords[i] - p2->coords[i];
        dist_sq += diff * diff;
    }
    
    return sqrtf(dist_sq);
}

// ============================================================================
// CONSTRUCTION HELPERS
// ============================================================================

static int compare_by_axis(const void* a, const void* b, int axis) {
    const KDTreeData* da = (const KDTreeData*)a;
    const KDTreeData* db = (const KDTreeData*)b;
    
    float va = da->point.coords[axis];
    float vb = db->point.coords[axis];
    
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

// Comparison functions for qsort
static int axis_to_sort = 0;
static int compare_wrapper(const void* a, const void* b) {
    return compare_by_axis(a, b, axis_to_sort);
}

static TCDE_KDNode* build_kdtree_recursive(KDTreeData* data, int n, int depth, int dim) {
    if (n <= 0) return NULL;
    
    // Select axis based on depth
    int axis = depth % dim;
    
    // Sort by axis
    axis_to_sort = axis;
    qsort(data, n, sizeof(KDTreeData), compare_wrapper);
    
    // Find median
    int median = n / 2;
    
    // Create node
    TCDE_KDNode* node = malloc(sizeof(TCDE_KDNode));
    if (!node) return NULL;
    
    node->point = data[median].point;
    node->coeff = data[median].coeff;
    node->width = data[median].width;
    node->index = data[median].index;
    node->axis = axis;
    
    // Recursively build subtrees
    node->left = build_kdtree_recursive(data, median, depth + 1, dim);
    node->right = build_kdtree_recursive(data + median + 1, n - median - 1, depth + 1, dim);
    
    return node;
}

/**
 * @brief Build KD-Tree from field
 */
TCDE_KDTree* TCDE_BuildKDTree(const TCDE_Field* field) {
    if (!field) return NULL;
    
    int num_centers = field->manifold_6d.num_centers;
    if (num_centers == 0) return NULL;
    
    // Allocate tree
    TCDE_KDTree* tree = malloc(sizeof(TCDE_KDTree));
    if (!tree) return NULL;
    
    tree->dimension = 6;
    tree->num_nodes = num_centers;
    
    // Copy data
    KDTreeData* data = malloc(num_centers * sizeof(KDTreeData));
    if (!data) {
        free(tree);
        return NULL;
    }
    
    for (int i = 0; i < num_centers; i++) {
        data[i].point = field->manifold_6d.centers[i].point;
        data[i].coeff = field->manifold_6d.centers[i].coeff;
        data[i].width = field->manifold_6d.centers[i].epsilon;
        data[i].index = i;
    }
    
    // Build tree
    tree->root = build_kdtree_recursive(data, num_centers, 0, 6);
    
    free(data);
    
    return tree;
}

/**
 * @brief Free KD-Tree recursively
 */
static void free_kdtree_recursive(TCDE_KDNode* node) {
    if (!node) return;
    
    free_kdtree_recursive(node->left);
    free_kdtree_recursive(node->right);
    
    TCDE_FreePoint(&node->point);
    free(node);
}

void TCDE_FreeKDTree(TCDE_KDTree* tree) {
    if (!tree) return;
    
    free_kdtree_recursive(tree->root);
    free(tree);
}

// ============================================================================
// QUERY HELPERS
// ============================================================================

static float point_distance_squared(const TCDE_Point* p1, const TCDE_Point* p2) {
    float dist_sq = 0.0f;
    int dim = (p1->dimension < p2->dimension) ? p1->dimension : p2->dimension;
    
    for (int i = 0; i < dim; i++) {
        float diff = p1->coords[i] - p2->coords[i];
        dist_sq += diff * diff;
    }
    
    return dist_sq;
}

// ============================================================================
// RADIUS SEARCH
// ============================================================================

static void radius_search_recursive(const TCDE_KDNode* node,
                                    const TCDE_Point* query,
                                    float radius_sq,
                                    TCDE_Neighbor* neighbors,
                                    int* count,
                                    int max_neighbors) {
    if (!node || *count >= max_neighbors) return;
    
    // Check if this node is within radius
    float dist_sq = point_distance_squared(&node->point, query);
    
    if (dist_sq <= radius_sq) {
        // Add to neighbors
        neighbors[*count].point = node->point;
        neighbors[*count].coeff = node->coeff;
        neighbors[*count].width = node->width;
        neighbors[*count].distance = sqrtf(dist_sq);
        neighbors[*count].index = node->index;
        (*count)++;
    }
    
    if (*count >= max_neighbors) return;
    
    // Determine which subtree to search first
    int axis = node->axis;
    float diff = query->coords[axis] - node->point.coords[axis];
    float diff_sq = diff * diff;
    
    TCDE_KDNode* near = (diff < 0) ? node->left : node->right;
    TCDE_KDNode* far = (diff < 0) ? node->right : node->left;
    
    // Search near subtree
    radius_search_recursive(near, query, radius_sq, neighbors, count, max_neighbors);
    
    // Search far subtree if sphere intersects splitting plane
    if (diff_sq <= radius_sq && *count < max_neighbors) {
        radius_search_recursive(far, query, radius_sq, neighbors, count, max_neighbors);
    }
}

int TCDE_KDTreeRadius(const TCDE_KDTree* tree,
                      const TCDE_Point* query,
                      float radius,
                      TCDE_Neighbor* neighbors,
                      int max_neighbors) {
    if (!tree || !query || !neighbors || max_neighbors <= 0) return 0;
    
    int count = 0;
    float radius_sq = radius * radius;
    
    radius_search_recursive(tree->root, query, radius_sq, neighbors, &count, max_neighbors);
    
    return count;
}

// ============================================================================
// KNN SEARCH (simplified - just use radius with large radius)
// ============================================================================

int TCDE_KDTreeKNN(const TCDE_KDTree* tree,
                   const TCDE_Point* query,
                   int k,
                   TCDE_Neighbor* neighbors) {
    if (!tree || !query || !neighbors || k <= 0) return 0;
    
    // Simple implementation: use large radius and take k closest
    // Production code should use priority queue
    float large_radius = 10.0f;  // Adjust based on domain
    
    TCDE_Neighbor* temp = malloc(tree->num_nodes * sizeof(TCDE_Neighbor));
    if (!temp) return 0;
    
    int found = TCDE_KDTreeRadius(tree, query, large_radius, temp, tree->num_nodes);
    
    // Sort by distance and take k closest
    // Simple bubble sort for small k
    for (int i = 0; i < found - 1 && i < k; i++) {
        for (int j = i + 1; j < found; j++) {
            if (temp[j].distance < temp[i].distance) {
                TCDE_Neighbor swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }
    
    // Copy k closest
    int result = (found < k) ? found : k;
    memcpy(neighbors, temp, result * sizeof(TCDE_Neighbor));
    
    free(temp);
    
    return result;
}

// ============================================================================
// OPTIMIZED FIELD OPERATIONS
// ============================================================================

/**
 * @brief Fast field evaluation using KD-Tree
 */
TCDE_Complex TCDE_Evaluate6D_Fast(const TCDE_Field* field,
                                   const TCDE_KDTree* tree,
                                   const TCDE_Point* point) {
    if (!field || !tree || !point) return 0.0f + 0.0f * I;
    
    // Search radius: 3σ (covers 99.7% of Gaussian)
    float search_radius = 3.0f * 0.2f;  // Assuming typical width ~0.2
    
    TCDE_Neighbor neighbors[100];  // Max neighbors to consider
    int found = TCDE_KDTreeRadius(tree, point, search_radius, neighbors, 100);
    
    // Evaluate RBF only for nearby centers
    TCDE_Complex sum = 0.0f + 0.0f * I;
    
    for (int i = 0; i < found; i++) {
        float dist_sq = neighbors[i].distance * neighbors[i].distance;
        float width_sq = neighbors[i].width * neighbors[i].width;
        
        // RBF kernel
        float rbf = expf(-0.5f * dist_sq / width_sq);
        
        // Accumulate
        sum += rbf * neighbors[i].coeff;
    }
    
    return sum;
}

/**
 * @brief Fast coupling computation using KD-Tree
 */
TCDE_Complex TCDE_ComputeCouplingFast(const TCDE_Field* field,
                                       const TCDE_KDTree* tree,
                                       const TCDE_Point* point,
                                       float sigma) {
    if (!field || !tree || !point) return 0.0f + 0.0f * I;
    
    // Search radius: 3σ for coupling
    float search_radius = 3.0f * sigma;
    
    TCDE_Neighbor neighbors[200];
    int found = TCDE_KDTreeRadius(tree, point, search_radius, neighbors, 200);
    
    // Compute coupling
    TCDE_Complex coupling = 0.0f + 0.0f * I;
    
    for (int i = 0; i < found; i++) {
        float dist_sq = neighbors[i].distance * neighbors[i].distance;
        float sigma_sq = sigma * sigma;
        
        // Coupling kernel
        float kernel = expf(-dist_sq / (2.0f * sigma_sq));
        
        // Accumulate
        coupling += kernel * neighbors[i].coeff;
    }
    
    // Normalize
    if (found > 0) {
        coupling /= (float)found;
    }
    
    return coupling;
}

// ============================================================================
// STATISTICS
// ============================================================================

static void compute_depth_recursive(const TCDE_KDNode* node, int depth,
                                    int* max_depth, int* total_depth, int* count) {
    if (!node) return;
    
    (*count)++;
    (*total_depth) += depth;
    
    if (depth > *max_depth) {
        *max_depth = depth;
    }
    
    compute_depth_recursive(node->left, depth + 1, max_depth, total_depth, count);
    compute_depth_recursive(node->right, depth + 1, max_depth, total_depth, count);
}

void TCDE_KDTreeStats(const TCDE_KDTree* tree,
                      int* max_depth,
                      float* avg_depth,
                      float* balance) {
    if (!tree || !max_depth || !avg_depth || !balance) return;
    
    *max_depth = 0;
    int total_depth = 0;
    int count = 0;
    
    compute_depth_recursive(tree->root, 0, max_depth, &total_depth, &count);
    
    *avg_depth = (count > 0) ? (float)total_depth / count : 0.0f;
    
    // Balance: compare to perfect tree
    float perfect_depth = log2f((float)tree->num_nodes);
    *balance = (perfect_depth > 0) ? perfect_depth / (*max_depth) : 1.0f;
}


// ============================================================================
// OPTIMIZED KNN WITH MAX-HEAP
// ============================================================================

/**
 * @brief Heap node for KNN
 */
typedef struct {
    int index;
    float distance;
} HeapNode;

/**
 * @brief Max-heap for KNN
 */
struct MaxHeap {
    HeapNode* nodes;
    int size;
    int capacity;
};

/**
 * @brief Create max-heap
 */
static MaxHeap* create_max_heap(int capacity) {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    if (!heap) return NULL;
    
    heap->nodes = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    if (!heap->nodes) {
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

/**
 * @brief Free max-heap
 */
static void free_max_heap(MaxHeap* heap) {
    if (heap) {
        free(heap->nodes);
        free(heap);
    }
}

/**
 * @brief Heapify up
 */
static void heapify_up(MaxHeap* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->nodes[idx].distance <= heap->nodes[parent].distance) {
            break;
        }
        
        // Swap
        HeapNode temp = heap->nodes[idx];
        heap->nodes[idx] = heap->nodes[parent];
        heap->nodes[parent] = temp;
        
        idx = parent;
    }
}

/**
 * @brief Heapify down
 */
static void heapify_down(MaxHeap* heap, int idx) {
    while (1) {
        int largest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        
        if (left < heap->size && 
            heap->nodes[left].distance > heap->nodes[largest].distance) {
            largest = left;
        }
        
        if (right < heap->size && 
            heap->nodes[right].distance > heap->nodes[largest].distance) {
            largest = right;
        }
        
        if (largest == idx) {
            break;
        }
        
        // Swap
        HeapNode temp = heap->nodes[idx];
        heap->nodes[idx] = heap->nodes[largest];
        heap->nodes[largest] = temp;
        
        idx = largest;
    }
}

/**
 * @brief Insert into heap
 */
static int heap_insert(MaxHeap* heap, int index, float distance) {
    if (heap->size < heap->capacity) {
        // Heap not full, just insert
        heap->nodes[heap->size].index = index;
        heap->nodes[heap->size].distance = distance;
        heapify_up(heap, heap->size);
        heap->size++;
        return 1;
    } else if (distance < heap->nodes[0].distance) {
        // Replace max if new distance is smaller
        heap->nodes[0].index = index;
        heap->nodes[0].distance = distance;
        heapify_down(heap, 0);
        return 1;
    }
    
    return 0;
}

/**
 * @brief Recursive KNN search with heap
 */
static void knn_search_heap(TCDE_KDNode* node,
                           const TCDE_Point* query,
                           MaxHeap* heap,
                           int depth) {
    if (!node) return;
    
    // Compute distance to current point
    float dist = point_distance(query, &node->point);
    heap_insert(heap, node->index, dist);
    
    // Determine which subtree to search first
    int axis = depth % query->dimension;
    bool go_left = (query->coords[axis] < node->point.coords[axis]);
    
    TCDE_KDNode* first = go_left ? node->left : node->right;
    TCDE_KDNode* second = go_left ? node->right : node->left;
    
    // Search first subtree
    if (first) {
        knn_search_heap(first, query, heap, depth + 1);
    }
    
    // Check if we need to search second subtree
    float axis_dist = fabsf(query->coords[axis] - node->point.coords[axis]);
    
    if (second && (heap->size < heap->capacity || 
                   axis_dist < heap->nodes[0].distance)) {
        knn_search_heap(second, query, heap, depth + 1);
    }
}

/**
 * @brief Optimized KNN search using max-heap
 */
void TCDE_KDTree_KNN_Optimized(const TCDE_KDTree* tree,
                               const TCDE_Point* query,
                               int k,
                               int* indices,
                               float* distances) {
    if (!tree || !query || !indices || !distances || k <= 0) {
        return;
    }
    
    MaxHeap* heap = create_max_heap(k);
    if (!heap) return;
    
    // Recursive search with heap
    knn_search_heap(tree->root, query, heap, 0);
    
    // Extract results (reverse order for ascending distances)
    for (int i = 0; i < heap->size; i++) {
        int result_idx = heap->size - 1 - i;
        if (result_idx < k) {
            indices[result_idx] = heap->nodes[0].index;
            distances[result_idx] = heap->nodes[0].distance;
            
            // Remove max
            heap->nodes[0] = heap->nodes[heap->size - 1];
            heap->size--;
            if (heap->size > 0) {
                heapify_down(heap, 0);
            }
        }
    }
    
    free_max_heap(heap);
}

/**
 * @brief Original KNN implementation (for comparison)
 */
void TCDE_KDTree_KNN(const TCDE_KDTree* tree,
                     const TCDE_Point* query,
                     int k,
                     int* indices,
                     float* distances) {
    // For now, just call the optimized version
    // In a real implementation, this would be the original bubble-sort version
    TCDE_KDTree_KNN_Optimized(tree, query, k, indices, distances);
}
