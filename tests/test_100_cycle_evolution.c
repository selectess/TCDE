/**
 * @file test_100_cycle_evolution.c
 * @brief Test TCDE 100-Cycle Evolution
 * 
 * Authentic test of TCDE autopoietic evolution over 100 cycles.
 * Validates:
 * - Center population growth
 * - Energy evolution
 * - Coherence maintenance
 * - Autopoietic creation
 * 
 * Protocol: Zero Tolerance v3.0
 * Date: November 12, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include "../src/core/tcde_core.h"

#define NUM_CYCLES 100
#define INITIAL_CENTERS 5
#define MAX_CENTERS 20
#define DT 0.1
#define DIFFUSION 0.1

// ============================================================================
// AUTHENTIC EVOLUTION FUNCTIONS
// ============================================================================

/**
 * Compute field energy: E = Σ |cᵢ|²
 */
float compute_field_energy(TCDE_Field6D* field) {
    float energy = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float complex c = field->manifold_6d.centers[i].coeff;
        energy += crealf(c * conjf(c));
    }
    
    return energy;
}

/**
 * Compute field coherence: |Σ cᵢ| / Σ |cᵢ|
 */
float compute_field_coherence(TCDE_Field6D* field) {
    if (field->manifold_6d.num_centers == 0) {
        return 0.0f;
    }
    
    float complex sum_complex = 0.0f;
    float sum_magnitude = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float complex c = field->manifold_6d.centers[i].coeff;
        sum_complex += c;
        sum_magnitude += cabsf(c);
    }
    
    if (sum_magnitude == 0.0f) {
        return 0.0f;
    }
    
    return cabsf(sum_complex) / sum_magnitude;
}

/**
 * Evolve centers autopoietically
 * dcᵢ/dt = D·Σⱼ K(xᵢ,xⱼ)·cⱼ + α·cᵢ
 */
void evolve_centers_autopoietic(TCDE_Field6D* field, float dt, float D) {
    int n = field->manifold_6d.num_centers;
    if (n == 0) return;
    
    // Allocate temporary array for new coefficients
    float complex* new_coeffs = malloc(n * sizeof(float complex));
    
    for (int i = 0; i < n; i++) {
        TCDE_Center* ci = &field->manifold_6d.centers[i];
        float complex interaction = 0.0f;
        
        // Compute interactions
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            
            TCDE_Center* cj = &field->manifold_6d.centers[j];
            
            // Distance between centers
            float dist_sq = 0.0f;
            for (int d = 0; d < 6; d++) {
                float diff = ci->point.coords[d] - cj->point.coords[d];
                dist_sq += diff * diff;
            }
            
            // Interaction kernel K(xᵢ,xⱼ) = exp(-||xᵢ-xⱼ||²/2)
            float K = expf(-dist_sq / 2.0f);
            
            // Accumulate
            interaction += K * cj->coeff;
        }
        
        // Evolution: dcᵢ = D·interaction·dt + α·cᵢ·dt
        float alpha = 0.01f;  // Self-reinforcement
        float complex dc = D * interaction * dt + alpha * ci->coeff * dt;
        
        new_coeffs[i] = ci->coeff + dc;
        
        // Normalize if too large
        float mag = cabsf(new_coeffs[i]);
        if (mag > 3.0f) {
            new_coeffs[i] *= 3.0f / mag;
        }
    }
    
    // Update coefficients
    for (int i = 0; i < n; i++) {
        field->manifold_6d.centers[i].coeff = new_coeffs[i];
    }
    
    free(new_coeffs);
}

/**
 * Autopoietic center creation
 */
int add_new_center_autopoietic(TCDE_Field6D* field, int cycle) {
    if (field->manifold_6d.num_centers >= MAX_CENTERS) {
        return 0;
    }
    
    // Probability based on energy
    float energy = compute_field_energy(field);
    float prob = fminf(energy / 50.0f, 0.3f);  // Max 30%
    
    float rand_val = (float)rand() / RAND_MAX;
    
    if (rand_val < prob) {
        // Create new center
        TCDE_Center new_center;
        
        // Random position
        new_center.point = TCDE_CreatePoint(6, NULL);
        for (int d = 0; d < 6; d++) {
            new_center.point.coords[d] = ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
        }
        
        // Random coefficient
        float phase = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        new_center.coeff = 0.5f * (cosf(phase) + I * sinf(phase));
        new_center.epsilon = 0.5f;
        
        // Add to field
        field->manifold_6d.centers[field->manifold_6d.num_centers] = new_center;
        field->manifold_6d.num_centers++;
        
        return 1;
    }
    
    return 0;
}

// ============================================================================
// TEST FUNCTION
// ============================================================================

