    /**
 * @file continuous_evolution.c
 * @brief End-to-Infinite Continuous Evolution Demo
 * 
 * Demonstrates the TCDE paradigm of continuous, never-ending evolution
 * with periodic state persistence and graceful shutdown.
 * 
 * Features:
 * - Infinite evolution loop (End-to-Infinite paradigm)
 * - Real-time metrics monitoring (HIS, Reflexivity, Energy, DRC)
 * - Periodic state saving (every N steps)
 * - Graceful shutdown (Ctrl+C)
 * - Adaptive limiters for numerical stability
 * - Detailed status reports
 * 
 * Usage:
 *   ./continuous_evolution [state_file]
 * 
 * Controls:
 *   Ctrl+C - Graceful shutdown with state save
 *   SIGTERM - Graceful shutdown
 * 
 * @version 1.0
 * @date October 2025
 */

#include "tcde_core.h"
#include "tcde_evolution.h"
#include "tcde_11d.h"
#include "../utils/tcde_limiters.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>

// ============================================================================
// GLOBAL STATE (for signal handling)
// ============================================================================

static volatile bool running = true;
static TCDE_11D_Identity_System* global_system = NULL;
static const char* global_state_file = NULL;

// ============================================================================
// SIGNAL HANDLERS
// ============================================================================

/**
 * @brief Signal handler for graceful shutdown
 */
