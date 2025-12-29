#ifndef TCDE_PROJECTION_3D_H
#define TCDE_PROJECTION_3D_H

#include "../core/tcde_core.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * TCDE 11D → 3D Projection Engine - ZERO TOLERANCE
 * 
 * Projette l'espace cognitif 11D TCDE sur un espace 3D visualisable
 * tout en préservant les propriétés topologiques essentielles.
 * 
 * Architecture 11D:
 * - 6D: Espace cognitif de base (manifold_6d)
 * - 3D: Espace intentionnel (Φ, U, Ψ)
 * - 2D: Temps bi-temporel (t_valid, t_transaction)
 * 
 * CRITÈRE SUCCÈS: Projection sans perte majeure d'information topologique
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - Aucune simulation/mock autorisée
 * - Projection basée sur données TCDE réelles
 * - Code production-ready
 */

// Méthode de projection
typedef enum {
    TCDE_PROJ_PCA,          // Analyse en composantes principales
    TCDE_PROJ_GEOMETRIC,    // Projection géométrique native
    TCDE_PROJ_ENERGY        // Projection basée sur l'énergie
} TCDE_ProjectionMethod;

// Point 3D projeté
typedef struct {
    float x, y, z;
    float energy;           // Énergie du point original
    float importance;       // Importance topologique
} TCDE_Point3D;

// Résultat de projection
typedef struct {
    TCDE_Point3D* points;
    uint32_t point_count;
    
    // Métriques de qualité
    float topology_preservation;  // [0,1] Préservation topologique
    float information_loss;       // [0,1] Perte d'information
    float projection_quality;     // [0,1] Qualité globale
    
    // Authenticité
    bool is_authentic;
    float authenticity_score;
} TCDE_ProjectionResult;

// Projecteur 11D→3D
typedef struct {
    TCDE_ProjectionMethod method;
    TCDE_Field* field;
    
    // Paramètres de projection
    float scale_factor;
    float energy_weight;
    
    // Résultat actuel
    TCDE_ProjectionResult* result;
} TCDE_HyperDimensionalProjector;

// Création/Destruction
TCDE_HyperDimensionalProjector* TCDE_CreateProjector(
    TCDE_Field* field,
    TCDE_ProjectionMethod method
);
void TCDE_DestroyProjector(TCDE_HyperDimensionalProjector* projector);

// Projection
TCDE_ProjectionResult* TCDE_Project11Dto3D(TCDE_HyperDimensionalProjector* projector);

// Mesures de qualité
float TCDE_MeasureTopologyPreservation(const TCDE_ProjectionResult* result);
float TCDE_MeasureInformationLoss(const TCDE_ProjectionResult* result);

// Affichage
void TCDE_PrintProjectionResults(const TCDE_ProjectionResult* result);
void TCDE_PrintProjectionQuality(const TCDE_ProjectionResult* result);

#endif // TCDE_PROJECTION_3D_H
