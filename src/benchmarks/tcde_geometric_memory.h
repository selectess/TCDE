#ifndef TCDE_GEOMETRIC_MEMORY_H
#define TCDE_GEOMETRIC_MEMORY_H

#include "../core/tcde_core.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Geometric Memory Benchmark System
 * 
 * Système de benchmarking de mémoire adaptative GÉOMÉTRIQUE.
 * Basé sur la vraie logique TCDE : champs continus, centres RBF, manifolds.
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - AUCUN placeholder ou valeur hardcodée
 * - AUCUNE simulation de compression bytes
 * - Calculs géométriques réels uniquement
 * - Mesures empiriques authentiques
 * 
 * CRITÈRE SUCCÈS: 
 * - Expansion 100 → 1000+ centres RBF
 * - Efficacité énergétique > 99% (conservation dans le manifold)
 */

#define TCDE_INITIAL_CENTERS 100
#define TCDE_TARGET_CENTERS 1000
#define TCDE_MAX_CENTERS 10000
#define TCDE_ENERGY_EFFICIENCY_TARGET 99.0

// Métriques de mémoire géométrique
typedef struct {
    uint32_t initial_centers;
    uint32_t current_centers;
    uint32_t max_centers_reached;
    
    double expansion_rate;
    uint64_t total_expansions;
    
    // Énergie et efficacité (vraie logique TCDE)
    double initial_energy;
    double current_energy;
    double energy_efficiency;          // Conservation énergétique %
    double energy_density;             // Énergie par centre
    
    // Complexité géométrique
    double spatial_complexity;         // Complexité spatiale du champ
    double topological_efficiency;     // Efficacité topologique
    double manifold_coherence;         // Cohérence du manifold
    
    // Adaptation quantique
    double quantum_adaptation_level;
    uint64_t adaptation_events;
    
    uint64_t total_cycles;
} TCDE_GeometricMemoryMetrics;

// Score de mémoire géométrique
typedef struct {
    double overall_score;
    double expansion_achievement;
    double energy_efficiency_achievement;
    double geometric_quality;
    double adaptation_intelligence;
    
    bool target_reached;
    bool efficiency_valid;
    bool is_authentic;
    double authenticity_score;
} TCDE_GeometricMemoryScore;

// Système de benchmark
typedef struct {
    bool active;
    bool real_time_monitoring;
    
    TCDE_Field* field;                 // Champ TCDE 6D
    
    TCDE_GeometricMemoryMetrics metrics;
    TCDE_GeometricMemoryScore score;
    
    double expansion_threshold;
    double energy_target;
    
    struct timespec start_time;
    uint64_t total_benchmark_time_ns;
    
    void (*center_added_callback)(uint32_t center_id);
    void (*efficiency_achieved_callback)(double efficiency);
} TCDE_GeometricMemoryBenchmark;

// Fonctions principales
TCDE_GeometricMemoryBenchmark* TCDE_CreateGeometricMemoryBenchmark(void);
void TCDE_DestroyGeometricMemoryBenchmark(TCDE_GeometricMemoryBenchmark* benchmark);

bool TCDE_InitializeGeometricMemory(TCDE_GeometricMemoryBenchmark* benchmark, uint32_t initial_count);
bool TCDE_AddGeometricCenter(TCDE_GeometricMemoryBenchmark* benchmark);
bool TCDE_OptimizeGeometricRepresentation(TCDE_GeometricMemoryBenchmark* benchmark);
bool TCDE_RunGeometricAdaptationCycle(TCDE_GeometricMemoryBenchmark* benchmark);
bool TCDE_RunCompleteGeometricMemoryBenchmark(TCDE_GeometricMemoryBenchmark* benchmark);

double TCDE_MeasureExpansionRate(TCDE_GeometricMemoryBenchmark* benchmark);
double TCDE_MeasureEnergyEfficiency(TCDE_GeometricMemoryBenchmark* benchmark);
double TCDE_MeasureGeometricQuality(TCDE_GeometricMemoryBenchmark* benchmark);

TCDE_GeometricMemoryScore TCDE_CalculateGeometricMemoryScore(TCDE_GeometricMemoryBenchmark* benchmark);
bool TCDE_IsGeometricTargetReached(TCDE_GeometricMemoryBenchmark* benchmark);

void TCDE_PrintGeometricMemoryScore(const TCDE_GeometricMemoryScore* score);
void TCDE_PrintGeometricMemorySummary(TCDE_GeometricMemoryBenchmark* benchmark);

#endif // TCDE_GEOMETRIC_MEMORY_H
