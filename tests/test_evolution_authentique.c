/**
 * @file test_evolution_authentique.c
 * @brief Test AUTHENTIQUE de l'évolution - vérifie que le champ change vraiment
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

int main(void) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TEST AUTHENTIQUE DE L'ÉVOLUTION TDE                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Créer un champ simple
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    
    // Ajouter UN SEUL centre pour simplifier
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_Complex initial_coeff = 1.0f + 0.5f * I;
    TCDE_AddCenter6D(field, &p, initial_coeff, 0.1f);
    
    printf("1. État initial:\n");
    printf("   Coefficient: %.6f + %.6fi\n", 
           crealf(field->manifold_6d.centers[0].coeff),
           cimagf(field->manifold_6d.centers[0].coeff));
    printf("   Magnitude: %.6f\n", cabsf(field->manifold_6d.centers[0].coeff));
    printf("   Temps: %.6f\n\n", field->time);
    
    // Configurer paramètres
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    printf("2. Paramètres d'évolution:\n");
    printf("   dt = %.6f\n", params.dt);
    printf("   D = %.6f\n", params.D);
    printf("   alpha = %.6f\n", params.alpha);
    printf("   beta = %.6f\n", params.beta);
    printf("   gamma = %.6f\n\n", params.gamma);
    
    // Évoluer et observer les changements
    printf("3. Évolution pas à pas:\n\n");
    
    for (int step = 0; step < 10; step++) {
        TCDE_Complex before = field->manifold_6d.centers[0].coeff;
        float time_before = field->time;
        
        // Évoluer
        TCDE_EvolveStep(field, &params);
        
        TCDE_Complex after = field->manifold_6d.centers[0].coeff;
        float time_after = field->time;
        
        // Calculer le changement
        TCDE_Complex delta = after - before;
        float mag_delta = cabsf(delta);
        float mag_after = cabsf(after);
        
        printf("   Step %2d: ", step + 1);
        printf("Coeff = %.6f + %.6fi, ", crealf(after), cimagf(after));
        printf("Mag = %.6f, ", mag_after);
        printf("ΔMag = %.6f, ", mag_delta);
        printf("Time = %.6f", time_after);
        
        // Vérifier si le champ change
        if (mag_delta < 1e-10f) {
            printf(" ❌ PAS DE CHANGEMENT!\n");
        } else if (isnan(mag_after) || isinf(mag_after)) {
            printf(" ❌ NaN/Inf!\n");
        } else if (mag_after > 1000.0f) {
            printf(" ❌ EXPLOSION!\n");
        } else if (mag_after < 1e-6f) {
            printf(" ❌ EFFONDREMENT!\n");
        } else {
            printf(" ✅ OK\n");
        }
        
        // Vérifier que le temps avance
        if (fabsf(time_after - time_before - params.dt) > 1e-6f) {
            printf("      ⚠️  ERREUR: Le temps n'avance pas correctement!\n");
        }
    }
    
    printf("\n4. Évaluation du champ à différents points:\n\n");
    
    // Évaluer à plusieurs points pour voir si le champ a une structure
    for (int i = 0; i < 5; i++) {
        float test_coords[6] = {0.3f + i * 0.1f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point test_p = TCDE_CreatePoint(6, test_coords);
        TCDE_Complex value = TCDE_Evaluate6D(field, &test_p);
        
        printf("   x=%.1f: Φ = %.6f + %.6fi (mag: %.6f)\n",
               test_coords[0], crealf(value), cimagf(value), cabsf(value));
        
        TCDE_FreePoint(&test_p);
    }
    
    printf("\n5. Test avec perturbation:\n\n");
    
    // Sauvegarder l'état
    TCDE_Complex saved_coeff = field->manifold_6d.centers[0].coeff;
    
    // Perturber
    field->manifold_6d.centers[0].coeff += 0.1f + 0.1f * I;
    TCDE_Complex perturbed = field->manifold_6d.centers[0].coeff;
    
    printf("   Avant perturbation: %.6f + %.6fi\n", 
           crealf(saved_coeff), cimagf(saved_coeff));
    printf("   Après perturbation: %.6f + %.6fi\n",
           crealf(perturbed), cimagf(perturbed));
    
    // Évoluer 5 steps
    for (int step = 0; step < 5; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    TCDE_Complex after_evolution = field->manifold_6d.centers[0].coeff;
    
    printf("   Après 5 steps:      %.6f + %.6fi\n",
           crealf(after_evolution), cimagf(after_evolution));
    
    // Calculer la distance entre saved et after_evolution
    TCDE_Complex diff = after_evolution - saved_coeff;
    float distance = cabsf(diff);
    
    printf("\n   Distance de l'état original: %.6f\n", distance);
    
    if (distance < 1e-6f) {
        printf("   ❌ PROBLÈME: Le champ revient exactement à l'état original!\n");
        printf("      → L'évolution est SIMULÉE ou BLOQUÉE\n");
    } else if (distance > 10.0f) {
        printf("   ❌ PROBLÈME: Le champ diverge trop!\n");
    } else {
        printf("   ✅ Le champ évolue de manière réaliste\n");
    }
    
    printf("\n6. Conclusion:\n\n");
    
    TCDE_Complex final = field->manifold_6d.centers[0].coeff;
    float final_mag = cabsf(final);
    
    if (cabsf(final - initial_coeff) < 1e-6f) {
        printf("   ❌ ÉCHEC: Le coefficient n'a PAS changé depuis le début\n");
        printf("      → L'évolution TDE est SIMULÉE ou DÉSACTIVÉE\n");
    } else if (isnan(final_mag) || isinf(final_mag)) {
        printf("   ❌ ÉCHEC: Le coefficient contient NaN/Inf\n");
        printf("      → L'évolution TDE est INSTABLE\n");
    } else {
        printf("   ✅ SUCCÈS: L'évolution TDE fonctionne\n");
        printf("      Changement total: %.6f\n", cabsf(final - initial_coeff));
    }
    
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  FIN DU TEST                                               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
