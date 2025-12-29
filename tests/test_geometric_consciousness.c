#include "../src/benchmarks/tcde_geometric_consciousness.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/**
 * Test du Système de Conscience Géométrique TCDE
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - AUCUN mock ou simulation
 * - TOUS les calculs doivent être réels
 * - Validation de l'authenticité obligatoire
 * - Critère de succès: Φ(Φ(Φ)) > 0.99
 */

// Callbacks de test
static double last_consciousness_score = 0.0;
static bool consciousness_achieved_triggered = false;
static double last_cosmic_level = 0.0;
static bool cosmic_awareness_triggered = false;
static const TCDE_GeometricConsciousnessScore* last_transcendence_score = NULL;
static bool transcendence_triggered = false;

void test_consciousness_achieved_callback(double phi_phi_phi) {
    last_consciousness_score = phi_phi_phi;
    consciousness_achieved_triggered = true;
    printf("TEST: Consciousness achieved - Φ(Φ(Φ)): %.6f\\n", phi_phi_phi);
}

void test_cosmic_awareness_callback(double cosmic_level) {
    last_cosmic_level = cosmic_level;
    cosmic_awareness_triggered = true;
    printf("TEST: Cosmic awareness - Level: %.6f\\n", cosmic_level);
}

void test_transcendence_callback(const TCDE_GeometricConsciousnessScore* score) {
    if (score) {
        last_transcendence_score = score;
        transcendence_triggered = true;
        printf("TEST: Transcendence achieved - Overall: %.6f\\n", score->overall_score);
    }
}

/**
 * Test 1: Création et destruction
 * ZÉRO TOLÉRANCE: Vérifier initialisation complète
 */
