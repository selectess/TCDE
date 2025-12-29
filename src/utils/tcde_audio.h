/**
 * @file tcde_audio.h
 * @brief TCDE Audio Processing - Continuous Spectro-Temporal Field
 * 
 * CRITICAL PRINCIPLE: NO FRAME DISCRETIZATION!
 * 
 * Audio is processed as continuous spectro-temporal field,
 * not as discrete frames. This preserves temporal continuity
 * and enables richer representations.
 * 
 * Approach:
 * - STFT with overlapping windows (continuous)
 * - Sample significant spectro-temporal points
 * - Extract local features (energy, phase)
 * - Create RBF centers as continuous perturbations
 * - Result: Continuous field Φ(x,y,z,τ₁,τ₂,m)
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#ifndef TCDE_AUDIO_H
#define TCDE_AUDIO_H

#include "../core/tcde_core.h"
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// AUDIO FEATURE STRUCTURES
// ============================================================================

/**
 * @brief Continuous audio feature (NOT a frame!)
 * 
 * Represents a continuous perturbation in spectro-temporal space,
 * extracted from audio without discrete frame boundaries.
 */
typedef struct {
    float spectral_position[3];  ///< (time, frequency, energy) in [0,1]³
    float temporal_coord;        ///< Temporal coordinate τ₁ (audio time)
    float anticipation;          ///< Anticipation τ₂ (predicted continuation)
    float modality;              ///< Modality m (0.2 for auditory)
    float energy;                ///< Spectral energy
    float phase;                 ///< Spectral phase (radians)
    float width;                 ///< RBF width σ (temporal/spectral scale)
} TCDE_AudioFeature;

/**
 * @brief Audio analysis results
 */
typedef struct {
    float temporal_center;       ///< Center of temporal activity
    float dominant_frequency;    ///< Dominant frequency (normalized)
    float spectral_complexity;   ///< Spectral complexity measure
    int num_events;              ///< Number of detected events
    float temporal_coherence;    ///< Temporal coherence [0,1]
} TCDE_AudioAnalysis;

/**
 * @brief Window types for STFT
 */
typedef enum {
    TCDE_WINDOW_HANN,
    TCDE_WINDOW_HAMMING,
    TCDE_WINDOW_BLACKMAN,
    TCDE_WINDOW_RECTANGULAR
} TCDE_WindowType;

/**
 * @brief Complete spectrogram structure
 */
typedef struct {
    float** magnitude;           ///< Magnitude [num_frames][num_bins]
    float** phase;               ///< Phase [num_frames][num_bins]
    int num_frames;              ///< Number of time frames
    int num_bins;                ///< Number of frequency bins
    float sample_rate;           ///< Sample rate (Hz)
    float hop_size;              ///< Hop size (samples)
    float window_size;           ///< Window size (samples)
    TCDE_WindowType window_type; ///< Window type used
} TCDE_Spectrogram;

// ============================================================================
// AUDIO TO FIELD CONVERSION
// ============================================================================

/**
 * @brief Convert audio to continuous field (NO FRAME DISCRETIZATION!)
 * @param samples Input audio samples (mono)
 * @param num_samples Number of samples
 * @param sample_rate Sample rate (Hz)
 * @param field Output field (must be pre-allocated)
 * @param num_features Target number of features
 * @return Number of features created
 * 
 * Process:
 * 1. Compute STFT with overlapping windows
 * 2. Sample significant spectro-temporal points
 * 3. Extract local features at each point
 * 4. Create RBF centers
 * 5. Add to field as continuous perturbations
 * 
 * The resulting field is continuous in time and frequency.
 * 
 * Example:
 * ```c
 * TCDE_Field* field = TCDE_CreateField(500, 2.5f);
 * int num_features = TCDE_AudioToField(samples, 44100, 44100, field, 200);
 * // field now contains continuous spectro-temporal representation
 * ```
 */
int TCDE_AudioToField(const float* samples,
                      int num_samples,
                      int sample_rate,
                      TCDE_Field* field,
                      int num_features);

/**
 * @brief Extract continuous features from audio
 * @param samples Input audio samples
 * @param num_samples Number of samples
 * @param sample_rate Sample rate (Hz)
 * @param features Output features array (pre-allocated)
 * @param max_features Maximum number of features
 * @return Number of features extracted
 * 
 * Extracts continuous features based on:
 * - STFT (overlapping windows)
 * - Spectral energy peaks
 * - Temporal events
 * - Phase information
 * 
 * NO discrete frame boundaries.
 */
int TCDE_ExtractAudioFeatures(const float* samples,
                              int num_samples,
                              int sample_rate,
                              TCDE_AudioFeature* features,
                              int max_features);

// ============================================================================
// AUDIO ANALYSIS
// ============================================================================

/**
 * @brief Analyze audio field for temporal properties
 * @param field Field containing audio representation
 * @param analysis Output analysis results
 * 
 * Extracts:
 * - Temporal activity center
 * - Dominant frequency
 * - Spectral complexity
 * - Event detection
 * 
 * All properties emerge from field structure.
 */
void TCDE_AnalyzeAudioField(const TCDE_Field* field,
                            TCDE_AudioAnalysis* analysis);

/**
 * @brief Compute audio similarity between fields
 * @param field1 First audio field
 * @param field2 Second audio field
 * @param metric Metric for distance computation
 * @return Similarity score [0,1]
 * 
 * Similarity based on spectro-temporal overlap.
 * Uses geodesic distance, not frame-wise comparison.
 */
float TCDE_AudioSimilarity(const TCDE_Field* field1,
                          const TCDE_Field* field2,
                          const TCDE_Metric* metric);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Simple STFT computation (for demonstration)
 * @param samples Input samples
 * @param num_samples Number of samples
 * @param window_size Window size (power of 2)
 * @param hop_size Hop size (overlap = window_size - hop_size)
 * @param spectrogram Output spectrogram [num_frames][window_size/2]
 * @param num_frames Output number of frames
 * 
 * Note: This is a simplified STFT for demonstration.
 * Production code should use optimized FFT library.
 */
void TCDE_SimpleSTFT(const float* samples,
                     int num_samples,
                     int window_size,
                     int hop_size,
                     float** spectrogram,
                     int* num_frames);

/**
 * @brief Complete STFT with windowing and phase
 * @param samples Input samples
 * @param num_samples Number of samples
 * @param window_size Window size (power of 2)
 * @param hop_size Hop size
 * @param spectrogram Output magnitude spectrogram
 * @param phase_data Output phase spectrogram (can be NULL)
 * @param num_frames Output number of frames
 */
void TCDE_CompleteSTFT(const float* samples,
                       int num_samples,
                       int window_size,
                       int hop_size,
                       float** spectrogram,
                       float** phase_data,
                       int* num_frames);

/**
 * @brief Create spectrogram structure
 * @param samples Input samples
 * @param num_samples Number of samples
 * @param sample_rate Sample rate (Hz)
 * @param window_size Window size
 * @param hop_size Hop size
 * @param window_type Window type
 * @return Allocated spectrogram (must be freed with TCDE_FreeSpectrogram)
 */
TCDE_Spectrogram* TCDE_CreateSpectrogram(const float* samples,
                                         int num_samples,
                                         float sample_rate,
                                         int window_size,
                                         int hop_size,
                                         TCDE_WindowType window_type);

/**
 * @brief Free spectrogram structure
 * @param spec Spectrogram to free
 */
void TCDE_FreeSpectrogram(TCDE_Spectrogram* spec);

#ifdef __cplusplus
}
#endif

#endif // TCDE_AUDIO_H
