/**
 * @file tcde_main_analysis_system.c
 * @brief Main Analysis System Implementation - Phase 6 Integration
 * 
 * This file implements the unified TCDE Exhaustive Analysis System that integrates
 * all components from Phases 1-5 into a cohesive system capable of analyzing
 * all 2,234 elements according to ZERO TOLERANCE protocol.
 * 
 * @author TCDE Research Team
 * @date December 2025
 * @version 1.0.0
 */

#include "tcde_main_analysis_system.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

// =============================================================================
// INTERNAL HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Get current timestamp in seconds
 */
static double get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/**
 * @brief Get current memory usage in MB
 */
static size_t get_memory_usage_mb(void) {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss / 1024; // Convert KB to MB on Linux
    }
    return 0;
}

/**
 * @brief Update system progress safely (thread-safe)
 */
static void update_progress(TCDEAnalysisSystem* system, SystemStatus status, 
                           double overall_progress, const char* operation) {
    if (!system) return;
    
    pthread_mutex_lock(&system->progress_mutex);
    
    system->progress.status = status;
    system->progress.overall_progress = overall_progress;
    system->progress.current_time = time(NULL);
    
    if (operation) {
        strncpy(system->progress.current_operation, operation, 
                sizeof(system->progress.current_operation) - 1);
        system->progress.current_operation[sizeof(system->progress.current_operation) - 1] = '\0';
    }
    
    // Update peak memory usage
    size_t current_memory = get_memory_usage_mb();
    if (current_memory > system->peak_memory_usage_mb) {
        system->peak_memory_usage_mb = current_memory;
    }
    
    pthread_mutex_unlock(&system->progress_mutex);
}

/**
 * @brief Add error to system error list
 */
static void add_system_error(TCDEAnalysisSystem* system, AnalysisErrorType type, 
                            const char* description, const char* element_path) {
    if (!system || !description) return;
    
    pthread_mutex_lock(&system->progress_mutex);
    
    if (system->error_count >= system->error_capacity) {
        // Expand error array
        size_t new_capacity = system->error_capacity * 2;
        AnalysisError* new_errors = realloc(system->errors, 
                                          new_capacity * sizeof(AnalysisError));
        if (new_errors) {
            system->errors = new_errors;
            system->error_capacity = new_capacity;
        } else {
            pthread_mutex_unlock(&system->progress_mutex);
            return; // Out of memory
        }
    }
    
    AnalysisError* error = &system->errors[system->error_count];
    error->type = type;
    strncpy(error->description, description, sizeof(error->description) - 1);
    error->description[sizeof(error->description) - 1] = '\0';
    
    if (element_path) {
        strncpy(error->element_path, element_path, sizeof(error->element_path) - 1);
        error->element_path[sizeof(error->element_path) - 1] = '\0';
    } else {
        error->element_path[0] = '\0';
    }
    
    error->severity_level = (type == ERROR_AUTHENTICITY_VIOLATION || 
                           type == ERROR_STANDARDS_NON_COMPLIANCE) ? 3 : 2;
    
    // Store last error message
    strncpy(system->progress.last_error, description, 
            sizeof(system->progress.last_error) - 1);
    system->progress.last_error[sizeof(system->progress.last_error) - 1] = '\0';
    
    system->error_count++;
    system->progress.errors_encountered++;
    
    pthread_mutex_unlock(&system->progress_mutex);
}

// =============================================================================
// SYSTEM INITIALIZATION AND CLEANUP
// =============================================================================

void tcde_system_get_default_config(SystemConfiguration* config) {
    if (!config) return;
    
    memset(config, 0, sizeof(SystemConfiguration));
    
    strncpy(config->project_root, TCDE_PROJECT_ROOT_DEFAULT, 
            sizeof(config->project_root) - 1);
    config->enable_parallel_processing = true;
    config->max_worker_threads = TCDE_MAX_PARALLEL_WORKERS;
    config->enable_caching = true;
    config->cache_size_mb = TCDE_CACHE_SIZE_MB;
    config->strict_zero_tolerance = true;
    config->generate_all_reports = true;
    config->export_knowledge_base = true;
    strncpy(config->output_directory, "./tcde_analysis_output", 
            sizeof(config->output_directory) - 1);
    config->verbose_logging = false;
    config->resume_from_cache = false;
    config->max_retry_attempts = TCDE_MAX_RETRY_ATTEMPTS;
}

