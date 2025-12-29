/**
 * TCDE Statistical Validator - Implementation
 * 
 * Date: November 10, 2025
 * Protocol: Zero Tolerance v3.0
 */

#include "tcde_statistical_validator.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Helper: compute mean
static double compute_mean(const double* samples, uint32_t count) {
    if (count == 0) return 0.0;
    double sum = 0.0;
    for (uint32_t i = 0; i < count; i++) {
        sum += samples[i];
    }
    return sum / count;
}

// Helper: compute variance
static double compute_variance(const double* samples, uint32_t count, double mean) {
    if (count <= 1) return 0.0;
    double sum_sq = 0.0;
    for (uint32_t i = 0; i < count; i++) {
        double diff = samples[i] - mean;
        sum_sq += diff * diff;
    }
    return sum_sq / (count - 1);
}

// Helper: compute standard deviation
static double compute_std_dev(const double* samples, uint32_t count, double mean) {
    return sqrt(compute_variance(samples, count, mean));
}

// Helper: t-distribution critical value (approximation)
static double t_critical(uint32_t df, double alpha) {
    // Simplified approximation for common cases
    if (df > 30) {
        // Use normal distribution approximation
        if (alpha <= 0.001) return 3.291;
        if (alpha <= 0.01) return 2.576;
        if (alpha <= 0.05) return 1.96;
        return 1.645;
    }
    
    // Conservative estimates for smaller samples
    if (alpha <= 0.001) return 4.0;
    if (alpha <= 0.01) return 3.0;
    if (alpha <= 0.05) return 2.5;
    return 2.0;
}

// Compute confidence interval
bool TCDE_ComputeConfidenceInterval(
    const double* samples,
    uint32_t count,
    double confidence_level,
    double* lower,
    double* upper
) {
    if (!samples || count < 2 || !lower || !upper) {
        return false;
    }
    
    double mean = compute_mean(samples, count);
    double std_dev = compute_std_dev(samples, count, mean);
    double std_error = std_dev / sqrt((double)count);
    
    double alpha = 1.0 - confidence_level;
    double t_crit = t_critical(count - 1, alpha / 2.0);
    
    double margin = t_crit * std_error;
    *lower = mean - margin;
    *upper = mean + margin;
    
    return true;
}

// Compute p-value for one-sample t-test
double TCDE_ComputePValue(
    const double* samples,
    uint32_t count,
    double hypothesized_mean
) {
    if (!samples || count < 2) {
        return 1.0;  // Cannot reject null
    }
    
    double mean = compute_mean(samples, count);
    double std_dev = compute_std_dev(samples, count, mean);
    double std_error = std_dev / sqrt((double)count);
    
    if (std_error == 0.0) {
        return (mean == hypothesized_mean) ? 1.0 : 0.0;
    }
    
    double t_stat = (mean - hypothesized_mean) / std_error;
    double t_abs = fabs(t_stat);
    
    // Simplified p-value approximation based on t-statistic
    // For proper implementation, would use t-distribution CDF
    if (t_abs > 3.291) return 0.0005;  // p < 0.001
    if (t_abs > 2.576) return 0.005;   // p < 0.01
    if (t_abs > 1.96) return 0.025;    // p < 0.05
    if (t_abs > 1.645) return 0.05;    // p < 0.10
    return 0.1;  // p > 0.10
}

// Perform one-sample t-test
bool TCDE_PerformTTest(
    const double* samples,
    uint32_t count,
    double hypothesized_mean,
    double alpha,
    TCDE_HypothesisTest* result
) {
    if (!samples || count < 2 || !result) {
        return false;
    }
    
    double mean = compute_mean(samples, count);
    double std_dev = compute_std_dev(samples, count, mean);
    double std_error = std_dev / sqrt((double)count);
    
    result->test_statistic = (mean - hypothesized_mean) / std_error;
    result->p_value = TCDE_ComputePValue(samples, count, hypothesized_mean);
    result->critical_value = t_critical(count - 1, alpha / 2.0);
    result->reject_null = (result->p_value < alpha);
    
    snprintf(result->test_name, sizeof(result->test_name), "One-sample t-test");
    
    return true;
}

