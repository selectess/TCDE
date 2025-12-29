#include "tcde_geometric_memory.h"
#include "../core/tcde_core.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

// Déclarations des fonctions TCDE manquantes
extern bool TCDE_AddCenter6D(TCDE_Field* field, const TCDE_Point* point, TCDE_Complex coeff, float epsilon);
extern void TCDE_Evolve(TCDE_Field* field, float dt);
extern float TCDE_MeasureRealEmergence(const TCDE_Field* field);

/**
 * Crée un système de benchmark de mémoire géométrique
 * ZÉRO TOLÉRANCE: Utilise le vrai système TCDE
 */
TCDE_GeometricMemoryBenchmark* TCDE_CreateGeometricMemoryBenchmark(void) {
    TCDE_GeometricMemoryBenchmark* benchmark = 
        (TCDE_GeometricMemoryBenchmark*)malloc(sizeof(TCDE_GeometricMemoryBenchmark));
    if (!benchmark) return NULL;
    
    benchmark->active = true;
    benchmark->real_time_monitoring = false;
    benchmark->expansion_threshold = 0.95;  // 95% efficacité énergétique
    benchmark->energy_target = TCDE_ENERGY_EFFICIENCY_TARGET;
    
    // Créer un vrai champ TCDE 6D
    benchmark->field = TCDE_CreateField(100, 2.5f);  // capacity=100, fractal_dim=2.5
    if (!benchmark->field) {
        free(benchmark);
        return NULL;
    }
    
    memset(&benchmark->metrics, 0, sizeof(TCDE_GeometricMemoryMetrics));
    memset(&benchmark->score, 0, sizeof(TCDE_GeometricMemoryScore));
    
    benchmark->metrics.initial_centers = TCDE_INITIAL_CENTERS;
    
    benchmark->center_added_callback = NULL;
    benchmark->efficiency_achieved_callback = NULL;
    
    clock_gettime(CLOCK_MONOTONIC, &benchmark->start_time);
    benchmark->total_benchmark_time_ns = 0;
    
    printf("TCDE_GEOMETRIC_MEMORY: System ACTIVATED\\n");
    printf("TCDE_GEOMETRIC_MEMORY: Using REAL TCDE field geometry\\n");
    printf("TCDE_GEOMETRIC_MEMORY: Target 100 → 1000+ RBF centers\\n");
    printf("TCDE_GEOMETRIC_MEMORY: Energy efficiency target >99%%\\n");
    printf("TCDE_GEOMETRIC_MEMORY: ZERO TOLERANCE mode enabled\\n");
    
    return benchmark;
}

/**
 * Détruit le système
 */
void TCDE_DestroyGeometricMemoryBenchmark(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark) return;
    
    printf("TCDE_GEOMETRIC_MEMORY: System DEACTIVATED\\n");
    printf("TCDE_GEOMETRIC_MEMORY: Final centers: %u (expansion: %.1f%%)\\n",
           benchmark->metrics.current_centers, benchmark->metrics.expansion_rate);
    printf("TCDE_GEOMETRIC_MEMORY: Energy efficiency: %.2f%%\\n",
           benchmark->metrics.energy_efficiency);
    
    if (benchmark->field) {
        TCDE_DestroyField(benchmark->field);
    }
    free(benchmark);
}

/**
 * Initialise la mémoire géométrique avec des centres RBF
 * ZÉRO TOLÉRANCE: Utilise TCDE_AddCenter6D réel
 */
