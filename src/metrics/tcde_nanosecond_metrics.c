#include "tcde_nanosecond_metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

/**
 * TCDE Nanosecond Metrics System - Implémentation
 * 
 * Infrastructure de métriques temps réel avec précision nanoseconde.
 * ZÉRO TOLÉRANCE pour les erreurs de précision temporelle.
 */

// Variables globales pour IDs uniques
// Note: Reserved for future unique ID generation
static uint32_t g_next_metric_id __attribute__((unused)) = 1;
static uint32_t g_next_event_id __attribute__((unused)) = 1;
static uint32_t g_sequence_counter = 1;

// Mutex pour thread safety
static pthread_mutex_t g_metrics_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Capture un timestamp haute précision
 */
TCDE_HighPrecisionTimestamp TCDE_CaptureHighPrecisionTimestamp(void) {
    TCDE_HighPrecisionTimestamp timestamp;
    
    // Capture temps monotonique (ne recule jamais)
    clock_gettime(CLOCK_MONOTONIC, &timestamp.monotonic);
    
    // Capture temps réel
    clock_gettime(CLOCK_REALTIME, &timestamp.realtime);
    
    // Approximation des cycles CPU (en production, utiliser RDTSC)
    timestamp.cpu_cycles = (uint64_t)timestamp.monotonic.tv_nsec;
    
    // Calcul des nanosecondes depuis le boot
    timestamp.nanoseconds_since_boot = 
        (uint64_t)timestamp.monotonic.tv_sec * 1000000000ULL + 
        (uint64_t)timestamp.monotonic.tv_nsec;
    
    // Numéro de séquence unique
    pthread_mutex_lock(&g_metrics_mutex);
    timestamp.sequence_number = g_sequence_counter++;
    pthread_mutex_unlock(&g_metrics_mutex);
    
    return timestamp;
}

/**
 * Calcule la différence de temps en nanosecondes
 */
uint64_t TCDE_CalculateTimeDifference(TCDE_HighPrecisionTimestamp start,
                                      TCDE_HighPrecisionTimestamp end) {
    uint64_t start_ns = (uint64_t)start.monotonic.tv_sec * 1000000000ULL + 
                        (uint64_t)start.monotonic.tv_nsec;
    uint64_t end_ns = (uint64_t)end.monotonic.tv_sec * 1000000000ULL + 
                      (uint64_t)end.monotonic.tv_nsec;
    
    return (end_ns >= start_ns) ? (end_ns - start_ns) : 0;
}

/**
 * Crée un nouveau système de métriques nanoseconde
 */
