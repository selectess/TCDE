#ifndef TCDE_TOPOLOGICAL_SURFACE_H
#define TCDE_TOPOLOGICAL_SURFACE_H

#include "../core/tcde_core.h"
#include "tcde_projection_3d.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * TCDE Multidimensional Topological Surface System - ZERO TOLERANCE
 * 
 * Génère des surfaces topologiques 3D représentant l'espace cognitif 11D
 * avec calculs de courbure Riemannienne et coloration dynamique selon
 * 20+ métriques d'émergence.
 * 
 * CRITÈRE SUCCÈS: Représentation visuelle de toutes les 11 dimensions
 * 
 * STANDARD ZÉRO TOLÉRANCE:
 * - Aucune simulation/mock autorisée
 * - Calculs géométriques réels uniquement
 * - Code production-ready
 */

// Type d'émergence détectée
typedef enum {
    TCDE_EMERGENCE_NONE,
    TCDE_EMERGENCE_DIMENSIONAL,     // Expansion dimensionnelle
    TCDE_EMERGENCE_MEMORY,          // Expansion mémoire
    TCDE_EMERGENCE_CONSCIOUSNESS,   // Émergence de conscience
    TCDE_EMERGENCE_AUTOPOIESIS,     // Autopoïèse
    TCDE_EMERGENCE_CREATIVITY       // Créativité
} TCDE_EmergenceType;

// Métrique d'émergence pour coloration
typedef struct {
    char name[64];
    float value;                    // [0,1]
    float threshold;                // Seuil d'émergence
    bool is_emergent;               // Au-dessus du seuil
} TCDE_EmergenceMetric;

// Vertex 3D avec propriétés topologiques
typedef struct {
    float position[3];              // x, y, z
    float normal[3];                // Normale de surface
    float color[4];                 // RGBA basé sur émergence
    
    // Propriétés topologiques locales
    float curvature_mean;           // Courbure moyenne
    float curvature_gaussian;       // Courbure gaussienne
    float energy_density;           // Densité d'énergie
    
    // Métriques d'émergence
    float emergence_intensity;      // [0,1]
    TCDE_EmergenceType emergence_type;
} TCDE_SurfaceVertex;

// Triangle de surface
typedef struct {
    uint32_t indices[3];            // Indices des vertices
    float area;                     // Aire du triangle
    float curvature;                // Courbure du triangle
} TCDE_SurfaceTriangle;

// Surface topologique 3D complète
typedef struct {
    // Géométrie
    TCDE_SurfaceVertex* vertices;
    uint32_t vertex_count;
    TCDE_SurfaceTriangle* triangles;
    uint32_t triangle_count;
    
    // Propriétés topologiques globales
    int genus;                      // Genre topologique (trous)
    float total_curvature;          // Courbure totale intégrée
    float surface_area;             // Aire totale
    float volume_enclosed;          // Volume enclos
    
    // Métriques d'émergence (20+ métriques)
    TCDE_EmergenceMetric metrics[32];
    uint32_t metric_count;
    
    // État d'animation
    float animation_time;
    bool is_evolving;
    
    // Authenticité
    bool is_authentic;
    float authenticity_score;
} TCDE_MultidimensionalSurface;

// Générateur de surfaces
typedef struct {
    TCDE_Field* field;
    TCDE_HyperDimensionalProjector* projector;
    
    // Paramètres de génération
    uint32_t resolution;            // Résolution de la grille
    float smoothness;               // Lissage de surface
    bool compute_curvature;         // Calculer courbures
    
    // Surface actuelle
    TCDE_MultidimensionalSurface* surface;
} TCDE_SurfaceGenerator;

// Création/Destruction
TCDE_SurfaceGenerator* TCDE_CreateSurfaceGenerator(
    TCDE_Field* field,
    uint32_t resolution
);
void TCDE_DestroySurfaceGenerator(TCDE_SurfaceGenerator* generator);
void TCDE_DestroySurface(TCDE_MultidimensionalSurface* surface);

// Génération de surface
TCDE_MultidimensionalSurface* TCDE_GenerateTopologicalSurface(
    TCDE_SurfaceGenerator* generator
);

// Calculs de courbure Riemannienne (via metrics module)
void TCDE_ComputeSurfaceCurvatures(TCDE_MultidimensionalSurface* surface);

// Métriques d'émergence
void TCDE_ComputeEmergenceMetrics(
    TCDE_MultidimensionalSurface* surface,
    TCDE_Field* field
);
void TCDE_AddEmergenceMetric(
    TCDE_MultidimensionalSurface* surface,
    const char* name,
    float value,
    float threshold
);

// Coloration dynamique
void TCDE_ColorSurfaceByEmergence(TCDE_MultidimensionalSurface* surface);
void TCDE_ColorSurfaceByCurvature(TCDE_MultidimensionalSurface* surface);
void TCDE_ColorSurfaceByEnergy(TCDE_MultidimensionalSurface* surface);

// Propriétés topologiques (genus calculé via metrics module)
float TCDE_ComputeTotalCurvature(const TCDE_MultidimensionalSurface* surface);
float TCDE_ComputeSurfaceArea(const TCDE_MultidimensionalSurface* surface);
float TCDE_ComputeSurfaceEnclosedVolume(const TCDE_MultidimensionalSurface* surface);

// Affichage
void TCDE_PrintSurfaceInfo(const TCDE_MultidimensionalSurface* surface);
void TCDE_PrintEmergenceMetrics(const TCDE_MultidimensionalSurface* surface);
void TCDE_PrintTopologicalProperties(const TCDE_MultidimensionalSurface* surface);

#endif // TCDE_TOPOLOGICAL_SURFACE_H
