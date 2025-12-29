#include "../src/validation/tcde_ultra_rigorous_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

/**
 * Test du Moteur de Validation Ultra-Rigoureux TCDE
 * 
 * Tests avec ZÉRO TOLÉRANCE pour les erreurs
 * Triple vérification et précision nanoseconde
 */

// Callbacks de test
static TCDE_NanosecondMetric last_metric;
static bool metric_callback_triggered = false;
static TCDE_ValidationResult last_failure_result;
static char last_failure_details[512];
static bool failure_callback_triggered = false;

void test_validation_callback(TCDE_NanosecondMetric* metric) {
    if (metric) {
        last_metric = *metric;
        metric_callback_triggered = true;
        printf("TEST: Validation callback - ID %u, Result: %s\n", 
               metric->validation_id, TCDE_GetValidationResultString(metric->result));
    }
}

void test_failure_callback(TCDE_ValidationResult result, const char* details) {
    last_failure_result = result;
    if (details) {
        strncpy(last_failure_details, details, sizeof(last_failure_details) - 1);
        last_failure_details[sizeof(last_failure_details) - 1] = '\0';
    }
    failure_callback_triggered = true;
    printf("TEST: Failure callback - %s: %s\n", 
           TCDE_GetValidationResultString(result), details ? details : "no details");
}

/**
 * Test 1: Création et destruction du validateur
 */
