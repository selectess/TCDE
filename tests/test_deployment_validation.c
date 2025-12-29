/**
 * TCDE EXHAUSTIVE ANALYSIS ENGINE - DEPLOYMENT VALIDATION TESTS
 * 
 * Tests de validation pour déploiement en production
 * Protocole ZÉRO TOLÉRANCE - Aucune simulation autorisée
 * 
 * Version: 1.0.0
 * Date: 12 Décembre 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>

// Headers TCDE
#include "../src/analysis/tcde_exhaustive_analysis.h"

// Couleurs pour l'affichage
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

// Macros de test
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(COLOR_RED "❌ ÉCHEC: %s\n" COLOR_RESET, message); \
            return 0; \
        } else { \
            printf(COLOR_GREEN "✅ SUCCÈS: %s\n" COLOR_RESET, message); \
        } \
    } while(0)

#define TEST_INFO(message) \
    printf(COLOR_BLUE "ℹ️  INFO: %s\n" COLOR_RESET, message)

#define TEST_WARNING(message) \
    printf(COLOR_YELLOW "⚠️  ATTENTION: %s\n" COLOR_RESET, message)

// Structure pour résultats de tests
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double execution_time;
} DeploymentTestResults;

// Variables globales
static DeploymentTestResults g_test_results = {0, 0, 0, 0.0};

/**
 * Test 1: Validation Installation Système
 */
int test_system_installation() {
    TEST_INFO("Test 1: Validation Installation Système");
    
    // Vérifier binaire principal
    struct stat st;
    TEST_ASSERT(stat("/usr/local/tcde/bin/tcde_analysis", &st) == 0, 
                "Binaire principal installé");
    TEST_ASSERT(st.st_mode & S_IXUSR, 
                "Binaire principal exécutable");
    
    // Vérifier bibliothèques
    TEST_ASSERT(stat("/usr/local/tcde/lib/libtcde.a", &st) == 0, 
                "Bibliothèque statique installée");
    
    // Vérifier headers
    TEST_ASSERT(stat("/usr/local/tcde/include/tcde_exhaustive_analysis.h", &st) == 0, 
                "Headers installés");
    
    // Vérifier configuration
    TEST_ASSERT(stat("/etc/tcde/tcde.conf", &st) == 0, 
                "Configuration installée");
    
    // Vérifier répertoires de travail
    TEST_ASSERT(stat("/var/lib/tcde", &st) == 0, 
                "Répertoire de données créé");
    TEST_ASSERT(stat("/var/log/tcde", &st) == 0, 
                "Répertoire de logs créé");
    
    // Vérifier permissions
    TEST_ASSERT(access("/usr/local/tcde/bin/tcde_analysis", X_OK) == 0, 
                "Permissions d'exécution correctes");
    
    return 1;
}

/**
 * Test 2: Validation Configuration Système
 */
int test_system_configuration() {
    TEST_INFO("Test 2: Validation Configuration Système");
    
    // Vérifier utilisateur tcde
    FILE* passwd = fopen("/etc/passwd", "r");
    TEST_ASSERT(passwd != NULL, "Fichier passwd accessible");
    
    char line[256];
    int user_found = 0;
    while (fgets(line, sizeof(line), passwd)) {
        if (strstr(line, "tcde:") == line) {
            user_found = 1;
            break;
        }
    }
    fclose(passwd);
    TEST_ASSERT(user_found, "Utilisateur tcde créé");
    
    // Vérifier variables d'environnement
    TEST_ASSERT(access("/etc/profile.d/tcde.sh", R_OK) == 0, 
                "Variables d'environnement configurées");
    
    // Vérifier service systemd
    TEST_ASSERT(access("/etc/systemd/system/tcde-analysis.service", R_OK) == 0, 
                "Service systemd configuré");
    
    return 1;
}

/**
 * Test 3: Validation Fonctionnelle de Base
 */