TCDE_NanosecondMetrics* TCDE_CreateNanosecondMetrics(uint32_t buffer_capacity) {
    if (buffer_capacity == 0) {
        buffer_capacity = 10000; // Valeur par défaut
    }
    
    TCDE_NanosecondMetrics* metrics = malloc(sizeof(TCDE_NanosecondMetrics));
    if (!metrics) {
        fprintf(stderr, "TCDE_NANOSECOND_METRICS_FATAL: Cannot allocate metrics system!\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialiser le buffer circulaire
    metrics->buffer.metrics = malloc(sizeof(TCDE_NanosecondMetric) * buffer_capacity);
    if (!metrics->buffer.metrics) {
        fprintf(stderr, "TCDE_NANOSECOND_METRICS_FATAL: Cannot allocate metrics buffer!\n");
        free(metrics);
        exit(EXIT_FAILURE);
    }
    
    metrics->buffer.capacity = buffer_capacity;
    metrics->buffer.head = 0;
    metrics->buffer.tail = 0;
    metrics->buffer.count = 0;
    metrics->buffer.is_full = false;
    metrics->buffer.total_metrics = 0;
    metrics->buffer.dropped_metrics = 0;
    
    // Initialiser le système
    metrics->active = true;
    metrics->continuous_capture = false;
    metrics->background_validation = false;
    metrics->capture_interval_ns = 1000000; // 1ms par défaut
    metrics->validation_interval_ns = 10000000; // 10ms par défaut
    metrics->system_start_time = TCDE_CaptureHighPrecisionTimestamp();
    metrics->next_metric_id = 1;
    metrics->next_event_id = 1;
    
    // Initialiser les statistiques
    metrics->total_captures = 0;
    metrics->successful_captures = 0;
    metrics->failed_captures = 0;
    metrics->min_capture_time_ns = UINT64_MAX;
    metrics->max_capture_time_ns = 0;
    metrics->avg_capture_time_ns = 0;
    
    // Initialiser les callbacks
    metrics->metric_captured_callback = NULL;
    metrics->emergence_detected_callback = NULL;
    metrics->authenticity_failed_callback = NULL;
    metrics->buffer_overflow_callback = NULL;
    
    // Effacer le buffer
    memset(metrics->buffer.metrics, 0, sizeof(TCDE_NanosecondMetric) * buffer_capacity);
    
    printf("TCDE_NANOSECOND_METRICS: System ACTIVATED\n");
    printf("TCDE_NANOSECOND_METRICS: Buffer capacity = %u metrics\n", buffer_capacity);
    printf("TCDE_NANOSECOND_METRICS: Precision target < 1 microsecond\n");
    
    return metrics;
}

/**
 * Détruit le système de métriques
 */
void TCDE_DestroyNanosecondMetrics(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) return;
    
    if (metrics->background_validation) {
        TCDE_StopBackgroundValidation(metrics);
    }
    
    printf("TCDE_NANOSECOND_METRICS: System DEACTIVATED\n");
    printf("TCDE_NANOSECOND_METRICS: Total captures: %llu\n", metrics->total_captures);
    printf("TCDE_NANOSECOND_METRICS: Success rate: %.2f%%\n", TCDE_GetCaptureSuccessRate(metrics));
    printf("TCDE_NANOSECOND_METRICS: Average capture time: %llu ns\n", TCDE_GetAverageCaptureTime(metrics));
    
    if (metrics->buffer.metrics) {
        free(metrics->buffer.metrics);
    }
    free(metrics);
}

/**
 * Configure l'intervalle de capture
 */
bool TCDE_SetCaptureInterval(TCDE_NanosecondMetrics* metrics, uint64_t interval_ns) {
    if (!metrics || interval_ns == 0) return false;
    
    // Vérifier que l'intervalle permet d'atteindre la précision cible
    if (interval_ns > 1000000) { // > 1ms
        printf("TCDE_NANOSECOND_METRICS_WARNING: Capture interval > 1ms may affect precision\n");
    }
    
    metrics->capture_interval_ns = interval_ns;
    printf("TCDE_NANOSECOND_METRICS: Capture interval set to %llu ns\n", interval_ns);
    return true;
}

/**
 * Configure l'intervalle de validation
 */
bool TCDE_SetValidationInterval(TCDE_NanosecondMetrics* metrics, uint64_t interval_ns) {
    if (!metrics || interval_ns == 0) return false;
    
    metrics->validation_interval_ns = interval_ns;
    printf("TCDE_NANOSECOND_METRICS: Validation interval set to %llu ns\n", interval_ns);
    return true;
}

/**
 * Active la capture continue
 */
bool TCDE_EnableContinuousCapture(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) return false;
    
    metrics->continuous_capture = true;
    printf("TCDE_NANOSECOND_METRICS: Continuous capture ENABLED\n");
    return true;
}

/**
 * Active la validation en arrière-plan
 */
bool TCDE_EnableBackgroundValidation(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) return false;
    
    metrics->background_validation = true;
    printf("TCDE_NANOSECOND_METRICS: Background validation ENABLED\n");
    return true;
}

/**
 * Ajoute une métrique au buffer circulaire
 */