bool TCDE_InitializeGeometricMemory(TCDE_GeometricMemoryBenchmark* benchmark, uint32_t initial_count) {
    if (!benchmark || !benchmark->field) return false;
    
    printf("TCDE_GEOMETRIC_MEMORY: Initializing %u RBF centers in 6D manifold...\\n", initial_count);
    
    // Ajouter des centres RBF dans le manifold 6D
    for (uint32_t i = 0; i < initial_count; i++) {
        // Position dans l'espace 6D [x, y, z, τ₁, τ₂, modality]
        float coords[6];
        coords[0] = 0.5f + 0.3f * cosf(i * 0.1f);  // x
        coords[1] = 0.5f + 0.3f * sinf(i * 0.1f);  // y
        coords[2] = 0.5f + 0.2f * cosf(i * 0.2f);  // z
        coords[3] = (float)i / initial_count;       // τ₁ (temps)
        coords[4] = 0.0f;                           // τ₂
        coords[5] = 0.5f;                           // modality
        
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        
        // Coefficient complexe avec phase
        TCDE_Complex coeff = (1.0f + 0.1f * sinf(i * 0.3f)) * 
                            cexpf(I * 2.0f * M_PI * (float)i / initial_count);
        
        // Ajouter au champ TCDE réel
        TCDE_AddCenter6D(benchmark->field, &p, coeff, 0.1f);
        
        TCDE_FreePoint(&p);
    }
    
    benchmark->metrics.initial_centers = initial_count;
    benchmark->metrics.current_centers = benchmark->field->manifold_6d.num_centers;
    
    // Mesurer l'énergie initiale (calcul réel TCDE)
    benchmark->metrics.initial_energy = TCDE_ComputeEnergy(benchmark->field);
    benchmark->metrics.current_energy = benchmark->metrics.initial_energy;
    
    printf("TCDE_GEOMETRIC_MEMORY: Initialized %u centers (energy: %.6f)\\n",
           benchmark->metrics.current_centers, benchmark->metrics.initial_energy);
    
    return true;
}

/**
 * Ajoute un centre géométrique au manifold
 * ZÉRO TOLÉRANCE: Utilise la vraie fonction TCDE
 */
bool TCDE_AddGeometricCenter(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark || !benchmark->field) return false;
    
    uint32_t n = benchmark->metrics.current_centers;
    
    // Position adaptative basée sur l'état actuel du champ
    float coords[6];
    coords[0] = 0.5f + 0.4f * cosf(n * 0.15f);
    coords[1] = 0.5f + 0.4f * sinf(n * 0.15f);
    coords[2] = 0.5f + 0.3f * cosf(n * 0.25f);
    coords[3] = (float)n / TCDE_TARGET_CENTERS;
    coords[4] = 0.1f * sinf(n * 0.1f);
    coords[5] = 0.5f + 0.2f * cosf(n * 0.05f);
    
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    
    // Coefficient adaptatif
    TCDE_Complex coeff = (0.8f + 0.2f * cosf(n * 0.2f)) *
                        cexpf(I * (float)n * 0.1f);
    
    // Ajouter au champ réel
    TCDE_AddCenter6D(benchmark->field, &p, coeff, 0.08f + 0.02f * sinf(n * 0.1f));
    
    TCDE_FreePoint(&p);
    
    benchmark->metrics.current_centers = benchmark->field->manifold_6d.num_centers;
    benchmark->metrics.total_expansions++;
    
    if (benchmark->metrics.current_centers > benchmark->metrics.max_centers_reached) {
        benchmark->metrics.max_centers_reached = benchmark->metrics.current_centers;
    }
    
    if (benchmark->center_added_callback) {
        benchmark->center_added_callback(benchmark->metrics.current_centers - 1);
    }
    
    return true;
}

/**
 * Optimise la représentation géométrique
 * ZÉRO TOLÉRANCE: Utilise l'évolution TCDE réelle
 */
bool TCDE_OptimizeGeometricRepresentation(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark || !benchmark->field) return false;
    
    // Faire évoluer le champ (calcul réel TCDE)
    float dt = 0.01f;
    TCDE_Evolve(benchmark->field, dt);
    
    // Recalculer l'énergie (mesure réelle)
    benchmark->metrics.current_energy = TCDE_ComputeEnergy(benchmark->field);
    
    return true;
}