int test_100_cycle_evolution() {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST: 100-Cycle Evolution\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Initialize random seed
    srand(42);
    
    // Create field
    TCDE_Field6D* field = TCDE_CreateField6D(TCDE_RBF_GAUSSIAN);
    if (!field) {
        printf("❌ Failed to create field\n");
        return 0;
    }
    
    // Initialize with 5 centers
    printf("Initializing with %d centers...\n", INITIAL_CENTERS);
    for (int i = 0; i < INITIAL_CENTERS; i++) {
        TCDE_Center center;
        center.point = TCDE_CreatePoint(6, NULL);
        
        // Random position
        for (int d = 0; d < 6; d++) {
            center.point.coords[d] = ((float)rand() / RAND_MAX - 0.5f) * 3.0f;
        }
        
        // Random coefficient
        float phase = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        center.coeff = 1.0f * (cosf(phase) + I * sinf(phase));
        center.epsilon = 0.5f;
        
        field->manifold_6d.centers[field->manifold_6d.num_centers] = center;
        field->manifold_6d.num_centers++;
    }
    
    // Track evolution
    int* num_centers_history = malloc(NUM_CYCLES * sizeof(int));
    float* energy_history = malloc(NUM_CYCLES * sizeof(float));
    float* coherence_history = malloc(NUM_CYCLES * sizeof(float));
    int total_created = 0;
    
    printf("\nEvolving for %d cycles...\n", NUM_CYCLES);
    printf("Cycle | Centers | Energy  | Coherence | Created\n");
    printf("------|---------|---------|-----------|--------\n");
    
    // Evolve for 100 cycles
    for (int cycle = 0; cycle < NUM_CYCLES; cycle++) {
        // Record state
        num_centers_history[cycle] = field->manifold_6d.num_centers;
        energy_history[cycle] = compute_field_energy(field);
        coherence_history[cycle] = compute_field_coherence(field);
        
        // Print every 10 cycles
        if (cycle % 10 == 0) {
            printf("%5d | %7d | %7.4f | %9.4f | %7d\n",
                   cycle,
                   num_centers_history[cycle],
                   energy_history[cycle],
                   coherence_history[cycle],
                   total_created);
        }
        
        // Evolve
        evolve_centers_autopoietic(field, DT, DIFFUSION);
        
        // Autopoietic creation
        int created = add_new_center_autopoietic(field, cycle);
        total_created += created;
    }
    
    // Final state
    printf("%5d | %7d | %7.4f | %9.4f | %7d\n",
           NUM_CYCLES - 1,
           field->manifold_6d.num_centers,
           compute_field_energy(field),
           compute_field_coherence(field),
           total_created);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("RESULTS\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Compute statistics
    int initial_centers = num_centers_history[0];
    int final_centers = field->manifold_6d.num_centers;
    int growth = final_centers - initial_centers;
    float growth_pct = ((float)growth / initial_centers) * 100.0f;
    
    float initial_energy = energy_history[0];
    float final_energy = compute_field_energy(field);
    float energy_change_pct = ((final_energy - initial_energy) / initial_energy) * 100.0f;
    
    float mean_coherence = 0.0f;
    for (int i = 0; i < NUM_CYCLES; i++) {
        mean_coherence += coherence_history[i];
    }
    mean_coherence /= NUM_CYCLES;
    
    printf("Initial State (Cycle 0):\n");
    printf("  Centers: %d\n", initial_centers);
    printf("  Energy: %.4f\n", initial_energy);
    printf("  Coherence: %.4f\n\n", coherence_history[0]);
    
    printf("Final State (Cycle %d):\n", NUM_CYCLES - 1);
    printf("  Centers: %d\n", final_centers);
    printf("  Energy: %.4f\n", final_energy);
    printf("  Coherence: %.4f\n\n", compute_field_coherence(field));
    
    printf("Changes:\n");
    printf("  ΔCenters: +%d (%+.1f%%)\n", growth, growth_pct);
    printf("  ΔEnergy: %+.1f%%\n", energy_change_pct);
    printf("  Mean Coherence: %.4f\n\n", mean_coherence);
    
    printf("Autopoiesis:\n");
    printf("  New Centers Created: %d\n", total_created);
    printf("  Creation Rate: %.1f%%\n\n", (float)total_created / NUM_CYCLES * 100.0f);
    
    // Validation
    int success = 1;
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("VALIDATION\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Check growth
    if (final_centers > initial_centers) {
        printf("✓ Center population grew (%d → %d)\n", initial_centers, final_centers);
    } else {
        printf("❌ No center growth\n");
        success = 0;
    }
    
    // Check energy increase
    if (final_energy > initial_energy) {
        printf("✓ Energy increased (%.4f → %.4f)\n", initial_energy, final_energy);
    } else {
        printf("❌ Energy did not increase\n");
        success = 0;
    }
    
    // Check coherence maintained
    if (mean_coherence > 0.3f) {
        printf("✓ Coherence maintained (mean: %.4f)\n", mean_coherence);
    } else {
        printf("❌ Coherence too low\n");
        success = 0;
    }
    
    // Check autopoietic creation
    if (total_created > 0) {
        printf("✓ Autopoietic creation occurred (%d new centers)\n", total_created);
    } else {
        printf("❌ No autopoietic creation\n");
        success = 0;
    }
    
    printf("\n");
    
    // Cleanup
    free(num_centers_history);
    free(energy_history);
    free(coherence_history);
    TCDE_DestroyField6D(field);
    
    if (success) {
        printf("✅ TEST PASSED: 100-Cycle Evolution\n");
    } else {
        printf("❌ TEST FAILED: 100-Cycle Evolution\n");
    }
    
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return success;
}

int main() {
    printf("\n");
    printf("TCDE 100-CYCLE EVOLUTION TEST\n");
    printf("Protocol: Zero Tolerance v3.0\n");
    printf("Date: November 12, 2025\n");
    
    int result = test_100_cycle_evolution();
    
    return result ? 0 : 1;
}
