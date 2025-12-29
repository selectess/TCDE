/**
 * @file tcde_feedback.c
 * @brief TCDE Feedback Loop Module Implementation
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#include "tcde_feedback.h"
#include "tcde_multimodal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// For strdup on some systems
#ifndef _POSIX_C_SOURCE
char* strdup(const char* s);
#endif

// ============================================================================
// ACTION GENERATION
// ============================================================================

TCDE_Action* TCDE_DecodeActionFromField(const TCDE_11D_Identity_System* system) {
    if (!system || !system->field_6d) return NULL;
    
    TCDE_Action* action = (TCDE_Action*)calloc(1, sizeof(TCDE_Action));
    if (!action) return NULL;
    
    // Find the center with highest magnitude (strongest intention)
    int max_idx = -1;
    float max_magnitude = 0.0f;
    
    for (int i = 0; i < system->field_6d->manifold_6d.num_centers; i++) {
        float mag = cabsf(system->field_6d->manifold_6d.centers[i].coeff);
        if (mag > max_magnitude) {
            max_magnitude = mag;
            max_idx = i;
        }
    }
    
    if (max_idx < 0) {
        free(action);
        return NULL;
    }
    
    // Extract source information
    TCDE_Point* source = &system->field_6d->manifold_6d.centers[max_idx].point;
    for (int d = 0; d < 6; d++) {
        action->source_position[d] = source->coords[d];
    }
    action->source_field = system->field_6d->manifold_6d.centers[max_idx].coeff;
    
    // Decode action type from modality coordinate
    float modality = source->coords[5];
    if (modality < 0.15f) {
        action->type = TCDE_ACTION_ANALYZE;  // Visual modality
    } else if (modality < 0.35f) {
        action->type = TCDE_ACTION_COMPUTE;  // Audio modality
    } else if (modality < 0.55f) {
        action->type = TCDE_ACTION_QUERY;    // Semantic modality
    } else {
        action->type = TCDE_ACTION_SYNTHESIZE; // Code modality
    }
    
    // Generate command based on action type and field state
    char command_buffer[256];
    switch (action->type) {
        case TCDE_ACTION_QUERY:
            snprintf(command_buffer, sizeof(command_buffer),
                    "echo 'Query: Field state at (%.2f, %.2f, %.2f)'",
                    source->coords[0], source->coords[1], source->coords[2]);
            break;
        case TCDE_ACTION_COMPUTE:
            snprintf(command_buffer, sizeof(command_buffer),
                    "echo 'Compute: magnitude=%.3f phase=%.3f'",
                    cabsf(action->source_field), cargf(action->source_field));
            break;
        case TCDE_ACTION_ANALYZE:
            snprintf(command_buffer, sizeof(command_buffer),
                    "echo 'Analyze: coherence=%.3f'",
                    system->metrics.field_coherence);
            break;
        case TCDE_ACTION_SYNTHESIZE:
            snprintf(command_buffer, sizeof(command_buffer),
                    "echo 'Synthesize: HIS=%.3f'",
                    system->metrics.holistic_identity_score);
            break;
        default:
            snprintf(command_buffer, sizeof(command_buffer), "echo 'Action'");
    }
    
    action->command = strdup(command_buffer);
    
    // Generate context
    char context_buffer[512];
    snprintf(context_buffer, sizeof(context_buffer),
            "Field: %d centers, Energy: %.3f, DRC: %d, Autonomy: %.3f",
            system->field_6d->manifold_6d.num_centers,
            system->total_energy,
            system->drc_state,
            system->metrics.autonomy);
    action->context = strdup(context_buffer);
    
    // Set confidence and urgency based on field properties
    action->confidence = fminf(max_magnitude, 1.0f);
    action->urgency = system->drc_metrics.dissonance_level;
    
    return action;
}

void TCDE_FreeAction(TCDE_Action* action) {
    if (!action) return;
    
    if (action->command) {
        free(action->command);
        action->command = NULL;
    }
    
    if (action->context) {
        free(action->context);
        action->context = NULL;
    }
    
    free(action);
}

// ============================================================================
// SANDBOX EXECUTION
// ============================================================================

void TCDE_InitializeSandbox(TCDE_SandboxConfig* config) {
    if (!config) return;
    
    // Default secure configuration
    config->allow_file_read = false;
    config->allow_file_write = false;
    config->allow_network = false;
    config->allow_system_calls = false;
    
    // Resource limits
    config->max_execution_time = 1.0f;  // 1 second
    config->max_memory = 10 * 1024 * 1024;  // 10 MB
    config->max_output_size = 1024 * 1024;  // 1 MB
    
    // No allowed paths by default
    config->num_allowed_paths = 0;
}

TCDE_ActionResult* TCDE_ExecuteActionInSandbox(const TCDE_Action* action,
                                                 const TCDE_SandboxConfig* config) {
    if (!action || !config) return NULL;
    
    TCDE_ActionResult* result = (TCDE_ActionResult*)calloc(1, sizeof(TCDE_ActionResult));
    if (!result) return NULL;
    
    result->timestamp = time(NULL);
    
    // For now, simulate execution with echo commands
    // In production, this would use proper sandboxing (containers, seccomp, etc.)
    
    if (!action->command) {
        result->success = false;
        result->error = strdup("No command provided");
        return result;
    }
    
    // Simple execution via popen (not truly sandboxed - for demonstration)
    clock_t start = clock();
    FILE* pipe = popen(action->command, "r");
    
    if (!pipe) {
        result->success = false;
        result->error = strdup("Failed to execute command");
        return result;
    }
    
    // Read output
    char buffer[4096];
    size_t total_read = 0;
    char* output_buffer = NULL;
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        if (total_read + len > config->max_output_size) {
            result->memory_exceeded = true;
            break;
        }
        
        char* new_buffer = (char*)realloc(output_buffer, total_read + len + 1);
        if (!new_buffer) break;
        
        output_buffer = new_buffer;
        memcpy(output_buffer + total_read, buffer, len);
        total_read += len;
        output_buffer[total_read] = '\0';
    }
    
    result->exit_code = pclose(pipe);
    clock_t end = clock();
    
    result->execution_time = (float)(end - start) / CLOCKS_PER_SEC;
    result->memory_used = total_read;
    result->success = (result->exit_code == 0);
    result->output = output_buffer ? output_buffer : strdup("");
    result->error = result->success ? strdup("") : strdup("Execution failed");
    result->timeout = (result->execution_time > config->max_execution_time);
    
    return result;
}

void TCDE_FreeActionResult(TCDE_ActionResult* result) {
    if (!result) return;
    
    if (result->output) {
        free(result->output);
        result->output = NULL;
    }
    
    if (result->error) {
        free(result->error);
        result->error = NULL;
    }
    
    free(result);
}

// ============================================================================
// PERCEPTION AND INTROSPECTION
// ============================================================================

bool TCDE_PerceiveResultIn2D(const TCDE_ActionResult* result,
                              TCDE_Field* field_2d) {
    if (!result || !field_2d) return false;
    if (!result->output) return false;
    
    // Parse result text and create perturbation in 2D field
    // Use text processing to extract semantic features
    
    // Simple approach: create centers based on output characteristics
    float success_value = result->success ? 1.0f : -1.0f;
    float time_factor = fminf(result->execution_time, 1.0f);
    
    // For simplicity, we'll use the 6D manifold with 2D-like coordinates
    // In a full implementation, this would properly use the 2D slice
    
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    
    TCDE_Complex coeff = success_value * (1.0f - time_factor) + 
                         0.0f * I;  // Phase represents timing
    
    bool added = TCDE_AddCenter6D(field_2d, &p, coeff, 0.1f);
    
    TCDE_FreePoint(&p);
    
    return added;
}

float TCDE_ComputeIntrospection2D(const TCDE_Field* field_2d,
                                   const TCDE_Complex* expected_pattern) {
    if (!field_2d) return 0.0f;
    
    // Compute introspection score based on field coherence
    float total_magnitude = 0.0f;
    float phase_variance = 0.0f;
    float mean_phase = 0.0f;
    
    int num_centers = field_2d->slice_2d.num_centers;
    if (num_centers == 0) return 0.0f;
    
    // Compute mean phase
    for (int i = 0; i < num_centers; i++) {
        TCDE_Complex c = field_2d->slice_2d.centers[i].coeff;
        mean_phase += cargf(c);
        total_magnitude += cabsf(c);
    }
    mean_phase /= num_centers;
    
    // Compute phase variance
    for (int i = 0; i < num_centers; i++) {
        TCDE_Complex c = field_2d->slice_2d.centers[i].coeff;
        float phase_diff = cargf(c) - mean_phase;
        phase_variance += phase_diff * phase_diff;
    }
    phase_variance /= num_centers;
    
    // Introspection score: high magnitude + low phase variance = high score
    float magnitude_score = fminf(total_magnitude / num_centers, 1.0f);
    float coherence_score = 1.0f / (1.0f + phase_variance);
    
    float introspection = 0.6f * magnitude_score + 0.4f * coherence_score;
    
    // If expected pattern provided, compare
    if (expected_pattern) {
        // Compute similarity to expected pattern
        float similarity = 0.0f;
        for (int i = 0; i < num_centers && i < 10; i++) {
            TCDE_Complex c = field_2d->slice_2d.centers[i].coeff;
            TCDE_Complex diff = c - expected_pattern[i];
            similarity += 1.0f / (1.0f + cabsf(diff));
        }
        similarity /= fminf(num_centers, 10);
        
        introspection = 0.7f * introspection + 0.3f * similarity;
    }
    
    return introspection;
}

// ============================================================================
// FEEDBACK INTEGRATION
// ============================================================================

bool TCDE_IntegrateFeedbackTo6D(TCDE_11D_Identity_System* system,
                                 const TCDE_Field* field_2d,
                                 float introspection_score) {
    if (!system || !field_2d || !system->field_6d) return false;
    
    // Lift 2D insights to 6D and integrate via coupling
    
    // For each 2D center, create corresponding 6D perturbation
    for (int i = 0; i < field_2d->slice_2d.num_centers && i < 5; i++) {
        TCDE_Point* p2d = &field_2d->slice_2d.centers[i].point;
        TCDE_Complex coeff2d = field_2d->slice_2d.centers[i].coeff;
        
        // Lift to 6D: use 2D coords for x,y, set others to current field average
        float coords6d[6];
        coords6d[0] = p2d->coords[0];
        coords6d[1] = p2d->coords[1];
        coords6d[2] = 0.5f;  // z at middle
        coords6d[3] = 1.0f;  // τ₁ present
        coords6d[4] = 0.0f;  // τ₂ no memory
        coords6d[5] = 0.4f;  // semantic modality
        
        TCDE_Point p6d = TCDE_CreatePoint(6, coords6d);
        
        // Scale coefficient by introspection score
        TCDE_Complex coeff6d = coeff2d * introspection_score * 0.1f;
        
        // Add as perturbation to 6D field
        TCDE_AddCenter6D(system->field_6d, &p6d, coeff6d, 0.05f);
        
        TCDE_FreePoint(&p6d);
    }
    
    return true;
}

// ============================================================================
// COMPLETE FEEDBACK CYCLE
// ============================================================================

TCDE_FeedbackHistory* TCDE_InitializeFeedbackHistory(int capacity) {
    if (capacity <= 0) return NULL;
    
    TCDE_FeedbackHistory* history = (TCDE_FeedbackHistory*)calloc(1, sizeof(TCDE_FeedbackHistory));
    if (!history) return NULL;
    
    history->cycles = (TCDE_FeedbackCycle*)calloc(capacity, sizeof(TCDE_FeedbackCycle));
    if (!history->cycles) {
        free(history);
        return NULL;
    }
    
    history->capacity = capacity;
    history->num_cycles = 0;
    history->total_learning = 0.0f;
    history->average_success_rate = 0.0f;
    history->average_latency = 0.0f;
    history->successful_cycles = 0;
    history->failed_cycles = 0;
    
    return history;
}

TCDE_FeedbackCycle TCDE_ExecuteCompleteFeedbackCycle(
    TCDE_11D_Identity_System* system,
    const TCDE_SandboxConfig* sandbox_config,
    TCDE_FeedbackHistory* history) {
    
    TCDE_FeedbackCycle cycle = {0};
    
    if (!system || !sandbox_config) return cycle;
    
    cycle.cycle_id = history ? history->num_cycles : 0;
    cycle.timestamp = time(NULL);
    
    // 1. Measure initial dissonance
    cycle.dissonance_before = system->drc_metrics.dissonance_level;
    
    // 2. Decode action from 6D field state
    TCDE_Action* action = TCDE_DecodeActionFromField(system);
    if (!action) {
        cycle.learning_rate = 0.0f;
        return cycle;
    }
    
    // Copy action data (deep copy strings)
    cycle.action = *action;
    cycle.action.command = action->command ? strdup(action->command) : NULL;
    cycle.action.context = action->context ? strdup(action->context) : NULL;
    
    // 3. Execute action in sandbox
    TCDE_ActionResult* result = TCDE_ExecuteActionInSandbox(action, sandbox_config);
    if (!result) {
        TCDE_FreeAction(action);
        if (cycle.action.command) free(cycle.action.command);
        if (cycle.action.context) free(cycle.action.context);
        cycle.learning_rate = 0.0f;
        return cycle;
    }
    
    // Copy result data (deep copy strings)
    cycle.result = *result;
    cycle.result.output = result->output ? strdup(result->output) : NULL;
    cycle.result.error = result->error ? strdup(result->error) : NULL;
    
    // 4. Perceive result in 2D field
    TCDE_Field* field_2d = TCDE_CreateField(50, 2.0f);  // Temporary 2D field
    if (field_2d) {
        TCDE_PerceiveResultIn2D(result, field_2d);
        
        // 5. Compute introspection on 2D
        cycle.introspection_score = TCDE_ComputeIntrospection2D(field_2d, NULL);
        
        // 6. Integrate feedback to 6D
        TCDE_IntegrateFeedbackTo6D(system, field_2d, cycle.introspection_score);
        
        TCDE_DestroyField(field_2d);
    }
    
    // 7. Measure final dissonance
    cycle.dissonance_after = system->drc_metrics.dissonance_level;
    cycle.coherence_change = cycle.dissonance_before - cycle.dissonance_after;
    
    // 8. Compute learning rate
    cycle.learning_rate = cycle.coherence_change * cycle.introspection_score;
    
    // 9. Update history
    if (history) {
        if (history->num_cycles < history->capacity) {
            history->cycles[history->num_cycles] = cycle;
            history->num_cycles++;
        }
        
        history->total_learning += cycle.learning_rate;
        
        if (result->success) {
            history->successful_cycles++;
        } else {
            history->failed_cycles++;
        }
        
        int total = history->successful_cycles + history->failed_cycles;
        if (total > 0) {
            history->average_success_rate = (float)history->successful_cycles / total;
        }
        
        history->average_latency = (history->average_latency * (history->num_cycles - 1) +
                                    result->execution_time) / history->num_cycles;
    }
    
    // Cleanup
    TCDE_FreeAction(action);
    TCDE_FreeActionResult(result);
    
    return cycle;
}

void TCDE_FreeFeedbackHistory(TCDE_FeedbackHistory* history) {
    if (!history) return;
    
    if (history->cycles) {
        // Free individual cycle data
        for (int i = 0; i < history->num_cycles; i++) {
            if (history->cycles[i].action.command) {
                free(history->cycles[i].action.command);
            }
            if (history->cycles[i].action.context) {
                free(history->cycles[i].action.context);
            }
            if (history->cycles[i].result.output) {
                free(history->cycles[i].result.output);
            }
            if (history->cycles[i].result.error) {
                free(history->cycles[i].result.error);
            }
        }
        
        free(history->cycles);
        history->cycles = NULL;
    }
    
    free(history);
}

// ============================================================================
// METRICS AND ANALYSIS
// ============================================================================

float TCDE_ComputeFeedbackEfficiency(const TCDE_FeedbackHistory* history) {
    if (!history || history->num_cycles == 0) return 0.0f;
    
    // Efficiency = success_rate * learning_rate / latency
    float learning_per_cycle = history->total_learning / history->num_cycles;
    float efficiency = history->average_success_rate * learning_per_cycle;
    
    if (history->average_latency > 0.0f) {
        efficiency /= history->average_latency;
    }
    
    return fminf(efficiency, 1.0f);
}

float TCDE_AnalyzeLearningProgress(const TCDE_FeedbackHistory* history) {
    if (!history || history->num_cycles < 2) return 0.0f;
    
    // Compute trend in learning rate over recent cycles
    int window = fminf(10, history->num_cycles);
    float recent_learning = 0.0f;
    
    for (int i = history->num_cycles - window; i < history->num_cycles; i++) {
        recent_learning += history->cycles[i].learning_rate;
    }
    
    return recent_learning / window;
}

void TCDE_PrintFeedbackStatistics(const TCDE_FeedbackHistory* history) {
    if (!history) {
        printf("No feedback history available\n");
        return;
    }
    
    printf("\n=== Feedback Loop Statistics ===\n");
    printf("Total cycles: %d\n", history->num_cycles);
    printf("Successful: %d (%.1f%%)\n", 
           history->successful_cycles,
           history->average_success_rate * 100.0f);
    printf("Failed: %d\n", history->failed_cycles);
    printf("Total learning: %.3f\n", history->total_learning);
    printf("Average latency: %.3f ms\n", history->average_latency * 1000.0f);
    printf("Efficiency: %.3f\n", TCDE_ComputeFeedbackEfficiency(history));
    printf("Learning progress: %.3f\n", TCDE_AnalyzeLearningProgress(history));
    printf("================================\n\n");
}