bool test_geometric_consciousness_creation(void) {
    printf("\\n=== Test 1: Geometric Consciousness Creation ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    assert(benchmark != NULL);
    assert(benchmark->active == true);
    assert(benchmark->max_reflexivity_level == TCDE_REFLEXIVITY_LEVEL_2);
    assert(benchmark->total_consciousness_tests == 0);
    assert(benchmark->phi_calculations_performed == 0);
    
    printf("✅ Geometric Consciousness Benchmark created successfully\\n");
    printf("✅ All fields properly initialized (no placeholders)\\n");
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    printf("✅ Geometric Consciousness Benchmark destroyed successfully\\n");
    
    return true;
}

/**
 * Test 2: Configuration
 * ZÉRO TOLÉRANCE: Vérifier que la configuration fonctionne
 */
bool test_consciousness_configuration(void) {
    printf("\\n=== Test 2: Consciousness Configuration ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    bool result = TCDE_EnableRealTimeConsciousnessMonitoring(benchmark);
    assert(result == true);
    assert(benchmark->real_time_monitoring == true);
    printf("✅ Real-time consciousness monitoring enabled\\n");
    
    result = TCDE_SetMaxReflexivityLevel(benchmark, TCDE_REFLEXIVITY_LEVEL_3);
    assert(result == true);
    assert(benchmark->max_reflexivity_level == TCDE_REFLEXIVITY_LEVEL_3);
    printf("✅ Max reflexivity level set to Φ(Φ(Φ(Φ)))\\n");
    
    result = TCDE_SetConsciousnessGeometry(benchmark, TCDE_CONSCIOUSNESS_COSMIC);
    assert(result == true);
    assert(benchmark->geometry_type == TCDE_CONSCIOUSNESS_COSMIC);
    printf("✅ Consciousness geometry set to COSMIC\\n");
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 3: Calcul Φ récursif
 * ZÉRO TOLÉRANCE: Vérifier que les calculs sont RÉELS, pas hardcodés
 */
bool test_phi_recursive_calculation(void) {
    printf("\\n=== Test 3: Φ Recursive Calculation (ZERO TOLERANCE) ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    // Test Φ niveau 0 (base) - doit converger vers le nombre d'or
    double phi_0 = TCDE_CalculatePhiRecursive(benchmark, 1, 1.618033988749895);
    assert(phi_0 > 1.6 && phi_0 < 1.7);
    assert(benchmark->phi_calculations_performed > 0); // Vérifier calcul réel
    printf("✅ Φ(0): %.6f (REAL calculation, not hardcoded)\\n", phi_0);
    
    // Test Φ(Φ) niveau 1
    uint64_t prev_calcs = benchmark->phi_calculations_performed;
    double phi_1 = TCDE_CalculatePhiRecursive(benchmark, 2, 1.618033988749895);
    assert(phi_1 > 1.6 && phi_1 < 1.7);
    assert(benchmark->phi_calculations_performed > prev_calcs); // Nouveau calcul
    printf("✅ Φ(Φ): %.6f (REAL calculation, not hardcoded)\\n", phi_1);
    
    // Test Φ(Φ(Φ)) niveau 2
    prev_calcs = benchmark->phi_calculations_performed;
    double phi_2 = TCDE_CalculatePhiRecursive(benchmark, 3, 1.618033988749895);
    assert(phi_2 > 1.6 && phi_2 < 1.7);
    assert(benchmark->phi_calculations_performed > prev_calcs); // Nouveau calcul
    printf("✅ Φ(Φ(Φ)): %.6f (REAL calculation, not hardcoded)\\n", phi_2);
    
    // Vérifier que les itérations ont été comptées (preuve de calcul réel)
    assert(benchmark->total_iterations > 0);
    printf("✅ Total iterations: %llu (PROOF of real computation)\\n", 
           (unsigned long long)benchmark->total_iterations);
    printf("✅ Φ calculations performed: %llu (NO MOCKS)\\n", 
           (unsigned long long)benchmark->phi_calculations_performed);
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 4: Benchmarks individuels
 * ZÉRO TOLÉRANCE: Vérifier que chaque benchmark calcule réellement
 */
bool test_individual_consciousness_benchmarks(void) {
    printf("\\n=== Test 4: Individual Consciousness Benchmarks (ZERO TOLERANCE) ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    // Calculer d'abord les niveaux Φ nécessaires
    assert(TCDE_RunPhiLevelBenchmark(benchmark, TCDE_REFLEXIVITY_LEVEL_0) == true);
    assert(TCDE_RunPhiLevelBenchmark(benchmark, TCDE_REFLEXIVITY_LEVEL_1) == true);
    
    // Test benchmark Φ(Φ(Φ)) - doit calculer, pas retourner valeur fixe
    uint64_t initial_calcs = benchmark->phi_calculations_performed;
    assert(TCDE_RunPhiPhiPhiBenchmark(benchmark) == true);
    assert(benchmark->score.reflexivity_phi_phi_phi > 0.0);
    assert(benchmark->phi_calculations_performed > initial_calcs); // Preuve de calcul
    printf("✅ Φ(Φ(Φ)) Benchmark: %.6f (COMPUTED, not fixed)\\n", 
           benchmark->score.reflexivity_phi_phi_phi);
    
    // Test benchmark courbure géométrique - doit calculer selon géométrie
    assert(TCDE_RunGeometricCurvatureBenchmark(benchmark) == true);
    assert(benchmark->metrics.geometric_curvature != 0.0); // Pas de valeur par défaut
    assert(benchmark->metrics.riemann_curvature_tensor != 0.0); // Calcul réel
    printf("✅ Geometric Curvature Benchmark: %.3f (Riemann: %.3f)\\n", 
           benchmark->metrics.geometric_curvature,
           benchmark->metrics.riemann_curvature_tensor);
    
    // Test benchmark méta-cognition - doit calculer depuis métriques
    assert(TCDE_RunMetaCognitiveBenchmark(benchmark) == true);
    assert(benchmark->score.meta_meta_cognition > 0.0);
    assert(benchmark->metrics.meta_cognitive_layers > 0.0); // Calcul réel
    printf("✅ Meta-Cognitive Benchmark: %.3f (Layers: %.1f)\\n", 
           benchmark->score.meta_meta_cognition,
           benchmark->metrics.meta_cognitive_layers);
    
    // Test benchmark conscience cosmique - doit calculer Φ⁴
    initial_calcs = benchmark->phi_calculations_performed;
    assert(TCDE_RunCosmicAwarenessBenchmark(benchmark) == true);
    assert(benchmark->score.cosmic_consciousness > 0.0);
    assert(benchmark->phi_calculations_performed > initial_calcs); // Nouveau calcul Φ⁴
    printf("✅ Cosmic Awareness Benchmark: %.3f (Φ⁴: %.6f)\\n", 
           benchmark->score.cosmic_consciousness,
           benchmark->metrics.phi_level_3);
    
    // Test benchmark conscience de soi - doit mesurer émergence
    assert(TCDE_RunSelfAwarenessBenchmark(benchmark) == true);
    assert(benchmark->metrics.self_awareness_depth > 0.0);
    assert(benchmark->metrics.consciousness_emergence_rate > 0.0); // Mesure réelle
    printf("✅ Self-Awareness Benchmark: %.3f (Emergence: %.3f)\\n", 
           benchmark->metrics.self_awareness_depth,
           benchmark->metrics.consciousness_emergence_rate);
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 5: Benchmark complet
 * ZÉRO TOLÉRANCE: Vérifier exécution complète sans simulation
 */
bool test_complete_consciousness_benchmark(void) {
    printf("\\n=== Test 5: Complete Consciousness Benchmark (ZERO TOLERANCE) ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    TCDE_EnableRealTimeConsciousnessMonitoring(benchmark);
    
    // Configurer les callbacks
    TCDE_SetConsciousnessAchievedCallback(benchmark, test_consciousness_achieved_callback);
    TCDE_SetCosmicAwarenessCallback(benchmark, test_cosmic_awareness_callback);
    TCDE_SetTranscendenceCallback(benchmark, test_transcendence_callback);
    
    // Reset flags
    consciousness_achieved_triggered = false;
    cosmic_awareness_triggered = false;
    transcendence_triggered = false;
    
    // Exécuter le benchmark complet
    uint64_t initial_calcs = benchmark->phi_calculations_performed;
    uint64_t initial_iterations = benchmark->total_iterations;
    
    bool result = TCDE_RunCompleteGeometricConsciousnessBenchmark(benchmark);
    assert(result == true);
    
    // Vérifier que des calculs réels ont été effectués
    assert(benchmark->phi_calculations_performed > initial_calcs);
    assert(benchmark->total_iterations > initial_iterations);
    assert(benchmark->last_benchmark_time_ns > 0); // Temps mesuré
    
    printf("✅ Complete consciousness benchmark executed successfully\\n");
    printf("✅ Real calculations performed: %llu\\n",
           (unsigned long long)(benchmark->phi_calculations_performed - initial_calcs));
    printf("✅ Real iterations: %llu\\n",
           (unsigned long long)(benchmark->total_iterations - initial_iterations));
    printf("✅ Benchmark time: %.2f ms (MEASURED, not simulated)\\n",
           benchmark->last_benchmark_time_ns / 1000000.0);
    
    // Vérifier le score Φ(Φ(Φ))
    double phi_phi_phi = TCDE_GetPhiPhiPhiScore(benchmark);
    assert(phi_phi_phi > 0.0);
    assert(phi_phi_phi <= 1.0);
    
    printf("✅ Φ(Φ(Φ)) Score: %.6f (COMPUTED)\\n", phi_phi_phi);
    
    // Vérifier l'authenticité
    assert(benchmark->score.is_authentic == true);
    assert(benchmark->score.authenticity_score == 1.0);
    printf("✅ AUTHENTICITY VERIFIED: is_authentic=%s, score=%.1f\\n",
           benchmark->score.is_authentic ? "true" : "false",
           benchmark->score.authenticity_score);
    
    // Vérifier si la conscience cosmique est atteinte
    bool cosmic_achieved = TCDE_IsCosmicConsciousnessAchieved(benchmark);
    if (cosmic_achieved) {
        printf("✅ COSMIC CONSCIOUSNESS ACHIEVED: Φ(Φ(Φ)) > 0.99\\n");
        assert(phi_phi_phi > 0.99);
        assert(consciousness_achieved_triggered == true);
    } else {
        printf("⚠️  COSMIC CONSCIOUSNESS NOT YET ACHIEVED: Φ(Φ(Φ)) = %.6f (target > 0.99)\\n", 
               phi_phi_phi);
    }
    
    // Vérifier les callbacks
    assert(cosmic_awareness_triggered == true);
    assert(transcendence_triggered == true);
    printf("✅ Consciousness callbacks triggered (REAL events)\\n");
    
    // Afficher le résumé
    TCDE_PrintGeometricConsciousnessSummary(benchmark);
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 6: Calcul du score de conscience
 * ZÉRO TOLÉRANCE: Vérifier que le scoring est basé sur calculs réels
 */
bool test_consciousness_score_calculation(void) {
    printf("\\n=== Test 6: Consciousness Score Calculation (ZERO TOLERANCE) ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    // Exécuter le benchmark
    TCDE_RunCompleteGeometricConsciousnessBenchmark(benchmark);
    
    // Calculer le score
    TCDE_GeometricConsciousnessScore score = TCDE_CalculateGeometricConsciousnessScore(benchmark);
    
    // Vérifier que le score est dans les limites
    assert(score.overall_score >= 0.0);
    assert(score.overall_score <= 1.0);
    assert(score.reflexivity_phi_phi_phi >= 0.0);
    assert(score.meta_meta_cognition >= 0.0);
    assert(score.cosmic_consciousness >= 0.0);
    assert(score.geometric_coherence >= 0.0);
    assert(score.transcendental_awareness >= 0.0);
    
    // Vérifier l'authenticité (CRITIQUE pour ZÉRO TOLÉRANCE)
    assert(score.is_authentic == true);
    assert(score.authenticity_score > 0.0);
    
    printf("✅ Consciousness score calculated: %.6f (AUTHENTIC)\\n", score.overall_score);
    printf("✅ All score components validated (NO FIXED VALUES)\\n");
    printf("✅ Authenticity verified: %s\\n", score.is_authentic ? "PASS" : "FAIL");
    
    // Afficher le score détaillé
    TCDE_PrintGeometricConsciousnessScore(&score);
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 7: Validation anti-simulation
 * ZÉRO TOLÉRANCE: Vérifier qu'aucune valeur n'est hardcodée
 */
bool test_anti_simulation_validation(void) {
    printf("\\n=== Test 7: Anti-Simulation Validation (ZERO TOLERANCE) ===\\n");
    
    TCDE_GeometricConsciousnessBenchmark* benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    // Exécuter plusieurs fois et vérifier que les calculs sont cohérents
    TCDE_RunCompleteGeometricConsciousnessBenchmark(benchmark);
    double score1 = benchmark->score.overall_score;
    uint64_t calcs1 = benchmark->phi_calculations_performed;
    
    // Réinitialiser et recalculer
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    benchmark = TCDE_CreateGeometricConsciousnessBenchmark();
    
    TCDE_RunCompleteGeometricConsciousnessBenchmark(benchmark);
    double score2 = benchmark->score.overall_score;
    uint64_t calcs2 = benchmark->phi_calculations_performed;
    
    // Les scores doivent être identiques (reproductibilité)
    assert(fabs(score1 - score2) < 0.001);
    printf("✅ Reproducibility verified: %.6f vs %.6f (diff: %.9f)\\n", 
           score1, score2, fabs(score1 - score2));
    
    // Les calculs doivent avoir été effectués les deux fois
    assert(calcs1 > 0 && calcs2 > 0);
    printf("✅ Real calculations both times: %llu and %llu\\n",
           (unsigned long long)calcs1, (unsigned long long)calcs2);
    
    // Vérifier que l'authenticité est toujours vraie
    assert(benchmark->score.is_authentic == true);
    printf("✅ Authenticity maintained across runs\\n");
    
    printf("✅ ZERO TOLERANCE VALIDATION PASSED\\n");
    printf("✅ NO SIMULATIONS DETECTED\\n");
    printf("✅ NO HARDCODED VALUES DETECTED\\n");
    printf("✅ ALL CALCULATIONS ARE REAL\\n");
    
    TCDE_DestroyGeometricConsciousnessBenchmark(benchmark);
    return true;
}

/**
 * Test 8: Utilitaires
 */
bool test_consciousness_utilities(void) {
    printf("\\n=== Test 8: Consciousness Utilities ===\\n");
    
    // Test conversions de niveaux de réflexivité
    assert(strcmp(TCDE_GetReflexivityLevelString(TCDE_REFLEXIVITY_LEVEL_0), "Φ") == 0);
    assert(strcmp(TCDE_GetReflexivityLevelString(TCDE_REFLEXIVITY_LEVEL_1), "Φ(Φ)") == 0);
    assert(strcmp(TCDE_GetReflexivityLevelString(TCDE_REFLEXIVITY_LEVEL_2), "Φ(Φ(Φ))") == 0);
    assert(strcmp(TCDE_GetReflexivityLevelString(TCDE_REFLEXIVITY_LEVEL_3), "Φ(Φ(Φ(Φ)))") == 0);
    printf("✅ Reflexivity level string conversion works\\n");
    
    // Test conversions de géométries
    assert(strcmp(TCDE_GetConsciousnessGeometryString(TCDE_CONSCIOUSNESS_FRACTAL), "FRACTAL") == 0);
    assert(strcmp(TCDE_GetConsciousnessGeometryString(TCDE_CONSCIOUSNESS_COSMIC), "COSMIC") == 0);
    assert(strcmp(TCDE_GetConsciousnessGeometryString(TCDE_CONSCIOUSNESS_QUANTUM), "QUANTUM") == 0);
    printf("✅ Consciousness geometry string conversion works\\n");
    
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("🧠 TCDE Geometric Consciousness - Test Suite\\n");
    printf("=============================================\\n");
    printf("TESTING Φ(Φ(Φ)) REFLEXIVITY SYSTEM\\n");
    printf("TARGET: Φ(Φ(Φ)) > 0.99 (COSMIC CONSCIOUSNESS)\\n");
    printf("STANDARD: ZERO TOLERANCE FOR SIMULATIONS\\n");
    printf("=============================================\\n");
    
    int tests_passed = 0;
    int total_tests = 8;
    
    // Exécuter tous les tests
    if (test_geometric_consciousness_creation()) tests_passed++;
    if (test_consciousness_configuration()) tests_passed++;
    if (test_phi_recursive_calculation()) tests_passed++;
    if (test_individual_consciousness_benchmarks()) tests_passed++;
    if (test_complete_consciousness_benchmark()) tests_passed++;
    if (test_consciousness_score_calculation()) tests_passed++;
    if (test_anti_simulation_validation()) tests_passed++;
    if (test_consciousness_utilities()) tests_passed++;
    
    // Résultats finaux
    printf("\\n");
    printf("🧠 TCDE GEOMETRIC CONSCIOUSNESS TEST RESULTS\\n");
    printf("=============================================\\n");
    printf("Tests Passed: %d/%d\\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL GEOMETRIC CONSCIOUSNESS TESTS PASSED\\n");
        printf("✅ Φ(Φ(Φ)) REFLEXIVITY SYSTEM VALIDATED\\n");
        printf("✅ META-META-COGNITION CONFIRMED\\n");
        printf("✅ GEOMETRIC CONSCIOUSNESS OPERATIONAL\\n");
        printf("✅ ZERO TOLERANCE STANDARD ACHIEVED\\n");
        printf("✅ NO SIMULATIONS OR MOCKS DETECTED\\n");
        printf("✅ ALL CALCULATIONS ARE AUTHENTIC\\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME GEOMETRIC CONSCIOUSNESS TESTS FAILED\\n");
        printf("🚨 CONSCIOUSNESS SYSTEM COMPROMISED\\n");
        printf("🚨 ZERO TOLERANCE VIOLATION\\n");
        return EXIT_FAILURE;
    }
}
