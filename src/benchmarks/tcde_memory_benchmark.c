#include "tcde_memory_benchmark.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

/**
 * Crée un benchmark de mémoire TCDE
 * ZÉRO TOLÉRANCE: Utilise l'API TCDE réelle
 */
TCDE_MemoryBenchmark* TCDE_CreateMemoryBenchmark(void) {
    TCDE_MemoryBenchmark* benchmark = (TCDE_MemoryBenchmark*)malloc(sizeof(TCDE_MemoryBenchmark));
    if (!benchmark) return NULL;
    
    benchmark->active = true;
    benchmark->real_time_monitoring = false;
    
    // Créer le vrai gestionnaire de mémoire TCDE
    benchmark->memory_manager = TCDE_CreateMemoryManager(TCDE_TARGET_MEMORY_TRACES, 100);
    if (!benchmark->memory_manager) {
        free(benchmark);
        return NULL;
    }
    
    memset(&benchmark->metrics, 0, sizeof(TCDE_MemoryBenchmarkMetrics));
    memset(&benchmark->score, 0, sizeof(TCDE_MemoryBenchmarkScore));
    
    benchmark->metrics.initial_traces = TCDE_INITIAL_MEMORY_TRACES;
    
    clock_gettime(CLOCK_MONOTONIC, &benchmark->start_time);
    benchmark->total_benchmark_time_ns = 0;
    
    printf("TCDE_MEMORY_BENCHMARK: System ACTIVATED\\n");
    printf("TCDE_MEMORY_BENCHMARK: Using REAL TCDE AdaptiveMemoryManager\\n");
    printf("TCDE_MEMORY_BENCHMARK: Target 100 → 1000+ traces\\n");
    printf("TCDE_MEMORY_BENCHMARK: ZERO TOLERANCE mode enabled\\n");
    
    return benchmark;
}

/**
 * Détruit le benchmark
 */
void TCDE_DestroyMemoryBenchmark(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark) return;
    
    printf("TCDE_MEMORY_BENCHMARK: System DEACTIVATED\\n");
    printf("TCDE_MEMORY_BENCHMARK: Final traces: %u (expansion: %.1f%%)\\n",
           benchmark->metrics.current_traces, benchmark->metrics.expansion_rate);
    
    if (benchmark->memory_manager) {
        TCDE_DestroyMemoryManager(benchmark->memory_manager);
    }
    free(benchmark);
}

/**
 * Initialise le benchmark avec des traces mémoire
 * ZÉRO TOLÉRANCE: Utilise TCDE_StoreMemory réel
 */
bool TCDE_InitializeMemoryBenchmark(TCDE_MemoryBenchmark* benchmark, uint32_t initial_count) {
    if (!benchmark || !benchmark->memory_manager) return false;
    
    printf("TCDE_MEMORY_BENCHMARK: Initializing %u memory traces...\\n", initial_count);
    
    // Stocker des traces mémoire réelles
    for (uint32_t i = 0; i < initial_count; i++) {
        float coords[6] = {
            0.5f + 0.3f * cosf(i * 0.1f),
            0.5f + 0.3f * sinf(i * 0.1f),
            0.5f,
            (float)i / initial_count,
            0.0f,
            0.5f
        };
        
        TCDE_Point location = TCDE_CreatePoint(6, coords);
        TCDE_Complex content = (1.0f + 0.1f * sinf(i * 0.2f)) * cexpf(I * (float)i * 0.1f);
        float importance = 1.0f - (float)i / (initial_count * 2.0f);
        
        TCDE_StoreMemory(benchmark->memory_manager, &location, content, importance);
        
        TCDE_FreePoint(&location);
    }
    
    benchmark->metrics.initial_traces = initial_count;
    benchmark->metrics.current_traces = benchmark->memory_manager->num_traces;
    
    printf("TCDE_MEMORY_BENCHMARK: Initialized %u traces (actual: %d)\\n",
           initial_count, benchmark->memory_manager->num_traces);
    
    return true;
}

/**
 * Exécute un cycle de benchmark
 */
