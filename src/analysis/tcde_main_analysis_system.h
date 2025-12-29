#ifndef TCDE_MAIN_ANALYSIS_SYSTEM_H
#define TCDE_MAIN_ANALYSIS_SYSTEM_H

/**
 * @file tcde_main_analysis_system.h
 * @brief Main Analysis System Interface - Phase 6 Integration
 * 
 * This header defines the unified interface for the TCDE Exhaustive Analysis System
 * that integrates all components from Phases 1-5 into a cohesive system capable
 * of analyzing all 2,234 elements according to ZERO TOLERANCE protocol.
 * 
 * @author TCDE Research Team
 * @date December 2025
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

// Include all Phase 1-5 components
#include "tcde_exhaustive_analysis.h"

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// MISSING TYPE DEFINITIONS FROM DESIGN DOCUMENT
// =============================================================================

typedef enum {
    ERROR_DISCOVERY_INCOMPLETE,    // Éléments manqués lors de la découverte
    ERROR_ANALYSIS_FAILED,         // Échec d'analyse d'un élément
    ERROR_VALIDATION_FAILED,       // Échec de validation
    ERROR_AUTHENTICITY_VIOLATION,  // Violation d'authenticité détectée
    ERROR_DEPENDENCY_CYCLE,        // Cycle de dépendance détecté
    ERROR_QUALITY_VIOLATION,       // Violation de qualité
    ERROR_STANDARDS_NON_COMPLIANCE, // Non-conformité aux standards
    ERROR_MEMORY_ALLOCATION,       // Erreur d'allocation mémoire
    ERROR_FILE_ACCESS,            // Erreur d'accès fichier
    ERROR_INVALID_INPUT           // Entrée invalide
} AnalysisErrorType;

typedef struct {
    AnalysisErrorType type;
    char description[512];
    char element_path[MAX_PATH];
    int severity_level;
    char remediation[1024];
} AnalysisError;

typedef struct {
    bool zero_tolerance_passed;
    size_t violations_found;
    char violation_details[10][256];
    time_t validation_timestamp;
} ValidationReport;

typedef struct {
    size_t expected_elements;
    size_t discovered_elements;
    size_t analyzed_elements;
    size_t validated_elements;
    bool discovery_complete;
    bool analysis_complete;
    bool validation_complete;
    bool overall_complete;
} CompletenessReport;

typedef struct {
    double discovery_time_seconds;
    double analysis_time_seconds;
    double validation_time_seconds;
    double dependency_time_seconds;
    double knowledge_base_time_seconds;
    double reporting_time_seconds;
    double total_time_seconds;
    size_t peak_memory_usage_mb;
    size_t elements_per_second;
} PerformanceMetrics;

// =============================================================================
// CONSTANTS AND CONFIGURATION
// =============================================================================

#define TCDE_PROJECT_ROOT_DEFAULT "."
#define TCDE_EXPECTED_ELEMENTS 2234
#define TCDE_EXPECTED_DIRECTORIES 42
#define TCDE_MAX_PARALLEL_WORKERS 8
#define TCDE_PROGRESS_UPDATE_INTERVAL 100
#define TCDE_CACHE_SIZE_MB 256
#define TCDE_MAX_RETRY_ATTEMPTS 3

// =============================================================================
// SYSTEM STATUS AND PROGRESS TRACKING
// =============================================================================

typedef enum {
    SYSTEM_STATUS_IDLE,
    SYSTEM_STATUS_INITIALIZING,
    SYSTEM_STATUS_DISCOVERING,
    SYSTEM_STATUS_ANALYZING,
    SYSTEM_STATUS_VALIDATING,
    SYSTEM_STATUS_BUILDING_DEPENDENCIES,
    SYSTEM_STATUS_GENERATING_KNOWLEDGE_BASE,
    SYSTEM_STATUS_GENERATING_REPORTS,
    SYSTEM_STATUS_COMPLETED,
    SYSTEM_STATUS_ERROR,
    SYSTEM_STATUS_CANCELLED
} SystemStatus;

typedef struct {
    SystemStatus status;
    double overall_progress;        // 0.0 to 100.0
    double phase_progress;          // Current phase progress 0.0 to 100.0
    size_t elements_discovered;
    size_t elements_analyzed;
    size_t elements_validated;
    size_t errors_encountered;
    size_t warnings_generated;
    time_t start_time;
    time_t current_time;
    time_t estimated_completion;
    char current_operation[256];
    char last_error[512];
} SystemProgress;

// =============================================================================
// SYSTEM CONFIGURATION
// =============================================================================

typedef struct {
    char project_root[MAX_PATH];
    bool enable_parallel_processing;
    int max_worker_threads;
    bool enable_caching;
    size_t cache_size_mb;
    bool strict_zero_tolerance;
    bool generate_all_reports;
    bool export_knowledge_base;
    char output_directory[MAX_PATH];
    bool verbose_logging;
    bool resume_from_cache;
    int max_retry_attempts;
} SystemConfiguration;

// =============================================================================
// INTEGRATED SYSTEM STATE
// =============================================================================

typedef struct {
    SystemConfiguration config;
    SystemProgress progress;
    
    // Phase 1-5 Components
    ElementRegistry* registry;
    AnalysisResults* analysis_results;
    ValidationResults* validation_results;
    DependencyGraph* dependency_graph;
    KnowledgeBase* knowledge_base;
    ReportSuite* report_suite;
    
    // System Management
    pthread_mutex_t progress_mutex;
    pthread_t* worker_threads;
    bool* worker_active;
    int active_workers;
    bool system_initialized;
    bool analysis_complete;
    
    // Error Handling
    AnalysisError* errors;
    size_t error_count;
    size_t error_capacity;
    
    // Performance Metrics
    double discovery_time_seconds;
    double analysis_time_seconds;
    double validation_time_seconds;
    double dependency_time_seconds;
    double knowledge_base_time_seconds;
    double reporting_time_seconds;
    size_t peak_memory_usage_mb;
    
} TCDEAnalysisSystem;

// =============================================================================
// CALLBACK INTERFACES
// =============================================================================

/**
 * @brief Progress callback function type
 * @param progress Current system progress
 * @param user_data User-provided data pointer
 */
