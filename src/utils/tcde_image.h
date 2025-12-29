/**
 * @file tcde_image.h
 * @brief TCDE Image Processing - Continuous Spatial Field Representation
 * 
 * CRITICAL PRINCIPLE: NO PATCHIFICATION!
 * 
 * Images are processed as continuous spatial fields,
 * not as discrete patches or grids. This preserves
 * spatial continuity and enables richer representations.
 * 
 * Approach:
 * - Compute saliency map (importance regions)
 * - Sample intelligently (more samples in salient regions)
 * - Extract local features (gradients, textures)
 * - Create RBF centers as continuous perturbations
 * - Result: Continuous field Φ(x,y,z,τ₁,τ₂,m)
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#ifndef TCDE_IMAGE_H
#define TCDE_IMAGE_H

#include "../core/tcde_core.h"
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// IMAGE FEATURE STRUCTURES
// ============================================================================

/**
 * @brief Continuous image feature (NOT a patch!)
 * 
 * Represents a continuous perturbation in spatial field,
 * extracted from image without discrete patchification.
 */
typedef struct {
    float spatial_position[3];   ///< Position in spatial space (x,y,z)
    float temporal_coord;        ///< Temporal coordinate τ₁ (capture time)
    float anticipation;          ///< Anticipation τ₂ (0 for static image)
    float modality;              ///< Modality m (0.0 for visual)
    float intensity;             ///< Saliency/importance
    float orientation;           ///< Local gradient orientation (radians)
    float width;                 ///< RBF width σ (spatial scale)
} TCDE_ImageFeature;

/**
 * @brief Image analysis results
 */
typedef struct {
    float focus_center[3];       ///< Center of visual attention
    float spatial_complexity;    ///< Spatial complexity measure
    float edge_density;          ///< Edge/gradient density
    int num_regions;             ///< Number of salient regions
    float coherence;             ///< Spatial coherence [0,1]
} TCDE_ImageAnalysis;

/**
 * @brief Saliency map
 */
typedef struct {
    float* values;               ///< Saliency values [height * width]
    int width;                   ///< Map width
    int height;                  ///< Map height
    float max_saliency;          ///< Maximum saliency value
} TCDE_SaliencyMap;

// ============================================================================
// IMAGE TO FIELD CONVERSION
// ============================================================================

/**
 * @brief Convert image to continuous field (NO PATCHIFICATION!)
 * @param image Input image (RGB or grayscale)
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels (1=gray, 3=RGB)
 * @param field Output field (must be pre-allocated)
 * @param num_samples Target number of samples (adaptive)
 * @return Number of features created
 * 
 * Process:
 * 1. Compute saliency map
 * 2. Sample intelligently (more in salient regions)
 * 3. Extract local features at each sample
 * 4. Create RBF centers
 * 5. Add to field as continuous perturbations
 * 
 * The resulting field is continuous everywhere, not discrete grid.
 * 
 * Example:
 * ```c
 * TCDE_Field* field = TCDE_CreateField(500, 2.5f);
 * int num_features = TCDE_ImageToField(image, 640, 480, 3, field, 200);
 * // field now contains continuous spatial representation
 * ```
 */
int TCDE_ImageToField(const unsigned char* image,
                      int width, int height, int channels,
                      TCDE_Field* field,
                      int num_samples);

/**
 * @brief Extract continuous features from image
 * @param image Input image
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param features Output features array (pre-allocated)
 * @param max_features Maximum number of features
 * @return Number of features extracted
 * 
 * Extracts continuous features based on:
 * - Saliency (importance sampling)
 * - Local gradients (edge detection)
 * - Texture statistics
 * - Color information
 * 
 * NO discrete patchification is performed.
 */
int TCDE_ExtractImageFeatures(const unsigned char* image,
                              int width, int height, int channels,
                              TCDE_ImageFeature* features,
                              int max_features);

// ============================================================================
// SALIENCY COMPUTATION
// ============================================================================

