/**
 * @file tcde_image.c
 * @brief TCDE Image Processing Implementation
 * 
 * Continuous image processing WITHOUT patchification.
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#include "tcde_image.h"
#include "../core/tcde_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get pixel value with bounds checking
 */
unsigned char TCDE_GetPixel(const unsigned char* image,
                            int width, int height, int channels,
                            int x, int y, int channel) {
    if (!image || x < 0 || x >= width || y < 0 || y >= height || 
        channel < 0 || channel >= channels) {
        return 0;
    }
    
    return image[(y * width + x) * channels + channel];
}

/**
 * @brief Compute local gradient
 */
void TCDE_LocalGradient(const unsigned char* image,
                        int width, int height, int channels,
                        int x, int y,
                        float* magnitude,
                        float* orientation) {
    if (!image || !magnitude || !orientation) return;
    
    // Sobel operators
    float gx = 0.0f, gy = 0.0f;
    
    // Average across channels
    for (int c = 0; c < channels; c++) {
        // Horizontal gradient (Sobel X)
        float dx = 0.0f;
        dx += -1.0f * TCDE_GetPixel(image, width, height, channels, x-1, y-1, c);
        dx += -2.0f * TCDE_GetPixel(image, width, height, channels, x-1, y, c);
        dx += -1.0f * TCDE_GetPixel(image, width, height, channels, x-1, y+1, c);
        dx +=  1.0f * TCDE_GetPixel(image, width, height, channels, x+1, y-1, c);
        dx +=  2.0f * TCDE_GetPixel(image, width, height, channels, x+1, y, c);
        dx +=  1.0f * TCDE_GetPixel(image, width, height, channels, x+1, y+1, c);
        
        // Vertical gradient (Sobel Y)
        float dy = 0.0f;
        dy += -1.0f * TCDE_GetPixel(image, width, height, channels, x-1, y-1, c);
        dy += -2.0f * TCDE_GetPixel(image, width, height, channels, x, y-1, c);
        dy += -1.0f * TCDE_GetPixel(image, width, height, channels, x+1, y-1, c);
        dy +=  1.0f * TCDE_GetPixel(image, width, height, channels, x-1, y+1, c);
        dy +=  2.0f * TCDE_GetPixel(image, width, height, channels, x, y+1, c);
        dy +=  1.0f * TCDE_GetPixel(image, width, height, channels, x+1, y+1, c);
        
        gx += dx / channels;
        gy += dy / channels;
    }
    
    *magnitude = sqrtf(gx * gx + gy * gy) / 255.0f;  // Normalize
    *orientation = atan2f(gy, gx);
}

// ============================================================================
// SALIENCY COMPUTATION
// ============================================================================

/**
 * @brief Compute saliency map
 */
TCDE_SaliencyMap* TCDE_ComputeSaliency(const unsigned char* image,
                                        int width, int height,
                                        int channels) {
    if (!image || width <= 0 || height <= 0) return NULL;
    
    TCDE_SaliencyMap* map = malloc(sizeof(TCDE_SaliencyMap));
    if (!map) return NULL;
    
    map->width = width;
    map->height = height;
    map->values = malloc(width * height * sizeof(float));
    if (!map->values) {
        free(map);
        return NULL;
    }
    
    map->max_saliency = 0.0f;
    
    // Compute saliency based on gradient magnitude
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float magnitude, orientation;
            TCDE_LocalGradient(image, width, height, channels, x, y, 
                             &magnitude, &orientation);
            
            int idx = y * width + x;
            map->values[idx] = magnitude;
            
            if (magnitude > map->max_saliency) {
                map->max_saliency = magnitude;
            }
        }
    }
    
    return map;
}

/**
 * @brief Free saliency map
 */
void TCDE_FreeSaliencyMap(TCDE_SaliencyMap* map) {
    if (!map) return;
    if (map->values) free(map->values);
    free(map);
}

/**
 * @brief Sample points based on saliency (importance sampling)
 */
void TCDE_SampleBySaliency(const TCDE_SaliencyMap* saliency,
                           int num_samples,
                           float (*positions)[2]) {
    if (!saliency || !positions || num_samples <= 0) return;
    
    // Simple importance sampling: probability proportional to saliency
    for (int i = 0; i < num_samples; i++) {
        // Random sampling with rejection based on saliency
        int max_attempts = 100;
        bool found = false;
        
        for (int attempt = 0; attempt < max_attempts && !found; attempt++) {
            // Random position
            int x = rand() % saliency->width;
            int y = rand() % saliency->height;
            
            // Get saliency at this position
            float sal = saliency->values[y * saliency->width + x];
            float prob = sal / (saliency->max_saliency + 1e-6f);
            
            // Accept with probability proportional to saliency
            if ((float)rand() / RAND_MAX < prob) {
                positions[i][0] = (float)x / saliency->width;
                positions[i][1] = (float)y / saliency->height;
                found = true;
            }
        }
        
        // Fallback: uniform random
        if (!found) {
            positions[i][0] = (float)rand() / RAND_MAX;
            positions[i][1] = (float)rand() / RAND_MAX;
        }
    }
}