// Perform two-sample t-test
bool TCDE_PerformTwoSampleTTest(
    const double* samples1,
    uint32_t count1,
    const double* samples2,
    uint32_t count2,
    double alpha,
    TCDE_HypothesisTest* result
) {
    if (!samples1 || !samples2 || count1 < 2 || count2 < 2 || !result) {
        return false;
    }
    
    double mean1 = compute_mean(samples1, count1);
    double mean2 = compute_mean(samples2, count2);
    double var1 = compute_variance(samples1, count1, mean1);
    double var2 = compute_variance(samples2, count2, mean2);
    
    // Pooled standard error
    double pooled_var = ((count1 - 1) * var1 + (count2 - 1) * var2) / (count1 + count2 - 2);
    double std_error = sqrt(pooled_var * (1.0 / count1 + 1.0 / count2));
    
    result->test_statistic = (mean1 - mean2) / std_error;
    result->critical_value = t_critical(count1 + count2 - 2, alpha / 2.0);
    result->reject_null = (fabs(result->test_statistic) > result->critical_value);
    
    // Simplified p-value
    double t_abs = fabs(result->test_statistic);
    if (t_abs > 3.291) result->p_value = 0.0005;
    else if (t_abs > 2.576) result->p_value = 0.005;
    else if (t_abs > 1.96) result->p_value = 0.025;
    else result->p_value = 0.1;
    
    snprintf(result->test_name, sizeof(result->test_name), "Two-sample t-test");
    
    return true;
}

// Compute reproducibility metrics
bool TCDE_ComputeReproducibilityMetrics(
    const double* run_means,
    uint32_t count,
    TCDE_ReproducibilityMetrics* metrics
) {
    if (!run_means || count < 2 || !metrics) {
        return false;
    }
    
    double mean = compute_mean(run_means, count);
    double variance = compute_variance(run_means, count, mean);
    
    metrics->inter_run_variance = variance;
    metrics->coefficient_of_variation = (mean != 0.0) ? (sqrt(variance) / fabs(mean)) : 0.0;
    
    // Reproducibility score: 1 - normalized variance
    // Perfect reproducibility = 1, no reproducibility = 0
    double max_variance = mean * mean;  // Worst case: variance = mean²
    if (max_variance > 0.0) {
        metrics->reproducibility_score = 1.0 - (variance / max_variance);
        if (metrics->reproducibility_score < 0.0) metrics->reproducibility_score = 0.0;
        if (metrics->reproducibility_score > 1.0) metrics->reproducibility_score = 1.0;
    } else {
        metrics->reproducibility_score = 1.0;
    }
    
    metrics->is_reproducible = (variance < 0.001);
    
    return true;
}

// Helper: compare function for qsort
static int compare_doubles(const void* a, const void* b) {
    double diff = *(const double*)a - *(const double*)b;
    return (diff > 0) - (diff < 0);
}