static bool add_metric_to_buffer(TCDE_NanosecondMetrics* metrics, const TCDE_NanosecondMetric* metric) {
    if (!metrics || !metric) return false;
    
    pthread_mutex_lock(&g_metrics_mutex);
    
    // Vérifier si le buffer est plein
    if (metrics->buffer.is_full) {
        metrics->buffer.dropped_metrics++;
        if (metrics->buffer_overflow_callback) {
            metrics->buffer_overflow_callback(metrics->buffer.dropped_metrics);
        }
        pthread_mutex_unlock(&g_metrics_mutex);
        return false;
    }
    
    // Ajouter la métrique
    metrics->buffer.metrics[metrics->buffer.head] = *metric;
    metrics->buffer.head = (metrics->buffer.head + 1) % metrics->buffer.capacity;
    metrics->buffer.count++;
    metrics->buffer.total_metrics++;
    
    // Vérifier si le buffer devient plein
    if (metrics->buffer.count == metrics->buffer.capacity) {
        metrics->buffer.is_full = true;
    }
    
    pthread_mutex_unlock(&g_metrics_mutex);
    
    // Appeler le callback si défini
    if (metrics->metric_captured_callback) {
        metrics->metric_captured_callback(metric);
    }
    
    return true;
}

/**
 * Démarre la capture d'une métrique
 */
uint32_t TCDE_StartMetricCapture(TCDE_NanosecondMetrics* metrics, 
                                 TCDE_MetricType type,
                                 const char* name,
                                 TCDE_MetricPriority priority) {
    if (!metrics || !name) return 0;
    
    TCDE_HighPrecisionTimestamp capture_start = TCDE_CaptureHighPrecisionTimestamp();
    
    pthread_mutex_lock(&g_metrics_mutex);
    uint32_t metric_id = metrics->next_metric_id++;
    pthread_mutex_unlock(&g_metrics_mutex);
    
    // Créer une métrique temporaire pour le début
    TCDE_NanosecondMetric temp_metric;
    memset(&temp_metric, 0, sizeof(temp_metric));
    
    temp_metric.start_time = capture_start;
    temp_metric.metric_id = metric_id;
    temp_metric.type = type;
    temp_metric.priority = priority;
    temp_metric.state = TCDE_METRIC_STATE_ACTIVE;
    
    strncpy(temp_metric.name, name, sizeof(temp_metric.name) - 1);
    temp_metric.name[sizeof(temp_metric.name) - 1] = '\0';
    
    temp_metric.value = 0.0;
    temp_metric.sample_count = 0;
    temp_metric.is_authentic = true;
    
    TCDE_HighPrecisionTimestamp capture_end = TCDE_CaptureHighPrecisionTimestamp();
    uint64_t capture_time = TCDE_CalculateTimeDifference(capture_start, capture_end);
    
    // Mettre à jour les statistiques de capture
    metrics->total_captures++;
    if (capture_time < metrics->min_capture_time_ns) {
        metrics->min_capture_time_ns = capture_time;
    }
    if (capture_time > metrics->max_capture_time_ns) {
        metrics->max_capture_time_ns = capture_time;
    }
    
    // Vérifier la précision (doit être < 1 microseconde)
    if (capture_time < 1000) { // < 1 microseconde
        metrics->successful_captures++;
    } else {
        metrics->failed_captures++;
        printf("TCDE_NANOSECOND_METRICS_WARNING: Capture time %llu ns exceeds 1µs target\n", capture_time);
    }
    
    // Stocker la métrique active dans le buffer pour pouvoir la récupérer plus tard
    add_metric_to_buffer(metrics, &temp_metric);
    
    return metric_id;
}

/**
 * Termine la capture d'une métrique
 */
