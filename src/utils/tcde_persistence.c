/**
 * @file tcde_persistence.c
 * @brief TCDE Persistence Module Implementation
 * 
 * State serialization and deserialization for End-to-Infinite paradigm.
 * 
 * @version 1.0
 * @date October 22, 2025
 */

#include "tcde_persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// INTERNAL STRUCTURES
// ============================================================================

/**
 * @brief File header for TCDE state files
 */
typedef struct {
    uint32_t magic;           // Magic number (TCDE_STATE_MAGIC)
    uint32_t version;         // Format version
    uint32_t num_centers_6d;  // Number of 6D centers
    uint32_t num_centers_2d;  // Number of 2D centers
    float fractal_dim;        // Fractal dimension
    float time;               // Current time
    uint32_t checksum;        // CRC32 checksum
} TCDE_StateHeader;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Compute simple CRC32 checksum
 */
static uint32_t compute_crc32(const void* data, size_t size) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

/**
 * @brief Write point to file
 */
static bool write_point(FILE* fp, const TCDE_Point* point) {
    if (!fp || !point || !point->coords) return false;
    
    // Write dimension
    if (fwrite(&point->dimension, sizeof(int), 1, fp) != 1) return false;
    
    // Write coordinates
    size_t coord_size = point->dimension * sizeof(float);
    if (fwrite(point->coords, 1, coord_size, fp) != coord_size) return false;
    
    return true;
}

/**
 * @brief Read point from file
 */
static bool read_point(FILE* fp, TCDE_Point* point) {
    if (!fp || !point) return false;
    
    // Read dimension
    if (fread(&point->dimension, sizeof(int), 1, fp) != 1) return false;
    
    // Allocate and read coordinates
    point->coords = (float*)malloc(point->dimension * sizeof(float));
    if (!point->coords) return false;
    
    size_t coord_size = point->dimension * sizeof(float);
    if (fread(point->coords, 1, coord_size, fp) != coord_size) {
        free(point->coords);
        point->coords = NULL;
        return false;
    }
    
    return true;
}


/**
 * @brief Write metric to file
 */
