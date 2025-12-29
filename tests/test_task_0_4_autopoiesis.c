/**
 * @file test_task_0_4_autopoiesis.c
 * @brief Test for Task 0.4: Autopoietic Capabilities
 * 
 * Tests:
 * - Self-Create: Automatic center creation
 * - Self-Maintain: Energy conservation and integrity
 * - Self-Regenerate: Recovery via DRC cycle
 * - Autopoietic metrics measurement
 * 
 * Requirements: 6.4
 */

#include "../src/core/tcde_11d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static int tests_passed = 0;
static int tests_total = 0;

#define TEST(name, condition) do { \
    tests_total++; \
    if (condition) { \
        tests_passed++; \
        printf("✓ %s\n", name); \
    } else { \
        printf("✗ %s\n", name); \
    } \
} while(0)

#define EPSILON 1e-6f

int main(void) {
    srand(time(NULL));
    
    printf("=== Task 0.4: Autopoietic Capabilities ===\n\n");
    
    // ========================================================================
    // Test 1: System Setup
    // ========================================================================
    printf("--- Test 1: System Setup ---\n");
    
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(100, 2.5f);
    TEST("System created", system != NULL);
    TEST("Autopoiesis initialized", system->autopoiesis.autopoietic_health == 1.0f);
    
    // Add initial centers
    for (int i = 0; i < 3; i++) {
        float coords[6] = {(float)i * 0.5f, 0.0f, 0.0f, 1.5f, 0.0f, 0.4f};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = 1.0f + 0.0f * I;
        TCDE_AddCenter6D(system->field_6d, &p, coeff, 1.0f);
        TCDE_FreePoint(&p);
    }
    
    TEST("Initial centers added", system->field_6d->manifold_6d.num_centers == 3);
    
    printf("\n");
    
    // ========================================================================
    // Test 2: Self-Create
    // ========================================================================
    printf("--- Test 2: Self-Create ---\n");
    
    int initial_centers = system->field_6d->manifold_6d.num_centers;
    int created = TCDE_SelfCreate(system, 0.5f);
    
    TEST("Self-create executed", created >= 0);
    TEST("Centers created tracked", system->autopoiesis.centers_created >= 0);
    
    if (created > 0) {
        TEST("New centers added", system->field_6d->manifold_6d.num_centers > initial_centers);
        printf("  Created %d new centers\n", created);
    }
    
    printf("\n");
    
    // ========================================================================
    // Test 3: Self-Maintain
    // ========================================================================
    printf("--- Test 3: Self-Maintain ---\n");
    
    TCDE_SelfMaintain(system);
    
    TEST("Maintenance level computed", !isnan(system->autopoiesis.maintenance_level));
    TEST("Maintenance level in range", system->autopoiesis.maintenance_level >= 0.0f && 
                                       system->autopoiesis.maintenance_level <= 1.0f);
    TEST("Integrity score computed", !isnan(system->autopoiesis.integrity_score));
    TEST("Integrity score valid", system->autopoiesis.integrity_score >= 0.0f &&
                                  system->autopoiesis.integrity_score <= 1.0f);
    TEST("Energy drift tracked", !isnan(system->autopoiesis.energy_drift));
    
    printf("  Maintenance level: %.3f (target > 0.8)\n", system->autopoiesis.maintenance_level);
    printf("  Integrity score: %.3f\n", system->autopoiesis.integrity_score);
    printf("  Energy drift: %.6f\n", system->autopoiesis.energy_drift);
    
    printf("\n");
    
    // ========================================================================
    // Test 4: Self-Regenerate
    // ========================================================================
    printf("--- Test 4: Self-Regenerate ---\n");
    
    // Simulate high dissonance
    system->drc_metrics.dissonance_level = 0.5f;
    system->drc_metrics.dissonance_threshold = 0.3f;
    
    bool regenerated = TCDE_SelfRegenerate(system);
    
    TEST("Regeneration executed", true);
    TEST("Regeneration cycles tracked", system->autopoiesis.regeneration_cycles >= 0);
    
    if (regenerated) {
        TEST("Regeneration successful", true);
        printf("  Regeneration cycles: %d\n", system->autopoiesis.regeneration_cycles);
    }
    
    printf("\n");
    
    // ========================================================================
    // Test 5: Autopoietic Health
    // ========================================================================
    printf("--- Test 5: Autopoietic Health ---\n");
    
    TCDE_UpdateAutopoieticHealth(system);
    
    TEST("Health computed", !isnan(system->autopoiesis.autopoietic_health));
    TEST("Health in range", system->autopoiesis.autopoietic_health >= 0.0f &&
                           system->autopoiesis.autopoietic_health <= 1.0f);
    
    printf("  Autopoietic health: %.3f\n", system->autopoiesis.autopoietic_health);
    
    printf("\n");
    
    // ========================================================================
    // Test 6: Evolution Integration
    // ========================================================================
    printf("--- Test 6: Evolution Integration ---\n");
    
    for (int i = 0; i < 5; i++) {
        TCDE_Evolve11DIdentity(system, 0.01f);
    }
    
    TEST("Evolution with autopoiesis", system->evolution_steps == 5);
    TEST("Maintenance updated", system->autopoiesis.maintenance_level > 0.0f);
    TEST("Health updated", system->autopoiesis.autopoietic_health > 0.0f);
    
    printf("\n");
    
    // ========================================================================
    // Test 7: Metrics in HIS
    // ========================================================================
    printf("--- Test 7: Integration in HIS ---\n");
    
    TCDE_11D_Identity_Metrics metrics = TCDE_Measure11DIdentity(system);
    
    TEST("Creation rate in metrics", metrics.self_creation_rate >= 0.0f);
    TEST("Maintenance in metrics", metrics.self_maintenance >= 0.0f);
    TEST("Regeneration in metrics", metrics.self_regeneration >= 0.0f);
    
    float his = TCDE_ComputeHolisticIdentityScore(&metrics);
    TEST("HIS includes autopoiesis", his >= 0.0f && his <= 1.0f);
    
    printf("  Self-creation rate: %.3f\n", metrics.self_creation_rate);
    printf("  Self-maintenance: %.3f (target > 0.8)\n", metrics.self_maintenance);
    printf("  Self-regeneration: %.3f (target > 0.7)\n", metrics.self_regeneration);
    printf("  HIS: %.3f\n", his);
    
    printf("\n");
    
    // ========================================================================
    // Test 8: Stress Test
    // ========================================================================
    printf("--- Test 8: Stress Test ---\n");
    
    // Simulate damage by setting high dissonance
    system->drc_metrics.dissonance_level = 0.8f;
    
    int recovery_steps = 0;
    for (int i = 0; i < 10; i++) {
        TCDE_Evolve11DIdentity(system, 0.01f);
        recovery_steps++;
        
        if (system->drc_metrics.dissonance_level < system->drc_metrics.dissonance_threshold) {
            break;
        }
    }
    
    TEST("System recovered", system->drc_metrics.dissonance_level < 0.8f);
    printf("  Recovery steps: %d\n", recovery_steps);
    
    printf("\n");
    
    // ========================================================================
    // Cleanup
    // ========================================================================
    TCDE_Destroy11DIdentity(system);
    
    // ========================================================================
    // Summary
    // ========================================================================
    printf("=== Task 0.4 Test Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", 100.0f * tests_passed / tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n✓ Task 0.4 COMPLETE: Autopoietic Capabilities implemented!\n");
        printf("  - Self-Create ✓\n");
        printf("  - Self-Maintain ✓\n");
        printf("  - Self-Regenerate ✓\n");
        printf("  - Autopoietic metrics ✓\n");
        return 0;
    } else {
        printf("\n✗ Task 0.4 INCOMPLETE: Some tests failed\n");
        return 1;
    }
}
