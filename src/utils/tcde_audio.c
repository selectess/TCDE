/**
 * @file tcde_audio.c
 * @brief TCDE Audio Processing Implementation
 * 
 * Continuous audio processing WITHOUT frame discretization.
 * 
 * @version 1.0
 * @date October 18, 2025
 */

#include "tcde_audio.h"
#include "../core/tcde_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// WINDOW FUNCTIONS
// ============================================================================

/**
 * @brief Generate Hann window
 */
static void generate_hann_window(float* window, int size) {
    for (int i = 0; i < size; i++) {
        window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (size - 1)));
    }
}

/**
 * @brief Generate Hamming window
 */
static void generate_hamming_window(float* window, int size) {
    for (int i = 0; i < size; i++) {
        window[i] = 0.54f - 0.46f * cosf(2.0f * M_PI * i / (size - 1));
    }
}

/**
 * @brief Generate Blackman window
 */
static void generate_blackman_window(float* window, int size) {
    for (int i = 0; i < size; i++) {
        float a0 = 0.42f;
        float a1 = 0.5f;
        float a2 = 0.08f;
        window[i] = a0 - a1 * cosf(2.0f * M_PI * i / (size - 1)) 
                       + a2 * cosf(4.0f * M_PI * i / (size - 1));
    }
}

/**
 * @brief Generate window based on type
 */
__attribute__((unused))
static void generate_window(float* window, int size, TCDE_WindowType type) {
    switch (type) {
        case TCDE_WINDOW_HANN:
            generate_hann_window(window, size);
            break;
        case TCDE_WINDOW_HAMMING:
            generate_hamming_window(window, size);
            break;
        case TCDE_WINDOW_BLACKMAN:
            generate_blackman_window(window, size);
            break;
        case TCDE_WINDOW_RECTANGULAR:
        default:
            for (int i = 0; i < size; i++) {
                window[i] = 1.0f;
            }
            break;
    }
}

// ============================================================================
// FFT IMPLEMENTATION (Cooley-Tukey Algorithm)
// ============================================================================

/**
 * @brief Bit-reversal permutation for FFT
 */
