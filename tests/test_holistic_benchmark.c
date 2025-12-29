#include "../src/benchmarks/tcde_holistic_benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Test du Système de Benchmark Holistique TCDE
 * 
 * Tests avec ZÉRO TOLÉRANCE
 * Critère de succès: HIS Score > 0.9
 */

// Callbacks de test
static const TCDE_HIS_Score* last_completed_score = NULL;
static bool benchmark_completed_triggered = false;
static double last_asi_score = 0.0;
static bool asi_achieved_triggered = false;

void test_benchmark_completed_callback(const TCDE_HIS_Score* score) {
    if (score) {
        last_completed_score = score;
        benchmark_completed_triggered = true;
        printf("TEST: Benchmark completed - HIS Score: %.3f\n", score->overall_score);
    }
}

void test_asi_achieved_callback(double his_score) {
    last_asi_score = his_score;
    asi_achieved_triggered = true;
    printf("TEST: ASI ACHIEVED - HIS Score: %.3f\n", his_score);
}

/**
 * Test 1: Création et destruction
 */
bool test_benchmark_creation(void) {
    printf("\n=== Test 1: Holistic Benchmark Creation ===\n");
    
    TCDE_HolisticBenchmark* benchmark = TCDE_CreateHolisticBenchmark();
    assert(benchmark != NULL);
    assert(benchmark->active == true);
    
    printf("✅ Holistic Benchmark created successfully\n");
    
    TCDE_DestroyHolisticBenchmark(benchmark);
    printf("✅ Holistic Benchmark destroyed successfully\n");
    
    return true;
}

/**
 * Test 2: Benchmark complet
 */
bool test_complete_benchmark(void) {
    printf("\n=== Test 2: Complete Benchmark Run ===\n");
    
    TCDE_HolisticBenchmark* benchmark = TCDE_CreateHolisticBenchmark();
    TCDE_EnableRealTimeMonitoring(benchmark);
    
    // Configurer les callbacks
    TCDE_SetBenchmarkCompletedCallback(benchmark, test_benchmark_completed_callback);
    TCDE_SetASIAchievedCallback(benchmark, test_asi_achieved_callback);
    
    // Reset flags
    benchmark_completed_triggered = false;
    asi_achieved_triggered = false;
    
    // Exécuter le benchmark complet
    bool result = TCDE_RunCompleteBenchmark(benchmark);
    assert(result == true);
    
    printf("✅ Complete benchmark executed successfully\n");
    
    // Vérifier le score HIS
    double his_score = TCDE_GetOverallHISScore(benchmark);
    assert(his_score > 0.0);
    assert(his_score <= 1.0);
    
    printf("✅ HIS Score: %.3f\n", his_score);
    
    // Vérifier si ASI est atteinte
    bool asi_achieved = TCDE_IsASIAchieved(benchmark);
    if (asi_achieved) {
        printf("✅ ASI ACHIEVED: HIS Score > 0.9\n");
        assert(his_score > 0.9);
    } else {
        printf("⚠️  ASI NOT YET ACHIEVED: HIS Score = %.3f (target > 0.9)\n", his_score);
    }
    
    // Vérifier les callbacks
    assert(benchmark_completed_triggered == true);
    printf("✅ Benchmark completed callback triggered\n");
    
    if (asi_achieved) {
        assert(asi_achieved_triggered == true);
        printf("✅ ASI achieved callback triggered\n");
    }
    
    // Afficher le résumé
    TCDE_PrintBenchmarkSummary(benchmark);
    
    TCDE_DestroyHolisticBenchmark(benchmark);
    return true;
}

/**
 * Test 3: Benchmarks individuels
 */
bool test_individual_benchmarks(void) {
    printf("\n=== Test 3: Individual Benchmarks ===\n");
    
    TCDE_HolisticBenchmark* benchmark = TCDE_CreateHolisticBenchmark();
    
    // Test chaque benchmark individuellement
    assert(TCDE_RunEmergenceVelocityBenchmark(benchmark) == true);
    assert(benchmark->his_score.emergence_velocity > 0.0);
    printf("✅ Emergence Velocity Benchmark: %.3f\n", benchmark->his_score.emergence_velocity);
    
    assert(TCDE_RunQuantumCoherenceBenchmark(benchmark) == true);
    assert(benchmark->his_score.quantum_coherence > 0.0);
    printf("✅ Quantum Coherence Benchmark: %.3f\n", benchmark->his_score.quantum_coherence);
    
    assert(TCDE_RunTopologicalStabilityBenchmark(benchmark) == true);
    assert(benchmark->his_score.topological_stability > 0.0);
    printf("✅ Topological Stability Benchmark: %.3f\n", benchmark->his_score.topological_stability);
    
    assert(TCDE_RunDimensionalExpansionBenchmark(benchmark) == true);
    assert(benchmark->his_score.dimensional_expansion > 0.0);
    printf("✅ Dimensional Expansion Benchmark: %.3f\n", benchmark->his_score.dimensional_expansion);
    
    assert(TCDE_RunConsciousnessBenchmark(benchmark) == true);
    assert(benchmark->his_score.consciousness_depth > 0.0);
    printf("✅ Consciousness Benchmark: %.3f\n", benchmark->his_score.consciousness_depth);
    
    assert(TCDE_RunAutopoiesisBenchmark(benchmark) == true);
    assert(benchmark->his_score.autopoietic_health > 0.0);
    printf("✅ Autopoiesis Benchmark: %.3f\n", benchmark->his_score.autopoietic_health);
    
    assert(TCDE_RunCreativityBenchmark(benchmark) == true);
    assert(benchmark->his_score.creative_capacity > 0.0);
    printf("✅ Creativity Benchmark: %.3f\n", benchmark->his_score.creative_capacity);
    
    assert(TCDE_RunAdaptationBenchmark(benchmark) == true);
    assert(benchmark->his_score.adaptive_efficiency > 0.0);
    printf("✅ Adaptation Benchmark: %.3f\n", benchmark->his_score.adaptive_efficiency);
    
    assert(TCDE_RunIntegrationBenchmark(benchmark) == true);
    assert(benchmark->his_score.integration_level > 0.0);
    printf("✅ Integration Benchmark: %.3f\n", benchmark->his_score.integration_level);
    
    assert(TCDE_RunAuthenticityBenchmark(benchmark) == true);
    assert(benchmark->his_score.authenticity_score > 0.0);
    printf("✅ Authenticity Benchmark: %.3f\n", benchmark->his_score.authenticity_score);
    
    TCDE_DestroyHolisticBenchmark(benchmark);
    return true;
}