// ============================================================================
// FEATURE EXTRACTION
// ============================================================================

/**
 * @brief Extract continuous features from image
 */
int TCDE_ExtractImageFeatures(const unsigned char* image,
                              int width, int height, int channels,
                              TCDE_ImageFeature* features,
                              int max_features) {
    if (!image || !features || max_features <= 0) return 0;
    
    // Compute saliency map
    TCDE_SaliencyMap* saliency = TCDE_ComputeSaliency(image, width, height, channels);
    if (!saliency) return 0;
    
    // Sample positions based on saliency
    float (*positions)[2] = malloc(max_features * sizeof(float[2]));
    if (!positions) {
        TCDE_FreeSaliencyMap(saliency);
        return 0;
    }
    
    TCDE_SampleBySaliency(saliency, max_features, positions);
    
    // Extract features at sampled positions
    int num_features = 0;
    for (int i = 0; i < max_features; i++) {
        float fx = positions[i][0];
        float fy = positions[i][1];
        
        int px = (int)(fx * width);
        int py = (int)(fy * height);
        
        if (px < 0) px = 0;
        if (px >= width) px = width - 1;
        if (py < 0) py = 0;
        if (py >= height) py = height - 1;
        
        // Get local gradient
        float magnitude, orientation;
        TCDE_LocalGradient(image, width, height, channels, px, py,
                         &magnitude, &orientation);
        
        // Skip if too low saliency
        if (magnitude < 0.01f) continue;
        
        // Create feature
        TCDE_ImageFeature* feat = &features[num_features];
        
        // Spatial position (x,y normalized, z from intensity)
        feat->spatial_position[0] = fx;
        feat->spatial_position[1] = fy;
        
        // Z coordinate from average intensity
        float avg_intensity = 0.0f;
        for (int c = 0; c < channels; c++) {
            avg_intensity += TCDE_GetPixel(image, width, height, channels, px, py, c);
        }
        avg_intensity /= (channels * 255.0f);
        feat->spatial_position[2] = avg_intensity;
        
        // Temporal (static image)
        feat->temporal_coord = 1.0f;
        feat->anticipation = 0.0f;
        
        // Modality (visual)
        feat->modality = 0.0f;
        
        // Intensity (saliency)
        feat->intensity = magnitude;
        
        // Orientation
        feat->orientation = orientation;
        
        // Width (adaptive based on saliency)
        feat->width = 0.05f + 0.1f * (1.0f - magnitude);
        
        num_features++;
    }
    
    free(positions);
    TCDE_FreeSaliencyMap(saliency);
    
    return num_features;
}

// ============================================================================
// IMAGE TO FIELD CONVERSION
// ============================================================================

/**
 * @brief Convert image to continuous field
 */