int tcde_system_init(TCDEAnalysisSystem* system, const SystemConfiguration* config) {
    if (!system) return -1;
    
    memset(system, 0, sizeof(TCDEAnalysisSystem));
    
    // Set configuration
    if (config) {
        system->config = *config;
    } else {
        tcde_system_get_default_config(&system->config);
    }
    
    // Initialize progress tracking
    system->progress.status = SYSTEM_STATUS_IDLE;
    system->progress.start_time = time(NULL);
    system->progress.current_time = system->progress.start_time;
    
    // Initialize mutex
    if (pthread_mutex_init(&system->progress_mutex, NULL) != 0) {
        return -2;
    }
    
    // Allocate component structures
    system->registry = malloc(sizeof(ElementRegistry));
    system->analysis_results = malloc(sizeof(AnalysisResults));
    system->validation_results = malloc(sizeof(ValidationResults));
    system->dependency_graph = malloc(sizeof(DependencyGraph));
    system->knowledge_base = malloc(sizeof(KnowledgeBase));
    system->report_suite = malloc(sizeof(ReportSuite));
    
    if (!system->registry || !system->analysis_results || !system->validation_results ||
        !system->dependency_graph || !system->knowledge_base || !system->report_suite) {
        tcde_system_cleanup(system);
        return -3;
    }
    
    // Initialize components
    init_element_registry(system->registry);
    init_analysis_results(system->analysis_results);
    init_validation_results(system->validation_results);
    init_dependency_graph(system->dependency_graph);
    init_knowledge_base(system->knowledge_base);
    init_report_suite(system->report_suite);
    
    // Initialize error tracking
    system->error_capacity = 100;
    system->errors = malloc(system->error_capacity * sizeof(AnalysisError));
    if (!system->errors) {
        tcde_system_cleanup(system);
        return -4;
    }
    
    // Initialize worker threads if parallel processing enabled
    if (system->config.enable_parallel_processing) {
        system->worker_threads = malloc(system->config.max_worker_threads * sizeof(pthread_t));
        system->worker_active = malloc(system->config.max_worker_threads * sizeof(bool));
        
        if (!system->worker_threads || !system->worker_active) {
            tcde_system_cleanup(system);
            return -5;
        }
        
        memset(system->worker_active, 0, system->config.max_worker_threads * sizeof(bool));
    }
    
    // Create output directory
    struct stat st = {0};
    if (stat(system->config.output_directory, &st) == -1) {
        if (mkdir(system->config.output_directory, 0755) != 0) {
            add_system_error(system, ERROR_FILE_ACCESS, 
                           "Failed to create output directory", 
                           system->config.output_directory);
            return -6;
        }
    }
    
    system->system_initialized = true;
    update_progress(system, SYSTEM_STATUS_IDLE, 0.0, "System initialized");
    
    return 0;
}

void tcde_system_cleanup(TCDEAnalysisSystem* system) {
    if (!system) return;
    
    // Cancel any running analysis
    tcde_system_cancel_analysis(system);
    
    // Free component structures
    if (system->registry) {
        free_element_registry(system->registry);
        free(system->registry);
    }
    if (system->analysis_results) {
        free_analysis_results(system->analysis_results);
        free(system->analysis_results);
    }
    if (system->validation_results) {
        free_validation_results(system->validation_results);
        free(system->validation_results);
    }
    if (system->dependency_graph) {
        free_dependency_graph(system->dependency_graph);
        free(system->dependency_graph);
    }
    if (system->knowledge_base) {
        free_knowledge_base(system->knowledge_base);
        free(system->knowledge_base);
    }
    if (system->report_suite) {
        free_report_suite(system->report_suite);
        free(system->report_suite);
    }
    
    // Free error tracking
    if (system->errors) {
        free(system->errors);
    }
    
    // Free worker thread arrays
    if (system->worker_threads) {
        free(system->worker_threads);
    }
    if (system->worker_active) {
        free(system->worker_active);
    }
    
    // Destroy mutex
    pthread_mutex_destroy(&system->progress_mutex);
    
    memset(system, 0, sizeof(TCDEAnalysisSystem));
}

// =============================================================================
// MAIN ANALYSIS WORKFLOW
// =============================================================================

int tcde_system_run_analysis(TCDEAnalysisSystem* system, const char* project_root) {
    return tcde_system_run_analysis_with_callbacks(system, project_root, 
                                                  NULL, NULL, NULL, NULL);
}

