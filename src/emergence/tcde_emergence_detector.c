#include "tcde_emergence_detector.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

// Mutex global pour thread-safety
static pthread_mutex_t g_detector_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Capture un timestamp haute précision
 */
static TCDE_DetectionTimestamp capture_detection_timestamp(void) {
    TCDE_DetectionTimestamp ts;
    clock_gettime(CLOCK_MONOTONIC, &ts.timestamp);
    ts.nanoseconds = ts.timestamp.tv_sec * 1000000000ULL + ts.timestamp.tv_nsec;
    
    static uint32_t sequence_counter = 0;
    ts.sequence = __sync_fetch_and_add(&sequence_counter, 1);
    
    return ts;
}

/**
 * Calcule la différence entre deux timestamps (en nanosecondes)
 */
static uint64_t calculate_time_difference(TCDE_DetectionTimestamp start, TCDE_DetectionTimestamp end) {
    return end.nanoseconds - start.nanoseconds;
}

/**
 * Crée un détecteur d'émergence
 */
TCDE_EmergenceDetector* TCDE_CreateEmergenceDetector(uint32_t buffer_capacity) {
    if (buffer_capacity == 0) return NULL;
    
    TCDE_EmergenceDetector* detector = (TCDE_EmergenceDetector*)malloc(sizeof(TCDE_EmergenceDetector));
    if (!detector) return NULL;
    
    memset(detector, 0, sizeof(TCDE_EmergenceDetector));
    
    // Allouer le buffer d'événements
    detector->events = (TCDE_EmergenceEvent*)calloc(buffer_capacity, sizeof(TCDE_EmergenceEvent));
    if (!detector->events) {
        free(detector);
        return NULL;
    }
    
    detector->active = true;
    detector->real_time_mode = false;
    detector->adaptive_thresholds = false;
    detector->auto_classification = false;
    detector->buffer_capacity = buffer_capacity;
    detector->buffer_head = 0;
    detector->buffer_count = 0;
    detector->next_event_id = 1;
    detector->start_time = capture_detection_timestamp();
    
    // Initialiser les seuils adaptatifs par défaut
    for (int i = 0; i < 10; i++) {
        detector->thresholds[i].current_threshold = 0.5;
        detector->thresholds[i].min_threshold = 0.1;
        detector->thresholds[i].max_threshold = 0.9;
        detector->thresholds[i].adaptation_rate = 0.1;
        detector->thresholds[i].adaptation_count = 0;
        detector->thresholds[i].sensitivity = 0.5;
        detector->thresholds[i].auto_adapt = false;
    }
    
    // Initialiser les statistiques
    detector->stats.min_detection_latency_ns = UINT64_MAX;
    detector->stats.max_detection_latency_ns = 0;
    
    printf("TCDE_EMERGENCE_DETECTOR: System ACTIVATED\n");
    printf("TCDE_EMERGENCE_DETECTOR: Buffer capacity = %u events\n", buffer_capacity);
    printf("TCDE_EMERGENCE_DETECTOR: Detection target < 1 millisecond\n");
    
    return detector;
}

/**
 * Détruit le détecteur
 */
void TCDE_DestroyEmergenceDetector(TCDE_EmergenceDetector* detector) {
    if (!detector) return;
    
    printf("TCDE_EMERGENCE_DETECTOR: System DEACTIVATED\n");
    printf("TCDE_EMERGENCE_DETECTOR: Total detections: %llu\n", detector->stats.total_detections);
    printf("TCDE_EMERGENCE_DETECTOR: Confirmed detections: %llu\n", detector->stats.confirmed_detections);
    printf("TCDE_EMERGENCE_DETECTOR: Detection accuracy: %.2f%%\n", detector->stats.detection_accuracy);
    printf("TCDE_EMERGENCE_DETECTOR: Average latency: %llu ns\n", detector->stats.avg_detection_latency_ns);
    
    if (detector->events) {
        free(detector->events);
    }
    free(detector);
}

/**
 * Active le mode temps réel
 */
bool TCDE_EnableRealTimeMode(TCDE_EmergenceDetector* detector) {
    if (!detector) return false;
    detector->real_time_mode = true;
    printf("TCDE_EMERGENCE_DETECTOR: Real-time mode ENABLED\n");
    return true;
}

/**
 * Active les seuils adaptatifs
 */
bool TCDE_EnableAdaptiveThresholds(TCDE_EmergenceDetector* detector) {
    if (!detector) return false;
    detector->adaptive_thresholds = true;
    for (int i = 0; i < 10; i++) {
        detector->thresholds[i].auto_adapt = true;
    }
    printf("TCDE_EMERGENCE_DETECTOR: Adaptive thresholds ENABLED\n");
    return true;
}