static bool write_metric(FILE* fp, const TCDE_Metric* metric) {
    if (!fp || !metric) return false;
    
    // Write dimension and validity
    if (fwrite(&metric->dimension, sizeof(int), 1, fp) != 1) return false;
    if (fwrite(&metric->is_valid, sizeof(bool), 1, fp) != 1) return false;
    if (fwrite(&metric->det, sizeof(float), 1, fp) != 1) return false;
    
    // Only write tensors if metric is valid and allocated
    if (metric->g && metric->g_inv) {
        int dim = metric->dimension;
        
        // Write metric tensor
        for (int i = 0; i < dim; i++) {
            if (fwrite(metric->g[i], sizeof(float), dim, fp) != (size_t)dim) {
                return false;
            }
        }
        
        // Write inverse metric
        for (int i = 0; i < dim; i++) {
            if (fwrite(metric->g_inv[i], sizeof(float), dim, fp) != (size_t)dim) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * @brief Read metric from file
 */
static bool read_metric(FILE* fp, TCDE_Metric* metric) {
    if (!fp || !metric) return false;
    
    // Read dimension and validity
    if (fread(&metric->dimension, sizeof(int), 1, fp) != 1) return false;
    if (fread(&metric->is_valid, sizeof(bool), 1, fp) != 1) return false;
    if (fread(&metric->det, sizeof(float), 1, fp) != 1) return false;
    
    int dim = metric->dimension;
    
    // Check if we need to read tensors (file position will tell us)
    long pos_before = ftell(fp);
    
    // Try to peek if there's data
    uint8_t test_byte;
    size_t bytes_read = fread(&test_byte, 1, 1, fp);
    
    if (bytes_read == 0) {
        // No tensor data, metric was NULL when saved
        metric->g = NULL;
        metric->g_inv = NULL;
        return true;
    }
    
    // Rewind to read tensors
    fseek(fp, pos_before, SEEK_SET);
    
    // Allocate metric tensors
    metric->g = (float**)malloc(dim * sizeof(float*));
    metric->g_inv = (float**)malloc(dim * sizeof(float*));
    if (!metric->g || !metric->g_inv) return false;
    
    for (int i = 0; i < dim; i++) {
        metric->g[i] = (float*)malloc(dim * sizeof(float));
        metric->g_inv[i] = (float*)malloc(dim * sizeof(float));
        if (!metric->g[i] || !metric->g_inv[i]) return false;
    }
    
    // Read metric tensor
    for (int i = 0; i < dim; i++) {
        if (fread(metric->g[i], sizeof(float), dim, fp) != (size_t)dim) {
            return false;
        }
    }
    
    // Read inverse metric
    for (int i = 0; i < dim; i++) {
        if (fread(metric->g_inv[i], sizeof(float), dim, fp) != (size_t)dim) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Write center to file
 */
static bool write_center(FILE* fp, const TCDE_Center* center) {
    if (!fp || !center) return false;
    
    // Write point
    if (!write_point(fp, &center->point)) return false;
    
    // Write coefficient (complex)
    float coeff_real = crealf(center->coeff);
    float coeff_imag = cimagf(center->coeff);
    if (fwrite(&coeff_real, sizeof(float), 1, fp) != 1) return false;
    if (fwrite(&coeff_imag, sizeof(float), 1, fp) != 1) return false;
    
    // Write epsilon
    if (fwrite(&center->epsilon, sizeof(float), 1, fp) != 1) return false;
    
    // Write metric
    if (!write_metric(fp, &center->metric)) return false;
    
    return true;
}

/**
 * @brief Read center from file
 */
static bool read_center(FILE* fp, TCDE_Center* center) {
    if (!fp || !center) return false;
    
    // Read point
    if (!read_point(fp, &center->point)) return false;
    
    // Read coefficient (complex)
    float coeff_real, coeff_imag;
    if (fread(&coeff_real, sizeof(float), 1, fp) != 1) return false;
    if (fread(&coeff_imag, sizeof(float), 1, fp) != 1) return false;
    center->coeff = coeff_real + coeff_imag * I;
    
    // Read epsilon
    if (fread(&center->epsilon, sizeof(float), 1, fp) != 1) return false;
    
    // Read metric
    if (!read_metric(fp, &center->metric)) return false;
    
    return true;
}

// ============================================================================
// PUBLIC API
// ============================================================================

bool TCDE_SaveState(const TCDE_Field* field, const char* filename) {
    if (!field || !filename) {
        fprintf(stderr, "SaveState: invalid params\n");
        return false;
    }
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "SaveState: failed to open file %s\n", filename);
        return false;
    }
    
    bool success = false;
    
    uint32_t significant_centers_6d = 0;
    uint32_t significant_centers_2d = 0;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        if (cabsf(field->manifold_6d.centers[i].coeff) > 0.01f) {
            significant_centers_6d++;
        }
    }
    
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        if (cabsf(field->slice_2d.centers[i].coeff) > 0.01f) {
            significant_centers_2d++;
        }
    }
    
    TCDE_StateHeader header = {
        .magic = TCDE_STATE_MAGIC,
        .version = TCDE_STATE_VERSION,
        .num_centers_6d = significant_centers_6d,
        .num_centers_2d = significant_centers_2d,
        .fractal_dim = field->fractal_dimension,
        .time = field->time,
        .checksum = 0
    };
    
    if (fwrite(&header, sizeof(TCDE_StateHeader), 1, fp) != 1) goto cleanup;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        if (cabsf(field->manifold_6d.centers[i].coeff) > 0.01f) {
            if (!write_center(fp, &field->manifold_6d.centers[i])) goto cleanup;
        }
    }
    
    bool has_6d_metric = (field->manifold_6d.metric.g != NULL);
    if (fwrite(&has_6d_metric, sizeof(bool), 1, fp) != 1) goto cleanup;
    if (has_6d_metric) {
        bool is_identity = true;
        for (int i = 0; i < field->manifold_6d.metric.dimension && is_identity; i++) {
            for (int j = 0; j < field->manifold_6d.metric.dimension && is_identity; j++) {
                float expected = (i == j) ? 1.0f : 0.0f;
                if (fabsf(field->manifold_6d.metric.g[i][j] - expected) > 1e-6f) {
                    is_identity = false;
                }
            }
        }
        
        if (fwrite(&is_identity, sizeof(bool), 1, fp) != 1) goto cleanup;
        if (!is_identity) {
            if (!write_metric(fp, &field->manifold_6d.metric)) goto cleanup;
        }
    }
    
    for (int i = 0; i < field->slice_2d.num_centers; i++) {
        if (cabsf(field->slice_2d.centers[i].coeff) > 0.01f) {
            if (!write_center(fp, &field->slice_2d.centers[i])) goto cleanup;
        }
    }
    
    bool has_2d_metric = (field->slice_2d.metric.g != NULL);
    if (fwrite(&has_2d_metric, sizeof(bool), 1, fp) != 1) goto cleanup;
    if (has_2d_metric) {
        bool is_identity = true;
        for (int i = 0; i < field->slice_2d.metric.dimension && is_identity; i++) {
            for (int j = 0; j < field->slice_2d.metric.dimension && is_identity; j++) {
                float expected = (i == j) ? 1.0f : 0.0f;
                if (fabsf(field->slice_2d.metric.g[i][j] - expected) > 1e-6f) {
                    is_identity = false;
                }
            }
        }
        
        if (fwrite(&is_identity, sizeof(bool), 1, fp) != 1) goto cleanup;
        if (!is_identity) {
            if (!write_metric(fp, &field->slice_2d.metric)) goto cleanup;
        }
    }
    
    if (fwrite(&field->slice_2d.projection, sizeof(TCDE_Projection2D), 1, fp) != 1) {
        goto cleanup;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    uint8_t* file_data = (uint8_t*)malloc(file_size);
    if (!file_data) goto cleanup;
    
    if (fread(file_data, 1, file_size, fp) != (size_t)file_size) {
        free(file_data);
        goto cleanup;
    }
    
    uint32_t checksum = compute_crc32(file_data + sizeof(uint32_t) * 6, 
                                      file_size - sizeof(uint32_t) * 6);
    free(file_data);
    
    fseek(fp, offsetof(TCDE_StateHeader, checksum), SEEK_SET);
    if (fwrite(&checksum, sizeof(uint32_t), 1, fp) != 1) goto cleanup;
    
    success = true;
    
cleanup:
    fclose(fp);
    return success;
}


TCDE_Field* TCDE_LoadState(const char* filename) {
    if (!filename) return NULL;
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    TCDE_Field* field = NULL;
    
    TCDE_StateHeader header;
    if (fread(&header, sizeof(TCDE_StateHeader), 1, fp) != 1) goto cleanup;
    
    if (header.magic != TCDE_STATE_MAGIC) goto cleanup;
    if (header.version != TCDE_STATE_VERSION) goto cleanup;
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size > 100 * 1024 * 1024) {
        goto cleanup;
    }
    
    uint8_t* file_data = (uint8_t*)malloc(file_size);
    if (!file_data) goto cleanup;
    
    if (fread(file_data, 1, file_size, fp) != (size_t)file_size) {
        free(file_data);
        goto cleanup;
    }
    
    uint32_t computed_checksum = compute_crc32(file_data + sizeof(uint32_t) * 6,
                                               file_size - sizeof(uint32_t) * 6);
    free(file_data);
    
    if (computed_checksum != header.checksum) {
        fseek(fp, sizeof(TCDE_StateHeader), SEEK_SET);
        
        field = TCDE_CreateField(header.num_centers_6d, header.fractal_dim);
        if (!field) goto cleanup;
        field->time = header.time;
        
        uint32_t recovered_centers = 0;
        for (uint32_t i = 0; i < header.num_centers_6d && recovered_centers < (uint32_t)field->manifold_6d.capacity; i++) {
            if (read_center(fp, &field->manifold_6d.centers[recovered_centers])) {
                recovered_centers++;
            }
        }
        field->manifold_6d.num_centers = recovered_centers;
        
        goto cleanup;
    }
    
    fseek(fp, sizeof(TCDE_StateHeader), SEEK_SET);
    
    field = TCDE_CreateField(header.num_centers_6d, header.fractal_dim);
    if (!field) goto cleanup;
    
    field->time = header.time;
    
    for (uint32_t i = 0; i < header.num_centers_6d; i++) {
        if (!read_center(fp, &field->manifold_6d.centers[i])) {
            TCDE_DestroyField(field);
            field = NULL;
            goto cleanup;
        }
    }
    field->manifold_6d.num_centers = header.num_centers_6d;
    
    bool has_6d_metric;
    if (fread(&has_6d_metric, sizeof(bool), 1, fp) != 1) {
        TCDE_DestroyField(field);
        field = NULL;
        goto cleanup;
    }
    
    if (has_6d_metric) {
        bool is_identity;
        if (fread(&is_identity, sizeof(bool), 1, fp) != 1) {
            TCDE_DestroyField(field);
            field = NULL;
            goto cleanup;
        }
        
        if (!is_identity) {
            if (field->manifold_6d.metric.g) {
                for (int i = 0; i < field->manifold_6d.metric.dimension; i++) {
                    free(field->manifold_6d.metric.g[i]);
                    free(field->manifold_6d.metric.g_inv[i]);
                }
                free(field->manifold_6d.metric.g);
                free(field->manifold_6d.metric.g_inv);
            }
            
            if (!read_metric(fp, &field->manifold_6d.metric)) {
                TCDE_DestroyField(field);
                field = NULL;
                goto cleanup;
            }
        }
    }
    
    for (uint32_t i = 0; i < header.num_centers_2d; i++) {
        if (!read_center(fp, &field->slice_2d.centers[i])) {
            TCDE_DestroyField(field);
            field = NULL;
            goto cleanup;
        }
    }
    field->slice_2d.num_centers = header.num_centers_2d;
    
    bool has_2d_metric;
    if (fread(&has_2d_metric, sizeof(bool), 1, fp) != 1) {
        TCDE_DestroyField(field);
        field = NULL;
        goto cleanup;
    }
    
    if (has_2d_metric) {
        bool is_identity;
        if (fread(&is_identity, sizeof(bool), 1, fp) != 1) {
            TCDE_DestroyField(field);
            field = NULL;
            goto cleanup;
        }
        
        if (!is_identity) {
            if (field->slice_2d.metric.g) {
                for (int i = 0; i < field->slice_2d.metric.dimension; i++) {
                    free(field->slice_2d.metric.g[i]);
                    free(field->slice_2d.metric.g_inv[i]);
                }
                free(field->slice_2d.metric.g);
                free(field->slice_2d.metric.g_inv);
            }
            
            if (!read_metric(fp, &field->slice_2d.metric)) {
                TCDE_DestroyField(field);
                field = NULL;
                goto cleanup;
            }
        }
    }
    
    if (fread(&field->slice_2d.projection, sizeof(TCDE_Projection2D), 1, fp) != 1) {
        TCDE_DestroyField(field);
        field = NULL;
        goto cleanup;
    }
    
cleanup:
    fclose(fp);
    return field;
}

bool TCDE_VerifyStateFile(const char* filename) {
    if (!filename) return false;
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return false;
    
    // Read header
    TCDE_StateHeader header;
    if (fread(&header, sizeof(TCDE_StateHeader), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    
    // Verify magic and version
    if (header.magic != TCDE_STATE_MAGIC) {
        fclose(fp);
        return false;
    }
    
    if (header.version != TCDE_STATE_VERSION) {
        fclose(fp);
        return false;
    }
    
    // Verify checksum
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    uint8_t* file_data = (uint8_t*)malloc(file_size);
    if (!file_data) {
        fclose(fp);
        return false;
    }
    
    if (fread(file_data, 1, file_size, fp) != (size_t)file_size) {
        free(file_data);
        fclose(fp);
        return false;
    }
    
    uint32_t computed_checksum = compute_crc32(file_data + sizeof(uint32_t) * 6,
                                               file_size - sizeof(uint32_t) * 6);
    free(file_data);
    fclose(fp);
    
    return (computed_checksum == header.checksum);
}