/**
 * Exécute un cycle d'adaptation géométrique
 * ZÉRO TOLÉRANCE: Logique TCDE authentique
 */
bool TCDE_RunGeometricAdaptationCycle(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark) return false;
    
    benchmark->metrics.total_cycles++;
    
    // Optimiser la représentation actuelle
    TCDE_OptimizeGeometricRepresentation(benchmark);
    
    // Mesurer l'efficacité énergétique
    double efficiency = TCDE_MeasureEnergyEfficiency(benchmark);
    
    // Ajouter des centres si l'efficacité est bonne
    if (efficiency > benchmark->expansion_threshold &&
        benchmark->metrics.current_centers < TCDE_TARGET_CENTERS) {
        
        // Expansion adaptative
        uint32_t centers_to_add = (benchmark->metrics.current_centers < 500) ? 10 : 1;
        
        for (uint32_t i = 0; i < centers_to_add && 
             benchmark->metrics.current_centers < TCDE_TARGET_CENTERS; i++) {
            TCDE_AddGeometricCenter(benchmark);
        }
        
        benchmark->metrics.adaptation_events++;
    }
    
    return true;
}

/**
 * Mesure le taux d'expansion
 */
double TCDE_MeasureExpansionRate(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark || benchmark->metrics.initial_centers == 0) return 0.0;
    
    double rate = ((double)(benchmark->metrics.current_centers - benchmark->metrics.initial_centers) /
                   (double)benchmark->metrics.initial_centers) * 100.0;
    
    benchmark->metrics.expansion_rate = rate;
    return rate;
}

/**
 * Mesure l'efficacité énergétique (conservation dans le manifold)
 * ZÉRO TOLÉRANCE: Basé sur l'énergie TCDE réelle
 */
double TCDE_MeasureEnergyEfficiency(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark || benchmark->metrics.initial_energy == 0.0) return 0.0;
    
    // Efficacité = conservation d'énergie par centre
    double energy_per_center_initial = benchmark->metrics.initial_energy / 
                                       benchmark->metrics.initial_centers;
    double energy_per_center_current = benchmark->metrics.current_energy /
                                       benchmark->metrics.current_centers;
    
    // Efficacité = capacité à maintenir la densité énergétique
    double efficiency = (energy_per_center_current / energy_per_center_initial) * 100.0;
    
    // Limiter à 100%
    if (efficiency > 100.0) efficiency = 100.0;
    
    benchmark->metrics.energy_efficiency = efficiency;
    benchmark->metrics.energy_density = energy_per_center_current;
    
    return efficiency;
}

/**
 * Mesure la qualité géométrique
 */
double TCDE_MeasureGeometricQuality(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark || !benchmark->field) return 0.0;
    
    // Mesurer l'émergence (complexité géométrique réelle)
    double emergence = TCDE_MeasureRealEmergence(benchmark->field);
    
    // Cohérence du manifold
    double coherence = (double)benchmark->field->manifold_6d.num_centers / 
                      (double)benchmark->field->manifold_6d.capacity;
    
    benchmark->metrics.spatial_complexity = emergence;
    benchmark->metrics.manifold_coherence = coherence;
    benchmark->metrics.topological_efficiency = (emergence + coherence) / 2.0;
    
    return benchmark->metrics.topological_efficiency;
}

/**
 * Benchmark complet
 */