typedef void (*ProgressCallback)(const SystemProgress* progress, void* user_data);

/**
 * @brief Error callback function type
 * @param error Error information
 * @param user_data User-provided data pointer
 */
typedef void (*ErrorCallback)(const AnalysisError* error, void* user_data);

/**
 * @brief Log callback function type
 * @param level Log level (0=DEBUG, 1=INFO, 2=WARN, 3=ERROR)
 * @param message Log message
 * @param user_data User-provided data pointer
 */
typedef void (*LogCallback)(int level, const char* message, void* user_data);

// =============================================================================
// MAIN SYSTEM INTERFACE
// =============================================================================

/**
 * @brief Initialize the TCDE Analysis System
 * @param system Pointer to system structure to initialize
 * @param config System configuration (NULL for defaults)
 * @return 0 on success, negative on error
 */
int tcde_system_init(TCDEAnalysisSystem* system, const SystemConfiguration* config);

/**
 * @brief Run complete analysis of TCDE project
 * @param system Initialized system structure
 * @param project_root Path to TCDE project root (NULL for current directory)
 * @return 0 on success, negative on error
 */
int tcde_system_run_analysis(TCDEAnalysisSystem* system, const char* project_root);

/**
 * @brief Run analysis with callbacks for progress monitoring
 * @param system Initialized system structure
 * @param project_root Path to TCDE project root
 * @param progress_cb Progress callback (can be NULL)
 * @param error_cb Error callback (can be NULL)
 * @param log_cb Log callback (can be NULL)
 * @param user_data User data passed to callbacks
 * @return 0 on success, negative on error
 */
int tcde_system_run_analysis_with_callbacks(
    TCDEAnalysisSystem* system,
    const char* project_root,
    ProgressCallback progress_cb,
    ErrorCallback error_cb,
    LogCallback log_cb,
    void* user_data
);

/**
 * @brief Get current system progress
 * @param system System structure
 * @param progress Output progress structure
 * @return 0 on success, negative on error
 */
int tcde_system_get_progress(const TCDEAnalysisSystem* system, SystemProgress* progress);

/**
 * @brief Cancel running analysis
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_cancel_analysis(TCDEAnalysisSystem* system);

/**
 * @brief Cleanup and free system resources
 * @param system System structure to cleanup
 */
void tcde_system_cleanup(TCDEAnalysisSystem* system);

// =============================================================================
// PHASE-SPECIFIC INTERFACES
// =============================================================================

/**
 * @brief Run Phase 1: Discovery Engine
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_run_discovery(TCDEAnalysisSystem* system);

/**
 * @brief Run Phase 2: Analysis Engine
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_run_analysis_engine(TCDEAnalysisSystem* system);

/**
 * @brief Run Phase 3: Validation Framework
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_run_validation(TCDEAnalysisSystem* system);

/**
 * @brief Run Phase 4: Dependency Analysis
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_run_dependency_analysis(TCDEAnalysisSystem* system);

/**
 * @brief Run Phase 5: Knowledge Base and Reporting
 * @param system System structure
 * @return 0 on success, negative on error
 */