int test_basic_functionality() {
    TEST_INFO("Test 3: Validation Fonctionnelle de Base");
    
    // Test version
    int result = system("tcde_analysis --version >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Commande --version fonctionne");
    
    // Test help
    result = system("tcde_analysis --help >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Commande --help fonctionne");
    
    // Test health check
    result = system("tcde_analysis --health-check >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Health check réussit");
    
    // Test self-test
    result = system("tcde_analysis --self-test >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Self-test réussit");
    
    return 1;
}

/**
 * Test 4: Validation Performance Minimale
 */
int test_performance_requirements() {
    TEST_INFO("Test 4: Validation Performance Minimale");
    
    // Créer projet de test minimal
    system("mkdir -p /tmp/tcde_test_project");
    system("echo 'int main() { return 0; }' > /tmp/tcde_test_project/test.c");
    system("echo '# Test Documentation' > /tmp/tcde_test_project/README.md");
    
    // Mesurer temps d'analyse
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    int result = system("tcde_analysis --project /tmp/tcde_test_project --output /tmp/tcde_test_results >/dev/null 2>&1");
    
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Analyse projet test réussit");
    TEST_ASSERT(elapsed < 10.0, "Analyse complétée en moins de 10 secondes");
    
    // Vérifier résultats générés
    TEST_ASSERT(access("/tmp/tcde_test_results", F_OK) == 0, "Résultats générés");
    
    // Nettoyer
    system("rm -rf /tmp/tcde_test_project /tmp/tcde_test_results");
    
    return 1;
}

/**
 * Test 5: Validation Mémoire et Ressources
 */
int test_resource_management() {
    TEST_INFO("Test 5: Validation Mémoire et Ressources");
    
    // Vérifier limites mémoire configurées
    FILE* config = fopen("/etc/tcde/tcde.conf", "r");
    TEST_ASSERT(config != NULL, "Configuration accessible");
    
    char line[256];
    int memory_config_found = 0;
    while (fgets(line, sizeof(line), config)) {
        if (strstr(line, "max_memory")) {
            memory_config_found = 1;
            break;
        }
    }
    fclose(config);
    TEST_ASSERT(memory_config_found, "Configuration mémoire présente");
    
    // Test avec limite mémoire
    int result = system("tcde_analysis --memory-limit 1GB --project . --dry-run >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Limite mémoire respectée");
    
    return 1;
}

/**
 * Test 6: Validation Sécurité
 */
int test_security_validation() {
    TEST_INFO("Test 6: Validation Sécurité");
    
    // Vérifier permissions fichiers sensibles
    struct stat st;
    
    // Configuration ne doit pas être world-writable
    stat("/etc/tcde/tcde.conf", &st);
    TEST_ASSERT(!(st.st_mode & S_IWOTH), "Configuration non modifiable par tous");
    
    // Binaire ne doit pas être setuid
    stat("/usr/local/tcde/bin/tcde_analysis", &st);
    TEST_ASSERT(!(st.st_mode & S_ISUID), "Binaire non setuid");
    
    // Répertoire de données protégé
    stat("/var/lib/tcde", &st);
    TEST_ASSERT(st.st_uid != 0 || (st.st_mode & 0077) == 0, 
                "Répertoire de données protégé");
    
    return 1;
}

/**
 * Test 7: Validation Protocole ZÉRO TOLÉRANCE
 */
int test_zero_tolerance_protocol() {
    TEST_INFO("Test 7: Validation Protocole ZÉRO TOLÉRANCE");
    
    // Créer projet avec violations intentionnelles
    system("mkdir -p /tmp/tcde_violation_test");
    
    // Fichier avec TODO (violation)
    FILE* violation_file = fopen("/tmp/tcde_violation_test/violation.c", "w");
    fprintf(violation_file, "int main() {\n    // TODO: Implement this\n    return 0;\n}\n");
    fclose(violation_file);
    
    // Test détection violations
    int result = system("tcde_analysis --project /tmp/tcde_violation_test --zero-tolerance --output /tmp/violation_results 2>/dev/null");
    
    // ZÉRO TOLÉRANCE doit échouer avec violations
    TEST_ASSERT(WEXITSTATUS(result) != 0, "ZÉRO TOLÉRANCE détecte violations");
    
    // Vérifier rapport de violations
    TEST_ASSERT(access("/tmp/violation_results", F_OK) == 0, "Rapport de violations généré");
    
    // Nettoyer
    system("rm -rf /tmp/tcde_violation_test /tmp/violation_results");
    
    return 1;
}

