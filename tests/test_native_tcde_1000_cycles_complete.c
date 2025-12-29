/**
 * @file test_native_tcde_1000_cycles_complete.c
 * @brief Complete Native TCDE Test - 1000 Cycles with Full Monitoring
 * 
 * Academic-grade demonstration of TCDE native functionality:
 * - 1000 evolution cycles
 * - Multi-dimensional monitoring (6D + 2D projection)
 * - Topological dynamics analysis
 * - Fractal dimension evolution
 * - Temporal dimension tracking
 * - Energy conservation
 * - Phase space trajectories
 * - Coherence metrics
 * - Statistical analysis
 * 
 * @date January 2025
 */

#include "tcde_monolithic.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// ============================================================================
// Monitoring Structures
// ============================================================================

typedef struct {
    float time;
    float energy;
    float temporal_dim;
    float fractal_dim_measured;
    int num_centers_6d;
    int num_centers_2d;
    
    // Field statistics
    float mean_amplitude;
    float std_amplitude;
    float max_amplitude;
    float min_amplitude;
    
    // Topological metrics
    float torsion_magnitude;
    float coupling_strength;
    float coherence;
    
    // Phase space
    float phase_mean;
    float phase_std;
    
    // Gradient metrics
    float gradient_norm_mean;
    float gradient_norm_max;
    
} CycleSnapshot;

typedef struct {
    int num_snapshots;
    int capacity;
    CycleSnapshot* snapshots;
    
    // Global statistics
    float energy_min;
    float energy_max;
    float energy_mean;
    float energy_std;
    
    float temporal_dim_min;
    float temporal_dim_max;
    
    bool converged;
    int convergence_cycle;
    
} MonitoringData;

// ============================================================================
// Monitoring Functions
// ============================================================================

MonitoringData* CreateMonitoringData(int capacity) {
    MonitoringData* data = (MonitoringData*)malloc(sizeof(MonitoringData));
    data->num_snapshots = 0;
    data->capacity = capacity;
    data->snapshots = (CycleSnapshot*)calloc(capacity, sizeof(CycleSnapshot));
    
    data->energy_min = INFINITY;
    data->energy_max = -INFINITY;
    data->energy_mean = 0.0f;
    data->energy_std = 0.0f;
    
    data->temporal_dim_min = INFINITY;
    data->temporal_dim_max = -INFINITY;
    
    data->converged = false;
    data->convergence_cycle = -1;
    
    return data;
}

void FreeMonitoringData(MonitoringData* data) {
    if (data) {
        free(data->snapshots);
        free(data);
    }
}

float ComputeFieldStatistics(const TCDE_Field* field, float* mean, float* std, 
                             float* max_val, float* min_val) {
    if (!field || field->manifold_6d.num_centers == 0) {
        *mean = *std = *max_val = *min_val = 0.0f;
        return 0.0f;
    }
    
    float sum = 0.0f;
    float sum_sq = 0.0f;
    *max_val = -INFINITY;
    *min_val = INFINITY;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float mag = cabsf(field->manifold_6d.centers[i].coeff);
        sum += mag;
        sum_sq += mag * mag;
        
        if (mag > *max_val) *max_val = mag;
        if (mag < *min_val) *min_val = mag;
    }
    
    int n = field->manifold_6d.num_centers;
    *mean = sum / n;
    *std = sqrtf(sum_sq / n - (*mean) * (*mean));
    
    return *mean;
}

float ComputeTorsionMagnitude(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    float total_torsion = 0.0f;
    int num_samples = fminf(10, field->manifold_6d.num_centers);
    
    for (int i = 0; i < num_samples; i++) {
        TCDE_Point* point = &field->manifold_6d.centers[i].point;
        TCDE_Complex torsion = TCDE_TopologicalTorsion(field, point);
        total_torsion += cabsf(torsion);
    }
    
    return total_torsion / num_samples;
}

float ComputeCouplingStrength(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers == 0) return 0.0f;
    
    float total_coupling = 0.0f;
    int num_samples = fminf(10, field->manifold_6d.num_centers);
    
    for (int i = 0; i < num_samples; i++) {
        TCDE_Point* point = &field->manifold_6d.centers[i].point;
        TCDE_Complex coupling = TCDE_ContextualCoupling(field, point, 0.15f, 20);
        total_coupling += cabsf(coupling);
    }
    
    return total_coupling / num_samples;
}

