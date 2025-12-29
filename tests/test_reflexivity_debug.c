#include "../src/core/tcde.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
    printf("=== Diagnostic Réflexivité ===\n\n");
    
    srand(42);  // Seed fixe pour reproductibilité
    
    // Create field with pattern
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    
    for (int i = 0; i < 20; i++) {
        float coords[6] = {
            (float)i / 20.0f, 0.5f, 0.5f,
            1.0f + 0.5f * (float)i / 20.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        float amplitude = 1.0f + 0.5f * sinf(2.0f * 3.14159f * (float)i / 20.0f);
        TCDE_AddCenter6D(field, &p, amplitude, 0.1f);
        TCDE_FreePoint(&p);
    }
    
    printf("Champ créé avec %d centres\n", field->manifold_6d.num_centers);
    
    // Sample point
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.25f, 0.0f, 0.4f};
    TCDE_Point sample = TCDE_CreatePoint(6, coords);
    
    // Evaluate original
    TCDE_Complex original = TCDE_Evaluate6D(field, &sample);
    printf("\nValeur originale: %.6f + %.6fi (mag: %.6f)\n",
           crealf(original), cimagf(original), cabsf(original));
    
    // Perturb (30%)
    float perturbation = 0.3f;
    printf("\nPerturbation %.0f%%...\n", perturbation * 100);
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float noise_real = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * perturbation;
        float noise_imag = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * perturbation;
        
        TCDE_Complex old_coeff = field->manifold_6d.centers[i].coeff;
        field->manifold_6d.centers[i].coeff += noise_real + noise_imag * I;
        
        if (i == 0) {
            printf("  Centre 0: %.3f+%.3fi → %.3f+%.3fi\n",
                   crealf(old_coeff), cimagf(old_coeff),
                   crealf(field->manifold_6d.centers[i].coeff),
                   cimagf(field->manifold_6d.centers[i].coeff));
        }
    }
    
    field->energy_valid = false;
    
    // Evaluate perturbed
    TCDE_Complex perturbed = TCDE_Evaluate6D(field, &sample);
    printf("\nValeur perturbée: %.6f + %.6fi (mag: %.6f)\n",
           crealf(perturbed), cimagf(perturbed), cabsf(perturbed));
    
    float change_perturb = cabsf(perturbed - original) / cabsf(original) * 100.0f;
    printf("Changement dû à perturbation: %.2f%%\n", change_perturb);
    
    // Evolve
    printf("\nÉvolution 50 steps...\n");
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    printf("  Paramètres: dt=%.4f, D=%.4f, alpha=%.4f\n",
           params.dt, params.D, params.alpha);
    
    for (int step = 0; step < 50; step++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Evaluate reconstructed
    TCDE_Complex reconstructed = TCDE_Evaluate6D(field, &sample);
    printf("\nValeur reconstruite: %.6f + %.6fi (mag: %.6f)\n",
           crealf(reconstructed), cimagf(reconstructed), cabsf(reconstructed));
    
    float change_evol = cabsf(reconstructed - perturbed) / cabsf(perturbed) * 100.0f;
    printf("Changement dû à évolution: %.2f%%\n", change_evol);
    
    // Compute correlation
    TCDE_Complex inner = conjf(original) * reconstructed;
    float norm_orig = cabsf(original);
    float norm_recon = cabsf(reconstructed);
    float correlation = cabsf(inner) / (norm_orig * norm_recon);
    
    printf("\nCorrélation: %.6f\n", correlation);
    
    if (correlation < 0.99f) {
        printf("✅ La réflexivité varie correctement\n");
    } else {
        printf("❌ La réflexivité reste trop proche de 1.0\n");
    }
    
    TCDE_FreePoint(&sample);
    TCDE_DestroyField(field);
    return 0;
}
