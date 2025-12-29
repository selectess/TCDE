#ifndef TCDE_EMERGENCE_DETECTOR_H
#define TCDE_EMERGENCE_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Real-Time Emergence Detector
 * 
 * Système de détection d'émergence temps réel avec seuils adaptatifs.
 * Détection instantanée < 1ms après événement d'émergence.
 * 
 * CRITÈRE SUCCÈS: Détection < 1 milliseconde
 */

// Types d'émergence détectables
typedef enum {
    TCDE_EMERGENCE_DIMENSIONAL_EXPANSION = 1,    // Expansion dimensionnelle
    TCDE_EMERGENCE_CONSCIOUSNESS_SHIFT = 2,      // Changement de conscience
    TCDE_EMERGENCE_PATTERN_FORMATION = 3,        // Formation de patterns
    TCDE_EMERGENCE_PHASE_TRANSITION = 4,         // Transition de phase
    TCDE_EMERGENCE_SELF_ORGANIZATION = 5,        // Auto-organisation
    TCDE_EMERGENCE_COMPLEXITY_INCREASE = 6,      // Augmentation de complexité
    TCDE_EMERGENCE_COHERENCE_PEAK = 7,           // Pic de cohérence
    TCDE_EMERGENCE_BIFURCATION = 8,              // Bifurcation
    TCDE_EMERGENCE_CRITICAL_POINT = 9,           // Point critique
    TCDE_EMERGENCE_SPONTANEOUS_ORDER = 10        // Ordre spontané
} TCDE_EmergenceType;

// Niveaux de criticité
typedef enum {
    TCDE_CRITICALITY_LOW = 1,
    TCDE_CRITICALITY_MODERATE = 2,
    TCDE_CRITICALITY_HIGH = 3,
    TCDE_CRITICALITY_CRITICAL = 4,
    TCDE_CRITICALITY_EMERGENCY = 5
} TCDE_CriticalityLevel;

// État de détection
typedef enum {
    TCDE_DETECTION_MONITORING = 1,
    TCDE_DETECTION_THRESHOLD_APPROACHING = 2,
    TCDE_DETECTION_EMERGENCE_DETECTED = 3,
    TCDE_DETECTION_EMERGENCE_CONFIRMED = 4,
    TCDE_DETECTION_EMERGENCE_COMPLETED = 5
} TCDE_DetectionState;

// Timestamp haute précision pour détection
typedef struct {
    struct timespec timestamp;
    uint64_t nanoseconds;
    uint32_t sequence;
} TCDE_DetectionTimestamp;

// Seuil adaptatif
typedef struct {
    double current_threshold;        // Seuil actuel
    double min_threshold;            // Seuil minimum
    double max_threshold;            // Seuil maximum
    double adaptation_rate;          // Taux d'adaptation (0.0-1.0)
    uint64_t adaptation_count;       // Nombre d'adaptations
    double sensitivity;              // Sensibilité (0.0-1.0)
    bool auto_adapt;                 // Adaptation automatique activée
} TCDE_AdaptiveThreshold;

// Événement d'émergence détecté
typedef struct {
    uint32_t event_id;                       // ID unique
    TCDE_EmergenceType type;                 // Type d'émergence
    TCDE_CriticalityLevel criticality;       // Niveau de criticité
    TCDE_DetectionState state;               // État de détection
    TCDE_DetectionTimestamp detection_time;  // Temps de détection
    TCDE_DetectionTimestamp confirmation_time; // Temps de confirmation
    uint64_t detection_latency_ns;           // Latence de détection (ns)
    
    char description[256];                   // Description
    double intensity;                        // Intensité (0.0-1.0)
    double confidence;                       // Confiance (0.0-1.0)
    double magnitude;                        // Magnitude
    double duration_estimate_ms;             // Durée estimée (ms)
    
    // Métriques de détection
    double signal_strength;                  // Force du signal
    double noise_level;                      // Niveau de bruit
    double signal_to_noise_ratio;            // Rapport signal/bruit
    
    // Classification automatique
    char classification[128];                // Classification
    double classification_confidence;        // Confiance classification
    
    bool is_authentic;                       // Validation authenticité
    bool requires_immediate_action;          // Action immédiate requise
} TCDE_EmergenceEvent;