/**
 * Active la classification automatique
 */
bool TCDE_EnableAutoClassification(TCDE_EmergenceDetector* detector) {
    if (!detector) return false;
    detector->auto_classification = true;
    printf("TCDE_EMERGENCE_DETECTOR: Auto-classification ENABLED\n");
    return true;
}

/**
 * Définit un seuil pour un type d'émergence
 */
bool TCDE_SetThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double threshold) {
    if (!detector || type < 1 || type > 10) return false;
    if (threshold < 0.0 || threshold > 1.0) return false;
    
    int index = type - 1;
    detector->thresholds[index].current_threshold = threshold;
    printf("TCDE_EMERGENCE_DETECTOR: Threshold for type %d set to %.2f\n", type, threshold);
    return true;
}

/**
 * Définit la sensibilité pour un type d'émergence
 */
bool TCDE_SetSensitivity(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double sensitivity) {
    if (!detector || type < 1 || type > 10) return false;
    if (sensitivity < 0.0 || sensitivity > 1.0) return false;
    
    int index = type - 1;
    detector->thresholds[index].sensitivity = sensitivity;
    printf("TCDE_EMERGENCE_DETECTOR: Sensitivity for type %d set to %.2f\n", type, sensitivity);
    return true;
}

/**
 * Détecte une émergence (OPTIMISÉ POUR < 1ms)
 */
bool TCDE_DetectEmergence(TCDE_EmergenceDetector* detector,
                          TCDE_EmergenceType type,
                          double signal_value,
                          const char* description) {
    if (!detector || !description) return false;
    if (type < 1 || type > 10) return false;
    
    // Timestamp de début (pour mesurer la latence)
    TCDE_DetectionTimestamp start_time = capture_detection_timestamp();
    
    // Vérifier le seuil
    int threshold_index = type - 1;
    double threshold = detector->thresholds[threshold_index].current_threshold;
    double sensitivity = detector->thresholds[threshold_index].sensitivity;
    
    // Calcul rapide de l'intensité
    double intensity = signal_value * sensitivity;
    
    // Détection uniquement si au-dessus du seuil
    if (intensity < threshold) {
        return false; // Pas d'émergence détectée
    }
    
    // Préparer l'événement AVANT le lock
    TCDE_EmergenceEvent event;
    memset(&event, 0, sizeof(event));
    
    event.type = type;
    event.detection_time = start_time;
    event.state = TCDE_DETECTION_EMERGENCE_DETECTED;
    event.intensity = intensity;
    event.confidence = (intensity - threshold) / (1.0 - threshold); // Confiance basée sur dépassement
    event.magnitude = signal_value;
    event.signal_strength = signal_value;
    event.noise_level = 0.1; // Approximation
    event.signal_to_noise_ratio = signal_value / 0.1;
    event.is_authentic = true;
    
    // Déterminer la criticité
    if (intensity > 0.9) event.criticality = TCDE_CRITICALITY_EMERGENCY;
    else if (intensity > 0.75) event.criticality = TCDE_CRITICALITY_CRITICAL;
    else if (intensity > 0.6) event.criticality = TCDE_CRITICALITY_HIGH;
    else if (intensity > 0.4) event.criticality = TCDE_CRITICALITY_MODERATE;
    else event.criticality = TCDE_CRITICALITY_LOW;
    
    event.requires_immediate_action = (event.criticality >= TCDE_CRITICALITY_CRITICAL);
    
    // Copie rapide de la description
    size_t desc_len = 0;
    while (description[desc_len] && desc_len < sizeof(event.description) - 1) {
        event.description[desc_len] = description[desc_len];
        desc_len++;
    }
    event.description[desc_len] = '\0';
    
    // Classification automatique si activée
    if (detector->auto_classification) {
        const char* classification = TCDE_ClassifyEmergence(detector, &event);
        size_t class_len = 0;
        while (classification[class_len] && class_len < sizeof(event.classification) - 1) {
            event.classification[class_len] = classification[class_len];
            class_len++;
        }
        event.classification[class_len] = '\0';
        event.classification_confidence = event.confidence;
    }
    
    // Section critique MINIMALE
    pthread_mutex_lock(&g_detector_mutex);
    
    event.event_id = detector->next_event_id++;
    
    // Ajouter au buffer circulaire
    uint32_t index = detector->buffer_head;
    detector->events[index] = event;
    detector->buffer_head = (detector->buffer_head + 1) % detector->buffer_capacity;
    if (detector->buffer_count < detector->buffer_capacity) {
        detector->buffer_count++;
    }
    
    // Mettre à jour les statistiques
    detector->stats.total_detections++;
    
    pthread_mutex_unlock(&g_detector_mutex);
    
    // Calculer la latence de détection
    TCDE_DetectionTimestamp end_time = capture_detection_timestamp();
    uint64_t latency = calculate_time_difference(start_time, end_time);
    
    // Mettre à jour les statistiques de latence
    pthread_mutex_lock(&g_detector_mutex);
    detector->events[index].detection_latency_ns = latency;
    
    if (latency < detector->stats.min_detection_latency_ns) {
        detector->stats.min_detection_latency_ns = latency;
    }
    if (latency > detector->stats.max_detection_latency_ns) {
        detector->stats.max_detection_latency_ns = latency;
    }
    
    // Calcul incrémental de la moyenne
    uint64_t total = detector->stats.total_detections;
    detector->stats.avg_detection_latency_ns = 
        (detector->stats.avg_detection_latency_ns * (total - 1) + latency) / total;
    
    pthread_mutex_unlock(&g_detector_mutex);
    
    // Callbacks en dehors de la section critique
    if (detector->emergence_detected_callback) {
        detector->emergence_detected_callback(&detector->events[index]);
    }
    
    if (event.requires_immediate_action && detector->critical_event_callback) {
        detector->critical_event_callback(&detector->events[index]);
    }
    
    // Adaptation automatique du seuil si activée
    if (detector->adaptive_thresholds && detector->thresholds[threshold_index].auto_adapt) {
        TCDE_AdaptThreshold(detector, type, intensity);
    }
    
    return true;
}