int tcde_system_run_analysis_with_callbacks(
    TCDEAnalysisSystem* system,
    const char* project_root,
    ProgressCallback progress_cb,
    ErrorCallback error_cb,
    LogCallback log_cb,
    void* user_data
) {
    if (!system || !system->system_initialized) return -1;
    
    // Set project root
    if (project_root) {
        strncpy(system->config.project_root, project_root, 
                sizeof(system->config.project_root) - 1);
        system->config.project_root[sizeof(system->config.project_root) - 1] = '\0';
    }
    
    double start_time = get_current_time();
    int result = 0;
    
    update_progress(system, SYSTEM_STATUS_INITIALIZING, 0.0, "Starting analysis");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "Starting TCDE exhaustive analysis", user_data);
    
    // Phase 1: Discovery (0-20%)
    update_progress(system, SYSTEM_STATUS_DISCOVERING, 5.0, "Running discovery engine");
    if (progress_cb) progress_cb(&system->progress, user_data);
    
    double phase_start = get_current_time();
    result = tcde_system_run_discovery(system);
    system->discovery_time_seconds = get_current_time() - phase_start;
    
    if (result != 0) {
        add_system_error(system, ERROR_DISCOVERY_INCOMPLETE, 
                        "Discovery phase failed", NULL);
        if (error_cb) error_cb(&system->errors[system->error_count - 1], user_data);
        update_progress(system, SYSTEM_STATUS_ERROR, 20.0, "Discovery failed");
        return result;
    }
    
    update_progress(system, SYSTEM_STATUS_DISCOVERING, 20.0, "Discovery completed");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "Discovery phase completed successfully", user_data);
    
    // Phase 2: Analysis (20-50%)
    update_progress(system, SYSTEM_STATUS_ANALYZING, 25.0, "Running analysis engine");
    if (progress_cb) progress_cb(&system->progress, user_data);
    
    phase_start = get_current_time();
    result = tcde_system_run_analysis_engine(system);
    system->analysis_time_seconds = get_current_time() - phase_start;
    
    if (result != 0) {
        add_system_error(system, ERROR_ANALYSIS_FAILED, 
                        "Analysis phase failed", NULL);
        if (error_cb) error_cb(&system->errors[system->error_count - 1], user_data);
        update_progress(system, SYSTEM_STATUS_ERROR, 50.0, "Analysis failed");
        return result;
    }
    
    update_progress(system, SYSTEM_STATUS_ANALYZING, 50.0, "Analysis completed");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "Analysis phase completed successfully", user_data);
    
    // Phase 3: Validation (50-65%)
    update_progress(system, SYSTEM_STATUS_VALIDATING, 55.0, "Running validation framework");
    if (progress_cb) progress_cb(&system->progress, user_data);
    
    phase_start = get_current_time();
    result = tcde_system_run_validation(system);
    system->validation_time_seconds = get_current_time() - phase_start;
    
    if (result != 0) {
        add_system_error(system, ERROR_VALIDATION_FAILED, 
                        "Validation phase failed", NULL);
        if (error_cb) error_cb(&system->errors[system->error_count - 1], user_data);
        update_progress(system, SYSTEM_STATUS_ERROR, 65.0, "Validation failed");
        return result;
    }
    
    update_progress(system, SYSTEM_STATUS_VALIDATING, 65.0, "Validation completed");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "Validation phase completed successfully", user_data);
    
    // Phase 4: Dependency Analysis (65-80%)
    update_progress(system, SYSTEM_STATUS_BUILDING_DEPENDENCIES, 70.0, 
                   "Building dependency graph");
    if (progress_cb) progress_cb(&system->progress, user_data);
    
    phase_start = get_current_time();
    result = tcde_system_run_dependency_analysis(system);
    system->dependency_time_seconds = get_current_time() - phase_start;
    
    if (result != 0) {
        add_system_error(system, ERROR_DEPENDENCY_CYCLE, 
                        "Dependency analysis failed", NULL);
        if (error_cb) error_cb(&system->errors[system->error_count - 1], user_data);
        update_progress(system, SYSTEM_STATUS_ERROR, 80.0, "Dependency analysis failed");
        return result;
    }
    
    update_progress(system, SYSTEM_STATUS_BUILDING_DEPENDENCIES, 80.0, 
                   "Dependency analysis completed");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "Dependency analysis completed successfully", user_data);
    
    // Phase 5: Knowledge Base and Reporting (80-100%)
    update_progress(system, SYSTEM_STATUS_GENERATING_KNOWLEDGE_BASE, 85.0, 
                   "Building knowledge base");
    if (progress_cb) progress_cb(&system->progress, user_data);
    
    phase_start = get_current_time();
    result = tcde_system_run_knowledge_base_and_reporting(system);
    system->knowledge_base_time_seconds = get_current_time() - phase_start;
    system->reporting_time_seconds = system->knowledge_base_time_seconds; // Combined
    
    if (result != 0) {
        add_system_error(system, ERROR_ANALYSIS_FAILED, 
                        "Knowledge base and reporting failed", NULL);
        if (error_cb) error_cb(&system->errors[system->error_count - 1], user_data);
        update_progress(system, SYSTEM_STATUS_ERROR, 100.0, "Reporting failed");
        return result;
    }
    
    // Final completion
    system->analysis_complete = true;
    update_progress(system, SYSTEM_STATUS_COMPLETED, 100.0, "Analysis completed successfully");
    if (progress_cb) progress_cb(&system->progress, user_data);
    if (log_cb) log_cb(1, "TCDE exhaustive analysis completed successfully", user_data);
    
    // Calculate total time
    double total_time = get_current_time() - start_time;
    if (log_cb) {
        char time_msg[256];
        snprintf(time_msg, sizeof(time_msg), 
                "Total analysis time: %.2f seconds", total_time);
        log_cb(1, time_msg, user_data);
    }
    
    return 0;
}