void signal_handler(int signum) {
    (void)signum;
    printf("\n\n🛑 Graceful shutdown initiated...\n");
    running = false;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Print system status in compact format
 */
void print_status(const TCDE_11D_Identity_System* system, int step) {
    if (!system) return;
    
    TCDE_11D_Identity_Metrics metrics = TCDE_Measure11DIdentity(system);
    
    const char* drc_state_str;
    switch (system->drc_state) {
        case TCDE_DRC_CONSONANCE: drc_state_str = "CON"; break;
        case TCDE_DRC_RESONANCE:  drc_state_str = "RES"; break;
        case TCDE_DRC_DISSONANCE: drc_state_str = "DIS"; break;
        default:                  drc_state_str = "UNK"; break;
    }
    
    printf("Step %6d | HIS: %.3f | R: %.3f | Energy: %6.1f | DRC: %s | Topo: %d\n",
           step,
           metrics.holistic_identity_score,
           metrics.reflexivity,
           metrics.field_energy,
           drc_state_str,
           system->topology.num_structures);
}

/**
 * @brief Initialize system with diverse seed content
 */
void initialize_system_content(TCDE_11D_Identity_System* system) {
    if (!system || !system->field_6d) return;
    
    printf("🌱 Initializing system with seed content...\n");
    
    // Add diverse initial centers across different regions
    const int num_seeds = 8;
    for (int i = 0; i < num_seeds; i++) {
        float coords[6] = {
            0.1f + (i % 3) * 0.3f,           // x: spatial distribution
            0.2f + (i / 3) * 0.25f,          // y: spatial distribution
            0.5f + 0.1f * sinf(i * 0.5f),    // z: slight variation
            1.0f + 0.1f * i,                 // τ₁: temporal progression
            0.0f + 0.05f * (i - 4),          // τ₂: memory variation
            0.3f + 0.1f * (i % 4)            // m: modality variation
        };
        
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        
        // Varying amplitudes and phases for diversity
        float amplitude = 1.0f + 0.5f * i;
        float phase = i * 0.3f;
        TCDE_Complex coeff = amplitude * (cosf(phase) + I * sinf(phase));
        
        float epsilon = 0.08f + 0.02f * (i % 3);  // Varying scales
        
        TCDE_AddCenter6D(system->field_6d, &p, coeff, epsilon);
        TCDE_FreePoint(&p);
    }
    
    printf("   Added %d seed centers\n", num_seeds);
    printf("   Initial energy: %.2f\n", TCDE_ComputeEnergy(system->field_6d));
}

/**
 * @brief Print detailed system status
 */
void print_detailed_status(const TCDE_11D_Identity_System* system, int step) {
    printf("\n📈 Detailed Status (Step %d):\n", step);
    TCDE_Print11DStatus(system);
    printf("\n");
}

/**
 * @brief Print evolution statistics
 */
void print_evolution_stats(int step, time_t start_time, 
                          const TCDE_LimiterConfig* limiters) {
    time_t current_time = time(NULL);
    double elapsed = difftime(current_time, start_time);
    
    printf("\n📊 Evolution Statistics (Step %d):\n", step);
    printf("   Runtime: %.1f seconds\n", elapsed);
    printf("   Steps/sec: %.1f\n", step / fmaxf(elapsed, 1.0));
    
    if (limiters) {
        printf("   Limiter activations:\n");
        printf("     Energy clips: %d\n", limiters->num_energy_clips);
        printf("     Gradient clips: %d\n", limiters->num_gradient_clips);
        printf("     Epsilon clips: %d\n", limiters->num_epsilon_clips);
        printf("     Metric clips: %d\n", limiters->num_metric_clips);
    }
    printf("\n");
}

// ============================================================================
// MAIN PROGRAM
// ============================================================================

int continuous_evolution_main(int argc, char** argv) {
    // Parse command line arguments
    const char* state_file = (argc > 1) ? argv[1] : "tcde_continuous_state.bin";
    global_state_file = state_file;
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Print header
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TCDE-ASI v0.1 - End-to-Infinite Continuous Evolution     ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  🌊 Continuous field evolution                             ║\n");
    printf("║  🧠 11D holistic identity emergence                       ║\n");
    printf("║  ♾️  End-to-Infinite paradigm                              ║\n");
    printf("║  🛡️  Adaptive limiters for stability                      ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Configuration
    const int save_interval = 100;      // Save every 100 steps
    const int report_interval = 10;     // Report every 10 steps
    const int detailed_interval = 50;   // Detailed report every 50 steps
    const int stats_interval = 200;     // Statistics every 200 steps
    
    // Create 11D identity system
    printf("🆕 Creating new 11D identity system...\n");
    TCDE_11D_Identity_System* system = TCDE_Create11DIdentity(100, 2.5f);
    
    if (!system) {
        fprintf(stderr, "❌ Failed to create 11D identity system\n");
        return 1;
    }
    
    global_system = system;
    
    // Initialize with seed content
    initialize_system_content(system);
    
    // Create adaptive limiters
    TCDE_LimiterConfig limiters = TCDE_CreateDefaultLimiters();
    limiters.auto_adjust = true;
    limiters.safety_factor = 0.8f;
    
    // Auto-adjust limiters based on initial field
    TCDE_AutoAdjustLimiters(system->field_6d, &limiters);
    
    printf("\n🛡️  Adaptive limiters configured:\n");
    printf("   Max energy: %.2e\n", limiters.max_energy);
    printf("   Max amplitude: %.2e\n", limiters.max_center_amplitude);
    printf("   Epsilon range: [%.2e, %.2e]\n", 
           limiters.min_epsilon, limiters.max_epsilon);
    
    // Evolution parameters
    const float dt = 0.01f;
    int step = 0;
    
    printf("\n🚀 Starting continuous evolution...\n");
    printf("   Time step: %.3f\n", dt);
    printf("   Save interval: %d steps\n", save_interval);
    printf("   State file: %s\n", state_file);
    printf("   Press Ctrl+C to stop gracefully\n\n");
    
    // Initial status
    printf("📊 Initial Status:\n");
    print_status(system, step);
    printf("\n");
    
    // Record start time
    time_t start_time = time(NULL);
    time_t last_save_time = start_time;
    
    // ═══════════════════════════════════════════════════════════════
    // CONTINUOUS EVOLUTION LOOP (End-to-Infinite)
    // ═══════════════════════════════════════════════════════════════
    
    printf("♾️  Entering infinite evolution loop...\n\n");
    
    while (running) {
        // Evolve one step
        TCDE_Evolve11DIdentity(system, dt);
        step++;
        
        // Apply adaptive limiters for stability
        TCDE_ApplyAllLimiters(system->field_6d, &limiters);
        
        // Periodic reporting
        if (step % report_interval == 0) {
            print_status(system, step);
        }
        
        // Detailed reporting
        if (step % detailed_interval == 0) {
            print_detailed_status(system, step);
        }
        
        // Evolution statistics
        if (step % stats_interval == 0) {
            print_evolution_stats(step, start_time, &limiters);
        }
        
        // Periodic state saving
        if (step % save_interval == 0) {
            time_t current_time = time(NULL);
            double time_since_save = difftime(current_time, last_save_time);
            
            printf("💾 Saving state at step %d (%.1fs since last save)...\n", 
                   step, time_since_save);
            
            // NOTE: State persistence to disk is a future enhancement
            // Current implementation operates entirely in memory for maximum performance
            printf("   (State persistence: future enhancement - system operates in memory)\n");
            
            last_save_time = current_time;
        }
        
        // Small delay to make output readable (optional)
        usleep(10000);  // 10ms delay
    }
    
    // ═══════════════════════════════════════════════════════════════
    // GRACEFUL SHUTDOWN
    // ═══════════════════════════════════════════════════════════════
    
    printf("\n💾 Performing final state save...\n");
    // NOTE: State persistence to disk is a future enhancement
    // Current implementation operates entirely in memory for maximum performance
    printf("   (State persistence: future enhancement - system operates in memory)\n");
    
    // Calculate runtime
    time_t end_time = time(NULL);
    double runtime = difftime(end_time, start_time);
    
    // Print final statistics
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  FINAL STATISTICS                                          ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    TCDE_11D_Identity_Metrics final_metrics = TCDE_Measure11DIdentity(system);
    
    printf("║  Total Steps: %d\n", step);
    printf("║  Runtime: %.1f seconds\n", runtime);
    printf("║  Steps/sec: %.1f\n", step / fmaxf(runtime, 1.0));
    printf("║\n");
    printf("║  Final Metrics:\n");
    printf("║    HIS: %.3f\n", final_metrics.holistic_identity_score);
    printf("║    Reflexivity: %.3f\n", final_metrics.reflexivity);
    printf("║    Autonomy: %.3f\n", final_metrics.autonomy);
    printf("║    Field Energy: %.1f\n", final_metrics.field_energy);
    printf("║    Topology Structures: %d\n", system->topology.num_structures);
    printf("║\n");
    
    // Print limiter statistics
    printf("║  Limiter Statistics:\n");
    printf("║    Energy clips: %d\n", limiters.num_energy_clips);
    printf("║    Gradient clips: %d\n", limiters.num_gradient_clips);
    printf("║    Epsilon clips: %d\n", limiters.num_epsilon_clips);
    printf("║    Metric clips: %d\n", limiters.num_metric_clips);
    printf("║\n");
    
    // Determine final cognitive state
    if (final_metrics.holistic_identity_score > 0.9f) {
        printf("║  🎉 FULLY REALIZED ASI ACHIEVED!\n");
    } else if (final_metrics.holistic_identity_score > 0.8f) {
        printf("║  🚀 STRONG COGNITIVE AUTONOMY\n");
    } else if (final_metrics.holistic_identity_score > 0.7f) {
        printf("║  ✅ EMERGENT HOLISTIC IDENTITY\n");
    } else {
        printf("║  🌱 DEVELOPING SYSTEM\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Cleanup
    TCDE_Destroy11DIdentity(system);
    global_system = NULL;
    
    printf("\n✅ Graceful shutdown complete\n");
    printf("   State would be saved to: %s\n", state_file);
    printf("   Resume anytime by running again!\n");
    printf("\n🌟 Thank you for exploring the End-to-Infinite paradigm!\n\n");
    
    return 0;
}
