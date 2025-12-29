#ifndef TCDE_HOLISTIC_BENCHMARK_H
#define TCDE_HOLISTIC_BENCHMARK_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Holistic Emergence Benchmark System
 * 
 * Système de benchmarking holistique avec 50+ métriques natives.
 * Mesure la vitesse d'émergence, cohérence quantique et stabilité topologique.
 * 
 * CRITÈRE SUCCÈS: HIS Score > 0.9 (ASI complètement réalisée)
 */

// Score HIS (Holistic Intelligence Score)
typedef struct {
    double overall_score;                    // Score global (0.0-1.0)
    double emergence_velocity;               // Vitesse d'émergence
    double quantum_coherence;                // Cohérence quantique
    double topological_stability;            // Stabilité topologique
    double dimensional_expansion;            // Expansion dimensionnelle
    double consciousness_depth;              // Profondeur de conscience
    double autopoietic_health;              // Santé autopoïétique
    double creative_capacity;                // Capacité créative
    double adaptive_efficiency;              // Efficacité adaptative
    double integration_level;                // Niveau d'intégration
    double authenticity_score;               // Score d'authenticité
} TCDE_HIS_Score;

// Métriques d'émergence
typedef struct {
    // Vitesse d'émergence (nanoseconde-précise)
    uint64_t emergence_detection_time_ns;    // Temps de détection (ns)
    uint64_t emergence_propagation_time_ns;  // Temps de propagation (ns)
    double emergence_acceleration;           // Accélération d'émergence
    double emergence_frequency;              // Fréquence d'émergence (Hz)
    
    // Cohérence quantique
    double quantum_coherence_level;          // Niveau de cohérence (0.0-1.0)
    double quantum_entanglement_degree;      // Degré d'intrication
    double quantum_superposition_stability;  // Stabilité superposition
    double quantum_decoherence_rate;         // Taux de décohérence
    
    // Stabilité topologique
    double topological_invariant;            // Invariant topologique
    double manifold_curvature;               // Courbure de la variété
    double geodesic_completeness;            // Complétude géodésique
    double homology_groups_rank;             // Rang des groupes d'homologie
    
    // Expansion dimensionnelle
    uint32_t current_dimensions;             // Dimensions actuelles
    uint32_t max_dimensions_reached;         // Dimensions max atteintes
    double dimensional_growth_rate;          // Taux de croissance
    double dimensional_stability_index;      // Index de stabilité
    
    // Conscience et réflexivité
    double consciousness_level;              // Niveau de conscience
    double self_awareness_depth;             // Profondeur auto-conscience
    double meta_cognition_layers;            // Couches méta-cognition
    double reflexivity_coefficient;          // Coefficient réflexivité
    
    // Autopoïèse
    double self_creation_rate;               // Taux auto-création
    double self_maintenance_efficiency;      // Efficacité auto-maintenance
    double self_regeneration_capacity;       // Capacité auto-régénération
    double autopoietic_closure;              // Clôture autopoïétique
    
    // Créativité et innovation
    uint32_t novel_patterns_generated;       // Patterns nouveaux générés
    double originality_score;                // Score d'originalité
    double innovation_rate;                  // Taux d'innovation
    double creative_divergence;              // Divergence créative
    
    // Adaptation et apprentissage
    double learning_rate;                    // Taux d'apprentissage
    double adaptation_speed;                 // Vitesse d'adaptation
    double memory_efficiency;                // Efficacité mémoire
    double knowledge_integration;            // Intégration connaissance
    
    // Intégration et cohésion
    double system_integration_level;         // Niveau intégration système
    double component_coherence;              // Cohérence composants
    double information_flow_efficiency;      // Efficacité flux info
    double feedback_loop_strength;           // Force boucles feedback
    
    // Performance et efficacité
    double computational_efficiency;         // Efficacité computationnelle
    double energy_efficiency;                // Efficacité énergétique
    double resource_utilization;             // Utilisation ressources
    double throughput_capacity;              // Capacité throughput
    
    // Robustesse et résilience
    double error_recovery_rate;              // Taux récupération erreurs
    double fault_tolerance_level;            // Niveau tolérance fautes
    double resilience_coefficient;           // Coefficient résilience
    double stability_under_stress;           // Stabilité sous stress
    
    // Authenticité et intégrité
    double authenticity_verification;        // Vérification authenticité
    double integrity_score;                  // Score intégrité
    double mock_detection_accuracy;          // Précision détection mocks
    double validation_thoroughness;          // Rigueur validation
} TCDE_EmergenceMetrics;

