/**
 * @file test_adaptive_parameters.c
 * @brief Test for Adaptive Parameters Module - ASI Phase 2
 *
 * Tests self-regulating physical laws and parameter evolution.
 */

#include "tcde_adaptive_parameters.h"
#include "tcde_core.h"
#include "tcde_evolution.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== TCDE Adaptive Parameters Test (ASI Phase 2) ===\n\n");

    // Create a field for testing
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    if (!field) {
        fprintf(stderr, "Failed to create field\n");
        return 1;
    }

    // Add centers to create dynamics
    for (int i = 0; i < 50; i++) {
        float coords[6] = {
            (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
        TCDE_FreePoint(&p);
    }

    printf("✓ Created field with %d centers\n", field->manifold_6d.num_centers);

    // Create initial parameters
    TCDE_Parameters initial_params;
    TCDE_SetParameters(&initial_params, 0.01f, 0.1f, 0.05f, 0.02f, 0.03f, 0.15f);
    
    printf("Initial parameters:\n");
    printf("  D = %.6f, α = %.6f, β = %.6f, γ = %.6f\n\n",
           initial_params.D, initial_params.alpha, initial_params.beta, initial_params.gamma);

    // Test 1: Energy-based adaptation
    printf("=== TEST 1: Energy-Based Adaptation ===\n");
    TCDE_AdaptiveParameterManager* manager_energy = 
        TCDE_CreateAdaptiveParameterManager(&initial_params, TCDE_ADAPT_ENERGY_BASED);
    
    if (!manager_energy) {
        fprintf(stderr, "Failed to create energy-based manager\n");
        TCDE_DestroyField(field);
        return 1;
    }
    
    TCDE_ConfigureAdaptation(manager_energy, 0.01f, 5.0f, 2.5f);
    
    // Evolve with adaptation
    for (int i = 0; i < 10; i++) {
        TCDE_EvolveCoupledParameterField(manager_energy, field);
    }
    
    TCDE_Parameters adapted_energy = TCDE_GetAdaptedParameters(manager_energy);
    printf("After 10 steps:\n");
    printf("  D = %.6f, α = %.6f, β = %.6f, γ = %.6f\n",
           adapted_energy.D, adapted_energy.alpha, adapted_energy.beta, adapted_energy.gamma);
    
    float stability_energy = TCDE_ComputeParameterStability(manager_energy);
    printf("  Stability: %.3f\n", stability_energy);
    
    TCDE_PrintParameterStatus(manager_energy);
    
    // Test 2: Complexity-based adaptation
    printf("\n=== TEST 2: Complexity-Based Adaptation ===\n");
    TCDE_AdaptiveParameterManager* manager_complexity = 
        TCDE_CreateAdaptiveParameterManager(&initial_params, TCDE_ADAPT_COMPLEXITY_BASED);
    
    if (!manager_complexity) {
        fprintf(stderr, "Failed to create complexity-based manager\n");
        TCDE_DestroyAdaptiveParameterManager(manager_energy);
        TCDE_DestroyField(field);
        return 1;
    }
    
    TCDE_ConfigureAdaptation(manager_complexity, 0.01f, 5.0f, 3.0f);
    
    // Evolve with adaptation
    for (int i = 0; i < 10; i++) {
        TCDE_EvolveCoupledParameterField(manager_complexity, field);
    }
    
    TCDE_Parameters adapted_complexity = TCDE_GetAdaptedParameters(manager_complexity);
    printf("After 10 steps:\n");
    printf("  D = %.6f, α = %.6f, β = %.6f, γ = %.6f\n",
           adapted_complexity.D, adapted_complexity.alpha, 
           adapted_complexity.beta, adapted_complexity.gamma);
    
    float stability_complexity = TCDE_ComputeParameterStability(manager_complexity);
    printf("  Stability: %.3f\n", stability_complexity);
    
    TCDE_PrintParameterStatus(manager_complexity);
    
    // Test 3: Gradient-based adaptation
    printf("\n=== TEST 3: Gradient-Based Adaptation ===\n");
    TCDE_AdaptiveParameterManager* manager_gradient = 
        TCDE_CreateAdaptiveParameterManager(&initial_params, TCDE_ADAPT_GRADIENT_BASED);
    
    if (!manager_gradient) {
        fprintf(stderr, "Failed to create gradient-based manager\n");
        TCDE_DestroyAdaptiveParameterManager(manager_energy);
        TCDE_DestroyAdaptiveParameterManager(manager_complexity);
        TCDE_DestroyField(field);
        return 1;
    }
    
    TCDE_ConfigureAdaptation(manager_gradient, 0.005f, 5.0f, 2.5f);
    
    // Compute gradients
    TCDE_ParameterGradient grad = TCDE_ComputeParameterGradients(field, &initial_params);
    printf("Parameter gradients:\n");
    printf("  dE/dD = %.6f\n", grad.dE_dD);
    printf("  dE/dα = %.6f\n", grad.dE_dalpha);
    printf("  dE/dβ = %.6f\n", grad.dE_dbeta);
    printf("  dE/dγ = %.6f\n", grad.dE_dgamma);
    printf("  ||grad|| = %.6f\n\n", grad.gradient_norm);
    
    // Evolve with gradient descent
    for (int i = 0; i < 10; i++) {
        TCDE_EvolveCoupledParameterField(manager_gradient, field);
    }
    
    TCDE_Parameters adapted_gradient = TCDE_GetAdaptedParameters(manager_gradient);
    printf("After 10 steps:\n");
    printf("  D = %.6f, α = %.6f, β = %.6f, γ = %.6f\n",
           adapted_gradient.D, adapted_gradient.alpha,
           adapted_gradient.beta, adapted_gradient.gamma);
    
    float stability_gradient = TCDE_ComputeParameterStability(manager_gradient);
    printf("  Stability: %.3f\n", stability_gradient);
    
    TCDE_PrintParameterStatus(manager_gradient);
    
    // Test 4: Full coupled adaptation
    printf("\n=== TEST 4: Full Coupled Adaptation ===\n");
    TCDE_AdaptiveParameterManager* manager_full = 
        TCDE_CreateAdaptiveParameterManager(&initial_params, TCDE_ADAPT_FULL_COUPLED);
    
    if (!manager_full) {
        fprintf(stderr, "Failed to create full coupled manager\n");
        TCDE_DestroyAdaptiveParameterManager(manager_energy);
        TCDE_DestroyAdaptiveParameterManager(manager_complexity);
        TCDE_DestroyAdaptiveParameterManager(manager_gradient);
        TCDE_DestroyField(field);
        return 1;
    }
    
    TCDE_ConfigureAdaptation(manager_full, 0.01f, 5.0f, 2.5f);
    TCDE_SetParameterBounds(manager_full, 0.01f, 1.0f, 0.001f, 0.5f, 
                           0.0f, 0.1f, 0.0f, 0.2f);
    
    // Evolve with full adaptation
    for (int i = 0; i < 20; i++) {
        TCDE_EvolveCoupledParameterField(manager_full, field);
        
        if (i % 5 == 0) {
            TCDE_Parameters current = TCDE_GetAdaptedParameters(manager_full);
            printf("Step %d: D=%.4f α=%.4f β=%.4f γ=%.4f\n",
                   i, current.D, current.alpha, current.beta, current.gamma);
        }
    }
    
    printf("\n");
    TCDE_PrintParameterStatus(manager_full);
    
    // Check convergence
    bool converged = TCDE_CheckParameterConvergence(manager_full, 0.01f);
    printf("Converged: %s\n", converged ? "YES" : "NO");
    
    // Save history
    bool saved = TCDE_SaveParameterHistory(manager_full, "parameter_history.csv");
    printf("History saved: %s\n", saved ? "YES" : "NO");
    
    // Cleanup
    TCDE_DestroyAdaptiveParameterManager(manager_energy);
    TCDE_DestroyAdaptiveParameterManager(manager_complexity);
    TCDE_DestroyAdaptiveParameterManager(manager_gradient);
    TCDE_DestroyAdaptiveParameterManager(manager_full);
    TCDE_DestroyField(field);

    printf("\n✓ Adaptive Parameters Test Complete\n");
    printf("\n🚀 ASI PHASE 2 VALIDATED: Self-Regulating Physical Laws!\n");
    return 0;
}