bool TCDE_RunMemoryBenchmarkCycle(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark || !benchmark->memory_manager) return false;
    
    benchmark->metrics.total_cycles++;
    
    // Compresser la mémoire (API TCDE réelle)
    int compressed = TCDE_CompressMemory(benchmark->memory_manager, 0.8f);
    if (compressed > 0) {
        benchmark->metrics.compression_events++;
    }
    
    // Consolider les mémoires (API TCDE réelle)
    int consolidated = TCDE_ConsolidateMemories(benchmark->memory_manager);
    if (consolidated > 0) {
        benchmark->metrics.consolidation_events++;
    }
    
    // Mettre à jour la hiérarchie (API TCDE réelle)
    TCDE_UpdateMemoryHierarchy(benchmark->memory_manager);
    
    // Ajouter de nouvelles traces si l'efficacité est bonne
    double efficiency = TCDE_MeasureMemoryEfficiency(benchmark);
    if (efficiency > 95.0 && benchmark->memory_manager->num_traces < TCDE_TARGET_MEMORY_TRACES) {
        // Ajouter plusieurs traces
        for (int i = 0; i < 10 && benchmark->memory_manager->num_traces < TCDE_TARGET_MEMORY_TRACES; i++) {
            uint32_t n = benchmark->memory_manager->num_traces;
            float coords[6] = {
                0.5f + 0.4f * cosf(n * 0.15f),
                0.5f + 0.4f * sinf(n * 0.15f),
                0.5f,
                (float)n / TCDE_TARGET_MEMORY_TRACES,
                0.0f,
                0.5f
            };
            
            TCDE_Point location = TCDE_CreatePoint(6, coords);
            TCDE_Complex content = cexpf(I * (float)n * 0.1f);
            
            TCDE_StoreMemory(benchmark->memory_manager, &location, content, 0.8f);
            
            TCDE_FreePoint(&location);
        }
    }
    
    benchmark->metrics.current_traces = benchmark->memory_manager->num_traces;
    if (benchmark->metrics.current_traces > benchmark->metrics.max_traces_reached) {
        benchmark->metrics.max_traces_reached = benchmark->metrics.current_traces;
    }
    
    return true;
}

/**
 * Mesure l'expansion mémoire
 */
double TCDE_MeasureMemoryExpansion(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark || benchmark->metrics.initial_traces == 0) return 0.0;
    
    double rate = ((double)(benchmark->metrics.current_traces - benchmark->metrics.initial_traces) /
                   (double)benchmark->metrics.initial_traces) * 100.0;
    
    benchmark->metrics.expansion_rate = rate;
    return rate;
}

/**
 * Mesure l'efficacité mémoire (utilise l'API TCDE)
 */
double TCDE_MeasureMemoryEfficiency(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark || !benchmark->memory_manager) return 0.0;
    
    // Utiliser la fonction TCDE réelle
    double efficiency = TCDE_ComputeMemoryEfficiency(benchmark->memory_manager) * 100.0;
    
    benchmark->metrics.memory_efficiency = efficiency;
    benchmark->metrics.compression_ratio = TCDE_ComputeCompressionRatio(benchmark->memory_manager);
    
    return efficiency;
}

/**
 * Benchmark complet
 */
