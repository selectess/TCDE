#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "../src/analysis/tcde_exhaustive_analysis.h"

// Test framework simple pour les propriétés
#define PROPERTY_TEST_ITERATIONS 100
#define MAX_TEST_ELEMENTS 50

/**
 * **Feature: tcde-exhaustive-analysis, Property 17: Detailed Issue Reporting**
 * For any validation results with quality issues, the system should generate 
 * detailed remediation reports with specific locations, descriptions, and correction procedures
 * **Validates: Requirements 3.5**
 */
void test_property_detailed_issue_reporting(void) {
    printf("🧪 Test Property 17: Detailed Issue Reporting\n");
    
    for (int iteration = 0; iteration < PROPERTY_TEST_ITERATIONS; iteration++) {
        // Créer des résultats d'analyse avec des problèmes de qualité
        AnalysisResults results;
        init_analysis_results(&results);
        
        // Générer des analyses avec différents niveaux de qualité
        size_t num_elements = rand() % MAX_TEST_ELEMENTS + 1;
        results.analyses = malloc(num_elements * sizeof(ElementAnalysis));
        results.count = num_elements;
        results.analyzed_count = num_elements;
        
        size_t issues_expected = 0;
        
        for (size_t i = 0; i < num_elements; i++) {
            ElementAnalysis* analysis = &results.analyses[i];
            
            // Générer des problèmes de qualité aléatoires
            analysis->quality.code_quality_score = (rand() % 100);
            analysis->authenticity.authenticity_score = (rand() % 100);
            analysis->authenticity.violation_count = rand() % 5;
            
            // Compter les problèmes attendus
            if (analysis->quality.code_quality_score < 80.0 || 
                analysis->authenticity.authenticity_score < 95.0 ||
                analysis->authenticity.violation_count > 0) {
                issues_expected++;
            }
            
            snprintf(analysis->name, sizeof(analysis->name), "test_element_%zu", i);
            analysis->category = CATEGORY_TEST;
        }
        
        // Générer le rapport de qualité
        Report quality_report;
        init_report(&quality_report);
        
        int ret = generate_quality_assessment_report(&results, &quality_report);
        assert(ret == 0);
        assert(quality_report.is_generated);
        assert(quality_report.section_count > 0);
        
        // Vérifier que le rapport contient des détails sur les problèmes
        bool has_issue_details = false;
        for (size_t i = 0; i < quality_report.section_count; i++) {
            if (strstr(quality_report.sections[i].content, "Faible qualité") != NULL ||
                strstr(quality_report.sections[i].content, "Recommandations") != NULL) {
                has_issue_details = true;
                break;
            }
        }
        
        if (issues_expected > 0) {
            assert(has_issue_details); // Le rapport doit contenir des détails sur les problèmes
        }
        
        // Générer le rapport d'authenticité
        Report auth_report;
        init_report(&auth_report);
        
        ret = generate_authenticity_validation_report(&results, &auth_report);
        assert(ret == 0);
        assert(auth_report.is_generated);
        
        // Vérifier que le rapport d'authenticité contient des actions requises
        bool has_remediation = false;
        for (size_t i = 0; i < auth_report.section_count; i++) {
            if (strstr(auth_report.sections[i].content, "Actions Requises") != NULL ||
                strstr(auth_report.sections[i].content, "Corriger") != NULL) {
                has_remediation = true;
                break;
            }
        }
        assert(has_remediation); // Le rapport doit toujours contenir des actions
        
        // Nettoyer
        free_report(&quality_report);
        free_report(&auth_report);
        free(results.analyses);
    }
    
    printf("✅ Property 17 validée sur %d itérations\n", PROPERTY_TEST_ITERATIONS);
}

/**
 * Test de la construction de base de connaissances
 * Vérifie que la base de connaissances est construite correctement
 */