float ComputeCoherence(const TCDE_Field* field) {
    if (!field || field->manifold_6d.num_centers < 2) return 0.0f;
    
    // Measure phase coherence between centers
    float phase_sum_cos = 0.0f;
    float phase_sum_sin = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float phase = cargf(field->manifold_6d.centers[i].coeff);
        phase_sum_cos += cosf(phase);
        phase_sum_sin += sinf(phase);
    }
    
    int n = field->manifold_6d.num_centers;
    float coherence = sqrtf(phase_sum_cos * phase_sum_cos + phase_sum_sin * phase_sum_sin) / n;
    
    return coherence;
}

void ComputePhaseStatistics(const TCDE_Field* field, float* mean, float* std) {
    if (!field || field->manifold_6d.num_centers == 0) {
        *mean = *std = 0.0f;
        return;
    }
    
    float sum = 0.0f;
    float sum_sq = 0.0f;
    
    for (int i = 0; i < field->manifold_6d.num_centers; i++) {
        float phase = cargf(field->manifold_6d.centers[i].coeff);
        sum += phase;
        sum_sq += phase * phase;
    }
    
    int n = field->manifold_6d.num_centers;
    *mean = sum / n;
    *std = sqrtf(sum_sq / n - (*mean) * (*mean));
}

void ComputeGradientStatistics(const TCDE_Field* field, float* mean_norm, float* max_norm) {
    if (!field || field->manifold_6d.num_centers == 0) {
        *mean_norm = *max_norm = 0.0f;
        return;
    }
    
    float sum_norm = 0.0f;
    *max_norm = 0.0f;
    
    int num_samples = fminf(20, field->manifold_6d.num_centers);
    
    for (int i = 0; i < num_samples; i++) {
        TCDE_Complex grad[6];
        TCDE_Gradient6D(field, &field->manifold_6d.centers[i].point, grad);
        
        float norm = 0.0f;
        for (int d = 0; d < 6; d++) {
            norm += cabsf(grad[d]) * cabsf(grad[d]);
        }
        norm = sqrtf(norm);
        
        sum_norm += norm;
        if (norm > *max_norm) *max_norm = norm;
    }
    
    *mean_norm = sum_norm / num_samples;
}

void RecordSnapshot(MonitoringData* data, const TCDE_Field* field, int cycle) {
    if (!data || !field || data->num_snapshots >= data->capacity) return;
    
    CycleSnapshot* snap = &data->snapshots[data->num_snapshots];
    
    // Basic metrics
    snap->time = field->time;
    snap->energy = TCDE_ComputeEnergy((TCDE_Field*)field);
    snap->temporal_dim = field->temporal_dimension;
    snap->fractal_dim_measured = TCDE_VerifyFractalDimension(field);
    snap->num_centers_6d = field->manifold_6d.num_centers;
    snap->num_centers_2d = field->slice_2d.num_centers;
    
    // Field statistics
    ComputeFieldStatistics(field, &snap->mean_amplitude, &snap->std_amplitude,
                          &snap->max_amplitude, &snap->min_amplitude);
    
    // Topological metrics
    snap->torsion_magnitude = ComputeTorsionMagnitude(field);
    snap->coupling_strength = ComputeCouplingStrength(field);
    snap->coherence = ComputeCoherence(field);
    
    // Phase space
    ComputePhaseStatistics(field, &snap->phase_mean, &snap->phase_std);
    
    // Gradient metrics
    ComputeGradientStatistics(field, &snap->gradient_norm_mean, &snap->gradient_norm_max);
    
    // Update global statistics
    if (snap->energy < data->energy_min) data->energy_min = snap->energy;
    if (snap->energy > data->energy_max) data->energy_max = snap->energy;
    
    if (snap->temporal_dim < data->temporal_dim_min) data->temporal_dim_min = snap->temporal_dim;
    if (snap->temporal_dim > data->temporal_dim_max) data->temporal_dim_max = snap->temporal_dim;
    
    data->num_snapshots++;
    
    // Check convergence (energy change < 0.1% for 50 cycles)
    if (cycle > 50 && !data->converged) {
        int lookback = 50;
        float energy_old = data->snapshots[data->num_snapshots - lookback].energy;
        float energy_new = snap->energy;
        float change = fabsf(energy_new - energy_old) / (fabsf(energy_old) + 1e-10f);
        
        if (change < 0.001f) {
            data->converged = true;
            data->convergence_cycle = cycle;
        }
    }
}