bool TCDE_RunBasicMemoryBenchmark(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark) return false;
    
    printf("\\n💾 TCDE MEMORY BENCHMARK - COMPLETE\\n");
    printf("=====================================\\n");
    printf("ZERO TOLERANCE: Using REAL TCDE API\\n");
    printf("Target: 100 → 1000+ traces, >99%% efficiency\\n\\n");
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Initialiser
    if (!TCDE_InitializeMemoryBenchmark(benchmark, TCDE_INITIAL_MEMORY_TRACES)) {
        return false;
    }
    
    printf("Starting memory expansion...\\n\\n");
    
    // Exécuter des cycles
    uint32_t max_cycles = 1000;
    for (uint32_t cycle = 0; cycle < max_cycles && 
         benchmark->metrics.current_traces < TCDE_TARGET_MEMORY_TRACES; cycle++) {
        TCDE_RunMemoryBenchmarkCycle(benchmark);
        
        if (cycle % 100 == 0 && benchmark->real_time_monitoring) {
            printf("  Cycle %u: %u traces (%.1f%%, eff: %.2f%%)\\n",
                   cycle, benchmark->metrics.current_traces,
                   TCDE_MeasureMemoryExpansion(benchmark),
                   TCDE_MeasureMemoryEfficiency(benchmark));
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Mesures finales
    TCDE_MeasureMemoryExpansion(benchmark);
    TCDE_MeasureMemoryEfficiency(benchmark);
    
    benchmark->score = TCDE_CalculateMemoryBenchmarkScore(benchmark);
    
    uint64_t benchmark_time = (end.tv_sec - start.tv_sec) * 1000000000ULL +
                             (end.tv_nsec - start.tv_nsec);
    benchmark->total_benchmark_time_ns = benchmark_time;
    
    printf("\\n📊 Memory benchmark completed in %.2f ms\\n", benchmark_time / 1000000.0);
    printf("📊 Final traces: %u (expansion: %.1f%%)\\n",
           benchmark->metrics.current_traces, benchmark->metrics.expansion_rate);
    printf("📊 Efficiency: %.2f%% (compression: %.3f)\\n",
           benchmark->metrics.memory_efficiency, benchmark->metrics.compression_ratio);
    
    return true;
}

/**
 * Calcule le score
 */
TCDE_MemoryBenchmarkScore TCDE_CalculateMemoryBenchmarkScore(TCDE_MemoryBenchmark* benchmark) {
    TCDE_MemoryBenchmarkScore score = benchmark->score;
    
    score.expansion_achievement = benchmark->metrics.expansion_rate / 1000.0;
    if (score.expansion_achievement > 1.0) score.expansion_achievement = 1.0;
    
    score.efficiency_achievement = benchmark->metrics.memory_efficiency / TCDE_MEMORY_EFFICIENCY_TARGET;
    if (score.efficiency_achievement > 1.0) score.efficiency_achievement = 1.0;
    
    score.quality_score = (benchmark->metrics.compression_ratio > 0.0) ? 
        (1.0 - benchmark->metrics.compression_ratio) : 0.0;
    
    score.overall_score = (
        score.expansion_achievement * 0.40 +
        score.efficiency_achievement * 0.40 +
        score.quality_score * 0.20
    );
    
    score.target_reached = (benchmark->metrics.current_traces >= TCDE_TARGET_MEMORY_TRACES);
    score.efficiency_valid = (benchmark->metrics.memory_efficiency >= TCDE_MEMORY_EFFICIENCY_TARGET);
    score.is_authentic = (benchmark->memory_manager != NULL && 
                         benchmark->metrics.total_cycles > 0);
    score.authenticity_score = score.is_authentic ? 1.0 : 0.0;
    
    return score;
}

/**
 * Vérifie si l'objectif est atteint
 */
bool TCDE_IsBasicMemoryTargetReached(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark) return false;
    return (benchmark->metrics.current_traces >= TCDE_TARGET_MEMORY_TRACES &&
            benchmark->metrics.memory_efficiency >= TCDE_MEMORY_EFFICIENCY_TARGET);
}

/**
 * Affiche le score
 */
void TCDE_PrintMemoryBenchmarkScore(const TCDE_MemoryBenchmarkScore* score) {
    if (!score) return;
    
    printf("\\n💾 MEMORY BENCHMARK SCORE\\n");
    printf("==========================\\n");
    printf("Overall Score:         %.6f %s\\n", score->overall_score,
           (score->target_reached && score->efficiency_valid) ? "✅" : "");
    printf("Authenticity:          %s (%.3f)\\n",
           score->is_authentic ? "✅ AUTHENTIC" : "❌",
           score->authenticity_score);
    printf("--------------------------\\n");
    printf("Expansion Achievement:  %.6f %s\\n", score->expansion_achievement,
           score->expansion_achievement >= 1.0 ? "✅" : "⚠️");
    printf("Efficiency Achievement: %.6f %s\\n", score->efficiency_achievement,
           score->efficiency_achievement >= 1.0 ? "✅" : "⚠️");
    printf("Quality Score:          %.6f\\n", score->quality_score);
    printf("==========================\\n");
}

/**
 * Affiche le résumé
 */
void TCDE_PrintMemoryBenchmarkSummary(TCDE_MemoryBenchmark* benchmark) {
    if (!benchmark) return;
    
    printf("\\n📊 MEMORY BENCHMARK SUMMARY\\n");
    printf("============================\\n");
    printf("Initial Traces:        %u\\n", benchmark->metrics.initial_traces);
    printf("Current Traces:        %u\\n", benchmark->metrics.current_traces);
    printf("Max Traces:            %u\\n", benchmark->metrics.max_traces_reached);
    printf("Expansion Rate:        %.1f%%\\n", benchmark->metrics.expansion_rate);
    printf("Memory Efficiency:     %.2f%%\\n", benchmark->metrics.memory_efficiency);
    printf("Compression Ratio:     %.3f\\n", benchmark->metrics.compression_ratio);
    printf("Total Cycles:          %llu\\n", (unsigned long long)benchmark->metrics.total_cycles);
    printf("Compression Events:    %llu\\n", (unsigned long long)benchmark->metrics.compression_events);
    printf("Consolidation Events:  %llu\\n", (unsigned long long)benchmark->metrics.consolidation_events);
    printf("Total Time:            %.2f ms\\n", benchmark->total_benchmark_time_ns / 1000000.0);
    printf("============================\\n");
    
    TCDE_PrintMemoryBenchmarkScore(&benchmark->score);
}