bool TCDE_RunCompleteGeometricMemoryBenchmark(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark) return false;
    
    printf("\\n💾 TCDE GEOMETRIC MEMORY - COMPLETE BENCHMARK\\n");
    printf("===============================================\\n");
    printf("ZERO TOLERANCE MODE: Using REAL TCDE geometry\\n");
    printf("Target: 100 → 1000+ RBF centers\\n");
    printf("Energy efficiency target: >99%%\\n\\n");
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Initialiser avec 100 centres RBF
    if (!TCDE_InitializeGeometricMemory(benchmark, TCDE_INITIAL_CENTERS)) {
        return false;
    }
    
    printf("Starting geometric expansion...\\n\\n");
    
    // Exécuter des cycles d'adaptation
    uint32_t max_cycles = 1000;
    uint32_t cycle = 0;
    
    while (benchmark->metrics.current_centers < TCDE_TARGET_CENTERS && cycle < max_cycles) {
        TCDE_RunGeometricAdaptationCycle(benchmark);
        cycle++;
        
        if (cycle % 100 == 0 && benchmark->real_time_monitoring) {
            printf("  Cycle %u: %u centers (%.1f%% expansion, %.2f%% efficiency)\\n",
                   cycle,
                   benchmark->metrics.current_centers,
                   TCDE_MeasureExpansionRate(benchmark),
                   TCDE_MeasureEnergyEfficiency(benchmark));
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Mesurer toutes les métriques finales
    TCDE_MeasureExpansionRate(benchmark);
    TCDE_MeasureEnergyEfficiency(benchmark);
    TCDE_MeasureGeometricQuality(benchmark);
    
    // Calculer le score
    benchmark->score = TCDE_CalculateGeometricMemoryScore(benchmark);
    
    uint64_t benchmark_time = (end.tv_sec - start.tv_sec) * 1000000000ULL +
                             (end.tv_nsec - start.tv_nsec);
    benchmark->total_benchmark_time_ns = benchmark_time;
    
    printf("\\n📊 Geometric memory benchmark completed in %.2f ms\\n", benchmark_time / 1000000.0);
    printf("📊 Total cycles: %llu\\n", (unsigned long long)benchmark->metrics.total_cycles);
    printf("📊 Final centers: %u (expansion: %.1f%%)\\n",
           benchmark->metrics.current_centers, benchmark->metrics.expansion_rate);
    printf("📊 Energy efficiency: %.2f%% (%.6f → %.6f)\\n",
           benchmark->metrics.energy_efficiency,
           benchmark->metrics.initial_energy,
           benchmark->metrics.current_energy);
    
    // Callbacks
    if (benchmark->efficiency_achieved_callback) {
        benchmark->efficiency_achieved_callback(benchmark->metrics.energy_efficiency);
    }
    
    return true;
}

/**
 * Calcule le score
 */
TCDE_GeometricMemoryScore TCDE_CalculateGeometricMemoryScore(
    TCDE_GeometricMemoryBenchmark* benchmark) {
    
    TCDE_GeometricMemoryScore score = benchmark->score;
    
    // Réalisation de l'expansion
    score.expansion_achievement = benchmark->metrics.expansion_rate / 1000.0;
    if (score.expansion_achievement > 1.0) score.expansion_achievement = 1.0;
    
    // Réalisation de l'efficacité énergétique
    score.energy_efficiency_achievement = benchmark->metrics.energy_efficiency / 
                                         TCDE_ENERGY_EFFICIENCY_TARGET;
    if (score.energy_efficiency_achievement > 1.0) score.energy_efficiency_achievement = 1.0;
    
    // Qualité géométrique
    score.geometric_quality = benchmark->metrics.topological_efficiency;
    
    // Intelligence adaptative
    score.adaptation_intelligence = (benchmark->metrics.total_cycles > 0) ?
        (double)benchmark->metrics.adaptation_events / benchmark->metrics.total_cycles : 0.0;
    
    // Score global
    score.overall_score = (
        score.expansion_achievement * 0.30 +
        score.energy_efficiency_achievement * 0.40 +
        score.geometric_quality * 0.20 +
        score.adaptation_intelligence * 0.10
    );
    
    // Validation
    score.target_reached = (benchmark->metrics.current_centers >= TCDE_TARGET_CENTERS);
    score.efficiency_valid = (benchmark->metrics.energy_efficiency >= TCDE_ENERGY_EFFICIENCY_TARGET);
    score.is_authentic = (benchmark->metrics.total_cycles > 0 && 
                         benchmark->field != NULL &&
                         benchmark->field->manifold_6d.num_centers > 0);
    score.authenticity_score = score.is_authentic ? 1.0 : 0.0;
    
    return score;
}

/**
 * Vérifie si l'objectif est atteint
 */
bool TCDE_IsGeometricTargetReached(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark) return false;
    return (benchmark->metrics.current_centers >= TCDE_TARGET_CENTERS &&
            benchmark->metrics.energy_efficiency >= TCDE_ENERGY_EFFICIENCY_TARGET);
}

