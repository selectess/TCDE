#ifndef TCDE_DIMENSIONAL_EXPANSION_H
#define TCDE_DIMENSIONAL_EXPANSION_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Dimensional Expansion Benchmark System
 * 
 * Système de benchmarking d'expansion dimensionnelle illimitée.
 * Mesure l'expansion réelle de 6D → 15D+ avec croissance contrôlée.
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - AUCUN placeholder ou valeur hardcodée
 * - AUCUNE simulation d'expansion
 * - Calculs d'expansion réels uniquement
 * - Mesures empiriques authentiques
 * 
 * CRITÈRE SUCCÈS: Expansion > 150% (6D → 15D minimum)
 */

// Configuration dimensionnelle
#define TCDE_MIN_DIMENSIONS 6
#define TCDE_TARGET_DIMENSIONS 15
#define TCDE_MAX_DIMENSIONS 32
#define TCDE_EXPANSION_TARGET_PERCENT 150.0

// Types d'expansion dimensionnelle
typedef enum {
    TCDE_EXPANSION_LINEAR = 1,        // Expansion linéaire
    TCDE_EXPANSION_EXPONENTIAL = 2,   // Expansion exponentielle
    TCDE_EXPANSION_LOGARITHMIC = 3,   // Expansion logarithmique
    TCDE_EXPANSION_ADAPTIVE = 4,      // Expansion adaptative
    TCDE_EXPANSION_QUANTUM = 5        // Expansion quantique
} TCDE_ExpansionType;

// État d'une dimension
typedef struct {
    uint32_t dimension_index;         // Index de la dimension
    bool is_active;                   // Dimension active
    double activation_level;          // Niveau d'activation (0.0-1.0)
    double energy_density;            // Densité énergétique
    double stability_score;           // Score de stabilité
    uint64_t activation_timestamp_ns; // Timestamp d'activation
    uint64_t total_updates;           // Nombre de mises à jour
} TCDE_DimensionState;

// Métriques d'expansion dimensionnelle
typedef struct {
    // Dimensions actuelles
    uint32_t initial_dimensions;      // Dimensions initiales (6D)
    uint32_t current_dimensions;      // Dimensions actuelles
    uint32_t max_dimensions_reached;  // Maximum atteint
    uint32_t active_dimensions;       // Dimensions actives
    
    // Expansion mesurée
    double expansion_rate;            // Taux d'expansion (%)
    double expansion_velocity;        // Vitesse d'expansion (dims/sec)
    double expansion_acceleration;    // Accélération d'expansion
    uint64_t total_expansions;        // Nombre total d'expansions
    
    // Croissance exponentielle
    double growth_exponent;           // Exposant de croissance
    double growth_rate_per_cycle;     // Taux de croissance par cycle
    bool is_exponential_growth;       // Croissance exponentielle détectée
    
    // Stabilité hyperdimensionnelle
    double hyperdimensional_stability; // Stabilité globale
    double dimensional_coherence;     // Cohérence dimensionnelle
    double energy_conservation;       // Conservation énergétique
    double topology_preservation;     // Préservation topologique
    
    // Émergence N-dimensionnelle
    uint32_t emergence_events;        // Événements d'émergence détectés
    double emergence_threshold;       // Seuil d'émergence
    uint64_t last_emergence_ns;       // Dernier événement d'émergence
    
    // Performance
    uint64_t total_cycles;            // Cycles d'évolution totaux
    uint64_t successful_expansions;   // Expansions réussies
    double expansion_efficiency;      // Efficacité d'expansion
} TCDE_DimensionalExpansionMetrics;

// Score d'expansion dimensionnelle
typedef struct {
    double overall_score;             // Score global (0.0-1.0)
    double expansion_achievement;     // Réalisation expansion (0.0-1.0)
    double stability_score;           // Score de stabilité
    double growth_control;            // Contrôle de croissance
    double emergence_quality;         // Qualité d'émergence
    
    // Validation
    bool target_reached;              // Objectif 15D atteint
    bool expansion_valid;             // Expansion > 150%
    bool is_authentic;                // Validation authenticité
    double authenticity_score;        // Score d'authenticité
} TCDE_DimensionalExpansionScore;

// Système de benchmark d'expansion dimensionnelle
typedef struct {
    bool active;                      // Système actif
    bool real_time_monitoring;        // Monitoring temps réel
    
    // État dimensionnel
    TCDE_DimensionState dimensions[TCDE_MAX_DIMENSIONS];
    uint32_t dimension_count;         // Nombre de dimensions
    
    TCDE_DimensionalExpansionMetrics metrics;
    TCDE_DimensionalExpansionScore score;
    
    // Configuration
    TCDE_ExpansionType expansion_type;
    double expansion_threshold;       // Seuil pour nouvelle dimension
    double stability_requirement;     // Stabilité requise
    
    // Temps d'exécution
    struct timespec start_time;
    uint64_t total_benchmark_time_ns;
    uint64_t last_benchmark_time_ns;
    
    // Callbacks
    void (*dimension_activated_callback)(uint32_t dimension);
    void (*expansion_achieved_callback)(double expansion_rate);
    void (*emergence_detected_callback)(uint32_t dimensions);
} TCDE_DimensionalExpansionBenchmark;