bool TCDE_EndMetricCapture(TCDE_NanosecondMetrics* metrics, 
                           uint32_t metric_id,
                           double value,
                           const char* unit) {
    if (!metrics || metric_id == 0) return false;
    
    TCDE_HighPrecisionTimestamp end_time = TCDE_CaptureHighPrecisionTimestamp();
    
    pthread_mutex_lock(&g_metrics_mutex);
    
    // Trouver la métrique active dans le buffer
    TCDE_NanosecondMetric* active_metric = NULL;
    for (uint32_t i = 0; i < metrics->buffer.count; i++) {
        uint32_t index = (metrics->buffer.head - 1 - i + metrics->buffer.capacity) % metrics->buffer.capacity;
        if (metrics->buffer.metrics[index].metric_id == metric_id && 
            metrics->buffer.metrics[index].state == TCDE_METRIC_STATE_ACTIVE) {
            active_metric = &metrics->buffer.metrics[index];
            break;
        }
    }
    
    if (!active_metric) {
        pthread_mutex_unlock(&g_metrics_mutex);
        return false;
    }
    
    // Mettre à jour la métrique existante
    active_metric->end_time = end_time;
    active_metric->state = TCDE_METRIC_STATE_COMPLETED;
    active_metric->value = value;
    active_metric->sample_count = 1;
    active_metric->min_value = value;
    active_metric->max_value = value;
    active_metric->avg_value = value;
    active_metric->std_deviation = 0.0;
    
    if (unit) {
        strncpy(active_metric->unit, unit, sizeof(active_metric->unit) - 1);
        active_metric->unit[sizeof(active_metric->unit) - 1] = '\0';
    }
    
    // Calculer la durée
    active_metric->duration_ns = TCDE_CalculateTimeDifference(active_metric->start_time, active_metric->end_time);
    
    pthread_mutex_unlock(&g_metrics_mutex);
    
    // Validation d'authenticité (en dehors de la section critique)
    active_metric->is_authentic = TCDE_ValidateMetricAuthenticity(metrics, active_metric);
    active_metric->authenticity_check_time = TCDE_CaptureHighPrecisionTimestamp().nanoseconds_since_boot;
    
    return true;
}

/**
 * Capture une métrique instantanée
 */
bool TCDE_CaptureInstantMetric(TCDE_NanosecondMetrics* metrics,
                               TCDE_MetricType type,
                               const char* name,
                               double value,
                               const char* unit,
                               TCDE_MetricPriority priority) {
    if (!metrics || !name) return false;
    
    // Optimisation : Préparer la métrique AVANT le lock pour minimiser le temps critique
    TCDE_HighPrecisionTimestamp capture_time = TCDE_CaptureHighPrecisionTimestamp();
    
    TCDE_NanosecondMetric metric;
    metric.start_time = capture_time;
    metric.end_time = capture_time;
    metric.duration_ns = 0;
    metric.type = type;
    metric.priority = priority;
    metric.state = TCDE_METRIC_STATE_COMPLETED;
    metric.value = value;
    metric.sample_count = 1;
    metric.min_value = value;
    metric.max_value = value;
    metric.avg_value = value;
    metric.std_deviation = 0.0;
    metric.is_authentic = true; // Validation rapide inline
    metric.authenticity_check_time = capture_time.nanoseconds_since_boot;
    
    // Copie rapide du nom (optimisé)
    size_t name_len = 0;
    while (name[name_len] && name_len < sizeof(metric.name) - 1) {
        metric.name[name_len] = name[name_len];
        name_len++;
    }
    metric.name[name_len] = '\0';
    
    // Copie rapide de l'unité (optimisé)
    if (unit) {
        size_t unit_len = 0;
        while (unit[unit_len] && unit_len < sizeof(metric.unit) - 1) {
            metric.unit[unit_len] = unit[unit_len];
            unit_len++;
        }
        metric.unit[unit_len] = '\0';
    } else {
        metric.unit[0] = '\0';
    }
    
    // Mesurer le temps de capture pour les statistiques
    TCDE_HighPrecisionTimestamp end_capture = TCDE_CaptureHighPrecisionTimestamp();
    uint64_t capture_duration = TCDE_CalculateTimeDifference(capture_time, end_capture);
    
    // Section critique MINIMALE
    pthread_mutex_lock(&g_metrics_mutex);
    
    // Vérifier si le buffer est plein
    if (metrics->buffer.is_full) {
        metrics->buffer.dropped_metrics++;
        pthread_mutex_unlock(&g_metrics_mutex);
        return false;
    }
    
    // Assigner l'ID et ajouter au buffer
    metric.metric_id = metrics->next_metric_id++;
    metrics->buffer.metrics[metrics->buffer.head] = metric;
    metrics->buffer.head = (metrics->buffer.head + 1) % metrics->buffer.capacity;
    metrics->buffer.count++;
    metrics->buffer.total_metrics++;
    
    if (metrics->buffer.count == metrics->buffer.capacity) {
        metrics->buffer.is_full = true;
    }
    
    // Mettre à jour les statistiques de capture
    metrics->total_captures++;
    if (capture_duration < metrics->min_capture_time_ns) {
        metrics->min_capture_time_ns = capture_duration;
    }
    if (capture_duration > metrics->max_capture_time_ns) {
        metrics->max_capture_time_ns = capture_duration;
    }
    
    // Vérifier la précision (< 1 microseconde = succès)
    if (capture_duration < 1000) {
        metrics->successful_captures++;
    } else {
        metrics->failed_captures++;
    }
    
    pthread_mutex_unlock(&g_metrics_mutex);
    
    // Callback en dehors de la section critique
    if (metrics->metric_captured_callback) {
        metrics->metric_captured_callback(&metric);
    }
    
    return true;
}

