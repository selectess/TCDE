#ifndef TCDE_NANOSECOND_METRICS_H
#define TCDE_NANOSECOND_METRICS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

/**
 * TCDE Nanosecond Metrics System
 * 
 * Infrastructure de métriques temps réel avec précision nanoseconde.
 * Capture sans latence et validation continue d'authenticité.
 * 
 * CRITÈRE SUCCÈS: Précision temporelle < 1 microseconde
 */

// Types de métriques
typedef enum {
    TCDE_METRIC_SYSTEM_PERFORMANCE = 1,
    TCDE_METRIC_VALIDATION_TIME = 2,
    TCDE_METRIC_EMERGENCE_EVENT = 3,
    TCDE_METRIC_AUTHENTICITY_CHECK = 4,
    TCDE_METRIC_MEMORY_USAGE = 5,
    TCDE_METRIC_CPU_CYCLES = 6,
    TCDE_METRIC_NETWORK_LATENCY = 7,
    TCDE_METRIC_DISK_IO = 8
} TCDE_MetricType;

// Niveaux de priorité
typedef enum {
    TCDE_PRIORITY_LOW = 1,
    TCDE_PRIORITY_NORMAL = 2,
    TCDE_PRIORITY_HIGH = 3,
    TCDE_PRIORITY_CRITICAL = 4,
    TCDE_PRIORITY_EMERGENCY = 5
} TCDE_MetricPriority;

// État de métrique
typedef enum {
    TCDE_METRIC_STATE_ACTIVE = 1,
    TCDE_METRIC_STATE_COMPLETED = 2,
    TCDE_METRIC_STATE_FAILED = 3,
    TCDE_METRIC_STATE_TIMEOUT = 4,
    TCDE_METRIC_STATE_CANCELLED = 5
} TCDE_MetricState;

// Timestamp haute précision
typedef struct {
    struct timespec monotonic;           // Temps monotonique (CLOCK_MONOTONIC)
    struct timespec realtime;            // Temps réel (CLOCK_REALTIME)
    uint64_t cpu_cycles;                 // Cycles CPU (approximation)
    uint64_t nanoseconds_since_boot;     // Nanosecondes depuis le boot
    uint32_t sequence_number;            // Numéro de séquence unique
} TCDE_HighPrecisionTimestamp;

// Événement d'émergence microseconde-précis
typedef struct {
    TCDE_HighPrecisionTimestamp timestamp;
    uint32_t event_id;                   // ID unique de l'événement
    char event_type[64];                 // Type d'événement
    char description[256];               // Description de l'événement
    float intensity;                     // Intensité de l'émergence (0.0-1.0)
    float confidence;                    // Confiance dans la détection (0.0-1.0)
    uint64_t duration_ns;                // Durée en nanosecondes
    void* context_data;                  // Données contextuelles
    size_t context_size;                 // Taille des données contextuelles
} TCDE_EmergenceEvent;

// Métrique nanoseconde complète
typedef struct {
    TCDE_HighPrecisionTimestamp start_time;
    TCDE_HighPrecisionTimestamp end_time;
    uint64_t duration_ns;                // Durée calculée en nanosecondes
    uint32_t metric_id;                  // ID unique de la métrique
    TCDE_MetricType type;                // Type de métrique
    TCDE_MetricPriority priority;        // Priorité
    TCDE_MetricState state;              // État actuel
    char name[128];                      // Nom de la métrique
    char description[256];               // Description détaillée
    double value;                        // Valeur mesurée
    char unit[32];                       // Unité de mesure
    uint64_t sample_count;               // Nombre d'échantillons
    double min_value;                    // Valeur minimale
    double max_value;                    // Valeur maximale
    double avg_value;                    // Valeur moyenne
    double std_deviation;                // Écart-type
    bool is_authentic;                   // Validation d'authenticité
    uint64_t authenticity_check_time;    // Temps de vérification d'authenticité
} TCDE_NanosecondMetric;

// Buffer circulaire pour métriques temps réel
typedef struct {
    TCDE_NanosecondMetric* metrics;      // Buffer des métriques
    uint32_t capacity;                   // Capacité du buffer
    uint32_t head;                       // Index de tête
    uint32_t tail;                       // Index de queue
    uint32_t count;                      // Nombre d'éléments
    bool is_full;                        // Buffer plein
    uint64_t total_metrics;              // Total des métriques capturées
    uint64_t dropped_metrics;            // Métriques perdues (overflow)
} TCDE_MetricsBuffer;

