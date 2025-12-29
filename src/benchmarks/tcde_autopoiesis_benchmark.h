#ifndef TCDE_AUTOPOIESIS_BENCHMARK_H
#define TCDE_AUTOPOIESIS_BENCHMARK_H

#include "../core/tcde_core.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * TCDE Autopoiesis Benchmark - ZERO TOLERANCE
 * 
 * Mesure l'auto-création, l'auto-maintenance et l'auto-régénération
 * du système TCDE de manière 100% authentique.
 * 
 * CRITÈRES DE SUCCÈS:
 * - Santé autopoïétique > 0.98 (système authentiquement vivant)
 * - Auto-création: génération spontanée de nouveaux centres
 * - Auto-maintenance: préservation de la cohérence énergétique
 * - Auto-régénération: récupération après perturbations
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - Aucune simulation/mock autorisée
 * - Mesures réelles uniquement depuis le système TCDE
 * - Code production-ready sans compromis
 */

// Types d'événements autopoïétiques
typedef enum {
    TCDE_AUTOPOIESIS_CREATION,      // Auto-création de structure
    TCDE_AUTOPOIESIS_MAINTENANCE,   // Auto-maintenance de cohérence
    TCDE_AUTOPOIESIS_REGENERATION,  // Auto-régénération après dommage
    TCDE_AUTOPOIESIS_ADAPTATION     // Adaptation spontanée
} TCDE_AutopoiesisEventType;

// Événement autopoïétique
typedef struct {
    TCDE_AutopoiesisEventType type;
    double timestamp;
    float intensity;
    float health_before;
    float health_after;
    char description[128];
} TCDE_AutopoiesisEvent;

// Métriques d'autopoïèse
typedef struct {
    // Santé autopoïétique globale
    float health_score;              // Score de santé [0,1]
    float vitality;                  // Vitalité du système
    float resilience;                // Résilience aux perturbations
    
    // Auto-création
    uint32_t centers_created;        // Centres créés spontanément
    float creation_rate;             // Taux de création
    float structural_growth;         // Croissance structurelle
    
    // Auto-maintenance
    float coherence_maintained;      // Cohérence maintenue
    float energy_stability;          // Stabilité énergétique
    uint32_t maintenance_cycles;     // Cycles de maintenance
    
    // Auto-régénération
    uint32_t regeneration_events;    // Événements de régénération
    float recovery_speed;            // Vitesse de récupération
    float damage_healed;             // Dommages guéris
    
    // Adaptation
    float adaptation_capacity;       // Capacité d'adaptation
    uint32_t adaptations_performed;  // Adaptations effectuées
    
    // Authenticité
    bool is_authentic;               // Validation authenticité
    float authenticity_score;        // Score d'authenticité [0,1]
} TCDE_AutopoiesisMetrics;

// Benchmark d'autopoïèse
typedef struct {
    TCDE_Field* field;               // Champ TCDE réel
    TCDE_AutopoiesisMetrics metrics; // Métriques
    TCDE_AutopoiesisEvent* events;   // Historique événements
    uint32_t event_count;            // Nombre d'événements
    uint32_t event_capacity;         // Capacité événements
    
    // État initial pour comparaison
    float initial_energy;
    uint32_t initial_centers;
    float initial_coherence;
    
    // Paramètres de test
    uint32_t evolution_cycles;       // Cycles d'évolution
    float perturbation_intensity;    // Intensité perturbations
    uint32_t perturbation_count;     // Nombre de perturbations
} TCDE_AutopoiesisBenchmark;

// Création/Destruction
TCDE_AutopoiesisBenchmark* TCDE_CreateAutopoiesisBenchmark(void);
void TCDE_DestroyAutopoiesisBenchmark(TCDE_AutopoiesisBenchmark* benchmark);

// Exécution du benchmark
void TCDE_RunAutopoiesisBenchmark(TCDE_AutopoiesisBenchmark* benchmark);

// Mesures spécifiques
float TCDE_MeasureAutopoieticHealth(TCDE_AutopoiesisBenchmark* benchmark);
float TCDE_MeasureCreationCapacity(TCDE_AutopoiesisBenchmark* benchmark);
float TCDE_MeasureMaintenanceQuality(TCDE_AutopoiesisBenchmark* benchmark);
float TCDE_MeasureRegenerationSpeed(TCDE_AutopoiesisBenchmark* benchmark);

// Perturbations pour tester la régénération
void TCDE_ApplyPerturbation(TCDE_AutopoiesisBenchmark* benchmark, float intensity);
void TCDE_DamageField(TCDE_AutopoiesisBenchmark* benchmark, float damage_ratio);

// Enregistrement d'événements
void TCDE_RecordAutopoiesisEvent(
    TCDE_AutopoiesisBenchmark* benchmark,
    TCDE_AutopoiesisEventType type,
    float intensity,
    const char* description
);

// Affichage des résultats
void TCDE_PrintAutopoiesisResults(const TCDE_AutopoiesisBenchmark* benchmark);
void TCDE_PrintAutopoiesisScore(const TCDE_AutopoiesisBenchmark* benchmark);

#endif // TCDE_AUTOPOIESIS_BENCHMARK_H