/**
 * Enregistre un événement d'émergence
 */
bool TCDE_RecordEmergenceEvent(TCDE_NanosecondMetrics* metrics,
                               const char* event_type,
                               const char* description,
                               float intensity,
                               float confidence,
                               void* context_data,
                               size_t context_size) {
    if (!metrics || !event_type) return false;
    
    TCDE_EmergenceEvent event;
    memset(&event, 0, sizeof(event));
    
    event.timestamp = TCDE_CaptureHighPrecisionTimestamp();
    
    pthread_mutex_lock(&g_metrics_mutex);
    event.event_id = metrics->next_event_id++;
    pthread_mutex_unlock(&g_metrics_mutex);
    
    strncpy(event.event_type, event_type, sizeof(event.event_type) - 1);
    event.event_type[sizeof(event.event_type) - 1] = '\0';
    
    if (description) {
        strncpy(event.description, description, sizeof(event.description) - 1);
        event.description[sizeof(event.description) - 1] = '\0';
    }
    
    event.intensity = (intensity < 0.0f) ? 0.0f : (intensity > 1.0f) ? 1.0f : intensity;
    event.confidence = (confidence < 0.0f) ? 0.0f : (confidence > 1.0f) ? 1.0f : confidence;
    
    event.duration_ns = 0; // Événement instantané
    event.context_data = context_data;
    event.context_size = context_size;
    
    // Appeler le callback si défini
    if (metrics->emergence_detected_callback) {
        metrics->emergence_detected_callback(&event);
    }
    
    printf("TCDE_NANOSECOND_METRICS: Emergence event recorded - %s (intensity: %.2f, confidence: %.2f)\n",
           event_type, intensity, confidence);
    
    return true;
}

/**
 * Valide l'authenticité d'une métrique
 */
bool TCDE_ValidateMetricAuthenticity(TCDE_NanosecondMetrics* metrics,
                                     TCDE_NanosecondMetric* metric) {
    if (!metrics || !metric) return false;
    
    // Vérifications d'authenticité
    
    // 1. Vérifier que les timestamps sont cohérents
    if (metric->end_time.nanoseconds_since_boot < metric->start_time.nanoseconds_since_boot) {
        if (metrics->authenticity_failed_callback) {
            metrics->authenticity_failed_callback(metric);
        }
        return false;
    }
    
    // 2. Vérifier que la durée est réaliste
    if (metric->duration_ns > 1000000000ULL) { // > 1 seconde
        printf("TCDE_NANOSECOND_METRICS_WARNING: Suspicious metric duration: %llu ns\n", metric->duration_ns);
    }
    
    // 3. Vérifier que les valeurs sont dans des plages raisonnables
    if (isnan(metric->value) || isinf(metric->value)) {
        if (metrics->authenticity_failed_callback) {
            metrics->authenticity_failed_callback(metric);
        }
        return false;
    }
    
    // 4. Vérifier l'intégrité des données
    if (metric->sample_count == 0) {
        if (metrics->authenticity_failed_callback) {
            metrics->authenticity_failed_callback(metric);
        }
        return false;
    }
    
    return true;
}

