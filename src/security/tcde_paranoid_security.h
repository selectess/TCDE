#ifndef TCDE_PARANOID_SECURITY_H
#define TCDE_PARANOID_SECURITY_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/**
 * TCDE Paranoid Security System
 * 
 * Système de sécurité ultra-strict pour garantir l'authenticité 100%
 * du code TCDE sans aucune simulation, mock ou stub.
 * 
 * ZÉRO TOLÉRANCE pour toute forme de simulation.
 */

// Types de sécurité
typedef enum {
    TCDE_SECURITY_NONE = 0,
    TCDE_SECURITY_BASIC = 1,
    TCDE_SECURITY_STRICT = 2,
    TCDE_SECURITY_PARANOID = 3
} TCDE_SecurityLevel;

// Types de violations détectées
typedef enum {
    TCDE_VIOLATION_NONE = 0,
    TCDE_VIOLATION_SIMULATION_DETECTED = 1,
    TCDE_VIOLATION_MOCK_DETECTED = 2,
    TCDE_VIOLATION_FAKE_DATA = 3,
    TCDE_VIOLATION_CODE_TAMPERING = 4,
    TCDE_VIOLATION_CHECKSUM_MISMATCH = 5
} TCDE_ViolationType;

// État de sécurité
typedef struct {
    TCDE_SecurityLevel level;
    bool active;
    bool simulation_forbidden;
    uint64_t code_checksum;
    uint64_t data_checksum;
    time_t last_check;
    int violation_count;
    TCDE_ViolationType last_violation;
} TCDE_SecurityState;

// Système de sécurité paranoïaque
typedef struct {
    TCDE_SecurityState state;
    bool continuous_monitoring;
    bool emergency_stop_enabled;
    void (*violation_callback)(TCDE_ViolationType violation);
    void (*emergency_stop_callback)(void);
} TCDE_ParanoidSecuritySystem;

// Fonctions principales
TCDE_ParanoidSecuritySystem* TCDE_CreateParanoidSecurity(void);
void TCDE_DestroyParanoidSecurity(TCDE_ParanoidSecuritySystem* security);

// Configuration
bool TCDE_SetSecurityLevel(TCDE_ParanoidSecuritySystem* security, TCDE_SecurityLevel level);
bool TCDE_EnableContinuousMonitoring(TCDE_ParanoidSecuritySystem* security);
bool TCDE_ForbidSimulation(TCDE_ParanoidSecuritySystem* security);

// Validation d'authenticité
bool TCDE_ValidateCodeAuthenticity(TCDE_ParanoidSecuritySystem* security);
bool TCDE_ValidateDataAuthenticity(TCDE_ParanoidSecuritySystem* security, void* data, size_t size);
bool TCDE_DetectSimulation(TCDE_ParanoidSecuritySystem* security);
bool TCDE_DetectMocks(TCDE_ParanoidSecuritySystem* security);

// Checksums cryptographiques
uint64_t TCDE_CalculateCodeChecksum(void);
uint64_t TCDE_CalculateDataChecksum(void* data, size_t size);
bool TCDE_VerifyChecksum(uint64_t expected, uint64_t actual);

// Surveillance temps réel
void TCDE_StartContinuousMonitoring(TCDE_ParanoidSecuritySystem* security);
void TCDE_StopContinuousMonitoring(TCDE_ParanoidSecuritySystem* security);
bool TCDE_CheckSecurityStatus(TCDE_ParanoidSecuritySystem* security);

// Arrêt d'urgence
void TCDE_TriggerEmergencyStop(TCDE_ParanoidSecuritySystem* security, TCDE_ViolationType violation);
bool TCDE_IsEmergencyStopTriggered(TCDE_ParanoidSecuritySystem* security);

// Callbacks
void TCDE_SetViolationCallback(TCDE_ParanoidSecuritySystem* security, 
                               void (*callback)(TCDE_ViolationType));
void TCDE_SetEmergencyStopCallback(TCDE_ParanoidSecuritySystem* security, 
                                   void (*callback)(void));

// Utilitaires de diagnostic
const char* TCDE_GetViolationString(TCDE_ViolationType violation);
const char* TCDE_GetSecurityLevelString(TCDE_SecurityLevel level);
void TCDE_PrintSecurityStatus(TCDE_ParanoidSecuritySystem* security);

// Macros de sécurité
#define TCDE_SECURITY_CHECK(security) \
    do { \
        if (!TCDE_CheckSecurityStatus(security)) { \
            TCDE_TriggerEmergencyStop(security, TCDE_VIOLATION_CODE_TAMPERING); \
            return false; \
        } \
    } while(0)

#define TCDE_FORBID_SIMULATION(security) \
    do { \
        if (TCDE_DetectSimulation(security)) { \
            TCDE_TriggerEmergencyStop(security, TCDE_VIOLATION_SIMULATION_DETECTED); \
            return false; \
        } \
    } while(0)

#define TCDE_FORBID_MOCKS(security) \
    do { \
        if (TCDE_DetectMocks(security)) { \
            TCDE_TriggerEmergencyStop(security, TCDE_VIOLATION_MOCK_DETECTED); \
            return false; \
        } \
    } while(0)

#endif // TCDE_PARANOID_SECURITY_H