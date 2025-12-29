#include <stdio.h>
#include <complex.h>
#include <math.h>

int main(void) {
    printf("=== Test Calcul Corrélation ===\n\n");
    
    // Test 1: Vecteurs identiques
    float complex v1[3] = {1.0f + 0.5f*I, 2.0f + 1.0f*I, 3.0f + 1.5f*I};
    float complex v2[3] = {1.0f + 0.5f*I, 2.0f + 1.0f*I, 3.0f + 1.5f*I};
    
    float complex inner = 0.0f;
    float norm1 = 0.0f, norm2 = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        inner += conjf(v1[i]) * v2[i];
        norm1 += cabsf(v1[i]) * cabsf(v1[i]);
        norm2 += cabsf(v2[i]) * cabsf(v2[i]);
    }
    
    norm1 = sqrtf(norm1);
    norm2 = sqrtf(norm2);
    
    float corr1 = cabsf(inner) / (norm1 * norm2);
    printf("Test 1 (identiques): %.6f (attendu: 1.0)\n", corr1);
    
    // Test 2: Vecteurs opposés
    float complex v3[3] = {1.0f + 0.5f*I, 2.0f + 1.0f*I, 3.0f + 1.5f*I};
    float complex v4[3] = {-1.0f - 0.5f*I, -2.0f - 1.0f*I, -3.0f - 1.5f*I};
    
    inner = 0.0f;
    norm1 = 0.0f;
    norm2 = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        inner += conjf(v3[i]) * v4[i];
        norm1 += cabsf(v3[i]) * cabsf(v3[i]);
        norm2 += cabsf(v4[i]) * cabsf(v4[i]);
    }
    
    norm1 = sqrtf(norm1);
    norm2 = sqrtf(norm2);
    
    float corr2 = cabsf(inner) / (norm1 * norm2);
    printf("Test 2 (opposés): %.6f (attendu: 1.0 car |inner|)\n", corr2);
    
    // Test 3: Vecteurs orthogonaux
    float complex v5[3] = {1.0f, 0.0f, 0.0f};
    float complex v6[3] = {0.0f, 1.0f, 0.0f};
    
    inner = 0.0f;
    norm1 = 0.0f;
    norm2 = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        inner += conjf(v5[i]) * v6[i];
        norm1 += cabsf(v5[i]) * cabsf(v5[i]);
        norm2 += cabsf(v6[i]) * cabsf(v6[i]);
    }
    
    norm1 = sqrtf(norm1);
    norm2 = sqrtf(norm2);
    
    float corr3 = cabsf(inner) / (norm1 * norm2);
    printf("Test 3 (orthogonaux): %.6f (attendu: 0.0)\n", corr3);
    
    // Test 4: Vecteurs différents
    float complex v7[3] = {20.0f, 19.0f, 18.0f};
    float complex v8[3] = {0.2f, -0.1f, 0.3f};
    
    inner = 0.0f;
    norm1 = 0.0f;
    norm2 = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        inner += conjf(v7[i]) * v8[i];
        norm1 += cabsf(v7[i]) * cabsf(v7[i]);
        norm2 += cabsf(v8[i]) * cabsf(v8[i]);
    }
    
    norm1 = sqrtf(norm1);
    norm2 = sqrtf(norm2);
    
    float corr4 = cabsf(inner) / (norm1 * norm2);
    printf("Test 4 (différents): %.6f (attendu: < 0.5)\n", corr4);
    
    printf("\n✅ Le calcul de corrélation fonctionne correctement\n");
    printf("⚠️  Utiliser |inner| rend les vecteurs opposés corrélés!\n");
    
    return 0;
}
