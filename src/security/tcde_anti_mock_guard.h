#ifndef TCDE_ANTI_MOCK_GUARD_H
#define TCDE_ANTI_MOCK_GUARD_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Anti-Mock Guard System
 * 
 * Gardien externe ultra-strict qui surveille le système TCDE
 * pour détecter et bloquer toute utilisation de mocks, stubs ou simulations.
 * 
 * PRINCIPE: Surveillance externe sans altérer le code TCDE core.
 */

// Types de mocks détectés
typedef enum {
    TCDE_MOCK_NONE = 0,
    TCDE_MOCK_FUNCTION_STUB = 1,
    TCDE_MOCK_DATA_FAKE = 2,
    TCDE_MOCK_LIBRARY_PRELOAD = 3,
    TCDE_MOCK_SYMBOL_INJECTION = 4,
    TCDE_MOCK_BYTECODE_TAMPERING = 5
} TCDE_MockType;

// Informations sur un mock détecté
typedef struct {
    TCDE_MockType type;
    char name[256];
    void* address;
    char library[256];
    time_t detected_at;
    bool is_active;
} TCDE_MockInfo;

// État du gardien anti-mock
typedef struct {
    bool active;
    bool continuous_scan;
    int mocks_detected;
    int functions_traced;
    int libraries_scanned;
    time_t last_scan;
    TCDE_MockInfo detected_mocks[64];
} TCDE_AntiMockState;

// Système de garde anti-mock
typedef struct {
    TCDE_AntiMockState state;
    bool strict_mode;
    bool bytecode_analysis;
    bool symbol_tracing;
    void (*mock_detected_callback)(TCDE_MockInfo* mock);
    void (*emergency_callback)(void);
} TCDE_AntiMockGuard;

// Fonctions principales
TCDE_AntiMockGuard* TCDE_CreateAntiMockGuard(void);
void TCDE_DestroyAntiMockGuard(TCDE_AntiMockGuard* guard);

// Configuration
bool TCDE_EnableStrictMode(TCDE_AntiMockGuard* guard);
bool TCDE_EnableBytecodeAnalysis(TCDE_AntiMockGuard* guard);
bool TCDE_EnableSymbolTracing(TCDE_AntiMockGuard* guard);
bool TCDE_EnableContinuousScanning(TCDE_AntiMockGuard* guard);

// Détection de mocks
bool TCDE_ScanForMocks(TCDE_AntiMockGuard* guard);
bool TCDE_DetectFunctionStubs(TCDE_AntiMockGuard* guard);
bool TCDE_DetectDataFakes(TCDE_AntiMockGuard* guard);
bool TCDE_DetectLibraryPreloads(TCDE_AntiMockGuard* guard);
bool TCDE_DetectSymbolInjection(TCDE_AntiMockGuard* guard);
bool TCDE_DetectBytecodeTampering(TCDE_AntiMockGuard* guard);

// Analyse de bytecode
bool TCDE_AnalyzeBytecode(TCDE_AntiMockGuard* guard, void* function_ptr);
bool TCDE_ValidateFunctionIntegrity(TCDE_AntiMockGuard* guard, const char* function_name);
bool TCDE_TraceFunctionCalls(TCDE_AntiMockGuard* guard);

// Validation d'intégrité
bool TCDE_ValidateCodeIntegrity(TCDE_AntiMockGuard* guard);
bool TCDE_ValidateLibraryIntegrity(TCDE_AntiMockGuard* guard, const char* library_name);
bool TCDE_ValidateSymbolTable(TCDE_AntiMockGuard* guard);

// Surveillance continue
void TCDE_StartContinuousGuarding(TCDE_AntiMockGuard* guard);
void TCDE_StopContinuousGuarding(TCDE_AntiMockGuard* guard);
bool TCDE_PerformRoutineCheck(TCDE_AntiMockGuard* guard);

// Gestion des mocks détectés
int TCDE_GetDetectedMocksCount(TCDE_AntiMockGuard* guard);
TCDE_MockInfo* TCDE_GetDetectedMock(TCDE_AntiMockGuard* guard, int index);
void TCDE_ClearDetectedMocks(TCDE_AntiMockGuard* guard);

// Callbacks
void TCDE_SetMockDetectedCallback(TCDE_AntiMockGuard* guard, 
                                  void (*callback)(TCDE_MockInfo* mock));
void TCDE_SetEmergencyCallback(TCDE_AntiMockGuard* guard, 
                               void (*callback)(void));

// Utilitaires
const char* TCDE_GetMockTypeString(TCDE_MockType type);
void TCDE_PrintMockInfo(TCDE_MockInfo* mock);
void TCDE_PrintGuardStatus(TCDE_AntiMockGuard* guard);

#endif // TCDE_ANTI_MOCK_GUARD_H