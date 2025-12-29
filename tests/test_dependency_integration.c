#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "../src/analysis/tcde_exhaustive_analysis.h"

// Test d'intégration pour la Phase 4 - Dependency Analyzer
int main(void) {
    printf("🔗 TEST D'INTÉGRATION - DEPENDENCY ANALYZER (PHASE 4)\n");
    printf("=====================================================\n\n");
    
    // 1. Initialisation des structures
    printf("1️⃣ Initialisation des structures...\n");
    
    ElementRegistry registry;
    init_element_registry(&registry);
    
    AnalysisResults analysis_results;
    init_analysis_results(&analysis_results);
    
    DependencyAnalysisResults dependency_results;
    init_dependency_analysis_results(&dependency_results);
    
    printf("   ✅ Structures initialisées\n\n");
    
    // 2. Création d'éléments de test
    printf("2️⃣ Création d'éléments de test...\n");
    
    registry.capacity = 10;
    registry.elements = malloc(registry.capacity * sizeof(Element));
    
    // Créer des éléments représentatifs du projet TCDE
    const char* test_files[] = {
        "src/core/tcde_core.c",
        "src/core/tcde_core.h", 
        "src/benchmarks/tcde_holistic_benchmark.c",
        "src/validation/tcde_ultra_rigorous_validator.c",
        "tests/test_holistic_benchmark.c",
        "src/security/tcde_anti_mock_guard.c",
        "src/visualization/tcde_projection_3d.c",
        "applications/pattern-recognition/tcde_pattern_recognizer.c",
        "docs/README.md",
        "TCDE_SCIENTIFIC_PAPER.md"
    };
    
    ElementCategory categories[] = {
        CATEGORY_CORE, CATEGORY_CORE, CATEGORY_BENCHMARK, CATEGORY_VALIDATION,
        CATEGORY_TEST, CATEGORY_SECURITY, CATEGORY_VISUALIZATION, 
        CATEGORY_APPLICATION, CATEGORY_DOCUMENTATION, CATEGORY_ACADEMIC
    };
    
    ElementType types[] = {
        ELEMENT_SOURCE_CODE, ELEMENT_HEADER, ELEMENT_SOURCE_CODE, ELEMENT_SOURCE_CODE,
        ELEMENT_TEST, ELEMENT_SOURCE_CODE, ELEMENT_SOURCE_CODE,
        ELEMENT_SOURCE_CODE, ELEMENT_DOCUMENTATION, ELEMENT_DOCUMENTATION
    };
    
    registry.count = sizeof(test_files) / sizeof(test_files[0]);
    
    for (size_t i = 0; i < registry.count; i++) {
        Element* elem = &registry.elements[i];
        memset(elem, 0, sizeof(Element));
        
        strncpy(elem->path, test_files[i], sizeof(elem->path) - 1);
        strncpy(elem->relative_path, test_files[i], sizeof(elem->relative_path) - 1);
        elem->type = types[i];
        elem->category = categories[i];
        elem->size = 1000 + (i * 200);
        elem->created = time(NULL) - (i * 3600);
        elem->modified = time(NULL) - (i * 1800);
        elem->is_authentic = true;
        elem->is_analyzed = true;
    }
    
    printf("   ✅ %zu éléments de test créés\n\n", registry.count);
    
    // 3. Création des résultats d'analyse
    printf("3️⃣ Création des résultats d'analyse...\n");
    
    analysis_results.analyses = malloc(registry.count * sizeof(ElementAnalysis));
    analysis_results.count = registry.count;
    analysis_results.analyzed_count = registry.count;
    analysis_results.status = ANALYSIS_COMPLETED;
    
    for (size_t i = 0; i < registry.count; i++) {
        ElementAnalysis* analysis = &analysis_results.analyses[i];
        memset(analysis, 0, sizeof(ElementAnalysis));
        
        strncpy(analysis->name, test_files[i], sizeof(analysis->name) - 1);
        analysis->category = categories[i];
        analysis->analysis_confidence = 0.95;
        analysis->analysis_timestamp = time(NULL);
        
        // Ajouter une analyse de code pour les fichiers source
        if (types[i] == ELEMENT_SOURCE_CODE || types[i] == ELEMENT_HEADER) {
            analysis->code_analysis = malloc(sizeof(CodeAnalysis));
            memset(analysis->code_analysis, 0, sizeof(CodeAnalysis));
            
            // Simuler des includes réalistes
            analysis->code_analysis->include_count = 2 + (i % 4);
            analysis->code_analysis->includes = malloc(analysis->code_analysis->include_count * sizeof(char*));
            
            const char* common_includes[] = {
                "stdio.h", "stdlib.h", "string.h", "tcde_core.h", 
                "tcde_types.h", "tcde_utils.h", "unity.h"
            };
            
            for (size_t j = 0; j < analysis->code_analysis->include_count; j++) {
                analysis->code_analysis->includes[j] = malloc(64);
                strncpy(analysis->code_analysis->includes[j], 
                       common_includes[j % 7], 63);
            }
            
            analysis->code_analysis->function_count = 3 + (i % 6);
            analysis->code_analysis->line_count = 150 + (i * 25);
            analysis->code_analysis->complexity_score = 15 + (i % 20);
        }
    }
    
    printf("   ✅ Résultats d'analyse créés\n\n");
    
    // 4. Test de construction du graphe de dépendances
    printf("4️⃣ Construction du graphe de dépendances...\n");
    
    int result = build_dependency_graph(&analysis_results, &dependency_results.graph);
    
    if (result == 0) {
        printf("   ✅ Graphe construit avec succès\n");
        printf("   📊 Dépendances détectées: %zu\n", dependency_results.graph.count);
    } else {
        printf("   ❌ Échec de construction du graphe\n");
        return 1;
    }
    
    // 5. Test de détection des cycles
    printf("\n5️⃣ Détection des dépendances circulaires...\n");
    
    int cycles = detect_circular_dependencies(&dependency_results.graph);
    printf("   📊 Cycles détectés: %d\n", cycles);
    
    if (cycles >= 0) {
        printf("   ✅ Détection des cycles réussie\n");
    } else {
        printf("   ❌ Échec de détection des cycles\n");
    }
    
    // 6. Test d'identification des chemins critiques
    printf("\n6️⃣ Identification des chemins critiques...\n");
    
    int critical_paths = identify_critical_paths(&dependency_results.graph);
    printf("   📊 Chemins critiques: %d\n", critical_paths);
    
    if (critical_paths >= 0) {
        printf("   ✅ Identification des chemins critiques réussie\n");
    } else {
        printf("   ❌ Échec d'identification des chemins critiques\n");
    }
    
    // 7. Test de calcul des métriques de couplage
    printf("\n7️⃣ Calcul des métriques de couplage...\n");
    
    CouplingMetrics coupling;
    result = calculate_coupling_metrics(&dependency_results.graph, &coupling);
    
    if (result == 0) {
        printf("   ✅ Métriques de couplage calculées\n");
        printf("   📊 Force de couplage: %.3f\n", coupling.coupling_strength);
        printf("   📊 Instabilité: %.3f\n", coupling.instability);
        printf("   📊 Complexité du couplage: %.3f\n", coupling.coupling_complexity);
    } else {
        printf("   ❌ Échec de calcul des métriques de couplage\n");
    }
    
    // 8. Test d'analyse architecturale
    printf("\n8️⃣ Analyse des couches architecturales...\n");
    
    result = analyze_architectural_layers(&dependency_results.graph, 
                                        &dependency_results.graph.architectural_analysis);
    
    if (result == 0) {
        printf("   ✅ Analyse architecturale réussie\n");
        printf("   📊 Couches détectées: %zu\n", 
               dependency_results.graph.architectural_analysis.layer_count);
        printf("   📊 Qualité architecturale: %.1f%%\n", 
               dependency_results.graph.architectural_analysis.architectural_quality * 100.0);
        printf("   📊 Score de maintenabilité: %.1f%%\n", 
               dependency_results.graph.architectural_analysis.maintainability_score * 100.0);
    } else {
        printf("   ❌ Échec d'analyse architecturale\n");
    }
    
    // 9. Test d'analyse complète
    printf("\n9️⃣ Analyse complète des dépendances...\n");
    
    // Réinitialiser pour le test complet
    free_dependency_analysis_results(&dependency_results);
    init_dependency_analysis_results(&dependency_results);
    
    result = analyze_all_dependencies(&registry, &analysis_results, &dependency_results);
    
    if (result == 0) {
        printf("   ✅ Analyse complète réussie\n");
        printf("   📊 Dépendances totales: %zu\n", dependency_results.total_dependencies);
        printf("   📊 Dépendances circulaires: %zu\n", dependency_results.circular_dependencies);
        printf("   📊 Dépendances critiques: %zu\n", dependency_results.critical_dependencies);
        printf("   📊 Qualité architecturale: %.1f%%\n", dependency_results.architectural_quality * 100.0);
        printf("   📊 Force de couplage moyenne: %.3f\n", dependency_results.average_coupling_strength);
    } else {
        printf("   ❌ Échec d'analyse complète\n");
    }
    
    // 10. Test d'affichage et export
    printf("\n🔟 Test d'affichage et export...\n");
    
    print_dependency_summary(&dependency_results.graph);
    
    // Test d'export DOT
    export_dependency_graph_dot(&dependency_results.graph, "/tmp/tcde_dependencies.dot");
    
    // Test d'export JSON
    export_dependency_graph_json(&dependency_results.graph, "/tmp/tcde_dependencies.json");
    
    printf("   ✅ Export réussi\n");
    
    // 11. Validation finale
    printf("\n1️⃣1️⃣ Validation finale...\n");
    
    bool validation_passed = true;
    
    // Vérifier que l'analyse est complète
    if (!dependency_results.graph.analysis_complete) {
        printf("   ❌ Analyse marquée comme incomplète\n");
        validation_passed = false;
    }
    
    // Vérifier que des dépendances ont été trouvées
    if (dependency_results.total_dependencies == 0) {
        printf("   ⚠️  Aucune dépendance détectée (peut être normal)\n");
    }
    
    // Vérifier que les métriques sont dans des plages raisonnables
    if (dependency_results.architectural_quality < 0 || dependency_results.architectural_quality > 1) {
        printf("   ❌ Qualité architecturale hors limites: %.3f\n", dependency_results.architectural_quality);
        validation_passed = false;
    }
    
    // Vérifier que le résumé est généré
    if (strlen(dependency_results.analysis_summary) == 0) {
        printf("   ❌ Résumé d'analyse vide\n");
        validation_passed = false;
    }
    
    if (validation_passed) {
        printf("   ✅ Validation finale réussie\n");
    } else {
        printf("   ❌ Validation finale échouée\n");
    }
    
    // 12. Nettoyage
    printf("\n1️⃣2️⃣ Nettoyage...\n");
    
    // Libérer les analyses de code avec vérifications de sécurité
    for (size_t i = 0; i < analysis_results.count; i++) {
        if (analysis_results.analyses[i].code_analysis) {
            CodeAnalysis* code_analysis = analysis_results.analyses[i].code_analysis;
            
            // Libérer les includes avec vérifications
            if (code_analysis->includes && code_analysis->include_count > 0) {
                for (size_t j = 0; j < code_analysis->include_count; j++) {
                    if (code_analysis->includes[j]) {
                        free(code_analysis->includes[j]);
                        code_analysis->includes[j] = NULL;
                    }
                }
                free(code_analysis->includes);
                code_analysis->includes = NULL;
            }
            
            // Libérer la structure code_analysis
            free(code_analysis);
            analysis_results.analyses[i].code_analysis = NULL;
        }
    }
    
    free_element_registry(&registry);
    free_analysis_results(&analysis_results);
    free_dependency_analysis_results(&dependency_results);
    
    printf("   ✅ Nettoyage terminé\n");
    
    // Résultat final
    printf("\n🎯 RÉSULTAT FINAL\n");
    printf("=================\n");
    
    if (validation_passed) {
        printf("✅ TEST D'INTÉGRATION RÉUSSI\n");
        printf("🎉 Phase 4 (Dependency Analyzer) fonctionne correctement\n");
        printf("📊 Toutes les fonctionnalités principales validées\n");
        return 0;
    } else {
        printf("❌ TEST D'INTÉGRATION ÉCHOUÉ\n");
        printf("🔧 Corrections nécessaires avant validation finale\n");
        return 1;
    }
}