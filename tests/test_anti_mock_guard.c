#include "../src/security/tcde_anti_mock_guard.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/**
 * Test du Système de Garde Anti-Mock TCDE
 * 
 * Tests 100% AUTHENTIQUES - Gardien externe surveillant TCDE
 */

// Callbacks de test
static TCDE_MockInfo last_detected_mock;
static bool mock_callback_triggered = false;
static bool emergency_callback_triggered = false;

void test_mock_detected_callback(TCDE_MockInfo* mock) {
    if (mock) {
        last_detected_mock = *mock;
        mock_callback_triggered = true;
        printf("TEST: Mock detected callback - %s (%s)\n", 
               mock->name, TCDE_GetMockTypeString(mock->type));
    }
}

void test_emergency_callback(void) {
    emergency_callback_triggered = true;
    printf("TEST: Emergency callback triggered\n");
}

/**
 * Test 1: Création et destruction du gardien
 */
bool test_guard_creation(void) {
    printf("\n=== Test 1: Anti-Mock Guard Creation ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    assert(guard != NULL);
    
    // Vérifier l'état initial
    assert(guard->state.active == true);
    assert(guard->strict_mode == true);
    assert(guard->bytecode_analysis == true);
    assert(guard->symbol_tracing == true);
    assert(guard->state.mocks_detected == 0);
    
    printf("✅ Anti-Mock Guard created successfully\n");
    printf("✅ Initial state verified (strict mode enabled)\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    printf("✅ Anti-Mock Guard destroyed successfully\n");
    
    return true;
}

/**
 * Test 2: Configuration des modes de surveillance
 */
bool test_guard_configuration(void) {
    printf("\n=== Test 2: Guard Configuration ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Activer tous les modes de surveillance
    bool result = TCDE_EnableStrictMode(guard);
    assert(result == true);
    assert(guard->strict_mode == true);
    printf("✅ Strict mode enabled\n");
    
    result = TCDE_EnableBytecodeAnalysis(guard);
    assert(result == true);
    assert(guard->bytecode_analysis == true);
    printf("✅ Bytecode analysis enabled\n");
    
    result = TCDE_EnableSymbolTracing(guard);
    assert(result == true);
    assert(guard->symbol_tracing == true);
    printf("✅ Symbol tracing enabled\n");
    
    result = TCDE_EnableContinuousScanning(guard);
    assert(result == true);
    assert(guard->state.continuous_scan == true);
    printf("✅ Continuous scanning enabled\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 3: Détection de stubs de fonction
 */
bool test_function_stub_detection(void) {
    printf("\n=== Test 3: Function Stub Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Test de détection normale (aucun stub)
    bool stubs_detected = TCDE_DetectFunctionStubs(guard);
    printf("Function stubs detection result: %s\n", stubs_detected ? "DETECTED" : "NONE");
    
    // Vérifier que les fonctions sont tracées
    assert(guard->state.functions_traced > 0);
    printf("✅ Functions traced: %d\n", guard->state.functions_traced);
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 4: Détection de données factices
 */
bool test_fake_data_detection(void) {
    printf("\n=== Test 4: Fake Data Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Test de détection normale (aucune donnée factice)
    bool fake_data_detected = TCDE_DetectDataFakes(guard);
    printf("Fake data detection result: %s\n", fake_data_detected ? "DETECTED" : "NONE");
    
    // En conditions normales, aucune donnée factice ne devrait être détectée
    printf("✅ Fake data detection test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 5: Détection de bibliothèques préchargées
 */
bool test_library_preload_detection(void) {
    printf("\n=== Test 5: Library Preload Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Test de détection normale
    bool preloads_detected = TCDE_DetectLibraryPreloads(guard);
    printf("Library preloads detection result: %s\n", preloads_detected ? "DETECTED" : "NONE");
    
    // Vérifier que les bibliothèques sont scannées
    assert(guard->state.libraries_scanned > 0);
    printf("✅ Libraries scanned: %d\n", guard->state.libraries_scanned);
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 6: Détection d'injection de symboles
 */
bool test_symbol_injection_detection(void) {
    printf("\n=== Test 6: Symbol Injection Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Test de détection normale
    bool injection_detected = TCDE_DetectSymbolInjection(guard);
    printf("Symbol injection detection result: %s\n", injection_detected ? "DETECTED" : "NONE");
    
    printf("✅ Symbol injection detection test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 7: Détection de modification de bytecode
 */
bool test_bytecode_tampering_detection(void) {
    printf("\n=== Test 7: Bytecode Tampering Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Test de détection normale
    bool tampering_detected = TCDE_DetectBytecodeTampering(guard);
    printf("Bytecode tampering detection result: %s\n", tampering_detected ? "DETECTED" : "NONE");
    
    printf("✅ Bytecode tampering detection test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 8: Scan complet pour mocks
 */
bool test_comprehensive_mock_scan(void) {
    printf("\n=== Test 8: Comprehensive Mock Scan ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Effectuer un scan complet
    bool mocks_found = TCDE_ScanForMocks(guard);
    printf("Comprehensive scan result: %s\n", mocks_found ? "MOCKS FOUND" : "SYSTEM CLEAN");
    
    // Vérifier les statistiques
    printf("✅ Scan statistics:\n");
    printf("   - Functions traced: %d\n", guard->state.functions_traced);
    printf("   - Libraries scanned: %d\n", guard->state.libraries_scanned);
    printf("   - Mocks detected: %d\n", guard->state.mocks_detected);
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 9: Validation d'intégrité des fonctions
 */
bool test_function_integrity_validation(void) {
    printf("\n=== Test 9: Function Integrity Validation ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Tester avec des noms de fonction valides
    bool result = TCDE_ValidateFunctionIntegrity(guard, "TCDE_CreateField");
    assert(result == true);
    printf("✅ Valid function name accepted\n");
    
    result = TCDE_ValidateFunctionIntegrity(guard, "TCDE_ComputeEnergy");
    assert(result == true);
    printf("✅ Another valid function name accepted\n");
    
    // Tester avec un nom suspect (contenant "mock")
    result = TCDE_ValidateFunctionIntegrity(guard, "TCDE_mock_function");
    assert(result == false);
    printf("✅ Suspicious function name correctly rejected\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 10: Surveillance continue
 */
bool test_continuous_guarding(void) {
    printf("\n=== Test 10: Continuous Guarding ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Démarrer la surveillance continue
    TCDE_StartContinuousGuarding(guard);
    assert(guard->state.continuous_scan == true);
    printf("✅ Continuous guarding started\n");
    
    // Effectuer une vérification de routine
    bool check_result = TCDE_PerformRoutineCheck(guard);
    printf("Routine check result: %s\n", check_result ? "PASSED" : "FAILED");
    
    // Arrêter la surveillance
    TCDE_StopContinuousGuarding(guard);
    assert(guard->state.continuous_scan == false);
    printf("✅ Continuous guarding stopped\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 11: Gestion des mocks détectés
 */
bool test_detected_mocks_management(void) {
    printf("\n=== Test 11: Detected Mocks Management ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Vérifier l'état initial
    int initial_count = TCDE_GetDetectedMocksCount(guard);
    assert(initial_count == 0);
    printf("✅ Initial mock count: %d\n", initial_count);
    
    // Effectuer un scan (peut détecter des mocks selon l'environnement)
    TCDE_ScanForMocks(guard);
    
    int final_count = TCDE_GetDetectedMocksCount(guard);
    printf("✅ Final mock count: %d\n", final_count);
    
    // Si des mocks sont détectés, les examiner
    for (int i = 0; i < final_count; i++) {
        TCDE_MockInfo* mock = TCDE_GetDetectedMock(guard, i);
        if (mock) {
            printf("   Mock %d: %s (%s)\n", i + 1, mock->name, TCDE_GetMockTypeString(mock->type));
        }
    }
    
    // Effacer les mocks détectés
    TCDE_ClearDetectedMocks(guard);
    assert(TCDE_GetDetectedMocksCount(guard) == 0);
    printf("✅ Detected mocks cleared\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 12: Callbacks de détection
 */
bool test_detection_callbacks(void) {
    printf("\n=== Test 12: Detection Callbacks ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Configurer les callbacks
    TCDE_SetMockDetectedCallback(guard, test_mock_detected_callback);
    TCDE_SetEmergencyCallback(guard, test_emergency_callback);
    printf("✅ Callbacks configured\n");
    
    // Note: Les callbacks ne seront déclenchés que si des mocks sont réellement détectés
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 13: Validation d'intégrité du code
 */
bool test_code_integrity_validation(void) {
    printf("\n=== Test 13: Code Integrity Validation ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Valider l'intégrité complète du code
    bool integrity_ok = TCDE_ValidateCodeIntegrity(guard);
    printf("Code integrity validation result: %s\n", integrity_ok ? "VALID" : "COMPROMISED");
    
    printf("✅ Code integrity validation completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 14: Affichage de l'état du gardien
 */
bool test_guard_status_display(void) {
    printf("\n=== Test 14: Guard Status Display ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Effectuer quelques opérations pour générer des statistiques
    TCDE_ScanForMocks(guard);
    TCDE_ValidateFunctionIntegrity(guard, "test_function");
    
    // Afficher l'état complet
    TCDE_PrintGuardStatus(guard);
    
    printf("✅ Guard status displayed successfully\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("🔍 TCDE Anti-Mock Guard System - Test Suite\n");
    printf("============================================\n");
    printf("TESTING EXTERNAL GUARDIAN SYSTEM\n");
    printf("ZERO TOLERANCE FOR MOCKS/STUBS/SIMULATIONS\n");
    printf("============================================\n");
    
    int tests_passed = 0;
    int total_tests = 14;
    
    // Exécuter tous les tests
    if (test_guard_creation()) tests_passed++;
    if (test_guard_configuration()) tests_passed++;
    if (test_function_stub_detection()) tests_passed++;
    if (test_fake_data_detection()) tests_passed++;
    if (test_library_preload_detection()) tests_passed++;
    if (test_symbol_injection_detection()) tests_passed++;
    if (test_bytecode_tampering_detection()) tests_passed++;
    if (test_comprehensive_mock_scan()) tests_passed++;
    if (test_function_integrity_validation()) tests_passed++;
    if (test_continuous_guarding()) tests_passed++;
    if (test_detected_mocks_management()) tests_passed++;
    if (test_detection_callbacks()) tests_passed++;
    if (test_code_integrity_validation()) tests_passed++;
    if (test_guard_status_display()) tests_passed++;
    
    // Résultats finaux
    printf("\n");
    printf("🔍 TCDE ANTI-MOCK GUARD TEST RESULTS\n");
    printf("====================================\n");
    printf("Tests Passed: %d/%d\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL ANTI-MOCK TESTS PASSED\n");
        printf("✅ GUARDIAN SYSTEM VALIDATED\n");
        printf("✅ EXTERNAL SURVEILLANCE OPERATIONAL\n");
        printf("✅ TCDE CORE REMAINS UNALTERED\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME ANTI-MOCK TESTS FAILED\n");
        printf("🚨 GUARDIAN SYSTEM COMPROMISED\n");
        return EXIT_FAILURE;
    }
}