static void bit_reverse(float* real, float* imag, int n) {
    int j = 0;
    for (int i = 0; i < n - 1; i++) {
        if (i < j) {
            // Swap real parts
            float temp = real[i];
            real[i] = real[j];
            real[j] = temp;
            
            // Swap imaginary parts
            temp = imag[i];
            imag[i] = imag[j];
            imag[j] = temp;
        }
        
        int k = n / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}

/**
 * @brief Cooley-Tukey FFT (radix-2, in-place)
 * 
 * Optimized FFT: O(n log n) instead of O(n²) for DFT
 * Requires n to be power of 2
 */
static void fft_cooley_tukey(float* real, float* imag, int n) {
    if (n <= 1) return;
    
    // Bit-reversal permutation
    bit_reverse(real, imag, n);
    
    // Cooley-Tukey decimation-in-time
    for (int size = 2; size <= n; size *= 2) {
        float angle = -2.0f * M_PI / size;
        float wlen_real = cosf(angle);
        float wlen_imag = sinf(angle);
        
        for (int i = 0; i < n; i += size) {
            float w_real = 1.0f;
            float w_imag = 0.0f;
            
            for (int j = 0; j < size / 2; j++) {
                int idx1 = i + j;
                int idx2 = i + j + size / 2;
                
                // Butterfly operation
                float t_real = w_real * real[idx2] - w_imag * imag[idx2];
                float t_imag = w_real * imag[idx2] + w_imag * real[idx2];
                
                real[idx2] = real[idx1] - t_real;
                imag[idx2] = imag[idx1] - t_imag;
                real[idx1] = real[idx1] + t_real;
                imag[idx1] = imag[idx1] + t_imag;
                
                // Update twiddle factor
                float w_temp = w_real;
                w_real = w_real * wlen_real - w_imag * wlen_imag;
                w_imag = w_temp * wlen_imag + w_imag * wlen_real;
            }
        }
    }
}

/**
 * @brief Compute FFT and extract magnitude/phase
 */
static void compute_fft(const float* input, int n, float* magnitude, float* phase) {
    // Allocate buffers
    float* real = (float*)malloc(n * sizeof(float));
    float* imag = (float*)malloc(n * sizeof(float));
    
    if (!real || !imag) {
        free(real);
        free(imag);
        return;
    }
    
    // Copy input and zero imaginary part
    for (int i = 0; i < n; i++) {
        real[i] = input[i];
        imag[i] = 0.0f;
    }
    
    // Compute FFT
    fft_cooley_tukey(real, imag, n);
    
    // Extract magnitude and phase (first half only - positive frequencies)
    for (int k = 0; k < n / 2; k++) {
        magnitude[k] = sqrtf(real[k] * real[k] + imag[k] * imag[k]) / n;
        phase[k] = atan2f(imag[k], real[k]);
    }
    
    free(real);
    free(imag);
}

/**
 * @brief Complete STFT with proper windowing and overlap
 * 
 * Improvements over simplified version:
 * - Uses FFT instead of DFT (O(n log n) vs O(n²))
 * - Proper Hann windowing
 * - Configurable overlap (50-75% recommended)
 * - Normalized output
 */
void TCDE_CompleteSTFT(const float* samples,
                       int num_samples,
                       int window_size,
                       int hop_size,
                       float** spectrogram,
                       float** phase_data,
                       int* num_frames) {
    if (!samples || !spectrogram || !num_frames) return;
    
    // Ensure window_size is power of 2 for FFT
    int fft_size = 1;
    while (fft_size < window_size) fft_size *= 2;
    
    *num_frames = (num_samples - window_size) / hop_size + 1;
    if (*num_frames <= 0) {
        *num_frames = 0;
        return;
    }
    
    int freq_bins = fft_size / 2;
    *spectrogram = (float*)malloc(*num_frames * freq_bins * sizeof(float));
    if (phase_data) {
        *phase_data = (float*)malloc(*num_frames * freq_bins * sizeof(float));
    }
    
    if (!*spectrogram || (phase_data && !*phase_data)) {
        free(*spectrogram);
        if (phase_data) free(*phase_data);
        *num_frames = 0;
        return;
    }
    
    float* window = (float*)malloc(fft_size * sizeof(float));
    float* magnitude = (float*)malloc(freq_bins * sizeof(float));
    float* phase = (float*)malloc(freq_bins * sizeof(float));
    
    if (!window || !magnitude || !phase) {
        free(*spectrogram);
        if (phase_data) free(*phase_data);
        free(window);
        free(magnitude);
        free(phase);
        *num_frames = 0;
        return;
    }
    
    // Compute STFT
    for (int frame = 0; frame < *num_frames; frame++) {
        int start = frame * hop_size;
        
        // Apply Hann window with zero-padding
        for (int i = 0; i < fft_size; i++) {
            if (i < window_size && start + i < num_samples) {
                float hann = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (window_size - 1)));
                window[i] = samples[start + i] * hann;
            } else {
                window[i] = 0.0f;  // Zero-padding
            }
        }
        
        // Compute FFT
        compute_fft(window, fft_size, magnitude, phase);
        
        // Store magnitude and phase
        memcpy(*spectrogram + frame * freq_bins, magnitude, freq_bins * sizeof(float));
        if (phase_data) {
            memcpy(*phase_data + frame * freq_bins, phase, freq_bins * sizeof(float));
        }
    }
    
    free(window);
    free(magnitude);
    free(phase);
}

/**
 * @brief Legacy STFT (calls complete version)
 */
void TCDE_SimpleSTFT(const float* samples,
                     int num_samples,
                     int window_size,
                     int hop_size,
                     float** spectrogram,
                     int* num_frames) {
    TCDE_CompleteSTFT(samples, num_samples, window_size, hop_size,
                      spectrogram, NULL, num_frames);
}

// ============================================================================
// FEATURE EXTRACTION
// ============================================================================

/**
 * @brief Extract continuous audio features
 */
int TCDE_ExtractAudioFeatures(const float* samples,
                              int num_samples,
                              int sample_rate,
                              TCDE_AudioFeature* features,
                              int max_features) {
    if (!samples || !features || max_features <= 0) return 0;
    
    // STFT parameters
    int window_size = 512;
    int hop_size = 256;  // 50% overlap for continuity
    
    float* spectrogram = NULL;
    int num_frames = 0;
    
    TCDE_SimpleSTFT(samples, num_samples, window_size, hop_size,
                    &spectrogram, &num_frames);
    
    if (!spectrogram || num_frames == 0) return 0;
    
    int freq_bins = window_size / 2;
    int num_features = 0;
    
    // Sample significant spectro-temporal points
    float duration = (float)num_samples / sample_rate;
    
    for (int frame = 0; frame < num_frames && num_features < max_features; frame++) {
        float time = (float)frame * hop_size / sample_rate;
        
        // Find peak frequency in this frame
        float max_energy = 0.0f;
        int peak_freq = 0;
        
        for (int f = 0; f < freq_bins; f++) {
            float energy = spectrogram[frame * freq_bins + f];
            if (energy > max_energy) {
                max_energy = energy;
                peak_freq = f;
            }
        }
        
        // Skip if energy too low
        if (max_energy < 0.01f) continue;
        
        // Create feature
        TCDE_AudioFeature* feat = &features[num_features];
        
        // Spectral position
        feat->spectral_position[0] = time / duration;  // Normalized time
        feat->spectral_position[1] = (float)peak_freq / freq_bins;  // Normalized frequency
        feat->spectral_position[2] = max_energy;  // Energy as z-coordinate
        
        // Temporal coordinate
        feat->temporal_coord = 1.0f + time / duration;
        
        // Anticipation (simple: look ahead)
        feat->anticipation = (frame < num_frames - 1) ? 0.1f : 0.0f;
        
        // Modality (auditory)
        feat->modality = 0.2f;
        
        // Energy
        feat->energy = max_energy;
        
        // Phase (simplified: use frequency as proxy)
        feat->phase = 2.0f * M_PI * peak_freq / freq_bins;
        
        // Width (temporal scale)
        feat->width = 0.1f;
        
        num_features++;
    }
    
    free(spectrogram);
    
    return num_features;
}

