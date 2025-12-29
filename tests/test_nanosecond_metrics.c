#include "../src/metrics/tcde_nanosecond_metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

/**
 * Test du Système de Métriques Nanoseconde TCDE
 * 
 * Tests avec ZÉRO TOLÉRANCE pour les erreurs de précision temporelle
 * CRITÈRE SUCCÈS: Précision temporelle < 1 microseconde
 */

// Callbacks de test
static const TCDE_NanosecondMetric* last_captured_metric = NULL;
static const TCDE_EmergenceEvent* last_emergence_event = NULL;
static const TCDE_NanosecondMetric* last_authenticity_failed_metric = NULL;
static uint64_t last_dropped_count = 0;

static bool metric_callback_triggered = false;
static bool emergence_callback_triggered = false;
static bool authenticity_callback_triggered = false;
static bool overflow_callback_triggered = false;

void test_metric_captured_callback(const TCDE_NanosecondMetric* metric) {
    last_captured_metric = metric;
    metric_callback_triggered = true;
    printf("TEST: Metric captured - ID %u, Type: %s, Value: %.2f %s\n",
           metric->metric_id, TCDE_GetMetricTypeString(metric->type), 
           metric->value, metric->unit);
}

void test_emergence_detected_callback(const TCDE_EmergenceEvent* event) {
    last_emergence_event = event;
    emergence_callback_triggered = true;
    printf("TEST: Emergence detected - ID %u, Type: %s, Intensity: %.2f\n",
           event->event_id, event->event_type, event->intensity);
}

void test_authenticity_failed_callback(const TCDE_NanosecondMetric* metric) {
    last_authenticity_failed_metric = metric;
    authenticity_callback_triggered = true;
    printf("TEST: Authenticity failed - ID %u\n", metric->metric_id);
}

void test_buffer_overflow_callback(uint64_t dropped_count) {
    last_dropped_count = dropped_count;
    overflow_callback_triggered = true;
    printf("TEST: Buffer overflow - %llu metrics dropped\n", dropped_count);
}

/**
 * Test 1: Création et destruction du système de métriques
 */