void ComputeGlobalStatistics(MonitoringData* data) {
    if (!data || data->num_snapshots == 0) return;
    
    float sum_energy = 0.0f;
    float sum_energy_sq = 0.0f;
    
    for (int i = 0; i < data->num_snapshots; i++) {
        float e = data->snapshots[i].energy;
        sum_energy += e;
        sum_energy_sq += e * e;
    }
    
    int n = data->num_snapshots;
    data->energy_mean = sum_energy / n;
    data->energy_std = sqrtf(sum_energy_sq / n - data->energy_mean * data->energy_mean);
}

// ============================================================================
// Reporting Functions
// ============================================================================

void PrintHeader() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                          ║\n");
    printf("║          TCDE NATIVE IMPLEMENTATION - 1000 CYCLE ANALYSIS                ║\n");
    printf("║                                                                          ║\n");
    printf("║     Complete Academic Demonstration of Topological Cognitive            ║\n");
    printf("║          Diffusive Emergence Dynamics on 6D Manifold                    ║\n");
    printf("║                                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void PrintConfiguration(const TCDE_Field* field, const TCDE_Parameters* params) {
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║ CONFIGURATION                                                            ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Topology:              Unified 6D Manifold with 2D Projection            ║\n");
    printf("║ Manifold Dimension:    6D (x, y, z, τ₁, τ₂, m)                          ║\n");
    printf("║ Projection:            2D (axes %d, %d)                                  ║\n",
           field->projection.axis1, field->projection.axis2);
    printf("║ Initial Centers:       %d                                                ║\n",
           field->manifold_6d.num_centers);
    printf("║ Capacity:              %d                                                ║\n",
           field->manifold_6d.capacity);
    printf("║ Fractal Dimension:     %.3f                                              ║\n",
           field->fractal_dimension);
    printf("║ RBF Kernel:            Gaussian                                          ║\n");
    printf("║                                                                          ║\n");
    printf("║ Evolution Parameters:                                                    ║\n");
    printf("║   dt (time step):      %.4f                                              ║\n", params->dt);
    printf("║   D (diffusion):       %.4f                                              ║\n", params->D);
    printf("║   α (nonlinearity):    %.4f                                              ║\n", params->alpha);
    printf("║   β (torsion):         %.4f                                              ║\n", params->beta);
    printf("║   γ (coupling):        %.4f                                              ║\n", params->gamma);
    printf("║   σ (coupling scale):  %.4f                                              ║\n", params->sigma);
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void PrintProgressBar(int cycle, int total, float energy, float temporal_dim) {
    int bar_width = 50;
    float progress = (float)cycle / total;
    int filled = (int)(progress * bar_width);
    
    printf("\r[");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %3d%% | Cycle: %4d/%d | E: %.4f | τ: %.3f", 
           (int)(progress * 100), cycle, total, energy, temporal_dim);
    fflush(stdout);
}

void PrintSnapshot(const CycleSnapshot* snap, int cycle) {
    printf("\n");
    printf("┌──────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ CYCLE %4d SNAPSHOT                                                       │\n", cycle);
    printf("├──────────────────────────────────────────────────────────────────────────┤\n");
    printf("│ Time:              %.4f                                                  │\n", snap->time);
    printf("│ Energy:            %.6f                                                  │\n", snap->energy);
    printf("│ Temporal Dim:      %.4f                                                  │\n", snap->temporal_dim);
    printf("│ Fractal Dim:       %.4f                                                  │\n", snap->fractal_dim_measured);
    printf("│ Centers (6D/2D):   %d / %d                                               │\n", 
           snap->num_centers_6d, snap->num_centers_2d);
    printf("│                                                                          │\n");
    printf("│ Field Amplitude:                                                         │\n");
    printf("│   Mean:            %.4f                                                  │\n", snap->mean_amplitude);
    printf("│   Std Dev:         %.4f                                                  │\n", snap->std_amplitude);
    printf("│   Range:           [%.4f, %.4f]                                          │\n", 
           snap->min_amplitude, snap->max_amplitude);
    printf("│                                                                          │\n");
    printf("│ Topological Metrics:                                                     │\n");
    printf("│   Torsion:         %.4f                                                  │\n", snap->torsion_magnitude);
    printf("│   Coupling:        %.4f                                                  │\n", snap->coupling_strength);
    printf("│   Coherence:       %.4f                                                  │\n", snap->coherence);
    printf("│                                                                          │\n");
    printf("│ Phase Space:                                                             │\n");
    printf("│   Mean Phase:      %.4f rad                                              │\n", snap->phase_mean);
    printf("│   Phase Std:       %.4f rad                                              │\n", snap->phase_std);
    printf("│                                                                          │\n");
    printf("│ Gradient Norms:                                                          │\n");
    printf("│   Mean:            %.4f                                                  │\n", snap->gradient_norm_mean);
    printf("│   Maximum:         %.4f                                                  │\n", snap->gradient_norm_max);
    printf("└──────────────────────────────────────────────────────────────────────────┘\n");
}