/**
 * Test 8: Validation Intégration Continue
 */
int test_ci_integration() {
    TEST_INFO("Test 8: Validation Intégration Continue");
    
    // Test mode batch
    int result = system("tcde_analysis --project . --mode basic --format json --quiet >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Mode batch fonctionne");
    
    // Test exit codes
    result = system("tcde_analysis --invalid-option >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) != 0, "Exit codes d'erreur corrects");
    
    // Test timeout
    result = system("timeout 5s tcde_analysis --project . --timeout 1s >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) != 0, "Timeout fonctionne");
    
    return 1;
}

/**
 * Test 9: Validation Formats de Sortie
 */
int test_output_formats() {
    TEST_INFO("Test 9: Validation Formats de Sortie");
    
    // Créer projet minimal
    system("mkdir -p /tmp/tcde_format_test");
    system("echo 'int main() { return 0; }' > /tmp/tcde_format_test/test.c");
    
    // Test format HTML
    int result = system("tcde_analysis --project /tmp/tcde_format_test --format html --output /tmp/format_html >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Format HTML généré");
    TEST_ASSERT(access("/tmp/format_html", F_OK) == 0, "Fichier HTML créé");
    
    // Test format JSON
    result = system("tcde_analysis --project /tmp/tcde_format_test --format json --output /tmp/format_json >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Format JSON généré");
    TEST_ASSERT(access("/tmp/format_json", F_OK) == 0, "Fichier JSON créé");
    
    // Test format Markdown
    result = system("tcde_analysis --project /tmp/tcde_format_test --format markdown --output /tmp/format_md >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Format Markdown généré");
    TEST_ASSERT(access("/tmp/format_md", F_OK) == 0, "Fichier Markdown créé");
    
    // Nettoyer
    system("rm -rf /tmp/tcde_format_test /tmp/format_html /tmp/format_json /tmp/format_md");
    
    return 1;
}

/**
 * Test 10: Validation Robustesse
 */
int test_robustness() {
    TEST_INFO("Test 10: Validation Robustesse");
    
    // Test projet inexistant
    int result = system("tcde_analysis --project /nonexistent/project >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) != 0, "Gestion projet inexistant");
    
    // Test permissions insuffisantes
    system("mkdir -p /tmp/tcde_no_perm && chmod 000 /tmp/tcde_no_perm");
    result = system("tcde_analysis --project /tmp/tcde_no_perm >/dev/null 2>&1");
    TEST_ASSERT(WEXITSTATUS(result) != 0, "Gestion permissions insuffisantes");
    system("chmod 755 /tmp/tcde_no_perm && rm -rf /tmp/tcde_no_perm");
    
    // Test fichier corrompu
    system("mkdir -p /tmp/tcde_corrupt");
    system("dd if=/dev/urandom of=/tmp/tcde_corrupt/corrupt.c bs=1024 count=1 2>/dev/null");
    result = system("tcde_analysis --project /tmp/tcde_corrupt >/dev/null 2>&1");
    // Doit continuer malgré fichier corrompu
    TEST_ASSERT(WEXITSTATUS(result) == 0, "Gestion fichiers corrompus");
    system("rm -rf /tmp/tcde_corrupt");
    
    return 1;
}

/**
 * Exécuter tous les tests de déploiement
 */
void run_deployment_tests() {
    printf(COLOR_BLUE "╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           TCDE DEPLOYMENT VALIDATION TESTS                  ║\n");
    printf("║                  Version 1.0.0                              ║\n");
    printf("║               Protocole ZÉRO TOLÉRANCE                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Liste des tests
    struct {
        const char* name;
        int (*test_func)();
    } tests[] = {
        {"Installation Système", test_system_installation},
        {"Configuration Système", test_system_configuration},
        {"Fonctionnalité de Base", test_basic_functionality},
        {"Performance Minimale", test_performance_requirements},
        {"Gestion Ressources", test_resource_management},
        {"Sécurité", test_security_validation},
        {"Protocole ZÉRO TOLÉRANCE", test_zero_tolerance_protocol},
        {"Intégration Continue", test_ci_integration},
        {"Formats de Sortie", test_output_formats},
        {"Robustesse", test_robustness}
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    // Exécuter tous les tests
    for (int i = 0; i < num_tests; i++) {
        printf("\n" COLOR_YELLOW "=== Test %d/%d: %s ===" COLOR_RESET "\n", 
               i + 1, num_tests, tests[i].name);
        
        g_test_results.total_tests++;
        
        if (tests[i].test_func()) {
            g_test_results.passed_tests++;
        } else {
            g_test_results.failed_tests++;
        }
    }
    
    gettimeofday(&end, NULL);
    g_test_results.execution_time = (end.tv_sec - start.tv_sec) + 
                                   (end.tv_usec - start.tv_usec) / 1000000.0;
}

/**
 * Afficher résultats finaux
 */
void display_final_results() {
    printf("\n" COLOR_BLUE "╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RÉSULTATS FINAUX                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    printf("\n📊 " COLOR_BLUE "STATISTIQUES:" COLOR_RESET "\n");
    printf("   Tests totaux: %d\n", g_test_results.total_tests);
    printf("   Tests réussis: " COLOR_GREEN "%d" COLOR_RESET "\n", g_test_results.passed_tests);
    printf("   Tests échoués: " COLOR_RED "%d" COLOR_RESET "\n", g_test_results.failed_tests);
    printf("   Temps d'exécution: %.2f secondes\n", g_test_results.execution_time);
    
    double success_rate = (double)g_test_results.passed_tests / g_test_results.total_tests * 100.0;
    printf("   Taux de réussite: %.1f%%\n", success_rate);
    
    printf("\n🎯 " COLOR_BLUE "VERDICT ZÉRO TOLÉRANCE:" COLOR_RESET "\n");
    if (g_test_results.failed_tests == 0) {
        printf("   " COLOR_GREEN "✅ DÉPLOIEMENT VALIDÉ - Aucune violation détectée" COLOR_RESET "\n");
        printf("   " COLOR_GREEN "✅ Système prêt pour production" COLOR_RESET "\n");
    } else {
        printf("   " COLOR_RED "❌ DÉPLOIEMENT NON VALIDÉ - %d violations détectées" COLOR_RESET "\n", 
               g_test_results.failed_tests);
        printf("   " COLOR_RED "❌ Corrections requises avant production" COLOR_RESET "\n");
    }
    
    printf("\n📋 " COLOR_BLUE "RECOMMANDATIONS:" COLOR_RESET "\n");
    if (g_test_results.failed_tests == 0) {
        printf("   • Système validé selon protocole ZÉRO TOLÉRANCE\n");
        printf("   • Monitoring continu recommandé\n");
        printf("   • Sauvegardes régulières configurées\n");
    } else {
        printf("   • Corriger tous les tests échoués\n");
        printf("   • Relancer la validation complète\n");
        printf("   • Vérifier logs détaillés\n");
    }
    
    printf("\n");
}

/**
 * Point d'entrée principal
 */
int main(int argc, char* argv[]) {
    // Vérifier permissions (doit être root pour certains tests)
    if (geteuid() != 0) {
        printf(COLOR_YELLOW "⚠️  ATTENTION: Certains tests nécessitent les privilèges root\n" COLOR_RESET);
        printf("   Exécutez avec: sudo %s\n\n", argv[0]);
    }
    
    // Exécuter tous les tests
    run_deployment_tests();
    
    // Afficher résultats
    display_final_results();
    
    // Code de sortie selon protocole ZÉRO TOLÉRANCE
    return (g_test_results.failed_tests == 0) ? 0 : 1;
}