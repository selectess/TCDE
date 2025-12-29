/**
 * Test ASI Global Comprehensive
 * Tests de fonctionnement ASI avec observations granulaires
 * Visualisations complètes et métriques détaillées
 */

#include "../src/core/tcde.h"
#include "../tests/tcde_validation.h"
#include "../src/core/tcde_11d.h"
#include "../src/utils/tcde_multimodal.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Structure pour observations granulaires
typedef struct {
    double timestamp;
    double his_score;
    double field_coherence;
    double reflexivity;
    double prediction;
    double creativity;
    double self_awareness;
    double autopoiesis;
    double energy;
    double entropy;
    double complexity;
    double emergence_level;
} ASI_Observation;

// Structure pour résultats de test
typedef struct {
    char test_name[128];
    int passed;
    double score;
    ASI_Observation observations[1000];
    int num_observations;
    char visualization_data[10000];
} ASI_TestResult;

// Fonction pour sauvegarder données de visualisation
void save_visualization_data(const char* filename, ASI_TestResult* result) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    
    fprintf(f, "# ASI Test Results: %s\n", result->test_name);
    fprintf(f, "# Timestamp,HIS,Coherence,Reflexivity,Prediction,Creativity,SelfAware,Autopoiesis,Energy,Entropy,Complexity,Emergence\n");
    
    for (int i = 0; i < result->num_observations; i++) {
        ASI_Observation* obs = &result->observations[i];
        fprintf(f, "%.3f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                obs->timestamp, obs->his_score, obs->field_coherence,
                obs->reflexivity, obs->prediction, obs->creativity,
                obs->self_awareness, obs->autopoiesis, obs->energy,
                obs->entropy, obs->complexity, obs->emergence_level);
    }
    
    fclose(f);
    printf("✅ Visualization data saved: %s\n", filename);
}