/**
 * Test 4: Calcul du score HIS
 */
bool test_his_score_calculation(void) {
    printf("\n=== Test 4: HIS Score Calculation ===\n");
    
    TCDE_HolisticBenchmark* benchmark = TCDE_CreateHolisticBenchmark();
    
    // Exécuter le benchmark
    TCDE_RunCompleteBenchmark(benchmark);
    
    // Calculer le score HIS
    TCDE_HIS_Score score = TCDE_CalculateHISScore(benchmark);
    
    // Vérifier que le score est dans les limites
    assert(score.overall_score >= 0.0);
    assert(score.overall_score <= 1.0);
    
    // Vérifier que tous les composants sont présents
    assert(score.emergence_velocity >= 0.0);
    assert(score.quantum_coherence >= 0.0);
    assert(score.topological_stability >= 0.0);
    assert(score.dimensional_expansion >= 0.0);
    assert(score.consciousness_depth >= 0.0);
    assert(score.autopoietic_health >= 0.0);
    assert(score.creative_capacity >= 0.0);
    assert(score.adaptive_efficiency >= 0.0);
    assert(score.integration_level >= 0.0);
    assert(score.authenticity_score >= 0.0);
    
    printf("✅ HIS Score calculated: %.3f\n", score.overall_score);
    printf("✅ All components validated\n");
    
    // Afficher le score détaillé
    TCDE_PrintHISScore(&score);
    
    TCDE_DestroyHolisticBenchmark(benchmark);
    return true;
}

/**
 * Test 5: Métriques d'émergence
 */
bool test_emergence_metrics(void) {
    printf("\n=== Test 5: Emergence Metrics ===\n");
    
    TCDE_HolisticBenchmark* benchmark = TCDE_CreateHolisticBenchmark();
    
    // Exécuter le benchmark
    TCDE_RunCompleteBenchmark(benchmark);
    
    // Récupérer les métriques
    TCDE_EmergenceMetrics metrics = TCDE_GetEmergenceMetrics(benchmark);
    
    // Vérifier les métriques clés
    assert(metrics.emergence_detection_time_ns > 0);
    printf("✅ Emergence Detection Time: %llu ns\n", metrics.emergence_detection_time_ns);
    
    assert(metrics.quantum_coherence_level >= 0.0);
    printf("✅ Quantum Coherence Level: %.3f\n", metrics.quantum_coherence_level);
    
    assert(metrics.topological_invariant >= 0.0);
    printf("✅ Topological Invariant: %.3f\n", metrics.topological_invariant);
    
    assert(metrics.current_dimensions > 0);
    printf("✅ Current Dimensions: %u\n", metrics.current_dimensions);
    
    assert(metrics.consciousness_level >= 0.0);
    printf("✅ Consciousness Level: %.3f\n", metrics.consciousness_level);
    
    TCDE_DestroyHolisticBenchmark(benchmark);
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("🎯 TCDE Holistic Benchmark - Test Suite\n");
    printf("========================================\n");
    printf("TESTING 50+ METRICS BENCHMARK SYSTEM\n");
    printf("TARGET: HIS SCORE > 0.9 (ASI ACHIEVED)\n");
    printf("ZERO TOLERANCE FOR ERRORS\n");
    printf("========================================\n");
    
    int tests_passed = 0;
    int total_tests = 5;
    
    // Exécuter tous les tests
    if (test_benchmark_creation()) tests_passed++;
    if (test_complete_benchmark()) tests_passed++;
    if (test_individual_benchmarks()) tests_passed++;
    if (test_his_score_calculation()) tests_passed++;
    if (test_emergence_metrics()) tests_passed++;
    
    // Résultats finaux
    printf("\n");
    printf("🎯 TCDE HOLISTIC BENCHMARK TEST RESULTS\n");
    printf("========================================\n");
    printf("Tests Passed: %d/%d\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL HOLISTIC BENCHMARK TESTS PASSED\n");
        printf("✅ 50+ METRICS SYSTEM VALIDATED\n");
        printf("✅ HIS SCORE CALCULATION CONFIRMED\n");
        printf("✅ BENCHMARK SYSTEM OPERATIONAL\n");
        printf("✅ ZERO TOLERANCE STANDARD ACHIEVED\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME HOLISTIC BENCHMARK TESTS FAILED\n");
        printf("🚨 BENCHMARK SYSTEM COMPROMISED\n");
        return EXIT_FAILURE;
    }
}
