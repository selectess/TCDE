#ifndef TCDE_MEMORY_BENCHMARK_H
#define TCDE_MEMORY_BENCHMARK_H

#include "../core/tcde_adaptive_memory.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Memory Benchmark - Utilise l'API TCDE existante
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - Utilise TCDE_AdaptiveMemoryManager réel
 * - Mesures basées sur l'API TCDE authentique
 * - Pas de simulation
 * 
 * CRITÈRE SUCCÈS:
 * - Expansion 100 → 1000+ traces
 * - Efficacité mémoire > 99%
 */

#define TCDE_INITIAL_MEMORY_TRACES 100
#define TCDE_TARGET_MEMORY_TRACES 1000
#define TCDE_MEMORY_EFFICIENCY_TARGET 99.0

// Métriques de benchmark
typedef struct {
    uint32_t initial_traces;
    uint32_t current_traces;
    uint32_t max_traces_reached;
    
    double expansion_rate;
    double memory_efficiency;
    double compression_ratio;
    
    uint64_t total_cycles;
    uint64_t compression_events;
    uint64_t consolidation_events;
} TCDE_MemoryBenchmarkMetrics;

// Score de benchmark
typedef struct {
    double overall_score;
    double expansion_achievement;
    double efficiency_achievement;
    double quality_score;
    
    bool target_reached;
    bool efficiency_valid;
    bool is_authentic;
    double authenticity_score;
} TCDE_MemoryBenchmarkScore;

// Système de benchmark
typedef struct {
    bool active;
    bool real_time_monitoring;
    
    TCDE_AdaptiveMemoryManager* memory_manager;  // Vrai gestionnaire TCDE
    
    TCDE_MemoryBenchmarkMetrics metrics;
    TCDE_MemoryBenchmarkScore score;
    
    struct timespec start_time;
    uint64_t total_benchmark_time_ns;
} TCDE_MemoryBenchmark;

// API
TCDE_MemoryBenchmark* TCDE_CreateMemoryBenchmark(void);
void TCDE_DestroyMemoryBenchmark(TCDE_MemoryBenchmark* benchmark);

bool TCDE_InitializeMemoryBenchmark(TCDE_MemoryBenchmark* benchmark, uint32_t initial_count);
bool TCDE_RunMemoryBenchmarkCycle(TCDE_MemoryBenchmark* benchmark);
bool TCDE_RunCompleteMemoryBenchmark(TCDE_MemoryBenchmark* benchmark);

double TCDE_MeasureMemoryExpansion(TCDE_MemoryBenchmark* benchmark);
double TCDE_MeasureMemoryEfficiency(TCDE_MemoryBenchmark* benchmark);

TCDE_MemoryBenchmarkScore TCDE_CalculateMemoryBenchmarkScore(TCDE_MemoryBenchmark* benchmark);
bool TCDE_IsMemoryTargetReached(TCDE_MemoryBenchmark* benchmark);

void TCDE_PrintMemoryBenchmarkScore(const TCDE_MemoryBenchmarkScore* score);
void TCDE_PrintMemoryBenchmarkSummary(TCDE_MemoryBenchmark* benchmark);

#endif // TCDE_MEMORY_BENCHMARK_H