// =============================================================================
// PHASE-SPECIFIC IMPLEMENTATIONS
// =============================================================================

int tcde_system_run_discovery(TCDEAnalysisSystem* system) {
    if (!system || !system->system_initialized) return -1;
    
    // Run discovery engine from Phase 1
    int result = discover_all_elements(system->registry, system->config.project_root);
    if (result != 0) {
        return result;
    }
    
    // Validate element count according to ZERO TOLERANCE protocol
    if (!validate_element_count(system->registry, TCDE_EXPECTED_ELEMENTS)) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "Expected %d elements, found %zu", 
                TCDE_EXPECTED_ELEMENTS, system->registry->count);
        add_system_error(system, ERROR_DISCOVERY_INCOMPLETE, error_msg, NULL);
        return -2;
    }
    
    // Update progress tracking
    system->progress.elements_discovered = system->registry->count;
    
    return 0;
}

int tcde_system_run_analysis_engine(TCDEAnalysisSystem* system) {
    if (!system || !system->system_initialized || !system->registry) return -1;
    
    // Run analysis engine from Phase 2
    int result = analyze_all_elements(system->registry, system->analysis_results);
    if (result != 0) {
        return result;
    }
    
    // Update progress tracking
    system->progress.elements_analyzed = system->analysis_results->analyzed_count;
    
    return 0;
}

int tcde_system_run_validation(TCDEAnalysisSystem* system) {
    if (!system || !system->system_initialized || !system->analysis_results) return -1;
    
    // Run validation framework from Phase 3
    int result = validate_all_elements_zero_tolerance(system->registry, 
                                                    system->analysis_results, 
                                                    system->validation_results);
    if (result != 0) {
        return result;
    }
    
    // Check for ZERO TOLERANCE violations
    if (system->config.strict_zero_tolerance) {
        if (system->validation_results->total_violations > 0) {
            char error_msg[512];
            snprintf(error_msg, sizeof(error_msg), 
                    "ZERO TOLERANCE violations detected: %zu total violations", 
                    system->validation_results->total_violations);
            add_system_error(system, ERROR_AUTHENTICITY_VIOLATION, 
                           error_msg, NULL);
            return -2;
        }
    }
    
    // Update progress tracking
    system->progress.elements_validated = system->validation_results->total_elements;
    
    return 0;
}

int tcde_system_run_dependency_analysis(TCDEAnalysisSystem* system) {
    if (!system || !system->system_initialized || !system->analysis_results) return -1;
    
    // Run dependency analyzer from Phase 4
    int result = build_dependency_graph(system->analysis_results, system->dependency_graph);
    if (result != 0) {
        return result;
    }
    
    // Detect circular dependencies
    result = detect_circular_dependencies(system->dependency_graph);
    if (result != 0) {
        add_system_error(system, ERROR_DEPENDENCY_CYCLE, 
                        "Circular dependencies detected", NULL);
        // Continue analysis but log the issue
    }
    
    // Identify critical paths
    result = identify_critical_paths(system->dependency_graph);
    if (result != 0) {
        return result;
    }
    
    return 0;
}

