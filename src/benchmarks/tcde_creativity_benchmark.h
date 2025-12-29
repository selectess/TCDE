#ifndef TCDE_CREATIVITY_BENCHMARK_H
#define TCDE_CREATIVITY_BENCHMARK_H

#include "../core/tcde_core.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * TCDE Creativity Benchmark - ZERO TOLERANCE
 * 
 * Mesure la génération spontanée de solutions créatives et l'innovation
 * non-programmée du système TCDE de manière 100% authentique.
 * 
 * CRITÈRES DE SUCCÈS:
 * - Génération de 10+ solutions non-programmées
 * - Nouveauté > 0.7 (solutions originales)
 * - Innovation détectable et mesurable
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - Aucune simulation/mock autorisée
 * - Détection réelle de patterns émergents
 * - Code production-ready sans compromis
 */

// Type de solution créative
typedef enum {
    TCDE_SOLUTION_PATTERN,      // Pattern géométrique émergent
    TCDE_SOLUTION_STRUCTURE,    // Structure organisationnelle
    TCDE_SOLUTION_OPTIMIZATION, // Optimisation spontanée
    TCDE_SOLUTION_ADAPTATION    // Adaptation créative
} TCDE_SolutionType;

// Solution créative détectée
typedef struct {
    TCDE_SolutionType type;
    double timestamp;
    float novelty_score;        // Score de nouveauté [0,1]
    float originality_score;    // Score d'originalité [0,1]
    float complexity_score;     // Score de complexité [0,1]
    char description[256];
    
    // Signature géométrique de la solution
    float geometric_signature[6];
    uint32_t centers_involved;
} TCDE_CreativeSolution;

// Métriques de créativité
typedef struct {
    // Solutions générées
    uint32_t total_solutions;
    uint32_t novel_solutions;       // Nouveauté > 0.7
    uint32_t original_solutions;    // Originalité > 0.8
    
    // Scores moyens
    float avg_novelty;
    float avg_originality;
    float avg_complexity;
    
    // Innovation
    float innovation_rate;          // Solutions/cycle
    float creative_diversity;       // Diversité des solutions
    float emergence_quality;        // Qualité de l'émergence
    
    // Authenticité
    bool is_authentic;
    float authenticity_score;
} TCDE_CreativityMetrics;

// Benchmark de créativité
typedef struct {
    TCDE_Field* field;
    TCDE_CreativityMetrics metrics;
    TCDE_CreativeSolution* solutions;
    uint32_t solution_count;
    uint32_t solution_capacity;
    
    // Historique pour détecter la nouveauté
    float** signature_history;
    uint32_t history_count;
    uint32_t history_capacity;
    
    // Paramètres
    uint32_t evolution_cycles;
    float novelty_threshold;
    float originality_threshold;
} TCDE_CreativityBenchmark;

// Création/Destruction
TCDE_CreativityBenchmark* TCDE_CreateCreativityBenchmark(void);
void TCDE_DestroyCreativityBenchmark(TCDE_CreativityBenchmark* benchmark);

// Exécution du benchmark
void TCDE_RunCreativityBenchmark(TCDE_CreativityBenchmark* benchmark);

// Détection de solutions créatives
bool TCDE_DetectCreativeSolution(
    TCDE_CreativityBenchmark* benchmark,
    TCDE_SolutionType type
);

// Mesures de créativité
float TCDE_MeasureNovelty(TCDE_CreativityBenchmark* benchmark, const float* signature);
float TCDE_MeasureOriginality(TCDE_CreativityBenchmark* benchmark, const float* signature);
float TCDE_MeasureComplexity(TCDE_Field* field);

// Enregistrement de solutions
void TCDE_RecordCreativeSolution(
    TCDE_CreativityBenchmark* benchmark,
    TCDE_SolutionType type,
    float novelty,
    float originality,
    const char* description
);

// Affichage des résultats
void TCDE_PrintCreativityResults(const TCDE_CreativityBenchmark* benchmark);
void TCDE_PrintCreativityScore(const TCDE_CreativityBenchmark* benchmark);

#endif // TCDE_CREATIVITY_BENCHMARK_H