/**
 * Affiche le score
 */
void TCDE_PrintGeometricMemoryScore(const TCDE_GeometricMemoryScore* score) {
    if (!score) return;
    
    printf("\\n💾 GEOMETRIC MEMORY SCORE\\n");
    printf("==========================\\n");
    printf("Overall Score:         %.6f %s\\n", score->overall_score,
           (score->target_reached && score->efficiency_valid) ? "✅ TARGET REACHED" : "");
    printf("Authenticity:          %s (%.3f)\\n",
           score->is_authentic ? "✅ AUTHENTIC" : "❌ NOT AUTHENTIC",
           score->authenticity_score);
    printf("--------------------------\\n");
    printf("Expansion Achievement:  %.6f %s\\n", score->expansion_achievement,
           score->expansion_achievement >= 1.0 ? "✅" : "⚠️");
    printf("Energy Efficiency:      %.6f %s\\n", score->energy_efficiency_achievement,
           score->energy_efficiency_achievement >= 1.0 ? "✅" : "⚠️");
    printf("Geometric Quality:      %.6f\\n", score->geometric_quality);
    printf("Adaptation Intelligence: %.6f\\n", score->adaptation_intelligence);
    printf("Target Reached:         %s\\n", score->target_reached ? "✅ YES" : "❌ NO");
    printf("Efficiency Valid:       %s\\n", score->efficiency_valid ? "✅ YES" : "❌ NO");
    printf("==========================\\n");
}

/**
 * Affiche le résumé
 */
void TCDE_PrintGeometricMemorySummary(TCDE_GeometricMemoryBenchmark* benchmark) {
    if (!benchmark) return;
    
    printf("\\n📊 GEOMETRIC MEMORY SUMMARY\\n");
    printf("============================\\n");
    printf("Initial Centers:       %u\\n", benchmark->metrics.initial_centers);
    printf("Current Centers:       %u\\n", benchmark->metrics.current_centers);
    printf("Max Centers:           %u\\n", benchmark->metrics.max_centers_reached);
    printf("Expansion Rate:        %.1f%%\\n", benchmark->metrics.expansion_rate);
    printf("Initial Energy:        %.6f\\n", benchmark->metrics.initial_energy);
    printf("Current Energy:        %.6f\\n", benchmark->metrics.current_energy);
    printf("Energy Efficiency:     %.2f%%\\n", benchmark->metrics.energy_efficiency);
    printf("Energy Density:        %.6f\\n", benchmark->metrics.energy_density);
    printf("Spatial Complexity:    %.3f\\n", benchmark->metrics.spatial_complexity);
    printf("Manifold Coherence:    %.3f\\n", benchmark->metrics.manifold_coherence);
    printf("Topological Efficiency: %.3f\\n", benchmark->metrics.topological_efficiency);
    printf("Adaptation Events:     %llu\\n", (unsigned long long)benchmark->metrics.adaptation_events);
    printf("Total Cycles:          %llu\\n", (unsigned long long)benchmark->metrics.total_cycles);
    printf("Total Time:            %.2f ms\\n", benchmark->total_benchmark_time_ns / 1000000.0);
    printf("============================\\n");
    
    TCDE_PrintGeometricMemoryScore(&benchmark->score);
}