int tcde_system_run_knowledge_base_and_reporting(TCDEAnalysisSystem* system) {
    if (!system || !system->system_initialized || 
        !system->analysis_results || !system->dependency_graph) return -1;
    
    // Build knowledge base from Phase 5
    int result = build_knowledge_base(system->analysis_results, 
                                    system->dependency_graph, 
                                    system->knowledge_base);
    if (result != 0) {
        return result;
    }
    
    // Generate all reports
    if (system->config.generate_all_reports) {
        result = generate_all_reports(system->analysis_results, 
                                    system->dependency_graph, 
                                    system->knowledge_base, 
                                    system->report_suite);
        if (result != 0) {
            return result;
        }
    }
    
    return 0;
}

// =============================================================================
// PROGRESS AND STATUS FUNCTIONS
// =============================================================================

int tcde_system_get_progress(const TCDEAnalysisSystem* system, SystemProgress* progress) {
    if (!system || !progress) return -1;
    
    pthread_mutex_lock((pthread_mutex_t*)&system->progress_mutex);
    *progress = system->progress;
    pthread_mutex_unlock((pthread_mutex_t*)&system->progress_mutex);
    
    return 0;
}

int tcde_system_cancel_analysis(TCDEAnalysisSystem* system) {
    if (!system) return -1;
    
    update_progress(system, SYSTEM_STATUS_CANCELLED, 
                   system->progress.overall_progress, "Analysis cancelled");
    
    // Signal worker threads to stop (if implemented)
    for (int i = 0; i < system->config.max_worker_threads; i++) {
        if (system->worker_active && system->worker_active[i]) {
            // In a full implementation, we would signal threads to stop
            system->worker_active[i] = false;
        }
    }
    
    return 0;
}

// =============================================================================
// UTILITY AND REPORTING FUNCTIONS
// =============================================================================

double tcde_system_estimate_analysis_time(size_t element_count, bool enable_parallel) {
    // Rough estimation based on typical performance
    double base_time_per_element = 0.1; // 100ms per element
    
    if (enable_parallel) {
        base_time_per_element /= 4; // Assume 4x speedup with parallel processing
    }
    
    return element_count * base_time_per_element;
}

void tcde_system_print_status(const TCDEAnalysisSystem* system) {
    if (!system) return;
    
    const char* status_names[] = {
        "IDLE", "INITIALIZING", "DISCOVERING", "ANALYZING", 
        "VALIDATING", "BUILDING_DEPENDENCIES", "GENERATING_KNOWLEDGE_BASE",
        "GENERATING_REPORTS", "COMPLETED", "ERROR", "CANCELLED"
    };
    
    printf("=== TCDE Analysis System Status ===\n");
    printf("Status: %s\n", status_names[system->progress.status]);
    printf("Overall Progress: %.1f%%\n", system->progress.overall_progress);
    printf("Current Operation: %s\n", system->progress.current_operation);
    printf("Elements Discovered: %zu\n", system->progress.elements_discovered);
    printf("Elements Analyzed: %zu\n", system->progress.elements_analyzed);
    printf("Elements Validated: %zu\n", system->progress.elements_validated);
    printf("Errors Encountered: %zu\n", system->progress.errors_encountered);
    printf("Peak Memory Usage: %zu MB\n", system->peak_memory_usage_mb);
    
    if (system->progress.last_error[0] != '\0') {
        printf("Last Error: %s\n", system->progress.last_error);
    }
    
    printf("=====================================\n");
}

void tcde_system_print_statistics(const TCDEAnalysisSystem* system) {
    if (!system) return;
    
    printf("=== TCDE Analysis System Statistics ===\n");
    printf("Discovery Time: %.2f seconds\n", system->discovery_time_seconds);
    printf("Analysis Time: %.2f seconds\n", system->analysis_time_seconds);
    printf("Validation Time: %.2f seconds\n", system->validation_time_seconds);
    printf("Dependency Analysis Time: %.2f seconds\n", system->dependency_time_seconds);
    printf("Knowledge Base Time: %.2f seconds\n", system->knowledge_base_time_seconds);
    printf("Reporting Time: %.2f seconds\n", system->reporting_time_seconds);
    
    double total_time = system->discovery_time_seconds + system->analysis_time_seconds +
                       system->validation_time_seconds + system->dependency_time_seconds +
                       system->knowledge_base_time_seconds + system->reporting_time_seconds;
    
    printf("Total Time: %.2f seconds\n", total_time);
    printf("Peak Memory Usage: %zu MB\n", system->peak_memory_usage_mb);
    printf("Analysis Complete: %s\n", system->analysis_complete ? "Yes" : "No");
    printf("========================================\n");
}