// Système de métriques nanoseconde
typedef struct {
    TCDE_MetricsBuffer buffer;
    bool active;                         // Système actif
    bool continuous_capture;             // Capture continue
    bool background_validation;          // Validation en arrière-plan
    uint64_t capture_interval_ns;        // Intervalle de capture (ns)
    uint64_t validation_interval_ns;     // Intervalle de validation (ns)
    TCDE_HighPrecisionTimestamp system_start_time;
    uint32_t next_metric_id;             // Prochain ID de métrique
    uint32_t next_event_id;              // Prochain ID d'événement
    
    // Statistiques de performance
    uint64_t total_captures;             // Total des captures
    uint64_t successful_captures;        // Captures réussies
    uint64_t failed_captures;            // Captures échouées
    uint64_t min_capture_time_ns;        // Temps minimum de capture
    uint64_t max_capture_time_ns;        // Temps maximum de capture
    uint64_t avg_capture_time_ns;        // Temps moyen de capture
    
    // Callbacks
    void (*metric_captured_callback)(const TCDE_NanosecondMetric* metric);
    void (*emergence_detected_callback)(const TCDE_EmergenceEvent* event);
    void (*authenticity_failed_callback)(const TCDE_NanosecondMetric* metric);
    void (*buffer_overflow_callback)(uint64_t dropped_count);
} TCDE_NanosecondMetrics;

// Fonctions principales
TCDE_NanosecondMetrics* TCDE_CreateNanosecondMetrics(uint32_t buffer_capacity);
void TCDE_DestroyNanosecondMetrics(TCDE_NanosecondMetrics* metrics);

// Configuration
bool TCDE_SetCaptureInterval(TCDE_NanosecondMetrics* metrics, uint64_t interval_ns);
bool TCDE_SetValidationInterval(TCDE_NanosecondMetrics* metrics, uint64_t interval_ns);
bool TCDE_EnableContinuousCapture(TCDE_NanosecondMetrics* metrics);
bool TCDE_EnableBackgroundValidation(TCDE_NanosecondMetrics* metrics);

// Capture de métriques sans latence
TCDE_HighPrecisionTimestamp TCDE_CaptureHighPrecisionTimestamp(void);
uint32_t TCDE_StartMetricCapture(TCDE_NanosecondMetrics* metrics, 
                                 TCDE_MetricType type,
                                 const char* name,
                                 TCDE_MetricPriority priority);
bool TCDE_EndMetricCapture(TCDE_NanosecondMetrics* metrics, 
                           uint32_t metric_id,
                           double value,
                           const char* unit);
bool TCDE_CaptureInstantMetric(TCDE_NanosecondMetrics* metrics,
                               TCDE_MetricType type,
                               const char* name,
                               double value,
                               const char* unit,
                               TCDE_MetricPriority priority);

// Événements d'émergence microseconde-précis
bool TCDE_RecordEmergenceEvent(TCDE_NanosecondMetrics* metrics,
                               const char* event_type,
                               const char* description,
                               float intensity,
                               float confidence,
                               void* context_data,
                               size_t context_size);

// Validation continue d'authenticité
bool TCDE_ValidateMetricAuthenticity(TCDE_NanosecondMetrics* metrics,
                                     TCDE_NanosecondMetric* metric);
bool TCDE_StartBackgroundValidation(TCDE_NanosecondMetrics* metrics);
bool TCDE_StopBackgroundValidation(TCDE_NanosecondMetrics* metrics);

// Accès aux métriques
const TCDE_NanosecondMetric* TCDE_GetLatestMetric(TCDE_NanosecondMetrics* metrics);
const TCDE_NanosecondMetric* TCDE_GetMetricById(TCDE_NanosecondMetrics* metrics, uint32_t metric_id);
uint32_t TCDE_GetMetricsByType(TCDE_NanosecondMetrics* metrics, 
                               TCDE_MetricType type,
                               TCDE_NanosecondMetric* results,
                               uint32_t max_results);