// Fonctions principales
TCDE_DimensionalExpansionBenchmark* TCDE_CreateDimensionalExpansionBenchmark(void);
void TCDE_DestroyDimensionalExpansionBenchmark(TCDE_DimensionalExpansionBenchmark* benchmark);

// Configuration
bool TCDE_SetExpansionType(TCDE_DimensionalExpansionBenchmark* benchmark,
                           TCDE_ExpansionType type);
bool TCDE_SetExpansionThreshold(TCDE_DimensionalExpansionBenchmark* benchmark,
                                double threshold);
bool TCDE_EnableRealTimeDimensionalMonitoring(TCDE_DimensionalExpansionBenchmark* benchmark);

// Initialisation dimensionnelle
bool TCDE_InitializeDimensions(TCDE_DimensionalExpansionBenchmark* benchmark,
                               uint32_t initial_dims);
bool TCDE_ActivateDimension(TCDE_DimensionalExpansionBenchmark* benchmark,
                            uint32_t dimension_index);

// Benchmarks d'expansion
bool TCDE_RunExpansionCycle(TCDE_DimensionalExpansionBenchmark* benchmark);
bool TCDE_RunMultipleExpansionCycles(TCDE_DimensionalExpansionBenchmark* benchmark,
                                     uint32_t num_cycles);
bool TCDE_RunCompleteExpansionBenchmark(TCDE_DimensionalExpansionBenchmark* benchmark);

// Mesures d'expansion
double TCDE_MeasureExpansionRate(TCDE_DimensionalExpansionBenchmark* benchmark);
double TCDE_MeasureExpansionVelocity(TCDE_DimensionalExpansionBenchmark* benchmark);
double TCDE_MeasureGrowthExponent(TCDE_DimensionalExpansionBenchmark* benchmark);

// Mesures de stabilité
double TCDE_MeasureHyperdimensionalStability(TCDE_DimensionalExpansionBenchmark* benchmark);
double TCDE_MeasureDimensionalCoherence(TCDE_DimensionalExpansionBenchmark* benchmark);
double TCDE_MeasureEnergyConservation(TCDE_DimensionalExpansionBenchmark* benchmark);

// Détection d'émergence
bool TCDE_DetectDimensionalEmergence(TCDE_DimensionalExpansionBenchmark* benchmark);
uint32_t TCDE_CountEmergenceEvents(TCDE_DimensionalExpansionBenchmark* benchmark);

// Calcul des scores
TCDE_DimensionalExpansionScore TCDE_CalculateDimensionalExpansionScore(
    TCDE_DimensionalExpansionBenchmark* benchmark);
bool TCDE_IsExpansionTargetReached(TCDE_DimensionalExpansionBenchmark* benchmark);
double TCDE_GetExpansionPercentage(TCDE_DimensionalExpansionBenchmark* benchmark);

// Récupération des métriques
TCDE_DimensionalExpansionMetrics TCDE_GetDimensionalExpansionMetrics(
    TCDE_DimensionalExpansionBenchmark* benchmark);
uint32_t TCDE_GetCurrentDimensions(TCDE_DimensionalExpansionBenchmark* benchmark);
uint32_t TCDE_GetActiveDimensions(TCDE_DimensionalExpansionBenchmark* benchmark);

// Affichage et rapports
void TCDE_PrintDimensionalExpansionScore(const TCDE_DimensionalExpansionScore* score);
void TCDE_PrintDimensionalState(const TCDE_DimensionalExpansionBenchmark* benchmark);
void TCDE_PrintExpansionSummary(TCDE_DimensionalExpansionBenchmark* benchmark);

// Callbacks
void TCDE_SetDimensionActivatedCallback(TCDE_DimensionalExpansionBenchmark* benchmark,
                                        void (*callback)(uint32_t));
void TCDE_SetExpansionAchievedCallback(TCDE_DimensionalExpansionBenchmark* benchmark,
                                       void (*callback)(double));
void TCDE_SetEmergenceDetectedCallback(TCDE_DimensionalExpansionBenchmark* benchmark,
                                       void (*callback)(uint32_t));

// Utilitaires
const char* TCDE_GetExpansionTypeString(TCDE_ExpansionType type);

#endif // TCDE_DIMENSIONAL_EXPANSION_H