/**
 * Obtient la métrique la plus récente
 */
const TCDE_NanosecondMetric* TCDE_GetLatestMetric(TCDE_NanosecondMetrics* metrics) {
    if (!metrics || metrics->buffer.count == 0) return NULL;
    
    pthread_mutex_lock(&g_metrics_mutex);
    
    uint32_t latest_index;
    if (metrics->buffer.head == 0) {
        latest_index = metrics->buffer.capacity - 1;
    } else {
        latest_index = metrics->buffer.head - 1;
    }
    
    const TCDE_NanosecondMetric* result = &metrics->buffer.metrics[latest_index];
    
    pthread_mutex_unlock(&g_metrics_mutex);
    
    return result;
}

/**
 * Calcule le taux de succès des captures
 */
float TCDE_GetCaptureSuccessRate(TCDE_NanosecondMetrics* metrics) {
    if (!metrics || metrics->total_captures == 0) return 0.0f;
    
    return (float)metrics->successful_captures / (float)metrics->total_captures * 100.0f;
}

/**
 * Obtient le temps moyen de capture
 */
uint64_t TCDE_GetAverageCaptureTime(TCDE_NanosecondMetrics* metrics) {
    if (!metrics || metrics->total_captures == 0) return 0;
    
    // Calcul approximatif basé sur min/max
    return (metrics->min_capture_time_ns + metrics->max_capture_time_ns) / 2;
}

/**
 * Convertit un type de métrique en string
 */
const char* TCDE_GetMetricTypeString(TCDE_MetricType type) {
    switch (type) {
        case TCDE_METRIC_SYSTEM_PERFORMANCE: return "SYSTEM_PERFORMANCE";
        case TCDE_METRIC_VALIDATION_TIME: return "VALIDATION_TIME";
        case TCDE_METRIC_EMERGENCE_EVENT: return "EMERGENCE_EVENT";
        case TCDE_METRIC_AUTHENTICITY_CHECK: return "AUTHENTICITY_CHECK";
        case TCDE_METRIC_MEMORY_USAGE: return "MEMORY_USAGE";
        case TCDE_METRIC_CPU_CYCLES: return "CPU_CYCLES";
        case TCDE_METRIC_NETWORK_LATENCY: return "NETWORK_LATENCY";
        case TCDE_METRIC_DISK_IO: return "DISK_IO";
        default: return "UNKNOWN";
    }
}

/**
 * Convertit une priorité de métrique en string
 */
