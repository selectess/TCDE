/**
 * TCDE Capability Validator
 * 
 * Generic validation framework for TCDE capabilities.
 * Provides standardized interfaces for capability testing and validation.
 * 
 * Date: November 10, 2025
 * Protocol: Zero Tolerance v3.0
 */

#ifndef TCDE_CAPABILITY_VALIDATOR_H
#define TCDE_CAPABILITY_VALIDATOR_H

#include <stddef.h>  // For size_t
#include <stdint.h>
#include <stdbool.h>

// Capability score structure
typedef struct {
    uint32_t capability_id;
    char capability_name[128];
    double score;
    double min_expected;
    double max_expected;
    bool is_valid;
    char validation_message[256];
} TCDE_CapabilityScore;

// Statistical validation results
typedef struct {
    double mean;
    double std_dev;
    double confidence_interval_95_lower;
    double confidence_interval_95_upper;
    double confidence_interval_99_lower;
    double confidence_interval_99_upper;
    double p_value;
    uint32_t sample_count;
    bool is_significant;  // p < 0.001
} TCDE_StatisticalResult;

// Validation context
typedef struct {
    uint32_t test_run_id;
    uint64_t timestamp;
    uint32_t iteration_count;
    double* samples;
    uint32_t sample_capacity;
} TCDE_ValidationContext;

// Function pointer for capability test
typedef TCDE_CapabilityScore (*TCDE_CapabilityTestFunc)(void* test_data);

/**
 * Initialize validation context
 * 
 * @param ctx Validation context to initialize
 * @param max_samples Maximum number of samples to store
 * @return true if successful, false otherwise
 */
bool TCDE_InitValidationContext(TCDE_ValidationContext* ctx, uint32_t max_samples);

/**
 * Free validation context resources
 * 
 * @param ctx Validation context to free
 */
void TCDE_FreeValidationContext(TCDE_ValidationContext* ctx);

/**
 * Execute capability test with statistical validation
 * 
 * @param test_func Test function to execute
 * @param test_data Data to pass to test function
 * @param iterations Number of iterations for statistical validation
 * @param ctx Validation context
 * @param result Statistical result output
 * @return Final capability score
 */
TCDE_CapabilityScore TCDE_ExecuteCapabilityTest(
    TCDE_CapabilityTestFunc test_func,
    void* test_data,
    uint32_t iterations,
    TCDE_ValidationContext* ctx,
    TCDE_StatisticalResult* result
);

/**
 * Validate capability score against expected range
 * 
 * @param score Capability score to validate
 * @return true if score is within expected range, false otherwise
 */
bool TCDE_ValidateCapabilityScore(const TCDE_CapabilityScore* score);

/**
 * Compute statistical metrics from samples
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param result Statistical result output
 */
void TCDE_ComputeStatistics(
    const double* samples,
    uint32_t count,
    TCDE_StatisticalResult* result
);

/**
 * Check if statistical result meets significance threshold
 * 
 * @param result Statistical result to check
 * @param threshold Significance threshold (default: 0.001)
 * @return true if p-value < threshold, false otherwise
 */
bool TCDE_IsStatisticallySignificant(
    const TCDE_StatisticalResult* result,
    double threshold
);

/**
 * Format capability score as JSON string
 * 
 * @param score Capability score to format
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return Number of characters written
 */
int TCDE_FormatCapabilityScoreJSON(
    const TCDE_CapabilityScore* score,
    char* buffer,
    size_t buffer_size
);

/**
 * Format statistical result as JSON string
 * 
 * @param result Statistical result to format
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return Number of characters written
 */
int TCDE_FormatStatisticalResultJSON(
    const TCDE_StatisticalResult* result,
    char* buffer,
    size_t buffer_size
);

/**
 * Validate reproducibility across multiple runs
 * 
 * @param results Array of statistical results from different runs
 * @param count Number of results
 * @param max_variance Maximum allowed variance (default: 0.001)
 * @return true if reproducible within variance threshold, false otherwise
 */
bool TCDE_ValidateReproducibility(
    const TCDE_StatisticalResult* results,
    uint32_t count,
    double max_variance
);

#endif // TCDE_CAPABILITY_VALIDATOR_H