void test_knowledge_base_construction(void) {
    printf("🧪 Test: Construction de base de connaissances\n");
    
    // Créer des résultats d'analyse de test
    AnalysisResults results;
    init_analysis_results(&results);
    
    size_t num_elements = 10;
    results.analyses = malloc(num_elements * sizeof(ElementAnalysis));
    results.count = num_elements;
    results.analyzed_count = num_elements;
    
    for (size_t i = 0; i < num_elements; i++) {
        ElementAnalysis* analysis = &results.analyses[i];
        snprintf(analysis->name, sizeof(analysis->name), "test_element_%zu", i);
        snprintf(analysis->description, sizeof(analysis->description), 
                "Description de l'élément de test %zu", i);
        analysis->category = (ElementCategory)(i % 5); // Varier les catégories
        analysis->quality.code_quality_score = 80.0 + (i * 2.0);
        analysis->authenticity.authenticity_score = 95.0 + i;
        analysis->analysis_confidence = 0.9;
    }
    
    // Construire la base de connaissances
    KnowledgeBase kb;
    init_knowledge_base(&kb);
    
    int ret = build_knowledge_base(&results, NULL, &kb);
    assert(ret == 0);
    assert(kb.count == num_elements);
    assert(kb.is_indexed);
    assert(kb.knowledge_completeness > 0.0);
    
    // Vérifier que les entrées sont correctement créées
    for (size_t i = 0; i < kb.count; i++) {
        const KnowledgeEntry* entry = &kb.entries[i];
        assert(strlen(entry->term) > 0);
        assert(strlen(entry->definition) > 0);
        assert(strlen(entry->category) > 0);
        assert(entry->relevance_score > 0.0);
    }
    
    printf("✅ Base de connaissances construite: %zu entrées\n", kb.count);
    
    // Nettoyer
    free_knowledge_base(&kb);
    free(results.analyses);
}

/**
 * Test de recherche dans la base de connaissances
 */
void test_knowledge_base_search(void) {
    printf("🧪 Test: Recherche dans la base de connaissances\n");
    
    // Créer une base de connaissances de test
    KnowledgeBase kb;
    init_knowledge_base(&kb);
    
    // Ajouter quelques entrées manuellement
    for (int i = 0; i < 5; i++) {
        KnowledgeEntry* entry = &kb.entries[kb.count];
        snprintf(entry->term, sizeof(entry->term), "test_term_%d", i);
        snprintf(entry->definition, sizeof(entry->definition), 
                "Définition du terme de test %d pour la recherche", i);
        strcpy(entry->category, "Test");
        entry->relevance_score = 0.8 + (i * 0.05);
        entry->related_elements = NULL;
        entry->related_count = 0;
        kb.count++;
    }
    
    // Construire l'index
    int ret = build_search_index(&kb);
    assert(ret == 0);
    assert(kb.is_indexed);
    assert(kb.search_index.total_terms == 5);
    
    // Tester la recherche
    SearchResults results;
    init_search_results(&results);
    
    ret = search_knowledge_base(&kb, "test_term_2", &results);
    assert(ret == 0);
    assert(results.count > 0);
    assert(strlen(results.query) > 0);
    assert(results.search_time_ms >= 0.0);
    
    // Vérifier que les résultats sont pertinents
    bool found_target = false;
    for (size_t i = 0; i < results.count; i++) {
        if (strstr(results.results[i].snippet, "terme de test 2") != NULL) {
            found_target = true;
            break;
        }
    }
    assert(found_target);
    
    printf("✅ Recherche effectuée: %zu résultats en %.2f ms\n", 
           results.count, results.search_time_ms);
    
    // Nettoyer
    free_search_results(&results);
    free_knowledge_base(&kb);
}

/**
 * Test de génération de rapports complets
 */
