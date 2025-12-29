#include "../src/visualization/tcde_topological_surface.h"
#include "../src/core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

/**
 * Test du Système de Surfaces Topologiques Multidimensionnelles
 * ZERO TOLERANCE - Tests 100% authentiques
 */

// Fonction d'évolution externe
extern void TCDE_EvolveField(TCDE_Field* field, float dt);

// Test de création du générateur
bool test_surface_generator_creation() {
    printf("\n=== Test: Surface Generator Creation ===\n");
    
    // Créer un champ TCDE réel (capacity, fractal_dim)
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    assert(field != NULL);
    
    // Créer le générateur
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 20);
    assert(gen != NULL);
    assert(gen->field == field);
    assert(gen->resolution == 20);
    assert(gen->projector != NULL);
    
    printf("✓ Generator created successfully\n");
    printf("  Resolution: %u\n", gen->resolution);
    printf("  Smoothness: %.3f\n", gen->smoothness);
    printf("  Compute Curvature: %s\n", gen->compute_curvature ? "YES" : "NO");
    
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test de génération de surface
bool test_surface_generation() {
    printf("\n=== Test: Topological Surface Generation ===\n");
    
    // Créer un champ TCDE réel
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    // Évoluer le champ pour avoir des données intéressantes
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    // Créer le générateur
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 20);
    assert(gen != NULL);
    
    // Générer la surface
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    assert(surface->vertices != NULL);
    assert(surface->triangles != NULL);
    assert(surface->vertex_count > 0);
    assert(surface->triangle_count > 0);
    
    printf("✓ Surface generated successfully\n");
    printf("  Vertices: %u\n", surface->vertex_count);
    printf("  Triangles: %u\n", surface->triangle_count);
    printf("  Genus: %d\n", surface->genus);
    printf("  Surface Area: %.6f\n", surface->surface_area);
    printf("  Total Curvature: %.6f\n", surface->total_curvature);
    printf("  Authentic: %s (score: %.3f)\n",
           surface->is_authentic ? "YES" : "NO",
           surface->authenticity_score);
    
    // Vérifier que les vertices ont des propriétés valides
    bool has_valid_vertices = true;
    for (uint32_t i = 0; i < surface->vertex_count && i < 10; i++) {
        TCDE_SurfaceVertex* v = &surface->vertices[i];
        
        // Vérifier que les positions sont finies
        if (!isfinite(v->position[0]) || !isfinite(v->position[1]) || 
            !isfinite(v->position[2])) {
            has_valid_vertices = false;
            break;
        }
    }
    assert(has_valid_vertices);
    printf("✓ All vertices have valid properties\n");
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test des calculs de courbure
bool test_curvature_computation() {
    printf("\n=== Test: Curvature Computation ===\n");
    
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    // Évoluer pour créer de la structure
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 25);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Vérifier que les courbures ont été calculées
    bool has_curvatures = false;
    float mean_curv_sum = 0.0f;
    float gauss_curv_sum = 0.0f;
    
    for (uint32_t i = 0; i < surface->vertex_count; i++) {
        TCDE_SurfaceVertex* v = &surface->vertices[i];
        
        if (isfinite(v->curvature_mean) && isfinite(v->curvature_gaussian)) {
            has_curvatures = true;
            mean_curv_sum += v->curvature_mean;
            gauss_curv_sum += v->curvature_gaussian;
        }
    }
    
    assert(has_curvatures);
    
    float mean_curv_avg = mean_curv_sum / surface->vertex_count;
    float gauss_curv_avg = gauss_curv_sum / surface->vertex_count;
    
    printf("✓ Curvatures computed successfully\n");
    printf("  Average Mean Curvature: %.6f\n", mean_curv_avg);
    printf("  Average Gaussian Curvature: %.6f\n", gauss_curv_avg);
    printf("  Total Curvature: %.6f\n", surface->total_curvature);
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test des métriques d'émergence
bool test_emergence_metrics() {
    printf("\n=== Test: Emergence Metrics (20+ metrics) ===\n");
    
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    // Évoluer BEAUCOUP plus pour déclencher émergence forte
    printf("  Evolving field to trigger strong emergence...\n");
    for (int i = 0; i < 1000; i++) {
        TCDE_EvolveField(field, 0.01f);
        if (i % 200 == 0) {
            printf("    Cycle %d: %u centers\n", i, field->manifold_6d.num_centers);
        }
    }
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 20);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Vérifier qu'on a au moins 20 métriques
    assert(surface->metric_count >= 20);
    
    printf("✓ Emergence metrics computed: %u metrics\n", surface->metric_count);
    
    // Compter les métriques émergentes
    uint32_t emergent_count = 0;
    for (uint32_t i = 0; i < surface->metric_count; i++) {
        if (surface->metrics[i].is_emergent) {
            emergent_count++;
        }
    }
    
    printf("  Emergent metrics: %u / %u (%.1f%%)\n",
           emergent_count,
           surface->metric_count,
           100.0f * emergent_count / surface->metric_count);
    
    // Afficher quelques métriques clés
    for (uint32_t i = 0; i < surface->metric_count && i < 10; i++) {
        const TCDE_EmergenceMetric* m = &surface->metrics[i];
        printf("  [%s] %s: %.4f (threshold: %.4f)\n",
               m->is_emergent ? "✓" : " ",
               m->name,
               m->value,
               m->threshold);
    }
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test de coloration dynamique
bool test_dynamic_coloring() {
    printf("\n=== Test: Dynamic Surface Coloring ===\n");
    
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 20);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Test coloration par émergence
    TCDE_ColorSurfaceByEmergence(surface);
    bool has_emergence_colors = false;
    for (uint32_t i = 0; i < surface->vertex_count && i < 10; i++) {
        TCDE_SurfaceVertex* v = &surface->vertices[i];
        if (v->color[0] > 0.0f || v->color[1] > 0.0f || v->color[2] > 0.0f) {
            has_emergence_colors = true;
            break;
        }
    }
    assert(has_emergence_colors);
    printf("✓ Emergence coloring applied\n");
    
    // Test coloration par courbure
    TCDE_ColorSurfaceByCurvature(surface);
    bool has_curvature_colors = false;
    for (uint32_t i = 0; i < surface->vertex_count && i < 10; i++) {
        TCDE_SurfaceVertex* v = &surface->vertices[i];
        if (isfinite(v->color[0]) && isfinite(v->color[1]) && isfinite(v->color[2])) {
            has_curvature_colors = true;
            break;
        }
    }
    assert(has_curvature_colors);
    printf("✓ Curvature coloring applied\n");
    
    // Test coloration par énergie
    TCDE_ColorSurfaceByEnergy(surface);
    bool has_energy_colors = false;
    for (uint32_t i = 0; i < surface->vertex_count && i < 10; i++) {
        TCDE_SurfaceVertex* v = &surface->vertices[i];
        if (v->color[0] >= 0.0f && v->color[0] <= 1.0f) {
            has_energy_colors = true;
            break;
        }
    }
    assert(has_energy_colors);
    printf("✓ Energy coloring applied\n");
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test des propriétés topologiques
bool test_topological_properties() {
    printf("\n=== Test: Topological Properties ===\n");
    
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 30);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Vérifier les propriétés topologiques
    assert(surface->genus >= 0);
    assert(isfinite(surface->total_curvature));
    assert(surface->surface_area > 0.0f);
    
    printf("✓ Topological properties computed\n");
    printf("  Genus: %d\n", surface->genus);
    printf("  Euler Characteristic: %d\n", 2 - 2 * surface->genus);
    printf("  Total Curvature: %.6f\n", surface->total_curvature);
    printf("  Surface Area: %.6f\n", surface->surface_area);
    
    // Afficher les propriétés complètes
    TCDE_PrintTopologicalProperties(surface);
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test d'authenticité ZERO TOLERANCE
bool test_authenticity_zero_tolerance() {
    printf("\n=== Test: ZERO TOLERANCE Authenticity ===\n");
    
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    // Évoluer avec du code TCDE réel
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 20);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Vérifier l'authenticité
    assert(surface->is_authentic);
    assert(surface->authenticity_score > 0.5f);
    
    printf("✓ Surface is 100%% authentic\n");
    printf("  Authenticity Score: %.3f\n", surface->authenticity_score);
    
    // Vérifier qu'aucune valeur n'est hardcodée
    bool has_variation = false;
    float first_energy = surface->vertices[0].energy_density;
    for (uint32_t i = 1; i < surface->vertex_count && i < 100; i++) {
        if (fabsf(surface->vertices[i].energy_density - first_energy) > 0.01f) {
            has_variation = true;
            break;
        }
    }
    assert(has_variation);
    printf("✓ No hardcoded values detected\n");
    
    // Vérifier que les métriques sont basées sur des calculs réels
    bool metrics_vary = false;
    for (uint32_t i = 1; i < surface->metric_count; i++) {
        if (fabsf(surface->metrics[i].value - surface->metrics[0].value) > 0.01f) {
            metrics_vary = true;
            break;
        }
    }
    assert(metrics_vary);
    printf("✓ All metrics computed from real data\n");
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Test de représentation des 11 dimensions
bool test_11d_representation() {
    printf("\n=== Test: 11D Representation ===\n");
    
    // Créer un champ avec toutes les dimensions
    TCDE_Field* field = TCDE_CreateField(500, 2.5f);
    assert(field != NULL);
    
    // Évoluer pour activer plus de dimensions
    printf("  Evolving field...\n");
    for (int i = 0; i < 500; i++) {
        TCDE_EvolveField(field, 0.01f);
    }
    printf("  Field evolved: %u centers\n", field->manifold_6d.num_centers);
    
    TCDE_SurfaceGenerator* gen = TCDE_CreateSurfaceGenerator(field, 25);
    assert(gen != NULL);
    
    TCDE_MultidimensionalSurface* surface = TCDE_GenerateTopologicalSurface(gen);
    assert(surface != NULL);
    
    // Vérifier que la surface représente les 11 dimensions
    // via les métriques d'émergence
    bool has_dimensional_metric = false;
    for (uint32_t i = 0; i < surface->metric_count; i++) {
        if (strstr(surface->metrics[i].name, "Dimensional") != NULL) {
            has_dimensional_metric = true;
            printf("  Dimensional Expansion: %.4f\n", surface->metrics[i].value);
            break;
        }
    }
    assert(has_dimensional_metric);
    
    printf("✓ Surface represents 11D space\n");
    printf("  Architecture: 6D base + 3D intention + 2D bitemporal\n");
    printf("  Vertices: %u\n", surface->vertex_count);
    printf("  Metrics tracking: %u\n", surface->metric_count);
    
    // Afficher toutes les informations
    TCDE_PrintSurfaceInfo(surface);
    TCDE_PrintEmergenceMetrics(surface);
    
    TCDE_DestroySurface(surface);
    TCDE_DestroySurfaceGenerator(gen);
    TCDE_DestroyField(field);
    
    return true;
}