int tcde_system_run_knowledge_base_and_reporting(TCDEAnalysisSystem* system);

// =============================================================================
// PERFORMANCE OPTIMIZATION
// =============================================================================

/**
 * @brief Enable parallel processing with specified worker count
 * @param system System structure
 * @param worker_count Number of worker threads (0 for auto-detect)
 * @return 0 on success, negative on error
 */
int tcde_system_enable_parallel_processing(TCDEAnalysisSystem* system, int worker_count);

/**
 * @brief Enable result caching for faster subsequent runs
 * @param system System structure
 * @param cache_size_mb Cache size in megabytes
 * @return 0 on success, negative on error
 */
int tcde_system_enable_caching(TCDEAnalysisSystem* system, size_t cache_size_mb);

/**
 * @brief Resume analysis from cached intermediate results
 * @param system System structure
 * @param cache_directory Directory containing cached results
 * @return 0 on success, negative on error
 */
int tcde_system_resume_from_cache(TCDEAnalysisSystem* system, const char* cache_directory);

// =============================================================================
// REPORTING AND EXPORT
// =============================================================================

/**
 * @brief Export complete analysis results to specified directory
 * @param system System structure with completed analysis
 * @param output_directory Directory to export results
 * @param format Export format ("markdown", "html", "json", "all")
 * @return 0 on success, negative on error
 */
int tcde_system_export_results(const TCDEAnalysisSystem* system, const char* output_directory, const char* format);

/**
 * @brief Generate executive summary report
 * @param system System structure with completed analysis
 * @param output_path Path for executive summary file
 * @return 0 on success, negative on error
 */
int tcde_system_generate_executive_summary(const TCDEAnalysisSystem* system, const char* output_path);

/**
 * @brief Generate detailed technical report
 * @param system System structure with completed analysis
 * @param output_path Path for technical report file
 * @return 0 on success, negative on error
 */
int tcde_system_generate_technical_report(const TCDEAnalysisSystem* system, const char* output_path);

// =============================================================================
// VALIDATION AND QUALITY ASSURANCE
// =============================================================================

/**
 * @brief Validate system results against ZERO TOLERANCE protocol
 * @param system System structure with completed analysis
 * @param validation_report Output validation report
 * @return 0 if validation passes, negative on violations
 */
int tcde_system_validate_zero_tolerance(const TCDEAnalysisSystem* system, ValidationReport* validation_report);

/**
 * @brief Verify analysis completeness (all 2,234 elements processed)
 * @param system System structure
 * @param completeness_report Output completeness report
 * @return 0 if complete, negative if incomplete
 */
int tcde_system_verify_completeness(const TCDEAnalysisSystem* system, CompletenessReport* completeness_report);

/**
 * @brief Run comprehensive system self-test
 * @param system System structure
 * @return 0 if all tests pass, negative on test failures
 */
int tcde_system_self_test(TCDEAnalysisSystem* system);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Get default system configuration
 * @param config Output configuration structure
 */
void tcde_system_get_default_config(SystemConfiguration* config);

/**
 * @brief Estimate analysis time based on project size
 * @param element_count Number of elements to analyze
 * @param enable_parallel Whether parallel processing is enabled
 * @return Estimated time in seconds
 */
double tcde_system_estimate_analysis_time(size_t element_count, bool enable_parallel);

/**
 * @brief Get system performance metrics
 * @param system System structure
 * @param metrics Output performance metrics
 * @return 0 on success, negative on error
 */
int tcde_system_get_performance_metrics(const TCDEAnalysisSystem* system, PerformanceMetrics* metrics);

/**
 * @brief Print system status to stdout
 * @param system System structure
 */
void tcde_system_print_status(const TCDEAnalysisSystem* system);

/**
 * @brief Print detailed system statistics
 * @param system System structure
 */
void tcde_system_print_statistics(const TCDEAnalysisSystem* system);

// =============================================================================
// ERROR HANDLING
// =============================================================================

/**
 * @brief Get last error message
 * @param system System structure
 * @return Error message string (NULL if no error)
 */
const char* tcde_system_get_last_error(const TCDEAnalysisSystem* system);

/**
 * @brief Get all system errors
 * @param system System structure
 * @param errors Output array of errors
 * @param error_count Output number of errors
 * @return 0 on success, negative on error
 */
int tcde_system_get_all_errors(const TCDEAnalysisSystem* system, AnalysisError** errors, size_t* error_count);

/**
 * @brief Clear all system errors
 * @param system System structure
 */
void tcde_system_clear_errors(TCDEAnalysisSystem* system);

#ifdef __cplusplus
}
#endif

#endif // TCDE_MAIN_ANALYSIS_SYSTEM_H
