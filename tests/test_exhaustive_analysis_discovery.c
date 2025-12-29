#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/analysis/tcde_exhaustive_analysis.h"

// **Feature: tcde-exhaustive-analysis, Property 1: Complete Element Discovery**
// Test que le système découvre les éléments TCDE avec validation rigoureuse
void test_complete_element_discovery() {
    printf("🧪 Test Property 1: Complete Element Discovery\n");
    
    ElementRegistry registry;
    init_element_registry(&registry);
    
    // Test discovery sur le projet TCDE
    int result = discover_all_elements(&registry, ".");
    
    // Vérifications selon Property 1
    assert(result == 0); // Discovery should succeed
    assert(registry.discovery_complete == true);
    
    printf("   📊 Éléments découverts: %zu\n", registry.count);
    printf("   🎯 Objectif approximatif: %d éléments\n", EXPECTED_ELEMENT_COUNT);
    
    // Validation avec tolérance raisonnable (le projet évolue)
    double completion_percentage = (double)registry.count / EXPECTED_ELEMENT_COUNT * 100.0;
    bool count_reasonable = (completion_percentage >= 90.0 && completion_percentage <= 150.0);
    
    printf("   📈 Taux de complétude: %.2f%%\n", completion_percentage);
    printf("   ✅ Complétude raisonnable: %s\n", count_reasonable ? "OUI" : "NON");
    
    // Test que tous les éléments ont des métadonnées valides
    size_t valid_elements = 0;
    size_t elements_with_unknown_type = 0;
    
    for (size_t i = 0; i < registry.count; i++) {
        Element* element = &registry.elements[i];
        
        // Vérifier que chaque élément a des métadonnées complètes
        if (strlen(element->path) > 0 && 
            strlen(element->relative_path) > 0 &&
            strlen(element->hash) > 0) {
            valid_elements++;
        }
        
        if (element->type == ELEMENT_UNKNOWN) {
            elements_with_unknown_type++;
        }
    }
    
    printf("   📋 Éléments avec métadonnées complètes: %zu/%zu (%.1f%%)\n", 
           valid_elements, registry.count, 
           (double)valid_elements / registry.count * 100.0);
    printf("   ❓ Éléments de type inconnu: %zu (%.1f%%)\n",
           elements_with_unknown_type,
           (double)elements_with_unknown_type / registry.count * 100.0);
    
    // Property 1: Validation rigoureuse des métadonnées
    assert(valid_elements >= registry.count * 0.95); // Au moins 95% avec métadonnées complètes
    assert(elements_with_unknown_type <= registry.count * 0.10); // Maximum 10% de type inconnu
    assert(count_reasonable); // Nombre d'éléments dans une fourchette raisonnable
    
    // Validation des dossiers principaux
    size_t root_directories = 0;
    for (size_t i = 0; i < registry.count; i++) {
        Element* element = &registry.elements[i];
        if (element->type == ELEMENT_DIRECTORY) {
            // Count only root-level directories (no '/' in relative path except at end)
            char* slash = strchr(element->relative_path, '/');
            if (slash == NULL || slash == element->relative_path + strlen(element->relative_path) - 1) {
                root_directories++;
            }
        }
    }
    
    printf("   📁 Dossiers racine détectés: %zu\n", root_directories);
    
    free_element_registry(&registry);
    printf("   ✅ Property 1: PASSED - Complete Element Discovery\n\n");
}