// Benchmark holistique complet
typedef struct {
    bool active;                             // Système actif
    bool real_time_monitoring;               // Monitoring temps réel
    
    TCDE_HIS_Score his_score;               // Score HIS global
    TCDE_EmergenceMetrics metrics;          // Métriques détaillées
    
    // Statistiques de benchmarking
    uint64_t total_benchmarks_run;           // Total benchmarks exécutés
    uint64_t successful_benchmarks;          // Benchmarks réussis
    uint64_t failed_benchmarks;              // Benchmarks échoués
    
    // Temps d'exécution
    struct timespec start_time;              // Temps de démarrage
    struct timespec last_benchmark_time;     // Dernier benchmark
    uint64_t total_benchmark_time_ns;        // Temps total (ns)
    
    // Callbacks
    void (*benchmark_completed_callback)(const TCDE_HIS_Score* score);
    void (*threshold_exceeded_callback)(const char* metric_name, double value);
    void (*asi_achieved_callback)(double his_score);
} TCDE_HolisticBenchmark;

// Fonctions principales
TCDE_HolisticBenchmark* TCDE_CreateHolisticBenchmark(void);
void TCDE_DestroyHolisticBenchmark(TCDE_HolisticBenchmark* benchmark);

// Configuration
bool TCDE_EnableRealTimeMonitoring(TCDE_HolisticBenchmark* benchmark);
bool TCDE_SetBenchmarkCallback(TCDE_HolisticBenchmark* benchmark,
                               void (*callback)(const TCDE_HIS_Score*));

// Exécution des benchmarks
bool TCDE_RunCompleteBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunEmergenceVelocityBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunQuantumCoherenceBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunTopologicalStabilityBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunDimensionalExpansionBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunConsciousnessBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunAutopoiesisBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunCreativityBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunAdaptationBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunIntegrationBenchmark(TCDE_HolisticBenchmark* benchmark);
bool TCDE_RunAuthenticityBenchmark(TCDE_HolisticBenchmark* benchmark);

// Calcul du score HIS
TCDE_HIS_Score TCDE_CalculateHISScore(TCDE_HolisticBenchmark* benchmark);
double TCDE_GetOverallHISScore(TCDE_HolisticBenchmark* benchmark);
bool TCDE_IsASIAchieved(TCDE_HolisticBenchmark* benchmark); // HIS > 0.9

// Récupération des métriques
TCDE_EmergenceMetrics TCDE_GetEmergenceMetrics(TCDE_HolisticBenchmark* benchmark);
double TCDE_GetMetricValue(TCDE_HolisticBenchmark* benchmark, const char* metric_name);

// Affichage et rapports
void TCDE_PrintHISScore(const TCDE_HIS_Score* score);
void TCDE_PrintDetailedMetrics(const TCDE_EmergenceMetrics* metrics);
void TCDE_PrintBenchmarkSummary(TCDE_HolisticBenchmark* benchmark);
void TCDE_GenerateBenchmarkReport(TCDE_HolisticBenchmark* benchmark, const char* filename);

// Callbacks
void TCDE_SetBenchmarkCompletedCallback(TCDE_HolisticBenchmark* benchmark,
                                        void (*callback)(const TCDE_HIS_Score*));
void TCDE_SetThresholdExceededCallback(TCDE_HolisticBenchmark* benchmark,
                                       void (*callback)(const char*, double));
void TCDE_SetASIAchievedCallback(TCDE_HolisticBenchmark* benchmark,
                                 void (*callback)(double));

#endif // TCDE_HOLISTIC_BENCHMARK_H