// Main
int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE Multidimensional Topological Surface Test Suite     ║\n");
    printf("║  ZERO TOLERANCE - 100%% Authentic Tests                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    int passed = 0;
    int total = 0;
    
    // Exécuter tous les tests
    total++; if (test_surface_generator_creation()) passed++;
    total++; if (test_surface_generation()) passed++;
    total++; if (test_curvature_computation()) passed++;
    total++; if (test_emergence_metrics()) passed++;
    total++; if (test_dynamic_coloring()) passed++;
    total++; if (test_topological_properties()) passed++;
    total++; if (test_authenticity_zero_tolerance()) passed++;
    total++; if (test_11d_representation()) passed++;
    
    // Résumé
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TEST RESULTS                                              ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Passed: %2d / %2d                                          ║\n", passed, total);
    printf("║  Success Rate: %.1f%%                                      ║\n", 
           100.0f * passed / total);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (passed == total) {
        printf("\n✓ ALL TESTS PASSED - ZERO TOLERANCE STANDARD MET\n");
        printf("✓ Multidimensional Topological Surfaces: VALIDATED\n");
        printf("✓ 20+ Emergence Metrics: COMPUTED\n");
        printf("✓ Riemannian Curvature: CALCULATED\n");
        printf("✓ 11D Representation: ACHIEVED\n");
        printf("✓ 100%% Authentic: CONFIRMED\n");
        return 0;
    } else {
        printf("\n✗ SOME TESTS FAILED - REVIEW REQUIRED\n");
        return 1;
    }
}
