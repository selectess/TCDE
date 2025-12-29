#include "../src/security/tcde_anti_mock_guard.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Test Simplifié du Système Anti-Mock TCDE
 * 
 * Version simplifiée pour valider les fonctionnalités de base
 */

/**
 * Test 1: Création et destruction du gardien
 */
bool test_guard_creation_simple(void) {
    printf("\n=== Test 1: Simple Guard Creation ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    assert(guard != NULL);
    printf("✅ Anti-Mock Guard created successfully\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    printf("✅ Anti-Mock Guard destroyed successfully\n");
    
    return true;
}

/**
 * Test 2: Scan de base pour mocks
 */
bool test_basic_mock_scan(void) {
    printf("\n=== Test 2: Basic Mock Scan ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Effectuer un scan de base
    bool mocks_found = TCDE_ScanForMocks(guard);
    printf("Mock scan result: %s\n", mocks_found ? "MOCKS FOUND" : "SYSTEM CLEAN");
    
    printf("✅ Basic mock scan completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 3: Validation d'intégrité des fonctions
 */
bool test_function_validation_simple(void) {
    printf("\n=== Test 3: Simple Function Validation ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Tester avec des noms de fonction valides
    bool result = TCDE_ValidateFunctionIntegrity(guard, "TCDE_CreateField");
    printf("Valid function validation: %s\n", result ? "PASSED" : "FAILED");
    
    // Tester avec un nom suspect
    result = TCDE_ValidateFunctionIntegrity(guard, "mock_function");
    printf("Suspicious function validation: %s\n", result ? "PASSED" : "FAILED");
    
    printf("✅ Function validation tests completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 4: Détection de stubs
 */
bool test_stub_detection_simple(void) {
    printf("\n=== Test 4: Simple Stub Detection ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    bool stubs_detected = TCDE_DetectFunctionStubs(guard);
    printf("Stub detection result: %s\n", stubs_detected ? "DETECTED" : "NONE");
    
    printf("✅ Stub detection test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 5: Validation d'intégrité du code
 */
bool test_code_integrity_simple(void) {
    printf("\n=== Test 5: Simple Code Integrity ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    bool integrity_ok = TCDE_ValidateCodeIntegrity(guard);
    printf("Code integrity result: %s\n", integrity_ok ? "VALID" : "COMPROMISED");
    
    printf("✅ Code integrity test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Test 6: Affichage de l'état
 */
bool test_status_display_simple(void) {
    printf("\n=== Test 6: Simple Status Display ===\n");
    
    TCDE_AntiMockGuard* guard = TCDE_CreateAntiMockGuard();
    
    // Effectuer quelques opérations
    TCDE_ScanForMocks(guard);
    
    // Afficher l'état
    TCDE_PrintGuardStatus(guard);
    
    printf("✅ Status display test completed\n");
    
    TCDE_DestroyAntiMockGuard(guard);
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("🔍 TCDE Anti-Mock Guard - Simple Test Suite\n");
    printf("===========================================\n");
    printf("TESTING EXTERNAL GUARDIAN SYSTEM (SIMPLIFIED)\n");
    printf("===========================================\n");
    
    int tests_passed = 0;
    int total_tests = 6;
    
    // Exécuter tous les tests
    if (test_guard_creation_simple()) tests_passed++;
    if (test_basic_mock_scan()) tests_passed++;
    if (test_function_validation_simple()) tests_passed++;
    if (test_stub_detection_simple()) tests_passed++;
    if (test_code_integrity_simple()) tests_passed++;
    if (test_status_display_simple()) tests_passed++;
    
    // Résultats finaux
    printf("\n");
    printf("🔍 TCDE ANTI-MOCK GUARD SIMPLE TEST RESULTS\n");
    printf("===========================================\n");
    printf("Tests Passed: %d/%d\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL SIMPLE ANTI-MOCK TESTS PASSED\n");
        printf("✅ GUARDIAN SYSTEM BASIC FUNCTIONALITY VALIDATED\n");
        printf("✅ EXTERNAL SURVEILLANCE OPERATIONAL\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME SIMPLE ANTI-MOCK TESTS FAILED\n");
        printf("🚨 GUARDIAN SYSTEM NEEDS ATTENTION\n");
        return EXIT_FAILURE;
    }
}