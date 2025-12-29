/**
 * @file test_reflexivity_diagnostic.c
 * @brief Diagnostic détaillé du test de réflexivité
 * 
 * Ce test identifie précisément pourquoi le test de réflexivité échoue.
 */

#include "tcde_core.h"
#include "tcde_validation.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

int main(void) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  DIAGNOSTIC TEST DE RÉFLEXIVITÉ                           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Créer un champ de test identique à celui du test qui échoue
    TCDE_Field* field = TCDE_CreateField(50, 2.0f);
    
    printf("1. Création du champ de test...\n");
    printf("   Capacité: 50 centres\n");
    printf("   Dimension fractale: 2.0\n\n");
    
    // Ajouter les mêmes centres que dans le test
    for (int i = 0; i < 30; i++) {
        float coords[6] = {
            0.2f + (i % 5) * 0.15f,
            0.2f + (i / 5) * 0.15f,
            0.5f,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = (0.3f + 0.1f * i) * cexpf(I * i * 0.2f);
        TCDE_AddCenter6D(field, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    printf("2. Ajout de 30 centres avec coefficients complexes\n");
    printf("   Nombre de centres: %d\n", field->manifold_6d.num_centers);
    printf("   Énergie initiale: %.6f\n\n", TCDE_ComputeEnergy(field));
    
    // Évaluer le champ à quelques points pour vérifier qu'il fonctionne
    printf("3. Évaluation du champ à des points de test:\n");
    for (int i = 0; i < 5; i++) {
        float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        coords[0] = 0.2f + i * 0.2f;
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex value = TCDE_Evaluate6D(field, &p);
        printf("   Point[%d]: Φ = %.4f + %.4fi (magnitude: %.4f)\n", 
               i, crealf(value), cimagf(value), cabsf(value));
        TCDE_FreePoint(&p);
    }
    printf("\n");
    
    // Tester avec différents paramètres
    printf("4. Test de réflexivité avec différents paramètres:\n\n");
    
    float perturbations[] = {0.05f, 0.1f, 0.2f, 0.3f};
    int evolution_steps[] = {1, 5, 10, 20};
    
    for (int p = 0; p < 4; p++) {
        for (int e = 0; e < 4; e++) {
            // Créer une copie du champ pour chaque test
            TCDE_Field* test_field = TCDE_CreateField(50, 2.0f);
            
            // Copier les centres
            for (int i = 0; i < field->manifold_6d.num_centers; i++) {
                TCDE_Point p_copy = field->manifold_6d.centers[i].point;
                TCDE_Complex coeff_copy = field->manifold_6d.centers[i].coeff;
                float epsilon_copy = field->manifold_6d.centers[i].epsilon;
                TCDE_AddCenter6D(test_field, &p_copy, coeff_copy, epsilon_copy);
            }
            
            // Tester la réflexivité
            float reflexivity = TCDE_ValidateReflexivity(test_field, 0, 
                                                         perturbations[p], 
                                                         evolution_steps[e]);
            
            printf("   Perturbation: %.2f, Évolution: %2d steps → Réflexivité: ", 
                   perturbations[p], evolution_steps[e]);
            
            if (isnan(reflexivity)) {
                printf("❌ NaN (ERREUR)\n");
            } else if (isinf(reflexivity)) {
                printf("❌ Inf (ERREUR)\n");
            } else if (reflexivity < 0.0f) {
                printf("❌ %.6f (NÉGATIF - ERREUR)\n", reflexivity);
            } else if (reflexivity > 1.0f) {
                printf("❌ %.6f (> 1.0 - ERREUR)\n", reflexivity);
            } else if (reflexivity >= 0.5f) {
                printf("✅ %.6f (PASS)\n", reflexivity);
            } else {
                printf("⚠️  %.6f (faible mais valide)\n", reflexivity);
            }
            
            TCDE_DestroyField(test_field);
        }
    }
    
    printf("\n5. Analyse des causes potentielles:\n\n");
    
    // Vérifier si le champ a des valeurs nulles
    int zero_count = 0;
    int nan_count = 0;
    int inf_count = 0;
    
    for (int i = 0; i < 10; i++) {
        float coords[6] = {
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex value = TCDE_Evaluate6D(field, &p);
        
        if (cabsf(value) < 1e-10f) zero_count++;
        if (isnan(crealf(value)) || isnan(cimagf(value))) nan_count++;
        if (isinf(crealf(value)) || isinf(cimagf(value))) inf_count++;
        
        TCDE_FreePoint(&p);
    }
    
    printf("   Échantillonnage de 10 points aléatoires:\n");
    printf("   - Valeurs nulles: %d/10\n", zero_count);
    printf("   - Valeurs NaN: %d/10\n", nan_count);
    printf("   - Valeurs Inf: %d/10\n", inf_count);
    printf("\n");
    
    // Vérifier les coefficients des centres
    float min_coeff = INFINITY;
    float max_coeff = 0.0f;
    float avg_coeff = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float mag = cabsf(field->manifold_6d.centers[i].coeff);
        if (mag < min_coeff) min_coeff = mag;
        if (mag > max_coeff) max_coeff = mag;
        avg_coeff += mag;
    }
    avg_coeff /= field->manifold_6d.num_centers;
    
    printf("   Statistiques des coefficients:\n");
    printf("   - Minimum: %.6f\n", min_coeff);
    printf("   - Maximum: %.6f\n", max_coeff);
    printf("   - Moyenne: %.6f\n", avg_coeff);
    printf("\n");
    
    // Conclusion
    printf("6. Conclusion:\n\n");
    
    if (nan_count > 0 || inf_count > 0) {
        printf("   ❌ PROBLÈME CRITIQUE: Le champ contient des valeurs NaN/Inf\n");
        printf("      → Vérifier l'évaluation RBF et les distances géodésiques\n");
    } else if (zero_count > 5) {
        printf("   ⚠️  AVERTISSEMENT: Beaucoup de valeurs nulles\n");
        printf("      → Les centres sont peut-être trop éloignés\n");
    } else {
        printf("   ✅ Le champ semble valide\n");
        printf("      → Le problème vient probablement de:\n");
        printf("         1. Paramètres de perturbation trop forts\n");
        printf("         2. Pas assez d'étapes d'évolution\n");
        printf("         3. Calcul de corrélation incorrect\n");
    }
    
    TCDE_DestroyField(field);
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  FIN DU DIAGNOSTIC                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
