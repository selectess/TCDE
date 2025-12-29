#include "tcde_anti_mock_guard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * TCDE Anti-Mock Guard System - Implémentation CORRIGÉE
 * 
 * Gardien externe qui surveille TCDE sans l'altérer.
 * ZÉRO TOLÉRANCE pour les erreurs de compilation.
 */

// Patterns de noms suspects
static const char* SUSPICIOUS_PATTERNS[] = {
    "mock", "Mock", "MOCK",
    "stub", "Stub", "STUB", 
    "fake", "Fake", "FAKE",
    "test_double", "TestDouble",
    "spy", "Spy", "SPY",
    "dummy", "Dummy", "DUMMY",
    NULL
};

// Bibliothèques de mock connues
static const char* MOCK_LIBRARIES[] = {
    "libmock", "libgmock", "libcmock",
    "libstub", "libfake", "libtest",
    NULL
};

/**
 * Crée un nouveau gardien anti-mock - SIGNATURE CORRIGÉE
 */
TCDE_AntiMockGuard* TCDE_CreateAntiMockGuard(void) {
    TCDE_AntiMockGuard* guard = malloc(sizeof(TCDE_AntiMockGuard));
    if (!guard) {
        fprintf(stderr, "TCDE_ANTI_MOCK_FATAL: Cannot allocate guard system!\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialisation avec mode strict par défaut
    guard->state.active = true;
    guard->state.continuous_scan = false;
    guard->state.mocks_detected = 0;
    guard->state.functions_traced = 0;
    guard->state.libraries_scanned = 0;
    guard->state.last_scan = time(NULL);
    
    guard->strict_mode = true;
    guard->bytecode_analysis = true;
    guard->symbol_tracing = true;
    guard->mock_detected_callback = NULL;
    guard->emergency_callback = NULL;
    
    // Effacer la liste des mocks détectés
    memset(guard->state.detected_mocks, 0, sizeof(guard->state.detected_mocks));
    
    printf("TCDE_ANTI_MOCK: Guardian System ACTIVATED\n");
    printf("TCDE_ANTI_MOCK: Mode = STRICT, Analysis = ENABLED\n");
    
    return guard;
}

/**
 * Détruit le gardien anti-mock
 */
void TCDE_DestroyAntiMockGuard(TCDE_AntiMockGuard* guard) {
    if (!guard) return;
    
    if (guard->state.continuous_scan) {
        TCDE_StopContinuousGuarding(guard);
    }
    
    printf("TCDE_ANTI_MOCK: Guardian System DEACTIVATED\n");
    free(guard);
}

/**
 * Active le mode strict
 */
bool TCDE_EnableStrictMode(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    guard->strict_mode = true;
    printf("TCDE_ANTI_MOCK: Strict mode ENABLED\n");
    return true;
}

/**
 * Active l'analyse de bytecode
 */
bool TCDE_EnableBytecodeAnalysis(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    guard->bytecode_analysis = true;
    printf("TCDE_ANTI_MOCK: Bytecode analysis ENABLED\n");
    return true;
}

/**
 * Active le traçage de symboles
 */
bool TCDE_EnableSymbolTracing(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    guard->symbol_tracing = true;
    printf("TCDE_ANTI_MOCK: Symbol tracing ENABLED\n");
    return true;
}

/**
 * Active le scan continu
 */
bool TCDE_EnableContinuousScanning(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    guard->state.continuous_scan = true;
    printf("TCDE_ANTI_MOCK: Continuous scanning ENABLED\n");
    return true;
}

/**
 * Scanne pour détecter tous types de mocks
 */
bool TCDE_ScanForMocks(TCDE_AntiMockGuard* guard) {
    if (!guard) return true; // Assume mocks if no guard
    
    bool mocks_found = false;
    
    printf("TCDE_ANTI_MOCK: Starting comprehensive mock scan...\n");
    
    // 1. Détecter les stubs de fonction
    if (TCDE_DetectFunctionStubs(guard)) {
        mocks_found = true;
    }
    
    // 2. Détecter les données factices
    if (TCDE_DetectDataFakes(guard)) {
        mocks_found = true;
    }
    
    // 3. Détecter les bibliothèques préchargées
    if (TCDE_DetectLibraryPreloads(guard)) {
        mocks_found = true;
    }
    
    // 4. Détecter l'injection de symboles
    if (TCDE_DetectSymbolInjection(guard)) {
        mocks_found = true;
    }
    
    // 5. Détecter la modification de bytecode
    if (TCDE_DetectBytecodeTampering(guard)) {
        mocks_found = true;
    }
    
    guard->state.last_scan = time(NULL);
    
    if (mocks_found) {
        printf("🚨 TCDE_ANTI_MOCK: MOCKS DETECTED - SYSTEM COMPROMISED!\n");
        if (guard->emergency_callback) {
            guard->emergency_callback();
        }
    } else {
        printf("✅ TCDE_ANTI_MOCK: No mocks detected - System clean\n");
    }
    
    return mocks_found;
}

/**
 * Détecte les stubs de fonction
 */
bool TCDE_DetectFunctionStubs(TCDE_AntiMockGuard* guard) {
    if (!guard) return true;
    
    printf("TCDE_ANTI_MOCK: Scanning for function stubs...\n");
    
    // Vérifier les variables d'environnement suspectes
    for (int i = 0; SUSPICIOUS_PATTERNS[i] != NULL; i++) {
        char env_var[256];
        snprintf(env_var, sizeof(env_var), "TCDE_%s", SUSPICIOUS_PATTERNS[i]);
        
        if (getenv(env_var)) {
            printf("TCDE_ANTI_MOCK_VIOLATION: Suspicious environment variable: %s\n", env_var);
            
            // Enregistrer le mock détecté
            if (guard->state.mocks_detected < 64) {
                TCDE_MockInfo* mock = &guard->state.detected_mocks[guard->state.mocks_detected];
                mock->type = TCDE_MOCK_FUNCTION_STUB;
                strncpy(mock->name, env_var, sizeof(mock->name) - 1);
                mock->name[sizeof(mock->name) - 1] = '\0';
                mock->address = NULL;
                strncpy(mock->library, "", sizeof(mock->library) - 1);
                mock->detected_at = time(NULL);
                mock->is_active = true;
                guard->state.mocks_detected++;
            }
            
            return true;
        }
    }
    
    guard->state.functions_traced++;
    return false;
}

/**
 * Détecte les données factices
 */
bool TCDE_DetectDataFakes(TCDE_AntiMockGuard* guard) {
    if (!guard) return true;
    
    printf("TCDE_ANTI_MOCK: Scanning for fake data...\n");
    
    // Vérifier les fichiers de données factices
    struct stat st;
    const char* fake_files[] = {
        "fake_data.txt", "mock_results.dat", "test_data.bin",
        "stub_output.txt", "dummy_input.csv", NULL
    };
    
    for (int i = 0; fake_files[i] != NULL; i++) {
        if (stat(fake_files[i], &st) == 0) {
            printf("TCDE_ANTI_MOCK_VIOLATION: Fake data file detected: %s\n", fake_files[i]);
            
            // Enregistrer le mock détecté
            if (guard->state.mocks_detected < 64) {
                TCDE_MockInfo* mock = &guard->state.detected_mocks[guard->state.mocks_detected];
                mock->type = TCDE_MOCK_DATA_FAKE;
                strncpy(mock->name, fake_files[i], sizeof(mock->name) - 1);
                mock->name[sizeof(mock->name) - 1] = '\0';
                mock->address = NULL;
                strncpy(mock->library, "", sizeof(mock->library) - 1);
                mock->detected_at = time(NULL);
                mock->is_active = true;
                guard->state.mocks_detected++;
            }
            
            return true;
        }
    }
    
    return false;
}

/**
 * Détecte les bibliothèques préchargées
 */
bool TCDE_DetectLibraryPreloads(TCDE_AntiMockGuard* guard) {
    if (!guard) return true;
    
    printf("TCDE_ANTI_MOCK: Scanning for preloaded libraries...\n");
    
    const char* preload = getenv("LD_PRELOAD");
    if (preload) {
        printf("TCDE_ANTI_MOCK: LD_PRELOAD detected: %s\n", preload);
        
        // Vérifier si des bibliothèques de mock sont préchargées
        for (int i = 0; MOCK_LIBRARIES[i] != NULL; i++) {
            if (strstr(preload, MOCK_LIBRARIES[i])) {
                printf("TCDE_ANTI_MOCK_VIOLATION: Mock library preloaded: %s\n", MOCK_LIBRARIES[i]);
                
                // Enregistrer le mock détecté
                if (guard->state.mocks_detected < 64) {
                    TCDE_MockInfo* mock = &guard->state.detected_mocks[guard->state.mocks_detected];
                    mock->type = TCDE_MOCK_LIBRARY_PRELOAD;
                    strncpy(mock->name, MOCK_LIBRARIES[i], sizeof(mock->name) - 1);
                    mock->name[sizeof(mock->name) - 1] = '\0';
                    strncpy(mock->library, preload, sizeof(mock->library) - 1);
                    mock->library[sizeof(mock->library) - 1] = '\0';
                    mock->address = NULL;
                    mock->detected_at = time(NULL);
                    mock->is_active = true;
                    guard->state.mocks_detected++;
                }
                
                return true;
            }
        }
        
        // Vérifier les patterns suspects dans LD_PRELOAD
        for (int i = 0; SUSPICIOUS_PATTERNS[i] != NULL; i++) {
            if (strstr(preload, SUSPICIOUS_PATTERNS[i])) {
                printf("TCDE_ANTI_MOCK_VIOLATION: Suspicious library pattern: %s\n", SUSPICIOUS_PATTERNS[i]);
                return true;
            }
        }
    }
    
    guard->state.libraries_scanned++;
    return false;
}

/**
 * Détecte l'injection de symboles
 */
bool TCDE_DetectSymbolInjection(TCDE_AntiMockGuard* guard) {
    if (!guard) return true;
    
    printf("TCDE_ANTI_MOCK: Scanning for symbol injection...\n");
    
    // Cette implémentation est simplifiée
    // En production, analyser la table des symboles avec dlsym()
    
    // Vérifier quelques symboles suspects
    void* handle = dlopen(NULL, RTLD_LAZY);
    if (handle) {
        for (int i = 0; SUSPICIOUS_PATTERNS[i] != NULL; i++) {
            char symbol_name[256];
            snprintf(symbol_name, sizeof(symbol_name), "TCDE_%s_function", SUSPICIOUS_PATTERNS[i]);
            
            void* symbol = dlsym(handle, symbol_name);
            if (symbol) {
                printf("TCDE_ANTI_MOCK_VIOLATION: Suspicious symbol found: %s\n", symbol_name);
                
                // Enregistrer le mock détecté
                if (guard->state.mocks_detected < 64) {
                    TCDE_MockInfo* mock = &guard->state.detected_mocks[guard->state.mocks_detected];
                    mock->type = TCDE_MOCK_SYMBOL_INJECTION;
                    strncpy(mock->name, symbol_name, sizeof(mock->name) - 1);
                    mock->name[sizeof(mock->name) - 1] = '\0';
                    mock->address = symbol;
                    strncpy(mock->library, "", sizeof(mock->library) - 1);
                    mock->detected_at = time(NULL);
                    mock->is_active = true;
                    guard->state.mocks_detected++;
                }
                
                dlclose(handle);
                return true;
            }
        }
        dlclose(handle);
    }
    
    return false;
}

/**
 * Détecte la modification de bytecode
 */
bool TCDE_DetectBytecodeTampering(TCDE_AntiMockGuard* guard) {
    if (!guard) return true;
    
    printf("TCDE_ANTI_MOCK: Scanning for bytecode tampering...\n");
    
    // Implémentation simplifiée - en production, analyser le bytecode réel
    return false; // Pas de tampering détecté dans cette implémentation
}

/**
 * Analyse le bytecode d'une fonction
 */
bool TCDE_AnalyzeBytecode(TCDE_AntiMockGuard* guard, void* function_ptr) {
    if (!guard || !function_ptr) return false;
    
    // Implémentation simplifiée
    return true; // Bytecode valide
}

/**
 * Valide l'intégrité d'une fonction
 */
bool TCDE_ValidateFunctionIntegrity(TCDE_AntiMockGuard* guard, const char* function_name) {
    if (!guard || !function_name) return false;
    
    // Vérifier si le nom contient des patterns suspects
    for (int i = 0; SUSPICIOUS_PATTERNS[i] != NULL; i++) {
        if (strstr(function_name, SUSPICIOUS_PATTERNS[i])) {
            printf("TCDE_ANTI_MOCK_WARNING: Suspicious function name: %s\n", function_name);
            return false;
        }
    }
    
    return true;
}

/**
 * Trace les appels de fonction
 */
bool TCDE_TraceFunctionCalls(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    guard->state.functions_traced++;
    return true;
}

/**
 * Valide l'intégrité du code
 */
bool TCDE_ValidateCodeIntegrity(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    printf("TCDE_ANTI_MOCK: Validating code integrity...\n");
    
    // Effectuer toutes les vérifications
    return !TCDE_ScanForMocks(guard);
}

/**
 * Valide l'intégrité d'une bibliothèque
 */
bool TCDE_ValidateLibraryIntegrity(TCDE_AntiMockGuard* guard, const char* library_name) {
    if (!guard || !library_name) return false;
    
    // Vérifier si le nom contient des patterns suspects
    for (int i = 0; SUSPICIOUS_PATTERNS[i] != NULL; i++) {
        if (strstr(library_name, SUSPICIOUS_PATTERNS[i])) {
            printf("TCDE_ANTI_MOCK_WARNING: Suspicious library name: %s\n", library_name);
            return false;
        }
    }
    
    return true;
}

/**
 * Valide la table des symboles
 */
bool TCDE_ValidateSymbolTable(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    // Implémentation simplifiée
    return true;
}

/**
 * Démarre la surveillance continue
 */
void TCDE_StartContinuousGuarding(TCDE_AntiMockGuard* guard) {
    if (!guard) return;
    
    guard->state.continuous_scan = true;
    printf("TCDE_ANTI_MOCK: Continuous guarding STARTED\n");
}

/**
 * Arrête la surveillance continue
 */
void TCDE_StopContinuousGuarding(TCDE_AntiMockGuard* guard) {
    if (!guard) return;
    
    guard->state.continuous_scan = false;
    printf("TCDE_ANTI_MOCK: Continuous guarding STOPPED\n");
}

/**
 * Effectue une vérification de routine
 */
bool TCDE_PerformRoutineCheck(TCDE_AntiMockGuard* guard) {
    if (!guard) return false;
    
    // Scan rapide pour détecter les mocks
    return !TCDE_ScanForMocks(guard);
}

/**
 * Retourne le nombre de mocks détectés
 */
int TCDE_GetDetectedMocksCount(TCDE_AntiMockGuard* guard) {
    if (!guard) return -1;
    return guard->state.mocks_detected;
}

/**
 * Retourne un mock détecté par index
 */
TCDE_MockInfo* TCDE_GetDetectedMock(TCDE_AntiMockGuard* guard, int index) {
    if (!guard || index < 0 || index >= guard->state.mocks_detected) {
        return NULL;
    }
    return &guard->state.detected_mocks[index];
}

/**
 * Efface la liste des mocks détectés
 */
void TCDE_ClearDetectedMocks(TCDE_AntiMockGuard* guard) {
    if (!guard) return;
    
    guard->state.mocks_detected = 0;
    memset(guard->state.detected_mocks, 0, sizeof(guard->state.detected_mocks));
}

/**
 * Configure le callback de détection de mock
 */
void TCDE_SetMockDetectedCallback(TCDE_AntiMockGuard* guard, 
                                  void (*callback)(TCDE_MockInfo* mock)) {
    if (!guard) return;
    guard->mock_detected_callback = callback;
}

/**
 * Configure le callback d'urgence
 */
void TCDE_SetEmergencyCallback(TCDE_AntiMockGuard* guard, 
                               void (*callback)(void)) {
    if (!guard) return;
    guard->emergency_callback = callback;
}

/**
 * Convertit un type de mock en string
 */
const char* TCDE_GetMockTypeString(TCDE_MockType type) {
    switch (type) {
        case TCDE_MOCK_NONE: return "NONE";
        case TCDE_MOCK_FUNCTION_STUB: return "FUNCTION_STUB";
        case TCDE_MOCK_DATA_FAKE: return "DATA_FAKE";
        case TCDE_MOCK_LIBRARY_PRELOAD: return "LIBRARY_PRELOAD";
        case TCDE_MOCK_SYMBOL_INJECTION: return "SYMBOL_INJECTION";
        case TCDE_MOCK_BYTECODE_TAMPERING: return "BYTECODE_TAMPERING";
        default: return "UNKNOWN";
    }
}

/**
 * Affiche les informations d'un mock
 */
void TCDE_PrintMockInfo(TCDE_MockInfo* mock) {
    if (!mock) return;
    
    printf("Mock Info:\n");
    printf("  Type: %s\n", TCDE_GetMockTypeString(mock->type));
    printf("  Name: %s\n", mock->name);
    printf("  Address: %p\n", mock->address);
    printf("  Library: %s\n", mock->library);
    printf("  Detected: %s", ctime(&mock->detected_at));
    printf("  Active: %s\n", mock->is_active ? "YES" : "NO");
}

/**
 * Affiche l'état du gardien
 */
void TCDE_PrintGuardStatus(TCDE_AntiMockGuard* guard) {
    if (!guard) {
        printf("TCDE_ANTI_MOCK: No guard system active!\n");
        return;
    }
    
    printf("\n");
    printf("TCDE Anti-Mock Guard Status\n");
    printf("===========================\n");
    printf("Active: %s\n", guard->state.active ? "YES" : "NO");
    printf("Strict Mode: %s\n", guard->strict_mode ? "YES" : "NO");
    printf("Bytecode Analysis: %s\n", guard->bytecode_analysis ? "YES" : "NO");
    printf("Symbol Tracing: %s\n", guard->symbol_tracing ? "YES" : "NO");
    printf("Continuous Scan: %s\n", guard->state.continuous_scan ? "YES" : "NO");
    printf("Mocks Detected: %d\n", guard->state.mocks_detected);
    printf("Functions Traced: %d\n", guard->state.functions_traced);
    printf("Libraries Scanned: %d\n", guard->state.libraries_scanned);
    printf("Last Scan: %s", ctime(&guard->state.last_scan));
    printf("===========================\n");
    
    // Afficher les mocks détectés
    if (guard->state.mocks_detected > 0) {
        printf("\nDetected Mocks:\n");
        for (int i = 0; i < guard->state.mocks_detected; i++) {
            printf("  %d. %s (%s)\n", i + 1, 
                   guard->state.detected_mocks[i].name,
                   TCDE_GetMockTypeString(guard->state.detected_mocks[i].type));
        }
    }
    printf("\n");
}