uint32_t TCDE_GetMetricsByTimeRange(TCDE_NanosecondMetrics* metrics,
                                    TCDE_HighPrecisionTimestamp start,
                                    TCDE_HighPrecisionTimestamp end,
                                    TCDE_NanosecondMetric* results,
                                    uint32_t max_results);

// Statistiques et analyse
uint64_t TCDE_CalculateTimeDifference(TCDE_HighPrecisionTimestamp start,
                                      TCDE_HighPrecisionTimestamp end);
double TCDE_CalculateMetricStatistics(TCDE_NanosecondMetrics* metrics,
                                      TCDE_MetricType type,
                                      double* min_val,
                                      double* max_val,
                                      double* avg_val,
                                      double* std_dev);
float TCDE_GetCaptureSuccessRate(TCDE_NanosecondMetrics* metrics);
uint64_t TCDE_GetAverageCaptureTime(TCDE_NanosecondMetrics* metrics);

// Callbacks
void TCDE_SetMetricCapturedCallback(TCDE_NanosecondMetrics* metrics,
                                    void (*callback)(const TCDE_NanosecondMetric* metric));
void TCDE_SetEmergenceDetectedCallback(TCDE_NanosecondMetrics* metrics,
                                       void (*callback)(const TCDE_EmergenceEvent* event));
void TCDE_SetAuthenticityFailedCallback(TCDE_NanosecondMetrics* metrics,
                                        void (*callback)(const TCDE_NanosecondMetric* metric));
void TCDE_SetBufferOverflowCallback(TCDE_NanosecondMetrics* metrics,
                                    void (*callback)(uint64_t dropped_count));

// Utilitaires
const char* TCDE_GetMetricTypeString(TCDE_MetricType type);
const char* TCDE_GetMetricPriorityString(TCDE_MetricPriority priority);
const char* TCDE_GetMetricStateString(TCDE_MetricState state);
void TCDE_PrintHighPrecisionTimestamp(const TCDE_HighPrecisionTimestamp* timestamp);
void TCDE_PrintNanosecondMetric(const TCDE_NanosecondMetric* metric);
void TCDE_PrintEmergenceEvent(const TCDE_EmergenceEvent* event);
void TCDE_PrintMetricsSystemStatus(TCDE_NanosecondMetrics* metrics);

// Export et import
bool TCDE_ExportMetricsToCSV(TCDE_NanosecondMetrics* metrics, const char* filename);
bool TCDE_ExportMetricsToJSON(TCDE_NanosecondMetrics* metrics, const char* filename);
bool TCDE_ExportMetricsToBinary(TCDE_NanosecondMetrics* metrics, const char* filename);

// Macros pour capture ultra-rapide
#define TCDE_START_METRIC(metrics, type, name, priority) \
    TCDE_StartMetricCapture(metrics, type, name, priority)

#define TCDE_END_METRIC(metrics, id, value, unit) \
    TCDE_EndMetricCapture(metrics, id, value, unit)

#define TCDE_INSTANT_METRIC(metrics, type, name, value, unit, priority) \
    TCDE_CaptureInstantMetric(metrics, type, name, value, unit, priority)

#define TCDE_RECORD_EMERGENCE(metrics, type, desc, intensity, confidence) \
    TCDE_RecordEmergenceEvent(metrics, type, desc, intensity, confidence, NULL, 0)

// Macros de mesure de performance
#define TCDE_MEASURE_BLOCK(metrics, name, priority, block) \
    do { \
        uint32_t __metric_id = TCDE_START_METRIC(metrics, TCDE_METRIC_SYSTEM_PERFORMANCE, name, priority); \
        block \
        TCDE_END_METRIC(metrics, __metric_id, 1.0, "execution"); \
    } while(0)

#define TCDE_MEASURE_FUNCTION(metrics, name, priority, func_call) \
    ({ \
        uint32_t __metric_id = TCDE_START_METRIC(metrics, TCDE_METRIC_SYSTEM_PERFORMANCE, name, priority); \
        auto __result = func_call; \
        TCDE_END_METRIC(metrics, __metric_id, 1.0, "execution"); \
        __result; \
    })

#endif // TCDE_NANOSECOND_METRICS_H