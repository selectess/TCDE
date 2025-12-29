/**
 * Test si l'évolution TDE change réellement le champ
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

int main(void) {
    printf("\n=== TEST: L'évolution TDE change-t-elle le champ? ===\n\n");
    
    // Créer un champ simple
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    
    // Ajouter quelques centres
    for (int i = 0; i < 5; i++) {
        float coords[6] = {0.3f + i * 0.1f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = (1.0f + 0.5f * i) * cexpf(I * i * 0.3f);
        TCDE_AddCenter6D(field, &p, coeff, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    printf("Champ créé avec %d centres\n\n", field->manifold_6d.num_centers);
    
    // Sauvegarder coefficients originaux
    TCDE_Complex* original_coeffs = malloc(5 * sizeof(TCDE_Complex));
    for (int i = 0; i < 5; i++) {
        original_coeffs[i] = field->manifold_6d.centers[i].coeff;
        printf("Centre %d AVANT: %.6f + %.6fi (mag: %.6f)\n", 
               i, crealf(original_coeffs[i]), cimagf(original_coeffs[i]),
               cabsf(original_coeffs[i]));
    }
    
    // Configurer paramètres
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    printf("\nParamètres: dt=%.4f, D=%.4f, alpha=%.4f\n\n", 
           params.dt, params.D, params.alpha);
    
    // Évoluer 10 steps
    printf("Évolution de 10 steps...\n\n");
    for (int step = 0; step < 10; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Comparer
    printf("Coefficients APRÈS:\n");
    float total_change = 0.0f;
    for (int i = 0; i < 5; i++) {
        TCDE_Complex new_coeff = field->manifold_6d.centers[i].coeff;
        TCDE_Complex delta = new_coeff - original_coeffs[i];
        float change = cabsf(delta);
        total_change += change;
        
        printf("Centre %d APRÈS: %.6f + %.6fi (mag: %.6f) - Change: %.6f\n", 
               i, crealf(new_coeff), cimagf(new_coeff),
               cabsf(new_coeff), change);
    }
    
    printf("\n=== RÉSULTAT ===\n");
    printf("Changement total: %.6f\n", total_change);
    
    if (total_change < 0.001f) {
        printf("❌ ÉCHEC: Le champ ne change PAS (%.6f < 0.001)\n", total_change);
        printf("   L'évolution ne fonctionne pas!\n");
    } else if (total_change < 0.01f) {
        printf("⚠️  FAIBLE: Le champ change très peu (%.6f)\n", total_change);
        printf("   L'évolution est trop faible\n");
    } else {
        printf("✅ SUCCÈS: Le champ change (%.6f)\n", total_change);
        printf("   L'évolution fonctionne!\n");
    }
    
    free(original_coeffs);
    TCDE_DestroyField(field);
    
    return 0;
}