void PrintFinalReport(const MonitoringData* data) {
    printf("\n\n");
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                          ║\n");
    printf("║                         FINAL ANALYSIS REPORT                            ║\n");
    printf("║                                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║ GLOBAL STATISTICS                                                        ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Total Cycles:          %d                                                ║\n", data->num_snapshots);
    printf("║                                                                          ║\n");
    printf("║ Energy Statistics:                                                       ║\n");
    printf("║   Mean:                %.6f                                              ║\n", data->energy_mean);
    printf("║   Std Dev:             %.6f                                              ║\n", data->energy_std);
    printf("║   Range:               [%.6f, %.6f]                                      ║\n", 
           data->energy_min, data->energy_max);
    printf("║   Variation:           %.2f%%                                            ║\n",
           100.0f * data->energy_std / (data->energy_mean + 1e-10f));
    printf("║                                                                          ║\n");
    printf("║ Temporal Dimension:                                                      ║\n");
    printf("║   Range:               [%.4f, %.4f]                                      ║\n",
           data->temporal_dim_min, data->temporal_dim_max);
    printf("║   Final:               %.4f                                              ║\n",
           data->snapshots[data->num_snapshots-1].temporal_dim);
    printf("║                                                                          ║\n");
    printf("║ Convergence:                                                             ║\n");
    if (data->converged) {
        printf("║   Status:              CONVERGED                                         ║\n");
        printf("║   Cycle:               %d                                                ║\n", 
               data->convergence_cycle);
    } else {
        printf("║   Status:              NOT CONVERGED (still evolving)                    ║\n");
    }
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Analyze trends
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║ TREND ANALYSIS                                                           ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
    
    // Energy trend
    float energy_start = data->snapshots[0].energy;
    float energy_end = data->snapshots[data->num_snapshots-1].energy;
    float energy_change = ((energy_end - energy_start) / energy_start) * 100.0f;
    
    printf("║ Energy Evolution:                                                        ║\n");
    printf("║   Initial:             %.6f                                              ║\n", energy_start);
    printf("║   Final:               %.6f                                              ║\n", energy_end);
    printf("║   Change:              %+.2f%%                                           ║\n", energy_change);
    printf("║                                                                          ║\n");
    
    // Coherence trend
    float coherence_start = data->snapshots[0].coherence;
    float coherence_end = data->snapshots[data->num_snapshots-1].coherence;
    
    printf("║ Coherence Evolution:                                                     ║\n");
    printf("║   Initial:             %.4f                                              ║\n", coherence_start);
    printf("║   Final:               %.4f                                              ║\n", coherence_end);
    printf("║   Change:              %+.4f                                             ║\n", 
           coherence_end - coherence_start);
    printf("║                                                                          ║\n");
    
    // Torsion trend
    float torsion_mean = 0.0f;
    for (int i = 0; i < data->num_snapshots; i++) {
        torsion_mean += data->snapshots[i].torsion_magnitude;
    }
    torsion_mean /= data->num_snapshots;
    
    printf("║ Topological Activity:                                                    ║\n");
    printf("║   Mean Torsion:        %.4f                                              ║\n", torsion_mean);
    printf("║   Final Torsion:       %.4f                                              ║\n",
           data->snapshots[data->num_snapshots-1].torsion_magnitude);
    printf("║   Mean Coupling:       %.4f                                              ║\n",
           data->snapshots[data->num_snapshots-1].coupling_strength);
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void SaveDataToCSV(const MonitoringData* data, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not open %s for writing\n", filename);
        return;
    }
    
    // Header
    fprintf(fp, "cycle,time,energy,temporal_dim,fractal_dim,num_centers_6d,num_centers_2d,");
    fprintf(fp, "mean_amplitude,std_amplitude,max_amplitude,min_amplitude,");
    fprintf(fp, "torsion,coupling,coherence,phase_mean,phase_std,");
    fprintf(fp, "gradient_norm_mean,gradient_norm_max\n");
    
    // Data
    for (int i = 0; i < data->num_snapshots; i++) {
        CycleSnapshot* s = &data->snapshots[i];
        fprintf(fp, "%d,%.6f,%.6f,%.6f,%.6f,%d,%d,",
                i, s->time, s->energy, s->temporal_dim, s->fractal_dim_measured,
                s->num_centers_6d, s->num_centers_2d);
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,",
                s->mean_amplitude, s->std_amplitude, s->max_amplitude, s->min_amplitude);
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f,",
                s->torsion_magnitude, s->coupling_strength, s->coherence,
                s->phase_mean, s->phase_std);
        fprintf(fp, "%.6f,%.6f\n",
                s->gradient_norm_mean, s->gradient_norm_max);
    }
    
    fclose(fp);
    printf("✓ Data saved to: %s\n", filename);
}