int TCDE_ImageToField(const unsigned char* image,
                      int width, int height, int channels,
                      TCDE_Field* field,
                      int num_samples) {
    if (!image || !field || num_samples <= 0) return 0;
    
    // Limit samples
    if (num_samples > 1000) num_samples = 1000;
    
    // Extract features
    TCDE_ImageFeature* features = malloc(num_samples * sizeof(TCDE_ImageFeature));
    if (!features) return 0;
    
    int num_features = TCDE_ExtractImageFeatures(image, width, height, channels,
                                                 features, num_samples);
    
    // Add features as RBF centers to field
    for (int i = 0; i < num_features; i++) {
        TCDE_ImageFeature* feat = &features[i];
        
        // Create 6D point
        float coords[6] = {
            feat->spatial_position[0],
            feat->spatial_position[1],
            feat->spatial_position[2],
            feat->temporal_coord,
            feat->anticipation,
            feat->modality
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        
        // Create complex coefficient (intensity and orientation)
        TCDE_Complex coeff = feat->intensity * cexpf(I * feat->orientation);
        
        // Add to field
        TCDE_AddCenter6D(field, &point, coeff, feat->width);
        
        TCDE_FreePoint(&point);
    }
    
    free(features);
    
    return num_features;
}

// ============================================================================
// ADVANCED IMAGE ANALYSIS
// ============================================================================

void TCDE_AnalyzeImageField(const TCDE_Field* field,
                            TCDE_ImageAnalysis* analysis) {
    if (!field || !analysis) return;
    
    memset(analysis, 0, sizeof(TCDE_ImageAnalysis));
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return;
    
    // Spatial distribution (center of mass)
    float cx = 0.0f, cy = 0.0f, cz = 0.0f;
    float total_energy = 0.0f;
    
    for (int i = 0; i < K; i++) {
        float energy = cabsf(field->manifold_6d.centers[i].coeff);
        cx += field->manifold_6d.centers[i].point.coords[0] * energy;
        cy += field->manifold_6d.centers[i].point.coords[1] * energy;
        cz += field->manifold_6d.centers[i].point.coords[2] * energy;
        total_energy += energy;
    }
    
    if (total_energy > 0.0f) {
        analysis->focus_center[0] = cx / total_energy;
        analysis->focus_center[1] = cy / total_energy;
        analysis->focus_center[2] = cz / total_energy;
    }
    
    // Energy distribution (variance) - spatial complexity
    float var_x = 0.0f, var_y = 0.0f, var_z = 0.0f;
    for (int i = 0; i < K; i++) {
        float energy = cabsf(field->manifold_6d.centers[i].coeff);
        float dx = field->manifold_6d.centers[i].point.coords[0] - analysis->focus_center[0];
        float dy = field->manifold_6d.centers[i].point.coords[1] - analysis->focus_center[1];
        float dz = field->manifold_6d.centers[i].point.coords[2] - analysis->focus_center[2];
        var_x += dx * dx * energy;
        var_y += dy * dy * energy;
        var_z += dz * dz * energy;
    }
    
    analysis->spatial_complexity = sqrtf((var_x + var_y + var_z) / total_energy);
    
    // Edge density (from gradient magnitude)
    float total_gradient = 0.0f;
    for (int i = 0; i < K; i++) {
        // Approximate gradient from nearby centers
        float grad_mag = 0.0f;
        for (int j = 0; j < K; j++) {
            if (i == j) continue;
            float dist = 0.0f;
            for (int d = 0; d < 3; d++) {
                float diff = field->manifold_6d.centers[i].point.coords[d] - 
                            field->manifold_6d.centers[j].point.coords[d];
                dist += diff * diff;
            }
            if (dist < 0.1f) {
                float coeff_diff = cabsf(field->manifold_6d.centers[i].coeff - 
                                        field->manifold_6d.centers[j].coeff);
                grad_mag += coeff_diff / sqrtf(dist + 1e-6f);
            }
        }
        total_gradient += grad_mag;
    }
    analysis->edge_density = total_gradient / K;
    
    // Number of salient regions (will be filled by ExtractSalientRegions)
    analysis->num_regions = 0;
    
    // Coherence (phase alignment)
    TCDE_Complex phase_sum = 0.0f + 0.0f * I;
    for (int i = 0; i < K; i++) {
        TCDE_Complex c = field->manifold_6d.centers[i].coeff;
        if (cabsf(c) > 0.0f) {
            phase_sum += c / cabsf(c);  // Normalized
        }
    }
    analysis->coherence = cabsf(phase_sum) / K;
}

float TCDE_ImageSimilarity(const TCDE_Field* field1,
                          const TCDE_Field* field2,
                          const TCDE_Metric* metric) {
    if (!field1 || !field2) return 0.0f;
    
    // TCDE Authentic: Similarity via geodesic distance in field space
    // Sample both fields at grid points and compute correlation
    
    int grid_size = 20;
    float step = 1.0f / grid_size;
    
    float correlation = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int ix = 0; ix < grid_size; ix++) {
        for (int iy = 0; iy < grid_size; iy++) {
            float x = ix * step;
            float y = iy * step;
            float z = 0.5f;  // Middle slice
            
            // Create sample point
            float coords[6] = {x, y, z, 1.5f, 0.0f, 0.0f};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            
            // Evaluate both fields
            TCDE_Complex v1 = TCDE_Evaluate6D(field1, &p);
            TCDE_Complex v2 = TCDE_Evaluate6D(field2, &p);
            
            // Correlation (real part of conjugate product)
            float re1 = crealf(v1), im1 = cimagf(v1);
            float re2 = crealf(v2), im2 = cimagf(v2);
            
            correlation += re1 * re2 + im1 * im2;
            norm1 += re1 * re1 + im1 * im1;
            norm2 += re2 * re2 + im2 * im2;
            
            TCDE_FreePoint(&p);
        }
    }
    
    // Normalized correlation (cosine similarity)
    if (norm1 > 0.0f && norm2 > 0.0f) {
        return correlation / (sqrtf(norm1) * sqrtf(norm2));
    }
    
    (void)metric;  // Could use for weighted distance
    return 0.0f;
}