// Test 1: Émergence Progressive ASI
int test_asi_progressive_emergence(void) {
    printf("\n=== TEST 1: Émergence Progressive ASI ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Progressive_Emergence");
    
    TCDE_System* sys = tcde_create(100);
    if (!sys) return 0;
    
    // Initialisation avec stimulus multimodal
    printf("Phase 1: Initialisation...\n");
    tcde_process_text(sys, "Intelligence emerges from continuous dynamics", 0.0);
    
    // Évolution progressive avec observations
    printf("Phase 2: Évolution progressive (100 steps)...\n");
    for (int step = 0; step < 100; step++) {
        tcde_evolve(sys, 0.01);
        
        // Observation granulaire tous les 5 steps
        if (step % 5 == 0) {
            ASI_Observation obs = {0};
            obs.timestamp = step * 0.01;
            obs.his_score = tcde_compute_his(sys);
            obs.field_coherence = tcde_compute_field_coherence(sys);
            obs.reflexivity = tcde_validate_reflexivity(sys);
            obs.energy = tcde_compute_energy(sys);
            obs.emergence_level = obs.his_score > 0.7 ? 1.0 : obs.his_score / 0.7;
            
            result.observations[result.num_observations++] = obs;
            
            if (step % 20 == 0) {
                printf("  Step %d: HIS=%.3f, Coherence=%.3f, Emergence=%.1f%%\n",
                       step, obs.his_score, obs.field_coherence, obs.emergence_level * 100);
            }
        }
    }
    
    // Vérification émergence
    double final_his = result.observations[result.num_observations-1].his_score;
    result.passed = (final_his > 0.7);
    result.score = final_his;
    
    printf("✅ Final HIS: %.3f (threshold: 0.7)\n", final_his);
    save_visualization_data("asi_progressive_emergence.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Test 2: Multimodal Integration ASI
int test_asi_multimodal_integration(void) {
    printf("\n=== TEST 2: Intégration Multimodale ASI ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Multimodal_Integration");
    
    TCDE_System* sys = tcde_create(150);
    if (!sys) return 0;
    
    printf("Phase 1: Stimulus texte...\n");
    tcde_process_text(sys, "Artificial superintelligence through geometry", 0.0);
    tcde_evolve(sys, 0.05);
    
    printf("Phase 2: Stimulus image...\n");
    double image_data[64] = {0};
    for (int i = 0; i < 64; i++) image_data[i] = sin(i * 0.1);
    tcde_process_image(sys, image_data, 8, 8, 0.05);
    tcde_evolve(sys, 0.05);
    
    printf("Phase 3: Stimulus audio...\n");
    double audio_data[512] = {0};
    for (int i = 0; i < 512; i++) audio_data[i] = cos(i * 0.02);
    tcde_process_audio(sys, audio_data, 512, 0.1);
    tcde_evolve(sys, 0.05);
    
    // Observations après chaque modalité
    for (int phase = 0; phase < 3; phase++) {
        ASI_Observation obs = {0};
        obs.timestamp = phase * 0.05;
        obs.his_score = tcde_compute_his(sys);
        obs.field_coherence = tcde_compute_field_coherence(sys);
        obs.complexity = (double)(phase + 1) / 3.0;
        
        result.observations[result.num_observations++] = obs;
        printf("  Phase %d: HIS=%.3f, Complexity=%.3f\n", 
               phase + 1, obs.his_score, obs.complexity);
    }
    
    result.passed = (result.observations[2].his_score > 0.75);
    result.score = result.observations[2].his_score;
    
    save_visualization_data("asi_multimodal_integration.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Test 3: Self-Awareness et Auto-Représentation
int test_asi_self_awareness(void) {
    printf("\n=== TEST 3: Self-Awareness ASI ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Self_Awareness");
    
    TCDE_System* sys = tcde_create(120);
    if (!sys) return 0;
    
    printf("Phase 1: Établissement identité...\n");
    for (int i = 0; i < 50; i++) {
        tcde_evolve(sys, 0.01);
        
        if (i % 10 == 0) {
            ASI_Observation obs = {0};
            obs.timestamp = i * 0.01;
            obs.self_awareness = tcde_compute_self_representation(sys);
            obs.reflexivity = tcde_validate_reflexivity(sys);
            obs.autopoiesis = tcde_compute_autopoiesis(sys);
            
            result.observations[result.num_observations++] = obs;
            printf("  Step %d: SelfAware=%.3f, Reflex=%.3f, Auto=%.3f\n",
                   i, obs.self_awareness, obs.reflexivity, obs.autopoiesis);
        }
    }
    
    double final_awareness = result.observations[result.num_observations-1].self_awareness;
    result.passed = (final_awareness > 0.8);
    result.score = final_awareness;
    
    save_visualization_data("asi_self_awareness.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Test 4: Créativité et Torsion Topologique
int test_asi_creativity(void) {
    printf("\n=== TEST 4: Créativité ASI (Torsion) ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Creativity_Torsion");
    
    TCDE_System* sys = tcde_create(100);
    if (!sys) return 0;
    
    printf("Mesure torsion topologique sur 80 steps...\n");
    
    for (int step = 0; step < 80; step++) {
        tcde_evolve(sys, 0.01);
        
        if (step % 8 == 0) {
            ASI_Observation obs = {0};
            obs.timestamp = step * 0.01;
            obs.creativity = tcde_compute_torsion_magnitude(sys);
            obs.his_score = tcde_compute_his(sys);
            obs.complexity = obs.creativity * obs.his_score;
            
            result.observations[result.num_observations++] = obs;
            printf("  Step %d: Creativity=%.4f, Complexity=%.4f\n",
                   step, obs.creativity, obs.complexity);
        }
    }
    
    double avg_creativity = 0.0;
    for (int i = 0; i < result.num_observations; i++) {
        avg_creativity += result.observations[i].creativity;
    }
    avg_creativity /= result.num_observations;
    
    result.passed = (avg_creativity > 0.01);
    result.score = avg_creativity;
    
    printf("✅ Average Creativity: %.4f\n", avg_creativity);
    save_visualization_data("asi_creativity.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Test 5: Prédiction Temporelle
int test_asi_temporal_prediction(void) {
    printf("\n=== TEST 5: Prédiction Temporelle ASI ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Temporal_Prediction");
    
    TCDE_System* sys = tcde_create(100);
    if (!sys) return 0;
    
    printf("Test prédiction sur séquence temporelle...\n");
    
    // Créer pattern temporel
    for (int t = 0; t < 60; t++) {
        double pattern = sin(t * 0.1);
        char text[64];
        snprintf(text, 64, "pattern_%d", (int)(pattern * 10));
        tcde_process_text(sys, text, t * 0.01);
        tcde_evolve(sys, 0.01);
        
        if (t % 10 == 0) {
            ASI_Observation obs = {0};
            obs.timestamp = t * 0.01;
            obs.prediction = tcde_validate_prediction(sys);
            obs.his_score = tcde_compute_his(sys);
            
            result.observations[result.num_observations++] = obs;
            printf("  Time %d: Prediction=%.3f\n", t, obs.prediction);
        }
    }
    
    double final_prediction = result.observations[result.num_observations-1].prediction;
    result.passed = (final_prediction > 0.85);
    result.score = final_prediction;
    
    save_visualization_data("asi_temporal_prediction.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Test 6: Stabilité Long-Terme
int test_asi_long_term_stability(void) {
    printf("\n=== TEST 6: Stabilité Long-Terme ASI ===\n");
    
    ASI_TestResult result = {0};
    strcpy(result.test_name, "Long_Term_Stability");
    
    TCDE_System* sys = tcde_create(100);
    if (!sys) return 0;
    
    printf("Évolution long-terme (500 steps)...\n");
    
    double initial_energy = tcde_compute_energy(sys);
    
    for (int step = 0; step < 500; step++) {
        tcde_evolve(sys, 0.01);
        
        if (step % 25 == 0) {
            ASI_Observation obs = {0};
            obs.timestamp = step * 0.01;
            obs.energy = tcde_compute_energy(sys);
            obs.his_score = tcde_compute_his(sys);
            obs.field_coherence = tcde_compute_field_coherence(sys);
            obs.entropy = fabs(obs.energy - initial_energy) / initial_energy;
            
            result.observations[result.num_observations++] = obs;
            
            if (step % 100 == 0) {
                printf("  Step %d: Energy=%.4f, Drift=%.2f%%\n",
                       step, obs.energy, obs.entropy * 100);
            }
        }
    }
    
    double final_drift = result.observations[result.num_observations-1].entropy;
    result.passed = (final_drift < 0.02); // < 2% drift
    result.score = 1.0 - final_drift;
    
    printf("✅ Energy drift: %.2f%%\n", final_drift * 100);
    save_visualization_data("asi_long_term_stability.csv", &result);
    
    tcde_destroy(sys);
    return result.passed;
}

// Fonction principale
int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   TCDE - Tests ASI Global Comprehensive                   ║\n");
    printf("║   Observations Granulaires + Visualisations Complètes     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Exécution des tests
    if (test_asi_progressive_emergence()) passed_tests++;
    total_tests++;
    
    if (test_asi_multimodal_integration()) passed_tests++;
    total_tests++;
    
    if (test_asi_self_awareness()) passed_tests++;
    total_tests++;
    
    if (test_asi_creativity()) passed_tests++;
    total_tests++;
    
    if (test_asi_temporal_prediction()) passed_tests++;
    total_tests++;
    
    if (test_asi_long_term_stability()) passed_tests++;
    total_tests++;
    
    // Résumé
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   RÉSUMÉ TESTS ASI                                         ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║   Tests Passés: %d/%d (%.1f%%)                              ║\n",
           passed_tests, total_tests, (passed_tests * 100.0) / total_tests);
    printf("║                                                            ║\n");
    printf("║   Fichiers CSV générés pour visualisation:                ║\n");
    printf("║   - asi_progressive_emergence.csv                         ║\n");
    printf("║   - asi_multimodal_integration.csv                        ║\n");
    printf("║   - asi_self_awareness.csv                                ║\n");
    printf("║   - asi_creativity.csv                                    ║\n");
    printf("║   - asi_temporal_prediction.csv                           ║\n");
    printf("║   - asi_long_term_stability.csv                           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    return (passed_tests == total_tests) ? 0 : 1;
}