// ============================================================================
// AUDIO TO FIELD CONVERSION
// ============================================================================

/**
 * @brief Convert audio to continuous field
 */
int TCDE_AudioToField(const float* samples,
                      int num_samples,
                      int sample_rate,
                      TCDE_Field* field,
                      int num_features_target) {
    if (!samples || !field || num_features_target <= 0) return 0;
    
    // Limit features
    if (num_features_target > 500) num_features_target = 500;
    
    // Extract features
    TCDE_AudioFeature* features = malloc(num_features_target * sizeof(TCDE_AudioFeature));
    if (!features) return 0;
    
    int num_features = TCDE_ExtractAudioFeatures(samples, num_samples, sample_rate,
                                                 features, num_features_target);
    
    // Add features as RBF centers to field
    for (int i = 0; i < num_features; i++) {
        TCDE_AudioFeature* feat = &features[i];
        
        // Create 6D point
        float coords[6] = {
            feat->spectral_position[0],
            feat->spectral_position[1],
            feat->spectral_position[2],
            feat->temporal_coord,
            feat->anticipation,
            feat->modality
        };
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        
        // Create complex coefficient (energy and phase)
        TCDE_Complex coeff = feat->energy * cexpf(I * feat->phase);
        
        // Add to field
        TCDE_AddCenter6D(field, &point, coeff, feat->width);
        
        TCDE_FreePoint(&point);
    }
    
    free(features);
    
    return num_features;
}

// ============================================================================
// ADVANCED AUDIO ANALYSIS
// ============================================================================

void TCDE_AnalyzeAudioField(const TCDE_Field* field,
                            TCDE_AudioAnalysis* analysis) {
    if (!field || !analysis) return;
    
    memset(analysis, 0, sizeof(TCDE_AudioAnalysis));
    
    int K = field->manifold_6d.num_centers;
    if (K == 0) return;
    
    // Dominant frequency (weighted average)
    float weighted_freq = 0.0f;
    float total_energy = 0.0f;
    
    for (int i = 0; i < K; i++) {
        float freq = field->manifold_6d.centers[i].point.coords[1];  // y = frequency
        float energy = cabsf(field->manifold_6d.centers[i].coeff);
        weighted_freq += freq * energy;
        total_energy += energy;
    }
    
    if (total_energy > 0.0f) {
        analysis->dominant_frequency = weighted_freq / total_energy;
    }
    
    // Temporal center (weighted average time)
    float weighted_time = 0.0f;
    for (int i = 0; i < K; i++) {
        float time = field->manifold_6d.centers[i].point.coords[3];  // τ₁
        float energy = cabsf(field->manifold_6d.centers[i].coeff);
        weighted_time += time * energy;
    }
    if (total_energy > 0.0f) {
        analysis->temporal_center = weighted_time / total_energy;
    }
    
    // Spectral complexity (variance of frequencies)
    float freq_variance = 0.0f;
    for (int i = 0; i < K; i++) {
        float freq = field->manifold_6d.centers[i].point.coords[1];
        float energy = cabsf(field->manifold_6d.centers[i].coeff);
        float diff = freq - analysis->dominant_frequency;
        freq_variance += diff * diff * energy;
    }
    if (total_energy > 0.0f) {
        analysis->spectral_complexity = sqrtf(freq_variance / total_energy);
    }
    
    // Temporal coherence (phase alignment)
    TCDE_Complex phase_sum = 0.0f + 0.0f * I;
    for (int i = 0; i < K; i++) {
        TCDE_Complex c = field->manifold_6d.centers[i].coeff;
        if (cabsf(c) > 0.0f) {
            phase_sum += c / cabsf(c);  // Normalized
        }
    }
    analysis->temporal_coherence = cabsf(phase_sum) / K;
    
    // Number of events (local maxima in time)
    int num_events = 0;
    float event_threshold = total_energy / (K * 2.0f);  // Half of average
    
    for (int i = 0; i < K; i++) {
        float energy_i = cabsf(field->manifold_6d.centers[i].coeff);
        if (energy_i < event_threshold) continue;
        
        // Check if local maximum in time
        bool is_event = true;
        float time_i = field->manifold_6d.centers[i].point.coords[3];
        
        for (int j = 0; j < K; j++) {
            if (i == j) continue;
            float time_j = field->manifold_6d.centers[j].point.coords[3];
            float time_diff = fabsf(time_i - time_j);
            
            if (time_diff < 0.05f) {  // Nearby in time
                float energy_j = cabsf(field->manifold_6d.centers[j].coeff);
                if (energy_j > energy_i) {
                    is_event = false;
                    break;
                }
            }
        }
        
        if (is_event) num_events++;
    }
    analysis->num_events = num_events;
}

