/**
 * TCDE Statistical Validator
 * 
 * Advanced statistical validation functions for TCDE capabilities.
 * Provides rigorous statistical analysis and hypothesis testing.
 * 
 * Date: November 10, 2025
 * Protocol: Zero Tolerance v3.0
 */

#ifndef TCDE_STATISTICAL_VALIDATOR_H
#define TCDE_STATISTICAL_VALIDATOR_H

#include <stdint.h>
#include <stdbool.h>

// Hypothesis test result
typedef struct {
    double test_statistic;
    double p_value;
    double critical_value;
    bool reject_null;
    char test_name[64];
} TCDE_HypothesisTest;

// Reproducibility metrics
typedef struct {
    double inter_run_variance;
    double coefficient_of_variation;
    double reproducibility_score;  // [0, 1], 1 = perfect reproducibility
    bool is_reproducible;  // variance < 0.001
} TCDE_ReproducibilityMetrics;

// Distribution analysis
typedef struct {
    double skewness;
    double kurtosis;
    double median;
    double q1;  // First quartile
    double q3;  // Third quartile
    double iqr;  // Interquartile range
    bool is_normal;  // Passes normality test
} TCDE_DistributionAnalysis;

/**
 * Compute confidence interval using t-distribution
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param confidence_level Confidence level (e.g., 0.95, 0.99)
 * @param lower Output: lower bound
 * @param upper Output: upper bound
 * @return true if successful, false otherwise
 */
bool TCDE_ComputeConfidenceInterval(
    const double* samples,
    uint32_t count,
    double confidence_level,
    double* lower,
    double* upper
);

/**
 * Compute p-value for one-sample t-test
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param hypothesized_mean Hypothesized population mean
 * @return p-value
 */
double TCDE_ComputePValue(
    const double* samples,
    uint32_t count,
    double hypothesized_mean
);

/**
 * Perform one-sample t-test
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param hypothesized_mean Hypothesized population mean
 * @param alpha Significance level (default: 0.001)
 * @param result Output: hypothesis test result
 * @return true if test performed successfully, false otherwise
 */
bool TCDE_PerformTTest(
    const double* samples,
    uint32_t count,
    double hypothesized_mean,
    double alpha,
    TCDE_HypothesisTest* result
);

/**
 * Perform two-sample t-test
 * 
 * @param samples1 First sample array
 * @param count1 Number of samples in first array
 * @param samples2 Second sample array
 * @param count2 Number of samples in second array
 * @param alpha Significance level
 * @param result Output: hypothesis test result
 * @return true if test performed successfully, false otherwise
 */
bool TCDE_PerformTwoSampleTTest(
    const double* samples1,
    uint32_t count1,
    const double* samples2,
    uint32_t count2,
    double alpha,
    TCDE_HypothesisTest* result
);

/**
 * Compute reproducibility metrics across multiple runs
 * 
 * @param run_means Array of mean values from different runs
 * @param count Number of runs
 * @param metrics Output: reproducibility metrics
 * @return true if successful, false otherwise
 */
bool TCDE_ComputeReproducibilityMetrics(
    const double* run_means,
    uint32_t count,
    TCDE_ReproducibilityMetrics* metrics
);

/**
 * Analyze distribution properties
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param analysis Output: distribution analysis
 * @return true if successful, false otherwise
 */
bool TCDE_AnalyzeDistribution(
    const double* samples,
    uint32_t count,
    TCDE_DistributionAnalysis* analysis
);

/**
 * Perform Shapiro-Wilk normality test (simplified)
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param alpha Significance level
 * @return true if data appears normal, false otherwise
 */
bool TCDE_TestNormality(
    const double* samples,
    uint32_t count,
    double alpha
);

/**
 * Compute effect size (Cohen's d)
 * 
 * @param samples1 First sample array
 * @param count1 Number of samples in first array
 * @param samples2 Second sample array
 * @param count2 Number of samples in second array
 * @return Cohen's d effect size
 */
double TCDE_ComputeEffectSize(
    const double* samples1,
    uint32_t count1,
    const double* samples2,
    uint32_t count2
);

/**
 * Perform power analysis
 * 
 * @param effect_size Expected effect size
 * @param alpha Significance level
 * @param power Desired statistical power (e.g., 0.80)
 * @return Required sample size
 */
uint32_t TCDE_ComputeRequiredSampleSize(
    double effect_size,
    double alpha,
    double power
);

/**
 * Validate statistical assumptions
 * 
 * @param samples Array of sample values
 * @param count Number of samples
 * @param min_sample_size Minimum required sample size
 * @param max_outliers Maximum allowed outliers
 * @return true if assumptions met, false otherwise
 */
bool TCDE_ValidateStatisticalAssumptions(
    const double* samples,
    uint32_t count,
    uint32_t min_sample_size,
    uint32_t max_outliers
);

#endif // TCDE_STATISTICAL_VALIDATOR_H