/**
 * @brief Compute saliency map from image
 * @param image Input image
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @return Saliency map (must be freed with TCDE_FreeSaliencyMap)
 * 
 * Computes visual saliency using:
 * - Gradient magnitude (edges)
 * - Local contrast
 * - Color distinctiveness (if RGB)
 * 
 * Result is continuous, not discrete grid.
 */
TCDE_SaliencyMap* TCDE_ComputeSaliency(const unsigned char* image,
                                        int width, int height,
                                        int channels);

/**
 * @brief Free saliency map
 * @param map Saliency map to free
 */
void TCDE_FreeSaliencyMap(TCDE_SaliencyMap* map);

/**
 * @brief Sample points based on saliency
 * @param saliency Saliency map
 * @param num_samples Number of samples to generate
 * @param positions Output positions [num_samples][2] (x,y in [0,1])
 * 
 * Importance sampling: more samples in high-saliency regions.
 * Creates continuous distribution, not uniform grid.
 */
void TCDE_SampleBySaliency(const TCDE_SaliencyMap* saliency,
                           int num_samples,
                           float (*positions)[2]);

// ============================================================================
// IMAGE ANALYSIS
// ============================================================================

/**
 * @brief Analyze image field for visual properties
 * @param field Field containing image representation
 * @param analysis Output analysis results
 * 
 * Extracts:
 * - Visual attention center
 * - Spatial complexity
 * - Edge density
 * - Salient regions
 * 
 * All properties emerge from field structure.
 */
void TCDE_AnalyzeImageField(const TCDE_Field* field,
                            TCDE_ImageAnalysis* analysis);

/**
 * @brief Compute visual similarity between image fields
 * @param field1 First image field
 * @param field2 Second image field
 * @param metric Metric for distance computation
 * @return Similarity score [0,1]
 * 
 * Similarity based on spatial field overlap.
 * Uses geodesic distance, not pixel-wise comparison.
 */
float TCDE_ImageSimilarity(const TCDE_Field* field1,
                          const TCDE_Field* field2,
                          const TCDE_Metric* metric);

// ============================================================================
// ADVANCED IMAGE PROCESSING
// ============================================================================

/**
 * @brief Multi-scale image to field conversion
 * @param image Input image
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param field Output field
 * @param num_scales Number of scales
 * @param scale_factor Scale reduction factor (typically 0.5)
 * @return Total number of features created
 * 
 * Creates features at multiple spatial scales:
 * - Fine scale: small σ (details)
 * - Coarse scale: large σ (structure)
 * 
 * Captures multi-scale spatial structure.
 */
int TCDE_ImageToFieldMultiScale(const unsigned char* image,
                                int width, int height, int channels,
                                TCDE_Field* field,
                                int num_scales,
                                float scale_factor);

/**
 * @brief Extract salient regions from image field
 * @param field Image field
 * @param regions Output region centers [max_regions][3]
 * @param max_regions Maximum regions to extract
 * @return Number of regions found
 * 
 * Regions are clusters of high energy density in spatial field.
 * Emerge from field structure, not predefined.
 */
int TCDE_ExtractSalientRegions(const TCDE_Field* field,
                               float (*regions)[3],
                               int max_regions);

/**
 * @brief Compute spatial entropy of image field
 * @param field Image field
 * @return Entropy (bits)
 * 
 * Measures spatial information content.
 * Higher entropy = more complex/diverse spatial structure.
 */
float TCDE_ImageEntropy(const TCDE_Field* field);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Compute local gradient at position
 * @param image Input image
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param x X position (pixels)
 * @param y Y position (pixels)
 * @param magnitude Output gradient magnitude
 * @param orientation Output gradient orientation (radians)
 */
void TCDE_LocalGradient(const unsigned char* image,
                        int width, int height, int channels,
                        int x, int y,
                        float* magnitude,
                        float* orientation);

/**
 * @brief Get pixel value (with bounds checking)
 * @param image Input image
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param x X position
 * @param y Y position
 * @param channel Channel index
 * @return Pixel value [0,255]
 */
unsigned char TCDE_GetPixel(const unsigned char* image,
                            int width, int height, int channels,
                            int x, int y, int channel);

#ifdef __cplusplus
}
#endif

#endif // TCDE_IMAGE_H