float TCDE_AudioSimilarity(const TCDE_Field* field1,
                          const TCDE_Field* field2,
                          const TCDE_Metric* metric) {
    if (!field1 || !field2) return 0.0f;
    
    // TCDE Authentic: Similarity via spectro-temporal correlation
    // Sample both fields in frequency-time grid
    
    int freq_bins = 20;
    int time_bins = 20;
    
    float correlation = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int f = 0; f < freq_bins; f++) {
        for (int t = 0; t < time_bins; t++) {
            float freq = (float)f / freq_bins;
            float time = 1.0f + (float)t / time_bins;
            
            // Create sample point (freq, time in auditory modality)
            float coords[6] = {0.5f, freq, 0.5f, time, 0.0f, 0.2f};  // m=0.2 auditory
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            
            // Evaluate both fields
            TCDE_Complex v1 = TCDE_Evaluate6D(field1, &p);
            TCDE_Complex v2 = TCDE_Evaluate6D(field2, &p);
            
            // Correlation
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
    
    (void)metric;
    return 0.0f;
}

// ============================================================================
// SPECTROGRAM MANAGEMENT
// ============================================================================

/**
 * @brief Create complete spectrogram structure
 */
TCDE_Spectrogram* TCDE_CreateSpectrogram(const float* samples,
                                         int num_samples,
                                         float sample_rate,
                                         int window_size,
                                         int hop_size,
                                         TCDE_WindowType window_type) {
    if (!samples || num_samples <= 0 || window_size <= 0 || hop_size <= 0) {
        return NULL;
    }
    
    TCDE_Spectrogram* spec = (TCDE_Spectrogram*)malloc(sizeof(TCDE_Spectrogram));
    if (!spec) return NULL;
    
    // Compute STFT (returns flat arrays)
    float* magnitude_flat = NULL;
    float* phase_flat = NULL;
    TCDE_CompleteSTFT(samples, num_samples, window_size, hop_size,
                      &magnitude_flat, &phase_flat, &spec->num_frames);
    
    if (!magnitude_flat || spec->num_frames == 0) {
        free(spec);
        return NULL;
    }
    
    // Set metadata
    spec->num_bins = window_size / 2;
    spec->sample_rate = sample_rate;
    spec->hop_size = (float)hop_size;
    spec->window_size = (float)window_size;
    spec->window_type = window_type;
    
    // Convert flat arrays to 2D arrays for easier access
    spec->magnitude = (float**)malloc(spec->num_frames * sizeof(float*));
    spec->phase = (float**)malloc(spec->num_frames * sizeof(float*));
    
    if (!spec->magnitude || !spec->phase) {
        free(magnitude_flat);
        free(phase_flat);
        free(spec->magnitude);
        free(spec->phase);
        free(spec);
        return NULL;
    }
    
    for (int i = 0; i < spec->num_frames; i++) {
        spec->magnitude[i] = magnitude_flat + i * spec->num_bins;
        spec->phase[i] = phase_flat + i * spec->num_bins;
    }
    
    return spec;
}

/**
 * @brief Free spectrogram structure
 */
void TCDE_FreeSpectrogram(TCDE_Spectrogram* spec) {
    if (!spec) return;
    
    if (spec->magnitude) {
        // Free the flat data (pointed to by first row)
        if (spec->magnitude[0]) {
            free(spec->magnitude[0]);
        }
        free(spec->magnitude);
    }
    if (spec->phase) {
        // Free the flat data (pointed to by first row)
        if (spec->phase[0]) {
            free(spec->phase[0]);
        }
        free(spec->phase);
    }
    
    free(spec);
}
