/**
 * Test des termes TDE individuels
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

int main(void) {
    printf("\n=== TEST: Termes TDE individuels ===\n\n");
    
    // Créer un champ simple
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    
    // Ajouter UN centre
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_Complex coeff = 1.0f + 0.5f * I;
    TCDE_AddCenter6D(field, &p, coeff, 0.1f);
    
    printf("Champ avec 1 centre: coeff = %.3f + %.3fi\n\n", 
           crealf(coeff), cimagf(coeff));
    
    // Utiliser paramètres configurés
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    // Calculer chaque terme
    TCDE_Complex diffusion = TCDE_DiffusionTerm(field, &p, params.D);
    TCDE_Complex nonlinearity = TCDE_NonlinearityTerm(field, &p, params.alpha);
    TCDE_Complex torsion = TCDE_TorsionTerm(field, &p, params.beta);
    TCDE_Complex coupling = TCDE_CouplingTerm(field, &p, params.gamma, params.sigma, 100);
    
    printf("Paramètres: dt=%.4f, D=%.4f, alpha=%.4f\n\n", 
           params.dt, params.D, params.alpha);
    
    printf("Termes TDE au centre:\n");
    printf("  Diffusion:     %.6f + %.6fi (mag: %.6f)\n", 
           crealf(diffusion), cimagf(diffusion), cabsf(diffusion));
    printf("  Nonlinéarité:  %.6f + %.6fi (mag: %.6f)\n", 
           crealf(nonlinearity), cimagf(nonlinearity), cabsf(nonlinearity));
    printf("  Torsion:       %.6f + %.6fi (mag: %.6f)\n", 
           crealf(torsion), cimagf(torsion), cabsf(torsion));
    printf("  Couplage:      %.6f + %.6fi (mag: %.6f)\n", 
           crealf(coupling), cimagf(coupling), cabsf(coupling));
    
    TCDE_Complex total = diffusion + nonlinearity + torsion + coupling;
    printf("\n  TOTAL:         %.6f + %.6fi (mag: %.6f)\n", 
           crealf(total), cimagf(total), cabsf(total));
    
    // Avec dt configuré
    float dt = params.dt;
    TCDE_Complex change = dt * total;
    printf("\n  Changement (dt=%.3f): %.6f + %.6fi (mag: %.6f)\n", 
           dt, crealf(change), cimagf(change), cabsf(change));
    
    TCDE_Complex new_coeff = coeff + change;
    printf("  Nouveau coeff: %.6f + %.6fi (mag: %.6f)\n", 
           crealf(new_coeff), cimagf(new_coeff), cabsf(new_coeff));
    
    printf("\n=== ANALYSE ===\n");
    if (cabsf(new_coeff) < cabsf(coeff) * 0.5f) {
        printf("❌ PROBLÈME: Le coefficient diminue de plus de 50%%!\n");
        printf("   Magnitude: %.6f → %.6f\n", cabsf(coeff), cabsf(new_coeff));
    } else if (cabsf(new_coeff) > cabsf(coeff) * 2.0f) {
        printf("❌ PROBLÈME: Le coefficient augmente de plus de 100%%!\n");
        printf("   Magnitude: %.6f → %.6f\n", cabsf(coeff), cabsf(new_coeff));
    } else {
        printf("✅ OK: Le coefficient évolue raisonnablement\n");
        printf("   Magnitude: %.6f → %.6f (%.1f%%)\n", 
               cabsf(coeff), cabsf(new_coeff), 
               100.0f * (cabsf(new_coeff) - cabsf(coeff)) / cabsf(coeff));
    }
    
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    
    return 0;
}