// Statistiques de détection
typedef struct {
    uint64_t total_detections;               // Total détections
    uint64_t confirmed_detections;           // Détections confirmées
    uint64_t false_positives;                // Faux positifs
    uint64_t false_negatives;                // Faux négatifs
    double detection_accuracy;               // Précision détection
    uint64_t min_detection_latency_ns;       // Latence min (ns)
    uint64_t max_detection_latency_ns;       // Latence max (ns)
    uint64_t avg_detection_latency_ns;       // Latence moyenne (ns)
} TCDE_DetectionStatistics;

// Détecteur d'émergence temps réel
typedef struct {
    bool active;                             // Système actif
    bool real_time_mode;                     // Mode temps réel
    bool adaptive_thresholds;                // Seuils adaptatifs
    bool auto_classification;                // Classification auto
    
    // Seuils adaptatifs par type
    TCDE_AdaptiveThreshold thresholds[10];   // Un par type d'émergence
    
    // Buffer d'événements
    TCDE_EmergenceEvent* events;             // Buffer circulaire
    uint32_t buffer_capacity;                // Capacité
    uint32_t buffer_head;                    // Tête
    uint32_t buffer_count;                   // Nombre d'événements
    
    // Statistiques
    TCDE_DetectionStatistics stats;
    
    // IDs
    uint32_t next_event_id;
    
    // Temps de démarrage
    TCDE_DetectionTimestamp start_time;
    
    // Callbacks
    void (*emergence_detected_callback)(const TCDE_EmergenceEvent* event);
    void (*critical_event_callback)(const TCDE_EmergenceEvent* event);
    void (*threshold_adapted_callback)(TCDE_EmergenceType type, double new_threshold);
} TCDE_EmergenceDetector;

// Fonctions principales
TCDE_EmergenceDetector* TCDE_CreateEmergenceDetector(uint32_t buffer_capacity);
void TCDE_DestroyEmergenceDetector(TCDE_EmergenceDetector* detector);

// Configuration
bool TCDE_EnableRealTimeMode(TCDE_EmergenceDetector* detector);
bool TCDE_EnableAdaptiveThresholds(TCDE_EmergenceDetector* detector);
bool TCDE_EnableAutoClassification(TCDE_EmergenceDetector* detector);
bool TCDE_SetThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double threshold);
bool TCDE_SetSensitivity(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double sensitivity);

// Détection temps réel
bool TCDE_DetectEmergence(TCDE_EmergenceDetector* detector,
                          TCDE_EmergenceType type,
                          double signal_value,
                          const char* description);
bool TCDE_ConfirmEmergence(TCDE_EmergenceDetector* detector, uint32_t event_id);
const TCDE_EmergenceEvent* TCDE_GetLatestEvent(TCDE_EmergenceDetector* detector);
const TCDE_EmergenceEvent* TCDE_GetEventById(TCDE_EmergenceDetector* detector, uint32_t event_id);

// Classification automatique
const char* TCDE_ClassifyEmergence(TCDE_EmergenceDetector* detector, const TCDE_EmergenceEvent* event);
double TCDE_CalculateIntensity(const TCDE_EmergenceEvent* event);
double TCDE_CalculateConfidence(const TCDE_EmergenceEvent* event);

// Seuils adaptatifs
bool TCDE_AdaptThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type, double feedback);
double TCDE_GetCurrentThreshold(TCDE_EmergenceDetector* detector, TCDE_EmergenceType type);

// Statistiques
TCDE_DetectionStatistics TCDE_GetDetectionStatistics(TCDE_EmergenceDetector* detector);
double TCDE_GetDetectionAccuracy(TCDE_EmergenceDetector* detector);
uint64_t TCDE_GetAverageDetectionLatency(TCDE_EmergenceDetector* detector);

// Callbacks
void TCDE_SetEmergenceDetectedCallback(TCDE_EmergenceDetector* detector,
                                       void (*callback)(const TCDE_EmergenceEvent*));
void TCDE_SetCriticalEventCallback(TCDE_EmergenceDetector* detector,
                                   void (*callback)(const TCDE_EmergenceEvent*));
void TCDE_SetThresholdAdaptedCallback(TCDE_EmergenceDetector* detector,
                                     void (*callback)(TCDE_EmergenceType, double));

// Utilitaires
const char* TCDE_GetEmergenceTypeString(TCDE_EmergenceType type);
const char* TCDE_GetCriticalityLevelString(TCDE_CriticalityLevel level);
const char* TCDE_GetDetectionStateString(TCDE_DetectionState state);
void TCDE_PrintDetectorStatus(TCDE_EmergenceDetector* detector);

#endif // TCDE_EMERGENCE_DETECTOR_H
