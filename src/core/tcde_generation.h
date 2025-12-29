/**
 * @file tcde_generation.h
 * @brief Pure TCDE modal generation (theory implementation)
 * 
 * Implements exactly the theory from docs/implementation_Model.md
 * No external additions - pure TCDE geometric generation
 */

#ifndef TCDE_GENERATION_H
#define TCDE_GENERATION_H

#include "tcde_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MODALITY TYPES (Pure TCDE Theory)
// ============================================================================

typedef enum {
    TCDE_TEXT_MODALITY = 0,   ///< m = 0.0
    TCDE_IMAGE_MODALITY = 1,  ///< m = 0.33
    TCDE_AUDIO_MODALITY = 2   ///< m = 0.67
} TCDE_ModalityType;

// ============================================================================
// PURE TCDE GENERATION FUNCTIONS
// ============================================================================

/**
 * @brief Generate text from field (pure TCDE)
 * @param field Field to sample
 * @param length Text length
 * @return Generated text (caller must free)
 * 
 * Pure TCDE: Sample field at m=0.0, convert values to characters
 * No external NLP - pure geometric sampling
 */
char* TCDE_GenerateText(const TCDE_Field* field, int length);

/**
 * @brief Generate image from field (pure TCDE)
 * @param field Field to sample
 * @param width Image width
 * @param height Image height
 * @return Generated image (caller must free)
 * 
 * Pure TCDE: Sample field at m=0.33, convert values to pixels
 * No external computer vision - pure geometric sampling
 */
unsigned char* TCDE_GenerateImage(const TCDE_Field* field, int width, int height);

/**
 * @brief Generate audio from field (pure TCDE)
 * @param field Field to sample
 * @param num_samples Number of audio samples
 * @param output_size Output size (set by function)
 * @return Generated audio (caller must free)
 * 
 * Pure TCDE: Sample field at m=0.67, convert to spectrum, IFFT
 * No external audio processing - pure geometric sampling
 */
float* TCDE_GenerateAudio(const TCDE_Field* field, int num_samples, int* output_size);

/**
 * @brief Generic modal generation (pure TCDE)
 * @param field Field to sample
 * @param output_modality Target modality
 * @param param1 First parameter (length/width/samples)
 * @param param2 Second parameter (unused/height/unused)
 * @return Generated output (caller must free)
 * 
 * Implements exact algorithm from docs/implementation_Model.md
 * Pure theory - no external dependencies
 */
void* TCDE_GenerateOutput(const TCDE_Field* field, 
                          TCDE_ModalityType output_modality,
                          int param1, int param2);

/**
 * @brief Cross-modal transformation (pure TCDE)
 * @param field Field to transform
 * @param from_modality Source modality
 * @param to_modality Target modality
 * 
 * Pure geometric transformation: change m dimension
 * No external models - pure TCDE theory
 */
void TCDE_TransformModality(TCDE_Field* field,
                            TCDE_ModalityType from_modality,
                            TCDE_ModalityType to_modality);

/**
 * @brief Validate cross-modal coherence (pure TCDE)
 * @param field Field to validate
 * @return Coherence score [0,1]
 * 
 * Pure geometric validation - measures correlation across modalities
 * No external metrics - pure TCDE theory
 */
float TCDE_ValidateCrossModalCoherence(const TCDE_Field* field);

#ifdef __cplusplus
}
#endif

#endif // TCDE_GENERATION_H