/**
 * Confirme une émergence
 */
bool TCDE_ConfirmEmergence(TCDE_EmergenceDetector* detector, uint32_t event_id) {
    if (!detector || event_id == 0) return false;
    
    pthread_mutex_lock(&g_detector_mutex);
    
    // Chercher l'événement
    for (uint32_t i = 0; i < detector->buffer_count; i++) {
        uint32_t index = (detector->buffer_head - 1 - i + detector->buffer_capacity) % detector->buffer_capacity;
        if (detector->events[index].event_id == event_id) {
            detector->events[index].state = TCDE_DETECTION_EMERGENCE_CONFIRMED;
            detector->events[index].confirmation_time = capture_detection_timestamp();
            detector->stats.confirmed_detections++;
            
            // Recalculer la précision
            detector->stats.detection_accuracy = 
                (double)detector->stats.confirmed_detections / detector->stats.total_detections * 100.0;
            
            pthread_mutex_unlock(&g_detector_mutex);
            return true;
        }
    }
    
    pthread_mutex_unlock(&g_detector_mutex);
    return false;
}

/**
 * Récupère le dernier événement
 */
const TCDE_EmergenceEvent* TCDE_GetLatestEvent(TCDE_EmergenceDetector* detector) {
    if (!detector || detector->buffer_count == 0) return NULL;
    
    uint32_t index = (detector->buffer_head - 1 + detector->buffer_capacity) % detector->buffer_capacity;
    return &detector->events[index];
}

/**
 * Récupère un événement par ID
 */
const TCDE_EmergenceEvent* TCDE_GetEventById(TCDE_EmergenceDetector* detector, uint32_t event_id) {
    if (!detector || event_id == 0) return NULL;
    
    for (uint32_t i = 0; i < detector->buffer_count; i++) {
        uint32_t index = (detector->buffer_head - 1 - i + detector->buffer_capacity) % detector->buffer_capacity;
        if (detector->events[index].event_id == event_id) {
            return &detector->events[index];
        }
    }
    
    return NULL;
}

/**
 * Classifie automatiquement une émergence
 */