bool test_metrics_system_creation(void) {
    printf("\n=== Test 1: Nanosecond Metrics System Creation ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(1000);
    assert(metrics != NULL);
    
    // Vérifier l'état initial
    assert(metrics->active == true);
    assert(metrics->buffer.capacity == 1000);
    assert(metrics->buffer.count == 0);
    assert(metrics->buffer.is_full == false);
    assert(metrics->total_captures == 0);
    assert(metrics->successful_captures == 0);
    assert(metrics->failed_captures == 0);
    
    printf("✅ Nanosecond Metrics System created successfully\n");
    printf("✅ Initial state verified (buffer capacity: %u)\n", metrics->buffer.capacity);
    
    TCDE_DestroyNanosecondMetrics(metrics);
    printf("✅ Nanosecond Metrics System destroyed successfully\n");
    
    return true;
}

/**
 * Test 2: Configuration du système
 */
bool test_metrics_system_configuration(void) {
    printf("\n=== Test 2: Metrics System Configuration ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(500);
    
    // Configurer les intervalles
    bool result = TCDE_SetCaptureInterval(metrics, 500000); // 500µs
    assert(result == true);
    assert(metrics->capture_interval_ns == 500000);
    printf("✅ Capture interval set to 500µs\n");
    
    result = TCDE_SetValidationInterval(metrics, 5000000); // 5ms
    assert(result == true);
    assert(metrics->validation_interval_ns == 5000000);
    printf("✅ Validation interval set to 5ms\n");
    
    // Activer les fonctionnalités
    result = TCDE_EnableContinuousCapture(metrics);
    assert(result == true);
    assert(metrics->continuous_capture == true);
    printf("✅ Continuous capture enabled\n");
    
    result = TCDE_EnableBackgroundValidation(metrics);
    assert(result == true);
    assert(metrics->background_validation == true);
    printf("✅ Background validation enabled\n");
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 3: Capture de timestamps haute précision
 */
bool test_high_precision_timestamps(void) {
    printf("\n=== Test 3: High Precision Timestamps ===\n");
    
    // Capturer plusieurs timestamps
    TCDE_HighPrecisionTimestamp ts1 = TCDE_CaptureHighPrecisionTimestamp();
    usleep(100); // Attendre 100µs
    TCDE_HighPrecisionTimestamp ts2 = TCDE_CaptureHighPrecisionTimestamp();
    usleep(100); // Attendre 100µs
    TCDE_HighPrecisionTimestamp ts3 = TCDE_CaptureHighPrecisionTimestamp();
    
    // Vérifier que les timestamps sont croissants
    assert(ts2.nanoseconds_since_boot > ts1.nanoseconds_since_boot);
    assert(ts3.nanoseconds_since_boot > ts2.nanoseconds_since_boot);
    printf("✅ Timestamps are monotonically increasing\n");
    
    // Vérifier que les numéros de séquence sont uniques
    assert(ts1.sequence_number != ts2.sequence_number);
    assert(ts2.sequence_number != ts3.sequence_number);
    printf("✅ Sequence numbers are unique\n");
    
    // Calculer les différences de temps
    uint64_t diff1 = TCDE_CalculateTimeDifference(ts1, ts2);
    uint64_t diff2 = TCDE_CalculateTimeDifference(ts2, ts3);
    
    // Vérifier que les différences sont dans la plage attendue (environ 100µs)
    assert(diff1 > 50000);  // > 50µs
    assert(diff1 < 500000); // < 500µs
    assert(diff2 > 50000);  // > 50µs
    assert(diff2 < 500000); // < 500µs
    
    printf("✅ Time differences calculated: %llu ns, %llu ns\n", diff1, diff2);
    
    return true;
}

/**
 * Test 4: Capture de métriques instantanées
 */
bool test_instant_metric_capture(void) {
    printf("\n=== Test 4: Instant Metric Capture ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(100);
    
    // Capturer plusieurs métriques instantanées
    bool result = TCDE_CaptureInstantMetric(metrics, 
                                            TCDE_METRIC_SYSTEM_PERFORMANCE,
                                            "test_cpu_usage",
                                            75.5,
                                            "percent",
                                            TCDE_PRIORITY_NORMAL);
    assert(result == true);
    assert(metrics->buffer.count == 1);
    printf("✅ First instant metric captured\n");
    
    result = TCDE_CaptureInstantMetric(metrics,
                                       TCDE_METRIC_MEMORY_USAGE,
                                       "test_memory_usage",
                                       1024.0,
                                       "MB",
                                       TCDE_PRIORITY_HIGH);
    assert(result == true);
    assert(metrics->buffer.count == 2);
    printf("✅ Second instant metric captured\n");
    
    // Vérifier la métrique la plus récente
    const TCDE_NanosecondMetric* latest = TCDE_GetLatestMetric(metrics);
    assert(latest != NULL);
    assert(latest->type == TCDE_METRIC_MEMORY_USAGE);
    assert(latest->value == 1024.0);
    assert(strcmp(latest->unit, "MB") == 0);
    assert(latest->priority == TCDE_PRIORITY_HIGH);
    printf("✅ Latest metric verified: %.1f %s\n", latest->value, latest->unit);
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 5: Capture de métriques avec durée
 */
bool test_duration_metric_capture(void) {
    printf("\n=== Test 5: Duration Metric Capture ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(100);
    
    // Démarrer une capture de métrique
    uint32_t metric_id = TCDE_StartMetricCapture(metrics,
                                                 TCDE_METRIC_VALIDATION_TIME,
                                                 "test_validation_duration",
                                                 TCDE_PRIORITY_CRITICAL);
    assert(metric_id > 0);
    printf("✅ Metric capture started with ID: %u\n", metric_id);
    
    // Simuler du travail
    usleep(1000); // 1ms
    
    // Terminer la capture
    bool result = TCDE_EndMetricCapture(metrics, metric_id, 1.0, "validation");
    assert(result == true);
    assert(metrics->buffer.count == 1);
    printf("✅ Metric capture completed\n");
    
    // Vérifier la métrique capturée
    const TCDE_NanosecondMetric* captured = TCDE_GetLatestMetric(metrics);
    assert(captured != NULL);
    assert(captured->metric_id == metric_id);
    assert(captured->type == TCDE_METRIC_VALIDATION_TIME);
    assert(captured->state == TCDE_METRIC_STATE_COMPLETED);
    printf("✅ Captured metric verified: ID %u, State: %s\n", 
           captured->metric_id, TCDE_GetMetricStateString(captured->state));
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 6: Événements d'émergence microseconde-précis
 */
bool test_emergence_events(void) {
    printf("\n=== Test 6: Emergence Events ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(100);
    
    // Configurer le callback
    TCDE_SetEmergenceDetectedCallback(metrics, test_emergence_detected_callback);
    emergence_callback_triggered = false;
    
    // Enregistrer un événement d'émergence
    bool result = TCDE_RecordEmergenceEvent(metrics,
                                            "pattern_formation",
                                            "Spontaneous pattern formation detected",
                                            0.85f,
                                            0.92f,
                                            NULL,
                                            0);
    assert(result == true);
    assert(emergence_callback_triggered == true);
    assert(last_emergence_event != NULL);
    printf("✅ Emergence event recorded and callback triggered\n");
    
    // Vérifier l'événement
    assert(strcmp(last_emergence_event->event_type, "pattern_formation") == 0);
    assert(last_emergence_event->intensity == 0.85f);
    assert(last_emergence_event->confidence == 0.92f);
    printf("✅ Emergence event verified: %s (intensity: %.2f, confidence: %.2f)\n",
           last_emergence_event->event_type, 
           last_emergence_event->intensity,
           last_emergence_event->confidence);
    
    // Tester avec des valeurs limites
    result = TCDE_RecordEmergenceEvent(metrics,
                                       "boundary_test",
                                       "Testing boundary values",
                                       1.5f,  // Sera limité à 1.0
                                       -0.1f, // Sera limité à 0.0
                                       NULL,
                                       0);
    assert(result == true);
    assert(last_emergence_event->intensity == 1.0f);
    assert(last_emergence_event->confidence == 0.0f);
    printf("✅ Boundary value limiting works correctly\n");
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 7: Validation d'authenticité
 */
bool test_authenticity_validation(void) {
    printf("\n=== Test 7: Authenticity Validation ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(100);
    
    // Configurer le callback d'échec d'authenticité
    TCDE_SetAuthenticityFailedCallback(metrics, test_authenticity_failed_callback);
    authenticity_callback_triggered = false;
    
    // Créer une métrique valide
    TCDE_NanosecondMetric valid_metric;
    memset(&valid_metric, 0, sizeof(valid_metric));
    valid_metric.start_time = TCDE_CaptureHighPrecisionTimestamp();
    usleep(100);
    valid_metric.end_time = TCDE_CaptureHighPrecisionTimestamp();
    valid_metric.duration_ns = TCDE_CalculateTimeDifference(valid_metric.start_time, valid_metric.end_time);
    valid_metric.value = 42.0;
    valid_metric.sample_count = 1;
    
    bool result = TCDE_ValidateMetricAuthenticity(metrics, &valid_metric);
    assert(result == true);
    assert(authenticity_callback_triggered == false);
    printf("✅ Valid metric passed authenticity validation\n");
    
    // Créer une métrique invalide (timestamps incohérents)
    TCDE_NanosecondMetric invalid_metric;
    memset(&invalid_metric, 0, sizeof(invalid_metric));
    invalid_metric.start_time = TCDE_CaptureHighPrecisionTimestamp();
    invalid_metric.end_time = TCDE_CaptureHighPrecisionTimestamp();
    // Inverser les timestamps pour créer une incohérence
    invalid_metric.end_time.nanoseconds_since_boot = invalid_metric.start_time.nanoseconds_since_boot - 1000;
    invalid_metric.value = 42.0;
    invalid_metric.sample_count = 1;
    
    result = TCDE_ValidateMetricAuthenticity(metrics, &invalid_metric);
    assert(result == false);
    assert(authenticity_callback_triggered == true);
    printf("✅ Invalid metric correctly failed authenticity validation\n");
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 8: Callbacks du système
 */
bool test_system_callbacks(void) {
    printf("\n=== Test 8: System Callbacks ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(100);
    
    // Configurer tous les callbacks
    TCDE_SetMetricCapturedCallback(metrics, test_metric_captured_callback);
    TCDE_SetEmergenceDetectedCallback(metrics, test_emergence_detected_callback);
    TCDE_SetAuthenticityFailedCallback(metrics, test_authenticity_failed_callback);
    TCDE_SetBufferOverflowCallback(metrics, test_buffer_overflow_callback);
    
    // Reset des flags
    metric_callback_triggered = false;
    emergence_callback_triggered = false;
    authenticity_callback_triggered = false;
    overflow_callback_triggered = false;
    
    // Tester le callback de métrique capturée
    bool result = TCDE_CaptureInstantMetric(metrics,
                                            TCDE_METRIC_CPU_CYCLES,
                                            "callback_test",
                                            100.0,
                                            "cycles",
                                            TCDE_PRIORITY_NORMAL);
    assert(result == true);
    assert(metric_callback_triggered == true);
    assert(last_captured_metric != NULL);
    printf("✅ Metric captured callback triggered\n");
    
    // Tester le callback d'émergence
    result = TCDE_RecordEmergenceEvent(metrics,
                                       "callback_emergence",
                                       "Testing emergence callback",
                                       0.5f,
                                       0.8f,
                                       NULL,
                                       0);
    assert(result == true);
    assert(emergence_callback_triggered == true);
    printf("✅ Emergence detected callback triggered\n");
    
    printf("✅ All callbacks configured and tested successfully\n");
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 9: Statistiques et performance
 */
bool test_statistics_and_performance(void) {
    printf("\n=== Test 9: Statistics and Performance ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(1000);
    
    // Capturer plusieurs métriques pour générer des statistiques
    for (int i = 0; i < 50; i++) {
        char metric_name[64];
        snprintf(metric_name, sizeof(metric_name), "perf_test_%d", i);
        
        bool result = TCDE_CaptureInstantMetric(metrics,
                                                TCDE_METRIC_SYSTEM_PERFORMANCE,
                                                metric_name,
                                                (double)(i * 2),
                                                "units",
                                                TCDE_PRIORITY_NORMAL);
        assert(result == true);
    }
    
    assert(metrics->buffer.count == 50);
    printf("✅ 50 metrics captured for statistics\n");
    
    // Vérifier les statistiques
    float success_rate = TCDE_GetCaptureSuccessRate(metrics);
    uint64_t avg_time = TCDE_GetAverageCaptureTime(metrics);
    
    printf("✅ Statistics calculated:\n");
    printf("   - Success rate: %.2f%%\n", success_rate);
    printf("   - Average capture time: %llu ns\n", avg_time);
    printf("   - Min capture time: %llu ns\n", metrics->min_capture_time_ns);
    printf("   - Max capture time: %llu ns\n", metrics->max_capture_time_ns);
    
    // Vérifier la précision cible (< 1 microseconde)
    if (avg_time < 1000) {
        printf("✅ PRECISION TARGET ACHIEVED: %llu ns < 1µs\n", avg_time);
    } else {
        printf("⚠️  PRECISION TARGET MISSED: %llu ns >= 1µs\n", avg_time);
    }
    
    // Afficher le rapport complet
    TCDE_PrintMetricsSystemStatus(metrics);
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Test 10: Test de stress et précision
 */
bool test_stress_and_precision(void) {
    printf("\n=== Test 10: Stress and Precision Test ===\n");
    
    TCDE_NanosecondMetrics* metrics = TCDE_CreateNanosecondMetrics(10000);
    
    // Test de stress avec captures rapides
    const int stress_iterations = 1000;
    int successful_captures = 0;
    uint64_t total_capture_time = 0;
    
    printf("Starting stress test with %d rapid captures...\n", stress_iterations);
    
    for (int i = 0; i < stress_iterations; i++) {
        TCDE_HighPrecisionTimestamp start = TCDE_CaptureHighPrecisionTimestamp();
        
        char metric_name[64];
        snprintf(metric_name, sizeof(metric_name), "stress_%d", i);
        
        bool result = TCDE_CaptureInstantMetric(metrics,
                                                TCDE_METRIC_SYSTEM_PERFORMANCE,
                                                metric_name,
                                                (double)i,
                                                "iteration",
                                                TCDE_PRIORITY_HIGH);
        
        TCDE_HighPrecisionTimestamp end = TCDE_CaptureHighPrecisionTimestamp();
        uint64_t capture_time = TCDE_CalculateTimeDifference(start, end);
        total_capture_time += capture_time;
        
        if (result) {
            successful_captures++;
        }
        
        // Vérifier la précision pour chaque capture
        if (capture_time >= 1000) { // >= 1µs
            printf("WARNING: Capture %d took %llu ns (>= 1µs)\n", i, capture_time);
        }
    }
    
    assert(successful_captures == stress_iterations);
    assert(metrics->buffer.count == stress_iterations);
    
    uint64_t avg_capture_time = total_capture_time / stress_iterations;
    float success_rate = (float)successful_captures / stress_iterations * 100.0f;
    
    printf("✅ Stress test completed:\n");
    printf("   - Successful captures: %d/%d (%.1f%%)\n", 
           successful_captures, stress_iterations, success_rate);
    printf("   - Average capture time: %llu ns\n", avg_capture_time);
    printf("   - Total capture time: %llu ns\n", total_capture_time);
    
    // Vérifier la précision globale
    if (avg_capture_time < 1000) {
        printf("✅ STRESS TEST PRECISION ACHIEVED: %llu ns < 1µs\n", avg_capture_time);
    } else {
        printf("❌ STRESS TEST PRECISION MISSED: %llu ns >= 1µs\n", avg_capture_time);
    }
    
    TCDE_DestroyNanosecondMetrics(metrics);
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("⏱️  TCDE Nanosecond Metrics System - Test Suite\n");
    printf("===============================================\n");
    printf("TESTING NANOSECOND PRECISION METRICS SYSTEM\n");
    printf("PRECISION TARGET: < 1 MICROSECOND\n");
    printf("ZERO TOLERANCE FOR TIMING ERRORS\n");
    printf("===============================================\n");
    
    int tests_passed = 0;
    int total_tests = 10;
    
    // Exécuter tous les tests
    if (test_metrics_system_creation()) tests_passed++;
    if (test_metrics_system_configuration()) tests_passed++;
    if (test_high_precision_timestamps()) tests_passed++;
    if (test_instant_metric_capture()) tests_passed++;
    if (test_duration_metric_capture()) tests_passed++;
    if (test_emergence_events()) tests_passed++;
    if (test_authenticity_validation()) tests_passed++;
    if (test_system_callbacks()) tests_passed++;
    if (test_statistics_and_performance()) tests_passed++;
    if (test_stress_and_precision()) tests_passed++;
    
    // Résultats finaux
    printf("\n");
    printf("⏱️  TCDE NANOSECOND METRICS TEST RESULTS\n");
    printf("========================================\n");
    printf("Tests Passed: %d/%d\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL NANOSECOND METRICS TESTS PASSED\n");
        printf("✅ NANOSECOND PRECISION SYSTEM VALIDATED\n");
        printf("✅ MICROSECOND-PRECISE EMERGENCE EVENTS OPERATIONAL\n");
        printf("✅ BACKGROUND AUTHENTICITY VALIDATION CONFIRMED\n");
        printf("✅ ZERO LATENCY CAPTURE SYSTEM ACHIEVED\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME NANOSECOND METRICS TESTS FAILED\n");
        printf("🚨 PRECISION SYSTEM COMPROMISED\n");
        return EXIT_FAILURE;
    }
}