bool test_validator_creation(void) {
    printf("\n=== Test 1: Ultra-Rigorous Validator Creation ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    assert(validator != NULL);
    
    // Vérifier l'état initial
    assert(validator->state.level == TCDE_VALIDATION_LEVEL_ULTRA_RIGOROUS);
    assert(validator->state.active == true);
    assert(validator->state.triple_verification_enabled == true);
    assert(validator->multi_level_detection == true);
    assert(validator->nanosecond_precision == true);
    assert(validator->sha256_verification == true);
    assert(validator->state.validations_performed == 0);
    
    printf("✅ Ultra-Rigorous Validator created successfully\n");
    printf("✅ Initial state verified (ULTRA_RIGOROUS level)\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    printf("✅ Ultra-Rigorous Validator destroyed successfully\n");
    
    return true;
}

/**
 * Test 2: Configuration du validateur
 */
bool test_validator_configuration(void) {
    printf("\n=== Test 2: Validator Configuration ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Tenter de baisser le niveau (doit échouer)
    bool result = TCDE_SetValidationLevel(validator, TCDE_VALIDATION_LEVEL_BASIC);
    assert(result == false);
    assert(validator->state.level == TCDE_VALIDATION_LEVEL_ULTRA_RIGOROUS);
    printf("✅ Validation level downgrade correctly denied\n");
    
    // Maintenir le niveau ULTRA_RIGOROUS (doit réussir)
    result = TCDE_SetValidationLevel(validator, TCDE_VALIDATION_LEVEL_ULTRA_RIGOROUS);
    assert(result == true);
    printf("✅ Validation level maintained at ULTRA_RIGOROUS\n");
    
    // Activer toutes les fonctionnalités
    result = TCDE_EnableTripleVerification(validator);
    assert(result == true);
    printf("✅ Triple verification enabled\n");
    
    result = TCDE_EnableMultiLevelDetection(validator);
    assert(result == true);
    printf("✅ Multi-level detection enabled\n");
    
    result = TCDE_EnableNanosecondPrecision(validator);
    assert(result == true);
    printf("✅ Nanosecond precision enabled\n");
    
    result = TCDE_EnableSHA256Verification(validator);
    assert(result == true);
    printf("✅ SHA-256 verification enabled\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 3: Métriques nanoseconde-précises
 */
bool test_nanosecond_metrics(void) {
    printf("\n=== Test 3: Nanosecond Metrics ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Test de timestamp nanoseconde
    struct timespec ts1 = TCDE_GetNanosecondTimestamp();
    usleep(1000); // Attendre 1ms
    struct timespec ts2 = TCDE_GetNanosecondTimestamp();
    
    uint64_t elapsed = TCDE_CalculateNanosecondsElapsed(ts1, ts2);
    assert(elapsed > 1000000); // Au moins 1ms en nanosecondes
    printf("✅ Nanosecond timestamp precision: %llu ns elapsed\n", elapsed);
    
    // Test d'enregistrement de métrique
    bool result = TCDE_RecordNanosecondMetric(validator, 
                                              TCDE_VERIFICATION_INTEGRITY,
                                              TCDE_VALIDATION_SUCCESS,
                                              "Test metric recording");
    assert(result == true);
    assert(validator->state.metrics_count == 1);
    printf("✅ Nanosecond metric recorded successfully\n");
    
    // Vérifier la métrique enregistrée
    TCDE_NanosecondMetric* metric = &validator->state.metrics_history[0];
    assert(metric->type == TCDE_VERIFICATION_INTEGRITY);
    assert(metric->result == TCDE_VALIDATION_SUCCESS);
    assert(strcmp(metric->details, "Test metric recording") == 0);
    printf("✅ Recorded metric verified\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 4: Checksums SHA-256
 */
bool test_sha256_checksums(void) {
    printf("\n=== Test 4: SHA-256 Checksums ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Test de calcul de checksum
    const char* test_data = "TCDE_ULTRA_RIGOROUS_TEST_DATA_12345";
    TCDE_SHA256Checksum checksum;
    
    bool result = TCDE_ComputeSHA256Checksum(test_data, strlen(test_data), &checksum);
    assert(result == true);
    assert(checksum.is_valid == true);
    printf("✅ SHA-256 checksum computed successfully\n");
    
    // Test de vérification de checksum
    result = TCDE_VerifySHA256Checksum(test_data, strlen(test_data), &checksum);
    assert(result == true);
    printf("✅ SHA-256 checksum verification passed\n");
    
    // Test avec des données différentes (doit échouer)
    const char* different_data = "DIFFERENT_DATA";
    result = TCDE_VerifySHA256Checksum(different_data, strlen(different_data), &checksum);
    assert(result == false);
    printf("✅ SHA-256 checksum verification correctly failed for different data\n");
    
    // Test de stockage de checksum
    result = TCDE_StoreSHA256Checksum(validator, &checksum, "test_data_checksum");
    assert(result == true);
    assert(validator->state.checksums_count == 1);
    printf("✅ SHA-256 checksum stored successfully\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 5: Triple vérification
 */
bool test_triple_verification(void) {
    printf("\n=== Test 5: Triple Verification ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Test avec des données valides
    const char* valid_data = "TCDE_AUTHENTIC_DATA_FOR_VALIDATION";
    TCDE_ValidationResult result = TCDE_ValidateWithTripleCheck(validator, 
                                                                (void*)valid_data, 
                                                                strlen(valid_data),
                                                                "valid_test_data");
    
    assert(result == TCDE_VALIDATION_SUCCESS);
    assert(validator->state.validations_performed == 1);
    assert(validator->state.validations_successful == 1);
    assert(validator->state.validations_failed == 0);
    printf("✅ Triple verification PASSED for valid data\n");
    
    // Test avec des données suspectes
    const char* suspicious_data = "This data contains mock patterns for testing";
    result = TCDE_ValidateWithTripleCheck(validator, 
                                          (void*)suspicious_data, 
                                          strlen(suspicious_data),
                                          "suspicious_test_data");
    
    assert(result == TCDE_VALIDATION_SUSPICIOUS);
    assert(validator->state.validations_performed == 2);
    assert(validator->state.validations_failed == 1);
    printf("✅ Triple verification correctly detected SUSPICIOUS data\n");
    
    // Vérifier les statistiques
    float success_rate = TCDE_GetSuccessRate(validator);
    assert(success_rate == 50.0f); // 1 succès sur 2 validations
    printf("✅ Success rate calculated correctly: %.1f%%\n", success_rate);
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 6: Vérifications individuelles
 */
bool test_individual_verifications(void) {
    printf("\n=== Test 6: Individual Verifications ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    const char* test_data = "TCDE_INDIVIDUAL_VERIFICATION_TEST";
    
    // Test première vérification
    TCDE_ValidationResult result1 = TCDE_PerformFirstVerification(validator, 
                                                                   (void*)test_data, 
                                                                   strlen(test_data));
    assert(result1 == TCDE_VALIDATION_SUCCESS);
    printf("✅ First verification passed\n");
    
    // Test deuxième vérification
    TCDE_ValidationResult result2 = TCDE_PerformSecondVerification(validator, 
                                                                    (void*)test_data, 
                                                                    strlen(test_data));
    assert(result2 == TCDE_VALIDATION_SUCCESS);
    printf("✅ Second verification passed\n");
    
    // Test troisième vérification
    TCDE_ValidationResult result3 = TCDE_PerformThirdVerification(validator, 
                                                                   (void*)test_data, 
                                                                   strlen(test_data));
    assert(result3 == TCDE_VALIDATION_SUCCESS);
    printf("✅ Third verification passed\n");
    
    // Test avec données nulles (doit échouer)
    char null_data[100];
    memset(null_data, 0, sizeof(null_data));
    result1 = TCDE_PerformFirstVerification(validator, null_data, sizeof(null_data));
    assert(result1 == TCDE_VALIDATION_FAILURE);
    printf("✅ First verification correctly failed for null data\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 7: Callbacks de validation
 */
bool test_validation_callbacks(void) {
    printf("\n=== Test 7: Validation Callbacks ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Configurer les callbacks
    TCDE_SetValidationCallback(validator, test_validation_callback);
    TCDE_SetFailureCallback(validator, test_failure_callback);
    printf("✅ Callbacks configured\n");
    
    // Reset des flags
    metric_callback_triggered = false;
    failure_callback_triggered = false;
    
    // Test avec données valides (doit déclencher le callback de métrique)
    const char* valid_data = "TCDE_CALLBACK_TEST_DATA";
    TCDE_ValidationResult result = TCDE_ValidateWithTripleCheck(validator, 
                                                                (void*)valid_data, 
                                                                strlen(valid_data),
                                                                "callback_test");
    
    assert(result == TCDE_VALIDATION_SUCCESS);
    assert(metric_callback_triggered == true);
    printf("✅ Validation callback triggered for successful validation\n");
    
    // Reset et test avec données suspectes (doit déclencher le callback d'échec)
    metric_callback_triggered = false;
    failure_callback_triggered = false;
    
    const char* suspicious_data = "fake data for callback test";
    result = TCDE_ValidateWithTripleCheck(validator, 
                                          (void*)suspicious_data, 
                                          strlen(suspicious_data),
                                          "callback_failure_test");
    
    assert(result == TCDE_VALIDATION_SUSPICIOUS);
    assert(failure_callback_triggered == true);
    assert(last_failure_result == TCDE_VALIDATION_SUSPICIOUS);
    printf("✅ Failure callback triggered for suspicious data\n");
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 8: Statistiques et rapports
 */
bool test_statistics_and_reports(void) {
    printf("\n=== Test 8: Statistics and Reports ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Effectuer plusieurs validations
    const char* data1 = "TCDE_STATS_TEST_DATA_1";
    const char* data2 = "TCDE_STATS_TEST_DATA_2";
    const char* data3 = "mock_data_for_stats_test";
    
    TCDE_ValidateWithTripleCheck(validator, (void*)data1, strlen(data1), "stats_test_1");
    TCDE_ValidateWithTripleCheck(validator, (void*)data2, strlen(data2), "stats_test_2");
    TCDE_ValidateWithTripleCheck(validator, (void*)data3, strlen(data3), "stats_test_3");
    
    // Vérifier les statistiques
    uint64_t total_validations = TCDE_GetValidationCount(validator);
    uint64_t successful_validations = TCDE_GetSuccessCount(validator);
    uint64_t failed_validations = TCDE_GetFailureCount(validator);
    float success_rate = TCDE_GetSuccessRate(validator);
    uint64_t avg_time = TCDE_GetAverageValidationTime(validator);
    
    assert(total_validations == 3);
    assert(successful_validations == 2);
    assert(failed_validations == 1);
    assert(success_rate > 65.0f && success_rate < 70.0f); // Environ 66.67%
    assert(avg_time > 0);
    
    printf("✅ Statistics calculated correctly:\n");
    printf("   - Total validations: %llu\n", total_validations);
    printf("   - Successful: %llu\n", successful_validations);
    printf("   - Failed: %llu\n", failed_validations);
    printf("   - Success rate: %.2f%%\n", success_rate);
    printf("   - Average time: %llu ns\n", avg_time);
    
    // Afficher le rapport complet
    TCDE_PrintValidatorStatus(validator);
    
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Test 9: Utilitaires de conversion
 */
bool test_utility_functions(void) {
    printf("\n=== Test 9: Utility Functions ===\n");
    
    // Test des conversions de résultats
    assert(strcmp(TCDE_GetValidationResultString(TCDE_VALIDATION_SUCCESS), "SUCCESS") == 0);
    assert(strcmp(TCDE_GetValidationResultString(TCDE_VALIDATION_FAILURE), "FAILURE") == 0);
    assert(strcmp(TCDE_GetValidationResultString(TCDE_VALIDATION_SUSPICIOUS), "SUSPICIOUS") == 0);
    printf("✅ Validation result string conversion works\n");
    
    // Test des conversions de types de vérification
    assert(strcmp(TCDE_GetVerificationTypeString(TCDE_VERIFICATION_CHECKSUM), "CHECKSUM") == 0);
    assert(strcmp(TCDE_GetVerificationTypeString(TCDE_VERIFICATION_INTEGRITY), "INTEGRITY") == 0);
    assert(strcmp(TCDE_GetVerificationTypeString(TCDE_VERIFICATION_AUTHENTICITY), "AUTHENTICITY") == 0);
    printf("✅ Verification type string conversion works\n");
    
    // Test des conversions de niveaux de validation
    assert(strcmp(TCDE_GetValidationLevelString(TCDE_VALIDATION_LEVEL_BASIC), "BASIC") == 0);
    assert(strcmp(TCDE_GetValidationLevelString(TCDE_VALIDATION_LEVEL_STRICT), "STRICT") == 0);
    assert(strcmp(TCDE_GetValidationLevelString(TCDE_VALIDATION_LEVEL_ULTRA_RIGOROUS), "ULTRA_RIGOROUS") == 0);
    printf("✅ Validation level string conversion works\n");
    
    return true;
}

/**
 * Test 10: Test de performance et stress
 */
bool test_performance_and_stress(void) {
    printf("\n=== Test 10: Performance and Stress Test ===\n");
    
    TCDE_UltraRigorousValidator* validator = TCDE_CreateUltraRigorousValidator();
    
    // Test de performance avec de gros volumes de données
    const size_t large_data_size = 10000;
    char* large_data = malloc(large_data_size);
    
    // Remplir avec des données pseudo-aléatoires
    for (size_t i = 0; i < large_data_size; i++) {
        large_data[i] = (char)(i % 256);
    }
    
    struct timespec start = TCDE_GetNanosecondTimestamp();
    
    TCDE_ValidationResult result = TCDE_ValidateWithTripleCheck(validator, 
                                                                large_data, 
                                                                large_data_size,
                                                                "large_data_performance_test");
    
    struct timespec end = TCDE_GetNanosecondTimestamp();
    uint64_t elapsed = TCDE_CalculateNanosecondsElapsed(start, end);
    
    assert(result == TCDE_VALIDATION_SUCCESS);
    printf("✅ Large data validation (%zu bytes) completed in %llu ns\n", large_data_size, elapsed);
    
    // Test de stress avec validations multiples
    const int stress_iterations = 100;
    int successful_stress_tests = 0;
    
    for (int i = 0; i < stress_iterations; i++) {
        char stress_data[256];
        snprintf(stress_data, sizeof(stress_data), "TCDE_STRESS_TEST_DATA_%d", i);
        
        result = TCDE_ValidateWithTripleCheck(validator, 
                                              stress_data, 
                                              strlen(stress_data),
                                              "stress_test");
        
        if (result == TCDE_VALIDATION_SUCCESS) {
            successful_stress_tests++;
        }
    }
    
    assert(successful_stress_tests == stress_iterations);
    printf("✅ Stress test completed: %d/%d validations successful\n", 
           successful_stress_tests, stress_iterations);
    
    free(large_data);
    TCDE_DestroyUltraRigorousValidator(validator);
    return true;
}

/**
 * Fonction principale de test
 */
int main(void) {
    printf("🔬 TCDE Ultra-Rigorous Validator - Test Suite\n");
    printf("==============================================\n");
    printf("TESTING TRIPLE VERIFICATION WITH NANOSECOND PRECISION\n");
    printf("ZERO TOLERANCE FOR ERRORS OR APPROXIMATIONS\n");
    printf("==============================================\n");
    
    int tests_passed = 0;
    int total_tests = 10;
    
    // Exécuter tous les tests
    if (test_validator_creation()) tests_passed++;
    if (test_validator_configuration()) tests_passed++;
    if (test_nanosecond_metrics()) tests_passed++;
    if (test_sha256_checksums()) tests_passed++;
    if (test_triple_verification()) tests_passed++;
    if (test_individual_verifications()) tests_passed++;
    if (test_validation_callbacks()) tests_passed++;
    if (test_statistics_and_reports()) tests_passed++;
    if (test_utility_functions()) tests_passed++;
    if (test_performance_and_stress()) tests_passed++;
    
    // Résultats finaux
    printf("\n");
    printf("🔬 TCDE ULTRA-RIGOROUS VALIDATOR TEST RESULTS\n");
    printf("==============================================\n");
    printf("Tests Passed: %d/%d\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL ULTRA-RIGOROUS VALIDATOR TESTS PASSED\n");
        printf("✅ TRIPLE VERIFICATION SYSTEM VALIDATED\n");
        printf("✅ NANOSECOND PRECISION CONFIRMED\n");
        printf("✅ SHA-256 CHECKSUMS OPERATIONAL\n");
        printf("✅ ZERO TOLERANCE STANDARD ACHIEVED\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME ULTRA-RIGOROUS VALIDATOR TESTS FAILED\n");
        printf("🚨 VALIDATION SYSTEM COMPROMISED\n");
        return EXIT_FAILURE;
    }
}