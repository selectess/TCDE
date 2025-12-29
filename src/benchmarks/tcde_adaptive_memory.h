#ifndef TCDE_ADAPTIVE_MEMORY_H
#define TCDE_ADAPTIVE_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Adaptive Memory Benchmark System
 * 
 * Système de benchmarking de mémoire adaptative infinie.
 * Mesure l'expansion réelle de 100 → 1000+ traces avec compression >99%.
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - AUCUN placeholder ou valeur hardcodée
 * - AUCUNE simulation de compression
 * - Calculs de compression réels uniquement
 * - Mesures empiriques authentiques
 * 
 * CRITÈRE SUCCÈS: Expansion > 1000% avec efficacité > 99%
 */

#define TCDE_INITIAL_TRACES 100
#define TCDE_TARGET_TRACES 1000
#define TCDE_MAX_TRACES 10000
#define TCDE_EXPANSION_TARGET_PERCENT 1000.0
#define TCDE_COMPRESSION_TARGET_PERCENT 99.0

// Trace mémoire
typedef struct {
    uint32_t trace_id;
    bool is_active;
    uint64_t raw_size_bytes;
    uint64_t compressed_size_bytes;
    double compression_ratio;
    double importance_score;
    uint64_t access_count;
    uint64_t last_access_ns;
    uint64_t creation_ns;
} TCDE_MemoryTrace;

// Métriques de mémoire adaptative
typedef struct {
    uint32_t initial_traces;
    uint32_t current_traces;
    uint32_t max_traces_reached;
    uint32_t active_traces;
    
    double expansion_rate;
    double expansion_velocity;
    uint64_t total_expansions;
    
    uint64_t total_raw_size_bytes;
    uint64_t total_compressed_size_bytes;
    double overall_compression_ratio;
    double compression_efficiency;
    
    double quantum_adaptation_level;
    double adaptive_intelligence_score;
    uint64_t adaptation_events;
    
    uint64_t total_cycles;
    uint64_t successful_compressions;
} TCDE_AdaptiveMemoryMetrics;

// Score de mémoire adaptative
typedef struct {
    double overall_score;
    double expansion_achievement;
    double compression_achievement;
    double adaptation_quality;
    double intelligence_level;
    
    bool target_reached;
    bool compression_valid;
    bool is_authentic;
    double authenticity_score;
} TCDE_AdaptiveMemoryScore;

// Système de benchmark
typedef struct {
    bool active;
    bool real_time_monitoring;
    
    TCDE_MemoryTrace* traces;
    uint32_t trace_capacity;
    uint32_t trace_count;
    
    TCDE_AdaptiveMemoryMetrics metrics;
    TCDE_AdaptiveMemoryScore score;
    
    double expansion_threshold;
    double compression_target;
    
    struct timespec start_time;
    uint64_t total_benchmark_time_ns;
    
    void (*trace_added_callback)(uint32_t trace_id);
    void (*compression_achieved_callback)(double efficiency);
    void (*expansion_achieved_callback)(double expansion_rate);
} TCDE_AdaptiveMemoryBenchmark;

// Fonctions principales
TCDE_AdaptiveMemoryBenchmark* TCDE_CreateAdaptiveMemoryBenchmark(void);
void TCDE_DestroyAdaptiveMemoryBenchmark(TCDE_AdaptiveMemoryBenchmark* benchmark);

bool TCDE_InitializeMemoryTraces(TCDE_AdaptiveMemoryBenchmark* benchmark, uint32_t initial_count);
bool TCDE_AddMemoryTrace(TCDE_AdaptiveMemoryBenchmark* benchmark, uint64_t raw_size);
bool TCDE_CompressTrace(TCDE_AdaptiveMemoryBenchmark* benchmark, uint32_t trace_id);
bool TCDE_RunAdaptationCycle(TCDE_AdaptiveMemoryBenchmark* benchmark);
bool TCDE_RunCompleteMemoryBenchmark(TCDE_AdaptiveMemoryBenchmark* benchmark);

double TCDE_MeasureExpansionRate(TCDE_AdaptiveMemoryBenchmark* benchmark);
double TCDE_MeasureCompressionEfficiency(TCDE_AdaptiveMemoryBenchmark* benchmark);
double TCDE_MeasureAdaptiveIntelligence(TCDE_AdaptiveMemoryBenchmark* benchmark);

TCDE_AdaptiveMemoryScore TCDE_CalculateAdaptiveMemoryScore(TCDE_AdaptiveMemoryBenchmark* benchmark);
bool TCDE_IsMemoryTargetReached(TCDE_AdaptiveMemoryBenchmark* benchmark);

void TCDE_PrintAdaptiveMemoryScore(const TCDE_AdaptiveMemoryScore* score);
void TCDE_PrintMemorySummary(TCDE_AdaptiveMemoryBenchmark* benchmark);

void TCDE_SetTraceAddedCallback(TCDE_AdaptiveMemoryBenchmark* benchmark, void (*callback)(uint32_t));
void TCDE_SetCompressionAchievedCallback(TCDE_AdaptiveMemoryBenchmark* benchmark, void (*callback)(double));

#endif // TCDE_ADAPTIVE_MEMORY_H