// ============================================================================
// Main Test
// ============================================================================

int main(void) {
    // Seed random number generator
    srand(time(NULL));
    
    PrintHeader();
    
    // Configuration
    const int NUM_CYCLES = 1000;
    const int INITIAL_CENTERS = 50;
    const int SNAPSHOT_INTERVAL = 10;  // Record every 10 cycles
    const int PRINT_INTERVAL = 100;    // Print detailed snapshot every 100 cycles
    
    printf("Initializing TCDE system...\n");
    
    // Create field with unified topology
    TCDE_Field* field = TCDE_CreateField(INITIAL_CENTERS * 2, 2.5f);
    if (!field) {
        printf("Error: Failed to create TCDE field\n");
        return 1;
    }
    
    // Initialize with fractal structure centered in 6D space
    float center_coords[6] = {0.5f, 0.5f, 0.5f, 1.5f, 0.0f, 0.5f};
    TCDE_Point center = TCDE_CreatePoint(6, center_coords);
    
    printf("Initializing fractal structure...\n");
    TCDE_InitializeFractal(field, &center, 0.3f, 1.0f, INITIAL_CENTERS);
    TCDE_FreePoint(&center);
    
    // Configure evolution parameters
    TCDE_Parameters params;
    TCDE_ConfigureParameters(&params, field);
    
    PrintConfiguration(field, &params);
    
    // Create monitoring data
    MonitoringData* monitoring = CreateMonitoringData(NUM_CYCLES / SNAPSHOT_INTERVAL + 1);
    
    printf("Starting evolution for %d cycles...\n\n", NUM_CYCLES);
    
    clock_t start_time = clock();
    
    // Evolution loop
    for (int cycle = 0; cycle <= NUM_CYCLES; cycle++) {
        // Record snapshot
        if (cycle % SNAPSHOT_INTERVAL == 0) {
            RecordSnapshot(monitoring, field, cycle);
        }
        
        // Print detailed snapshot
        if (cycle % PRINT_INTERVAL == 0 && cycle > 0) {
            CycleSnapshot* snap = &monitoring->snapshots[monitoring->num_snapshots - 1];
            PrintSnapshot(snap, cycle);
        }
        
        // Update progress bar
        if (cycle % 10 == 0) {
            float energy = TCDE_ComputeEnergy(field);
            PrintProgressBar(cycle, NUM_CYCLES, energy, field->temporal_dimension);
        }
        
        // Evolve field
        if (cycle < NUM_CYCLES) {
            TCDE_EvolveStep(field, &params);
            
            // Periodic adaptive refinement
            if (cycle % 200 == 0 && cycle > 0) {
                TCDE_AdaptiveMeshRefinement(field);
            }
        }
    }
    
    printf("\n\n");
    
    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Evolution completed in %.2f seconds\n", elapsed);
    printf("Average time per cycle: %.4f ms\n", (elapsed * 1000.0) / NUM_CYCLES);
    printf("\n");
    
    // Compute global statistics
    ComputeGlobalStatistics(monitoring);
    
    // Print final report
    PrintFinalReport(monitoring);
    
    // Save data
    printf("Saving monitoring data...\n");
    SaveDataToCSV(monitoring, "tcde_1000_cycles_data.csv");
    
    // Cleanup
    printf("\nCleaning up...\n");
    TCDE_DestroyField(field);
    FreeMonitoringData(monitoring);
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                          ║\n");
    printf("║                    TEST COMPLETED SUCCESSFULLY                           ║\n");
    printf("║                                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    return 0;
}