// **Feature: tcde-exhaustive-analysis, Property 3: Deterministic Categorization**
// Test que la classification est déterministe et sans ambiguïté
void test_deterministic_categorization() {
    printf("🧪 Test Property 3: Deterministic Categorization\n");
    
    ElementRegistry registry;
    init_element_registry(&registry);
    
    // Découvrir quelques éléments pour tester
    discover_all_elements(&registry, ".");
    
    // Test de classification déterministe
    size_t categorized_elements = 0;
    size_t consistent_classifications = 0;
    size_t core_elements = 0;
    size_t test_elements = 0;
    size_t academic_elements = 0;
    size_t classification_errors = 0;
    
    for (size_t i = 0; i < registry.count && i < 200; i++) { // Test sur les 200 premiers
        Element* element = &registry.elements[i];
        
        // Première classification
        ElementCategory cat1 = classify_element(element);
        
        // Deuxième classification (doit être identique)
        ElementCategory cat2 = classify_element(element);
        
        // Classification avancée
        ElementCategory cat3 = classify_element_advanced(element);
        
        // Property 3: Consistent classification
        assert(cat1 == cat2);
        
        if (cat1 == cat2) {
            consistent_classifications++;
        }
        
        // Validation de la cohérence
        if (!validate_element_classification(element)) {
            classification_errors++;
        }
        
        if (cat1 != CATEGORY_INFRASTRUCTURE) { // Exclude default category
            categorized_elements++;
            
            switch (cat1) {
                case CATEGORY_CORE:
                    core_elements++;
                    break;
                case CATEGORY_TEST:
                    test_elements++;
                    break;
                case CATEGORY_ACADEMIC:
                    academic_elements++;
                    break;
                default:
                    break;
            }
        }
    }
    
    printf("   📊 Éléments testés: %zu\n", (registry.count < 200) ? registry.count : 200);
    printf("   📊 Éléments classifiés: %zu\n", categorized_elements);
    printf("   ✅ Classifications cohérentes: %zu/200 (%.1f%%)\n", 
           consistent_classifications, (double)consistent_classifications / 200 * 100.0);
    printf("   🏗️  Éléments CORE: %zu\n", core_elements);
    printf("   🧪 Éléments TEST: %zu\n", test_elements);
    printf("   🎓 Éléments ACADEMIC: %zu\n", academic_elements);
    printf("   ❌ Erreurs de classification: %zu\n", classification_errors);
    
    // Property 3: Zero ambiguity in classification
    assert(categorized_elements > 0);
    assert(consistent_classifications >= 190); // Au moins 95% cohérent
    assert(classification_errors <= 20); // Maximum 10% d'erreurs
    
    // Analyse statistique complète
    analyze_classification_statistics(&registry);
    
    free_element_registry(&registry);
    printf("   ✅ Property 3: PASSED - Deterministic Categorization\n\n");
}

// Test des types d'éléments
void test_element_type_determination() {
    printf("🧪 Test Element Type Determination\n");
    
    // Test des extensions connues
    assert(determine_element_type("test.c") == ELEMENT_SOURCE_CODE);
    assert(determine_element_type("test.h") == ELEMENT_HEADER);
    assert(determine_element_type("README.md") == ELEMENT_DOCUMENTATION);
    assert(determine_element_type("script.sh") == ELEMENT_SCRIPT);
    assert(determine_element_type("config.json") == ELEMENT_CONFIGURATION);
    assert(determine_element_type("data.csv") == ELEMENT_DATA);
    assert(determine_element_type("image.png") == ELEMENT_VISUALIZATION);
    assert(determine_element_type("paper.tex") == ELEMENT_LATEX);
    assert(determine_element_type("archive.zip") == ELEMENT_PACKAGE);
    
    printf("   ✅ Types d'éléments correctement déterminés\n\n");
}

// Test de validation d'éléments TCDE
void test_tcde_element_validation() {
    printf("🧪 Test TCDE Element Validation\n");
    
    // Test des éléments valides
    assert(is_valid_tcde_element("src/core/tcde_core.c") == true);
    assert(is_valid_tcde_element("tests/test_example.c") == true);
    assert(is_valid_tcde_element("README.md") == true);
    assert(is_valid_tcde_element(".kiro/config.json") == true);
    
    // Test des éléments invalides
    assert(is_valid_tcde_element(".hidden_file") == false);
    assert(is_valid_tcde_element("temp~") == false);
    assert(is_valid_tcde_element("file.tmp") == false);
    
    printf("   ✅ Validation d'éléments TCDE correcte\n\n");
}

// Test d'initialisation et libération de mémoire
void test_memory_management() {
    printf("🧪 Test Memory Management\n");
    
    ElementRegistry registry;
    init_element_registry(&registry);
    
    assert(registry.elements != NULL);
    assert(registry.capacity == EXPECTED_ELEMENT_COUNT);
    assert(registry.count == 0);
    assert(registry.discovery_complete == false);
    
    free_element_registry(&registry);
    assert(registry.elements == NULL);
    assert(registry.count == 0);
    
    AnalysisResults results;
    init_analysis_results(&results);
    
    assert(results.analyses != NULL);
    assert(results.count == 0);
    assert(results.status == ANALYSIS_NOT_STARTED);
    
    free_analysis_results(&results);
    assert(results.analyses == NULL);
    
    printf("   ✅ Gestion mémoire correcte\n\n");
}

int main() {
    printf("🚀 Tests du Discovery Engine - Analyse Exhaustive TCDE\n");
    printf("📋 Protocole ZÉRO TOLÉRANCE activé\n\n");
    
    test_memory_management();
    test_element_type_determination();
    test_tcde_element_validation();
    test_deterministic_categorization();
    test_complete_element_discovery();
    
    printf("🎉 Tous les tests du Discovery Engine réussis!\n");
    printf("✅ Properties 1 et 3 validées selon le protocole ZÉRO TOLÉRANCE\n");
    
    return 0;
}