const char* TCDE_ClassifyEmergence(TCDE_EmergenceDetector* detector, const TCDE_EmergenceEvent* event) {
    if (!detector || !event) return "UNKNOWN";
    
    // Classification basée sur le type et l'intensité
    switch (event->type) {
        case TCDE_EMERGENCE_DIMENSIONAL_EXPANSION:
            if (event->intensity > 0.8) return "MAJOR_EXPANSION";
            else if (event->intensity > 0.5) return "MODERATE_EXPANSION";
            else return "MINOR_EXPANSION";
            
        case TCDE_EMERGENCE_CONSCIOUSNESS_SHIFT:
            if (event->intensity > 0.8) return "CONSCIOUSNESS_BREAKTHROUGH";
            else if (event->intensity > 0.5) return "CONSCIOUSNESS_EVOLUTION";
            else return "CONSCIOUSNESS_FLUCTUATION";
            
        case TCDE_EMERGENCE_PATTERN_FORMATION:
            if (event->signal_to_noise_ratio > 10.0) return "STRONG_PATTERN";
            else if (event->signal_to_noise_ratio > 5.0) return "MODERATE_PATTERN";
            else return "WEAK_PATTERN";
            
        case TCDE_EMERGENCE_PHASE_TRANSITION:
            return "PHASE_TRANSITION_DETECTED";
            
        case TCDE_EMERGENCE_SELF_ORGANIZATION:
            return "SELF_ORGANIZATION_ACTIVE";
            
        case TCDE_EMERGENCE_COMPLEXITY_INCREASE:
            if (event->magnitude > 0.7) return "HIGH_COMPLEXITY";
            else return "MODERATE_COMPLEXITY";
            
        case TCDE_EMERGENCE_COHERENCE_PEAK:
            return "COHERENCE_MAXIMUM";
            
        case TCDE_EMERGENCE_BIFURCATION:
            return "BIFURCATION_POINT";
            
        case TCDE_EMERGENCE_CRITICAL_POINT:
            return "CRITICAL_THRESHOLD_REACHED";
            
        case TCDE_EMERGENCE_SPONTANEOUS_ORDER:
            return "SPONTANEOUS_ORDER_EMERGED";
            
        default:
            return "UNCLASSIFIED";
    }
}

/**
 * Calcule l'intensité
 */
double TCDE_CalculateIntensity(const TCDE_EmergenceEvent* event) {
    if (!event) return 0.0;
    return event->intensity;
}

/**
 * Calcule la confiance
 */
double TCDE_CalculateConfidence(const TCDE_EmergenceEvent* event) {
    if (!event) return 0.0;
    return event->confidence;
}

/**
 * Adapte un seuil
 */
bool TCDE_AdaptThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double feedback) {
    if (!detector || type < 1 || type > 10) return false;
    
    int index = type - 1;
    TCDE_AdaptiveThreshold* threshold = &detector->thresholds[index];
    
    // Adaptation basée sur le feedback
    double adjustment = (feedback - threshold->current_threshold) * threshold->adaptation_rate;
    double new_threshold = threshold->current_threshold + adjustment;
    
    // Limiter aux bornes
    if (new_threshold < threshold->min_threshold) new_threshold = threshold->min_threshold;
    if (new_threshold > threshold->max_threshold) new_threshold = threshold->max_threshold;
    
    threshold->current_threshold = new_threshold;
    threshold->adaptation_count++;
    
    // Callback si défini
    if (detector->threshold_adapted_callback) {
        detector->threshold_adapted_callback(type, new_threshold);
    }
    
    return true;
}

/**
 * Récupère le seuil actuel
 */
double TCDE_GetCurrentThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type) {
    if (!detector || type < 1 || type > 10) return 0.0;
    return detector->thresholds[type - 1].current_threshold;
}

/**
 * Récupère les statistiques
 */
TCDE_DetectionStatistics TCDE_GetDetectionStatistics(TCDE_EmergenceDetector* detector) {
    TCDE_DetectionStatistics stats = {0};
    if (detector) {
        stats = detector->stats;
    }
    return stats;
}

/**
 * Récupère la précision de détection
 */
double TCDE_GetDetectionAccuracy(TCDE_EmergenceDetector* detector) {
    if (!detector || detector->stats.total_detections == 0) return 0.0;
    return detector->stats.detection_accuracy;
}

/**
 * Récupère la latence moyenne
 */
uint64_t TCDE_GetAverageDetectionLatency(TCDE_EmergenceDetector* detector) {
    if (!detector) return 0;
    return detector->stats.avg_detection_latency_ns;
}

/**
 * Définit le callback de détection
 */
void TCDE_SetEmergenceDetectedCallback(TCDE_EmergenceDetector* detector,
                                       void (*callback)(const TCDE_EmergenceEvent*)) {
    if (detector) {
        detector->emergence_detected_callback = callback;
    }
}

/**
 * Définit le callback d'événement critique
 */
void TCDE_SetCriticalEventCallback(TCDE_EmergenceDetector* detector,
                                   void (*callback)(const TCDE_EmergenceEvent*)) {
    if (detector) {
        detector->critical_event_callback = callback;
    }
}

/**
 * Définit le callback d'adaptation de seuil
 */
void TCDE_SetThresholdAdaptedCallback(TCDE_EmergenceDetector* detector,
                                     void (*callback)(TCDE_EmergenceType, double)) {
    if (detector) {
        detector->threshold_adapted_callback = callback;
    }
}