void test_complete_report_generation(void) {
    printf("🧪 Test: Génération de rapports complets\n");
    
    // Créer des données de test
    AnalysisResults results;
    init_analysis_results(&results);
    
    size_t num_elements = 20;
    results.analyses = malloc(num_elements * sizeof(ElementAnalysis));
    results.count = num_elements;
    results.analyzed_count = num_elements;
    results.failed_count = 0;
    
    for (size_t i = 0; i < num_elements; i++) {
        ElementAnalysis* analysis = &results.analyses[i];
        snprintf(analysis->name, sizeof(analysis->name), "element_%zu", i);
        analysis->category = (ElementCategory)(i % 6);
        analysis->quality.code_quality_score = 60.0 + (rand() % 40);
        analysis->authenticity.authenticity_score = 90.0 + (rand() % 10);
        analysis->authenticity.violation_count = rand() % 3;
    }
    
    // Générer une suite de rapports
    ReportSuite suite;
    init_report_suite(&suite);
    strcpy(suite.output_directory, "/tmp/tcde_reports");
    
    int ret = generate_all_reports(&results, NULL, NULL, &suite);
    assert(ret == 0);
    assert(suite.generation_complete);
    assert(suite.count > 0);
    assert(suite.overall_progress == 1.0);
    assert(strlen(suite.summary) > 0);
    
    // Vérifier que tous les rapports sont générés
    for (size_t i = 0; i < suite.count; i++) {
        const Report* report = &suite.reports[i];
        assert(report->is_generated);
        assert(strlen(report->title) > 0);
        assert(report->section_count > 0);
        assert(report->completeness_score > 0.0);
    }
    
    printf("✅ Suite de rapports générée: %zu rapports\n", suite.count);
    
    // Nettoyer
    free_report_suite(&suite);
    free(results.analyses);
}

/**
 * Test d'export de rapport
 */
void test_report_export(void) {
    printf("🧪 Test: Export de rapport\n");
    
    // Créer un rapport de test
    Report report;
    init_report(&report);
    
    strcpy(report.title, "Rapport de Test");
    strcpy(report.description, "Description du rapport de test");
    report.type = REPORT_EXECUTIVE_SUMMARY;
    report.generated_timestamp = time(NULL);
    report.is_generated = true;
    
    // Ajouter une section
    const char* content = "# Section de Test\n\nContenu de la section de test.";
    int ret = add_report_section(&report, "Section Test", content, FORMAT_MARKDOWN);
    assert(ret == 0);
    assert(report.section_count == 1);
    
    // Exporter le rapport
    const char* output_path = "/tmp/test_report.md";
    ret = export_report(&report, output_path);
    assert(ret == 0);
    
    // Vérifier que le fichier existe
    FILE* file = fopen(output_path, "r");
    assert(file != NULL);
    
    // Vérifier le contenu
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    
    assert(strstr(buffer, "Rapport de Test") != NULL);
    assert(strstr(buffer, "Section de Test") != NULL);
    
    printf("✅ Rapport exporté: %s\n", output_path);
    
    // Nettoyer
    free_report(&report);
    remove(output_path);
}

/**
 * Test de validation de complétude de rapport
 */
void test_report_completeness_validation(void) {
    printf("🧪 Test: Validation de complétude de rapport\n");
    
    Report report;
    init_report(&report);
    
    // Rapport incomplet
    int ret = validate_report_completeness(&report);
    assert(ret != 0); // Doit échouer car le rapport est vide
    
    // Ajouter du contenu
    strcpy(report.title, "Rapport Complet");
    report.is_generated = true;
    add_report_section(&report, "Section 1", "Contenu 1", FORMAT_MARKDOWN);
    
    ret = validate_report_completeness(&report);
    assert(ret == 0); // Doit réussir maintenant
    
    printf("✅ Validation de complétude testée\n");
    
    free_report(&report);
}

int main(void) {
    printf("🚀 Tests des propriétés Knowledge Base Engine & Reporting Engine\n");
    printf("================================================\n");
    
    // Initialiser le générateur aléatoire
    srand(time(NULL));
    
    // Tests de propriétés
    test_property_detailed_issue_reporting();
    
    // Tests unitaires
    test_knowledge_base_construction();
    test_knowledge_base_search();
    test_complete_report_generation();
    test_report_export();
    test_report_completeness_validation();
    
    printf("\n✅ Tous les tests Phase 5 réussis!\n");
    printf("📊 Knowledge Base Engine: FONCTIONNEL\n");
    printf("📄 Reporting Engine: FONCTIONNEL\n");
    printf("🎯 Phase 5 prête pour intégration\n");
    
    return 0;
}
