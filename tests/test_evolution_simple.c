#include "../src/core/tcde.h"
#include <stdio.h>
#include <math.h>

int main(void) {
    printf("=== Test Évolution Simple ===\n\n");
    
    // Create field with one center
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_AddCenter6D(field, &p, 1.0f + 0.5f * I, 0.1f);
    
    // Evaluate before
    TCDE_Complex before = TCDE_Evaluate6D(field, &p);
    printf("Avant évolution: %.6f + %.6fi (mag: %.6f)\n", 
           crealf(before), cimagf(before), cabsf(before));
    
    // Evolve
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    printf("\nParamètres: dt=%.4f, D=%.4f, alpha=%.4f\n", 
           params.dt, params.D, params.alpha);
    
    for (int i = 0; i < 10; i++) {
        TCDE_EvolveStep(field, &params);
    }
    
    // Evaluate after
    TCDE_Complex after = TCDE_Evaluate6D(field, &p);
    printf("\nAprès 10 steps: %.6f + %.6fi (mag: %.6f)\n", 
           crealf(after), cimagf(after), cabsf(after));
    
    float change = cabsf(after - before) / cabsf(before) * 100.0f;
    printf("Changement: %.2f%%\n", change);
    
    if (change > 0.1f) {
        printf("✅ Le champ évolue\n");
    } else {
        printf("❌ Le champ n'évolue pas assez\n");
    }
    
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    return 0;
}