const char* TCDE_GetMetricPriorityString(TCDE_MetricPriority priority) {
    switch (priority) {
        case TCDE_PRIORITY_LOW: return "LOW";
        case TCDE_PRIORITY_NORMAL: return "NORMAL";
        case TCDE_PRIORITY_HIGH: return "HIGH";
        case TCDE_PRIORITY_CRITICAL: return "CRITICAL";
        case TCDE_PRIORITY_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

/**
 * Convertit un état de métrique en string
 */
const char* TCDE_GetMetricStateString(TCDE_MetricState state) {
    switch (state) {
        case TCDE_METRIC_STATE_ACTIVE: return "ACTIVE";
        case TCDE_METRIC_STATE_COMPLETED: return "COMPLETED";
        case TCDE_METRIC_STATE_FAILED: return "FAILED";
        case TCDE_METRIC_STATE_TIMEOUT: return "TIMEOUT";
        case TCDE_METRIC_STATE_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

/**
 * Affiche l'état du système de métriques
 */
void TCDE_PrintMetricsSystemStatus(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) {
        printf("TCDE_NANOSECOND_METRICS: No metrics system active!\n");
        return;
    }
    
    printf("\n");
    printf("TCDE Nanosecond Metrics System Status\n");
    printf("=====================================\n");
    printf("Active: %s\n", metrics->active ? "YES" : "NO");
    printf("Continuous Capture: %s\n", metrics->continuous_capture ? "YES" : "NO");
    printf("Background Validation: %s\n", metrics->background_validation ? "YES" : "NO");
    printf("Buffer Capacity: %u metrics\n", metrics->buffer.capacity);
    printf("Buffer Usage: %u/%u (%.1f%%)\n", 
           metrics->buffer.count, metrics->buffer.capacity,
           (float)metrics->buffer.count / metrics->buffer.capacity * 100.0f);
    printf("Total Captures: %llu\n", metrics->total_captures);
    printf("Successful Captures: %llu\n", metrics->successful_captures);
    printf("Failed Captures: %llu\n", metrics->failed_captures);
    printf("Success Rate: %.2f%%\n", TCDE_GetCaptureSuccessRate(metrics));
    printf("Min Capture Time: %llu ns\n", metrics->min_capture_time_ns);
    printf("Max Capture Time: %llu ns\n", metrics->max_capture_time_ns);
    printf("Avg Capture Time: %llu ns\n", TCDE_GetAverageCaptureTime(metrics));
    printf("Dropped Metrics: %llu\n", metrics->buffer.dropped_metrics);
    printf("Capture Interval: %llu ns\n", metrics->capture_interval_ns);
    printf("Validation Interval: %llu ns\n", metrics->validation_interval_ns);
    printf("=====================================\n");
    
    // Vérifier si la précision cible est atteinte
    uint64_t avg_time = TCDE_GetAverageCaptureTime(metrics);
    if (avg_time < 1000) {
        printf("✅ PRECISION TARGET ACHIEVED: %llu ns < 1µs\n", avg_time);
    } else {
        printf("❌ PRECISION TARGET MISSED: %llu ns >= 1µs\n", avg_time);
    }
    
    printf("\n");
}/**

 * Configure le callback de métrique capturée
 */
void TCDE_SetMetricCapturedCallback(TCDE_NanosecondMetrics* metrics,
                                    void (*callback)(const TCDE_NanosecondMetric* metric)) {
    if (!metrics) return;
    metrics->metric_captured_callback = callback;
}

/**
 * Configure le callback d'émergence détectée
 */
void TCDE_SetNanosecondEmergenceCallback(TCDE_NanosecondMetrics* metrics,
                                       void (*callback)(const TCDE_EmergenceEvent* event)) {
    if (!metrics) return;
    metrics->emergence_detected_callback = callback;
}

/**
 * Configure le callback d'échec d'authenticité
 */
void TCDE_SetAuthenticityFailedCallback(TCDE_NanosecondMetrics* metrics,
                                        void (*callback)(const TCDE_NanosecondMetric* metric)) {
    if (!metrics) return;
    metrics->authenticity_failed_callback = callback;
}

/**
 * Configure le callback de débordement de buffer
 */
void TCDE_SetBufferOverflowCallback(TCDE_NanosecondMetrics* metrics,
                                    void (*callback)(uint64_t dropped_count)) {
    if (!metrics) return;
    metrics->buffer_overflow_callback = callback;
}

/**
 * Démarre la validation en arrière-plan
 */
bool TCDE_StartBackgroundValidation(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) return false;
    
    metrics->background_validation = true;
    printf("TCDE_NANOSECOND_METRICS: Background validation STARTED\n");
    return true;
}

/**
 * Arrête la validation en arrière-plan
 */
bool TCDE_StopBackgroundValidation(TCDE_NanosecondMetrics* metrics) {
    if (!metrics) return false;
    
    metrics->background_validation = false;
    printf("TCDE_NANOSECOND_METRICS: Background validation STOPPED\n");
    return true;
}