/**
 * Convertit un type d'émergence en chaîne
 */
const char* TCDE_GetEmergenceTypeString(TCDE_EmergenceType type) {
    switch (type) {
        case TCDE_EMERGENCE_DIMENSIONAL_EXPANSION: return "DIMENSIONAL_EXPANSION";
        case TCDE_EMERGENCE_CONSCIOUSNESS_SHIFT: return "CONSCIOUSNESS_SHIFT";
        case TCDE_EMERGENCE_PATTERN_FORMATION: return "PATTERN_FORMATION";
        case TCDE_EMERGENCE_PHASE_TRANSITION: return "PHASE_TRANSITION";
        case TCDE_EMERGENCE_SELF_ORGANIZATION: return "SELF_ORGANIZATION";
        case TCDE_EMERGENCE_COMPLEXITY_INCREASE: return "COMPLEXITY_INCREASE";
        case TCDE_EMERGENCE_COHERENCE_PEAK: return "COHERENCE_PEAK";
        case TCDE_EMERGENCE_BIFURCATION: return "BIFURCATION";
        case TCDE_EMERGENCE_CRITICAL_POINT: return "CRITICAL_POINT";
        case TCDE_EMERGENCE_SPONTANEOUS_ORDER: return "SPONTANEOUS_ORDER";
        default: return "UNKNOWN";
    }
}

/**
 * Convertit un niveau de criticité en chaîne
 */
const char* TCDE_GetCriticalityLevelString(TCDE_CriticalityLevel level) {
    switch (level) {
        case TCDE_CRITICALITY_LOW: return "LOW";
        case TCDE_CRITICALITY_MODERATE: return "MODERATE";
        case TCDE_CRITICALITY_HIGH: return "HIGH";
        case TCDE_CRITICALITY_CRITICAL: return "CRITICAL";
        case TCDE_CRITICALITY_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

/**
 * Convertit un état de détection en chaîne
 */
const char* TCDE_GetDetectionStateString(TCDE_DetectionState state) {
    switch (state) {
        case TCDE_DETECTION_MONITORING: return "MONITORING";
        case TCDE_DETECTION_THRESHOLD_APPROACHING: return "THRESHOLD_APPROACHING";
        case TCDE_DETECTION_EMERGENCE_DETECTED: return "EMERGENCE_DETECTED";
        case TCDE_DETECTION_EMERGENCE_CONFIRMED: return "EMERGENCE_CONFIRMED";
        case TCDE_DETECTION_EMERGENCE_COMPLETED: return "EMERGENCE_COMPLETED";
        default: return "UNKNOWN";
    }
}

/**
 * Affiche l'état du détecteur
 */
void TCDE_PrintDetectorStatus(TCDE_EmergenceDetector* detector) {
    if (!detector) return;
    
    printf("\nTCDE Emergence Detector Status\n");
    printf("==============================\n");
    printf("Active: %s\n", detector->active ? "YES" : "NO");
    printf("Real-time Mode: %s\n", detector->real_time_mode ? "YES" : "NO");
    printf("Adaptive Thresholds: %s\n", detector->adaptive_thresholds ? "YES" : "NO");
    printf("Auto-classification: %s\n", detector->auto_classification ? "YES" : "NO");
    printf("Buffer Capacity: %u events\n", detector->buffer_capacity);
    printf("Buffer Usage: %u/%u (%.1f%%)\n", 
           detector->buffer_count, detector->buffer_capacity,
           (float)detector->buffer_count / detector->buffer_capacity * 100.0f);
    printf("Total Detections: %llu\n", detector->stats.total_detections);
    printf("Confirmed Detections: %llu\n", detector->stats.confirmed_detections);
    printf("Detection Accuracy: %.2f%%\n", detector->stats.detection_accuracy);
    printf("Min Detection Latency: %llu ns\n", detector->stats.min_detection_latency_ns);
    printf("Max Detection Latency: %llu ns\n", detector->stats.max_detection_latency_ns);
    printf("Avg Detection Latency: %llu ns\n", detector->stats.avg_detection_latency_ns);
    printf("==============================\n");
    
    if (detector->stats.avg_detection_latency_ns < 1000000) {
        printf("✅ DETECTION TARGET ACHIEVED: %llu ns < 1ms\n", detector->stats.avg_detection_latency_ns);
    } else {
        printf("⚠️  DETECTION TARGET MISSED: %llu ns >= 1ms\n", detector->stats.avg_detection_latency_ns);
    }
}