// Analyze distribution
bool TCDE_AnalyzeDistribution(
    const double* samples,
    uint32_t count,
    TCDE_DistributionAnalysis* analysis
) {
    if (!samples || count < 4 || !analysis) {
        return false;
    }
    
    // Sort samples for quantile computation
    double* sorted = (double*)malloc(count * sizeof(double));
    if (!sorted) return false;
    
    memcpy(sorted, samples, count * sizeof(double));
    qsort(sorted, count, sizeof(double), compare_doubles);
    
    // Compute median
    if (count % 2 == 0) {
        analysis->median = (sorted[count/2 - 1] + sorted[count/2]) / 2.0;
    } else {
        analysis->median = sorted[count/2];
    }
    
    // Compute quartiles
    uint32_t q1_idx = count / 4;
    uint32_t q3_idx = (3 * count) / 4;
    analysis->q1 = sorted[q1_idx];
    analysis->q3 = sorted[q3_idx];
    analysis->iqr = analysis->q3 - analysis->q1;
    
    // Compute moments
    double mean = compute_mean(samples, count);
    double variance = compute_variance(samples, count, mean);
    double std_dev = sqrt(variance);
    
    // Compute skewness
    double sum_cubed = 0.0;
    for (uint32_t i = 0; i < count; i++) {
        double z = (samples[i] - mean) / std_dev;
        sum_cubed += z * z * z;
    }
    analysis->skewness = sum_cubed / count;
    
    // Compute kurtosis
    double sum_fourth = 0.0;
    for (uint32_t i = 0; i < count; i++) {
        double z = (samples[i] - mean) / std_dev;
        sum_fourth += z * z * z * z;
    }
    analysis->kurtosis = (sum_fourth / count) - 3.0;  // Excess kurtosis
    
    // Simple normality check: skewness and kurtosis near 0
    analysis->is_normal = (fabs(analysis->skewness) < 1.0 && fabs(analysis->kurtosis) < 1.0);
    
    free(sorted);
    return true;
}

// Test normality (simplified Shapiro-Wilk)
bool TCDE_TestNormality(
    const double* samples,
    uint32_t count,
    double alpha
) {
    (void)alpha; // Reserved for significance level in full Shapiro-Wilk test
    
    if (!samples || count < 3) {
        return false;
    }
    
    TCDE_DistributionAnalysis analysis;
    if (!TCDE_AnalyzeDistribution(samples, count, &analysis)) {
        return false;
    }
    
    // Simplified: check if skewness and kurtosis are within acceptable range
    return analysis.is_normal;
}

// Compute effect size (Cohen's d)
double TCDE_ComputeEffectSize(
    const double* samples1,
    uint32_t count1,
    const double* samples2,
    uint32_t count2
) {
    if (!samples1 || !samples2 || count1 < 2 || count2 < 2) {
        return 0.0;
    }
    
    double mean1 = compute_mean(samples1, count1);
    double mean2 = compute_mean(samples2, count2);
    double var1 = compute_variance(samples1, count1, mean1);
    double var2 = compute_variance(samples2, count2, mean2);
    
    // Pooled standard deviation
    double pooled_sd = sqrt(((count1 - 1) * var1 + (count2 - 1) * var2) / (count1 + count2 - 2));
    
    if (pooled_sd == 0.0) return 0.0;
    
    return (mean1 - mean2) / pooled_sd;
}

// Compute required sample size
uint32_t TCDE_ComputeRequiredSampleSize(
    double effect_size,
    double alpha,
    double power
) {
    // Simplified formula for two-sample t-test
    // n ≈ 2 * (z_α/2 + z_β)² / d²
    
    double z_alpha = t_critical(1000, alpha / 2.0);  // Use large df for z-approximation
    double z_beta = t_critical(1000, 1.0 - power);
    
    double n = 2.0 * pow(z_alpha + z_beta, 2.0) / pow(effect_size, 2.0);
    
    return (uint32_t)ceil(n);
}

// Validate statistical assumptions
bool TCDE_ValidateStatisticalAssumptions(
    const double* samples,
    uint32_t count,
    uint32_t min_sample_size,
    uint32_t max_outliers
) {
    if (!samples || count < min_sample_size) {
        return false;
    }
    
    // Check for outliers using IQR method
    TCDE_DistributionAnalysis analysis;
    if (!TCDE_AnalyzeDistribution(samples, count, &analysis)) {
        return false;
    }
    
    double lower_fence = analysis.q1 - 1.5 * analysis.iqr;
    double upper_fence = analysis.q3 + 1.5 * analysis.iqr;
    
    uint32_t outlier_count = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (samples[i] < lower_fence || samples[i] > upper_fence) {
            outlier_count++;
        }
    }
    
    return (outlier_count <= max_outliers);
}