int TCDE_ImageToFieldMultiScale(const unsigned char* image,
                                int width, int height, int channels,
                                TCDE_Field* field,
                                int num_scales,
                                float scale_factor) {
    if (!image || !field || num_scales <= 0) return 0;
    
    int total_centers = 0;
    
    // Process at multiple scales (pyramid)
    for (int scale = 0; scale < num_scales; scale++) {
        float current_scale = powf(scale_factor, (float)scale);
        int scaled_width = (int)(width / current_scale);
        int scaled_height = (int)(height / current_scale);
        
        if (scaled_width < 4 || scaled_height < 4) break;
        
        // Downsample image (simple averaging)
        unsigned char* scaled_image = (unsigned char*)malloc(
            scaled_width * scaled_height * channels);
        if (!scaled_image) continue;
        
        for (int y = 0; y < scaled_height; y++) {
            for (int x = 0; x < scaled_width; x++) {
                // Average pixels in window
                int src_x = (int)(x * current_scale);
                int src_y = (int)(y * current_scale);
                int window = (int)current_scale;
                
                float avg[3] = {0.0f, 0.0f, 0.0f};
                int count = 0;
                
                for (int wy = 0; wy < window && src_y + wy < height; wy++) {
                    for (int wx = 0; wx < window && src_x + wx < width; wx++) {
                        int idx = ((src_y + wy) * width + (src_x + wx)) * channels;
                        for (int c = 0; c < channels && c < 3; c++) {
                            avg[c] += image[idx + c];
                        }
                        count++;
                    }
                }
                
                int dst_idx = (y * scaled_width + x) * channels;
                for (int c = 0; c < channels && c < 3; c++) {
                    scaled_image[dst_idx + c] = (unsigned char)(avg[c] / count);
                }
            }
        }
        
        // Process scaled image (use fewer samples for scaled versions)
        int samples_for_scale = 100 / (scale + 1);
        int centers_added = TCDE_ImageToField(scaled_image, scaled_width, 
                                              scaled_height, channels, field, samples_for_scale);
        total_centers += centers_added;
        
        free(scaled_image);
    }
    
    return total_centers;
}

int TCDE_ExtractSalientRegions(const TCDE_Field* field,
                               float (*regions)[3],
                               int max_regions) {
    if (!field || !regions || max_regions <= 0) return 0;
    
    // TCDE Authentic: Salient regions = local maxima of field magnitude
    // This is geometric extraction, not external algorithm
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return 0;
    
    int num_regions = 0;
    float search_radius = 0.15f;
    float min_saliency = 0.3f;  // Threshold for significance
    
    // Find local maxima in spatial dimensions (x,y,z)
    for (int i = 0; i < K && num_regions < max_regions; i++) {
        float mag_i = cabsf(field->manifold_6d.centers[i].coeff);
        
        if (mag_i < min_saliency) continue;
        
        // Check if local maximum
        bool is_maximum = true;
        
        for (int j = 0; j < K; j++) {
            if (i == j) continue;
            
            // Distance in spatial dimensions only
            float dist_sq = 0.0f;
            for (int d = 0; d < 3; d++) {
                float diff = field->manifold_6d.centers[i].point.coords[d] - 
                            field->manifold_6d.centers[j].point.coords[d];
                dist_sq += diff * diff;
            }
            
            if (dist_sq < search_radius * search_radius) {
                float mag_j = cabsf(field->manifold_6d.centers[j].coeff);
                if (mag_j > mag_i) {
                    is_maximum = false;
                    break;
                }
            }
        }
        
        if (is_maximum) {
            // Store spatial position
            for (int d = 0; d < 3; d++) {
                regions[num_regions][d] = field->manifold_6d.centers[i].point.coords[d];
            }
            num_regions++;
        }
    }
    
    return num_regions;
}

float TCDE_ImageEntropy(const TCDE_Field* field) {
    if (!field) return 0.0f;
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return 0.0f;
    
    // Shannon entropy of energy distribution
    float* probs = (float*)malloc(K * sizeof(float));
    if (!probs) return 0.0f;
    
    float total = 0.0f;
    for (int i = 0; i < K; i++) {
        probs[i] = cabsf(field->manifold_6d.centers[i].coeff);
        total += probs[i];
    }
    
    // Normalize
    if (total > 0.0f) {
        for (int i = 0; i < K; i++) {
            probs[i] /= total;
        }
    }
    
    // Compute entropy
    float entropy = 0.0f;
    for (int i = 0; i < K; i++) {
        if (probs[i] > 1e-10f) {
            entropy -= probs[i] * log2f(probs[i]);
        }
    }
    
    free(probs);
    
    // Normalize by maximum entropy
    float max_entropy = log2f((float)K);
    return (max_entropy > 0.0f) ? entropy / max_entropy : 0.0f;
}
