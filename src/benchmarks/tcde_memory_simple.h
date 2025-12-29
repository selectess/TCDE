#ifndef TCDE_MEMORY_SIMPLE_H
#define TCDE_MEMORY_SIMPLE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Memory Benchmark Simplifié
 * Version standalone sans dépendances complexes
 * Respecte la logique TCDE : traces géométriques, efficacité énergétique
 * 
 * STANDARD ZÉRO TOLÉRANCE
 */

#define TCDE_MEM_INITIAL 100
#define TCDE_MEM_TARGET 1000
#define TCDE_MEM_EFFICIENCY_TARGET 99.0

typedef struct {
    float position[6];
    float energy;
    float importance;
    uint64_t access_count;
    bool is_active;
} TCDE_MemTrace;

typedef struct {
    uint32_t initial_traces;
    uint32_t current_traces;
    double expansion_rate;
    double energy_efficiency;
    uint64_t total_cycles;
} TCDE_MemMetrics;

typedef struct {
    double overall_score;
    double expansion_achievement;
    double efficiency_achievement;
    bool target_reached;
    bool is_authentic;
} TCDE_MemScore;

typedef struct {
    bool active;
    TCDE_MemTrace* traces;
    uint32_t trace_count;
    uint32_t capacity;
    double total_energy;
    double initial_energy;
    TCDE_MemMetrics metrics;
    TCDE_MemScore score;
    uint64_t total_time_ns;
} TCDE_MemBenchmark;

TCDE_MemBenchmark* TCDE_CreateMemBenchmark(void);
void TCDE_DestroyMemBenchmark(TCDE_MemBenchmark* b);
bool TCDE_InitMem(TCDE_MemBenchmark* b, uint32_t count);
bool TCDE_RunMemCycle(TCDE_MemBenchmark* b);
bool TCDE_RunCompleteMem(TCDE_MemBenchmark* b);
double TCDE_MeasureMemExpansion(TCDE_MemBenchmark* b);
double TCDE_MeasureMemEfficiency(TCDE_MemBenchmark* b);
TCDE_MemScore TCDE_CalcMemScore(TCDE_MemBenchmark* b);
void TCDE_PrintMemScore(const TCDE_MemScore* s);
void TCDE_PrintMemSummary(TCDE_MemBenchmark* b);

#endif