// =============================================================================
// ERROR HANDLING
// =============================================================================

const char* tcde_system_get_last_error(const TCDEAnalysisSystem* system) {
    if (!system || system->progress.last_error[0] == '\0') {
        return NULL;
    }
    return system->progress.last_error;
}

int tcde_system_get_all_errors(const TCDEAnalysisSystem* system, 
                              AnalysisError** errors, size_t* error_count) {
    if (!system || !errors || !error_count) return -1;
    
    *errors = system->errors;
    *error_count = system->error_count;
    
    return 0;
}

void tcde_system_clear_errors(TCDEAnalysisSystem* system) {
    if (!system) return;
    
    pthread_mutex_lock(&system->progress_mutex);
    system->error_count = 0;
    system->progress.errors_encountered = 0;
    system->progress.last_error[0] = '\0';
    pthread_mutex_unlock(&system->progress_mutex);
}

// =============================================================================
// EXPORT AND VALIDATION FUNCTIONS
// =============================================================================

int tcde_system_export_results(const TCDEAnalysisSystem* system, 
                               const char* output_directory, const char* format) {
    if (!system || !output_directory || !format) return -1;
    
    if (!system->analysis_complete) {
        return -2; // Analysis not complete
    }
    
    // Create output directory
    struct stat st = {0};
    if (stat(output_directory, &st) == -1) {
        if (mkdir(output_directory, 0755) != 0) {
            return -3;
        }
    }
    
    // Export based on format
    if (strcmp(format, "markdown") == 0 || strcmp(format, "all") == 0) {
        // Export reports as markdown
        for (size_t i = 0; i < system->report_suite->count; i++) {
            Report* report = &system->report_suite->reports[i];
            char output_path[MAX_PATH];
            snprintf(output_path, sizeof(output_path), 
                    "%s/report_%zu.md", output_directory, i);
            
            int result = export_report_markdown(report, output_path);
            if (result != 0) {
                return result;
            }
        }
    }
    
    return 0;
}

int tcde_system_validate_zero_tolerance(const TCDEAnalysisSystem* system, 
                                       ValidationReport* validation_report) {
    if (!system || !validation_report || !system->analysis_complete) return -1;
    
    // Initialize validation report
    memset(validation_report, 0, sizeof(ValidationReport));
    validation_report->zero_tolerance_passed = true;
    
    // Check validation results
    if (system->validation_results->total_violations > 0) {
        validation_report->zero_tolerance_passed = false;
        validation_report->violations_found = system->validation_results->total_violations;
        
        // Copy critical issues summary
        strncpy(validation_report->violation_details[0], 
                system->validation_results->critical_issues_summary,
                sizeof(validation_report->violation_details[0]) - 1);
    }
    
    return validation_report->zero_tolerance_passed ? 0 : -2;
}

int tcde_system_verify_completeness(const TCDEAnalysisSystem* system, 
                                   CompletenessReport* completeness_report) {
    if (!system || !completeness_report) return -1;
    
    memset(completeness_report, 0, sizeof(CompletenessReport));
    
    // Check element count
    completeness_report->expected_elements = TCDE_EXPECTED_ELEMENTS;
    completeness_report->discovered_elements = system->progress.elements_discovered;
    completeness_report->analyzed_elements = system->progress.elements_analyzed;
    completeness_report->validated_elements = system->progress.elements_validated;
    
    completeness_report->discovery_complete = 
        (completeness_report->discovered_elements == TCDE_EXPECTED_ELEMENTS);
    completeness_report->analysis_complete = 
        (completeness_report->analyzed_elements == completeness_report->discovered_elements);
    completeness_report->validation_complete = 
        (completeness_report->validated_elements == completeness_report->analyzed_elements);
    
    completeness_report->overall_complete = 
        completeness_report->discovery_complete && 
        completeness_report->analysis_complete && 
        completeness_report->validation_complete;
    
    return completeness_report->overall_complete ? 0 : -2;
}

int tcde_system_self_test(TCDEAnalysisSystem* system) {
    if (!system) return -1;
    
    // Basic system integrity checks
    if (!system->system_initialized) return -2;
    if (!system->registry) return -3;
    if (!system->analysis_results) return -4;
    if (!system->validation_results) return -5;
    if (!system->dependency_graph) return -6;
    if (!system->knowledge_base) return -7;
    if (!system->report_suite) return -8;
    
    // All tests passed
    return 0;
}
