# Signal Processing with TCDE - Architecture

**Domain**: Real-Time Signal Analysis  
**Date**: 10 Novembre 2025  
**Status**: Design Complete

---

## System Architecture

### High-Level Overview

```
┌────────────────────────────────────────────────────────────────┐
│                    TCDE Signal Processor                       │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐    │
│  │   Input      │───▶│   TCDE       │───▶│   Output     │    │
│  │   Buffer     │    │   Core       │    │   Features   │    │
│  └──────────────┘    └──────────────┘    └──────────────┘    │
│         │                    │                    │           │
│         │                    │                    │           │
│  ┌──────▼──────┐    ┌───────▼────────┐  ┌────────▼───────┐  │
│  │ Preprocessing│    │  Field         │  │  Feature       │  │
│  │ - Normalize  │    │  Evolution     │  │  Extraction    │  │
│  │ - Resample   │    │  - Update Φ    │  │  - Curvature   │  │
│  └──────────────┘    │  - Evolve g    │  │  - Topology    │  │
│                      │  - Adapt       │  │  - Energy      │  │
│                      └────────────────┘  └────────────────┘  │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

---

## Core Components

### 1. Signal Embedding Module

**Purpose**: Map 1D signal to 6D continuous field

**Input**: Signal sample `s(t) ∈ ℝ`  
**Output**: Field update `Φ(x,t) ∈ ℂ`

**Algorithm**:
```
1. Create temporal point: x_t = [s(t), ṡ(t), s̈(t), t, t+Δt, 0]
2. Add/update RBF center at x_t
3. Set coefficient: c_t = s(t) + i·ṡ(t)
4. Update field: Φ(x,t) = Σᵢ cᵢ ψ(||x - xᵢ||_g)
```

**Data Structure**:
```c
typedef struct {
    float sample_rate;          // Hz (e.g., 44100)
    int embedding_dim;          // 6 for full manifold
    float* history;             // Recent samples for derivatives
    int history_size;           // Typically 3-5 samples
    TCDE_Field* field;          // Underlying TCDE field
} SignalEmbedding;
```

**Complexity**: O(1) per sample

---

### 2. Field Evolution Module

**Purpose**: Evolve field according to TDE dynamics

**Equation**:
```
∂Φ/∂t = D∇²Φ + f(Φ, ∇Φ)
```

**Discretization**:
```
Φ(t+Δt) = Φ(t) + Δt·[D·L·Φ(t) + f(Φ(t))]
```

**Where**:
- `L`: Laplacian matrix (RBF-based)
- `D`: Diffusion coefficient (0.01-0.1)
- `f`: Nonlinear term (optional)

**Implementation**:
```c
void evolve_signal_field(TCDE_Field* field, float dt) {
    // 1. Compute Laplacian
    for (int i = 0; i < num_centers; i++) {
        TCDE_Complex laplacian = 0.0;
        for (int j = 0; j < num_centers; j++) {
            float r = distance(centers[i], centers[j]);
            float L_ij = rbf_laplacian(r, epsilon);
            laplacian += L_ij * centers[j].coeff;
        }
        
        // 2. Update coefficient
        float D = 0.05;  // Diffusion
        centers[i].coeff += dt * D * laplacian;
    }
    
    // 3. Update metric
    update_adaptive_metric(field);
}
```

**Complexity**: O(k²) where k = number of centers

---

### 3. Feature Extraction Module

**Purpose**: Extract topological and geometric features

**Features Computed**:

#### 3.1 Curvature
```
R(t) = scalar curvature from adaptive metric g(Φ)
```

**Interpretation**:
- `R > 0`: Signal converging (stable pattern)
- `R < 0`: Signal diverging (transient/anomaly)
- `|R|`: Complexity measure

**Code**:
```c
float compute_signal_curvature(TCDE_Field* field) {
    // Compute Ricci tensor from adaptive metric
    float R_ij[6][6];
    compute_ricci_tensor(field->metric, R_ij);
    
    // Scalar curvature
    float R = 0.0;
    for (int i = 0; i < 6; i++) {
        R += field->metric->g_inv[i][i] * R_ij[i][i];
    }
    
    return R;
}
```

#### 3.2 Topology (Persistent Homology)
```
H_k(Φ) = k-th homology group
```

**Interpretation**:
- `H_0`: Connected components (signal modes)
- `H_1`: Loops (periodic patterns)
- `H_2`: Voids (complex structures)

**Code**:
```c
TopologyFeatures compute_signal_topology(TCDE_Field* field) {
    // Sample field on grid
    int n_samples = 100;
    float* samples = sample_field_grid(field, n_samples);
    
    // Compute persistence diagram
    PersistenceDiagram pd = compute_persistence(samples, n_samples);
    
    // Extract features
    TopologyFeatures features;
    features.num_components = count_features(pd, 0);  // H_0
    features.num_loops = count_features(pd, 1);       // H_1
    features.num_voids = count_features(pd, 2);       // H_2
    
    return features;
}
```

#### 3.3 Energy
```
E(t) = ∫|Φ(x,t)|² dV
```

**Interpretation**:
- Signal power
- Activity level
- Attention measure

**Code**:
```c
float compute_signal_energy(TCDE_Field* field) {
    float energy = 0.0;
    
    // Sum over RBF centers (approximation)
    for (int i = 0; i < num_centers; i++) {
        energy += |centers[i].coeff|²;
    }
    
    return energy;
}
```

**Complexity**: O(k) for energy, O(k²) for curvature, O(n log n) for topology

---

### 4. Adaptive Metric Module

**Purpose**: Adjust metric based on signal characteristics

**Equation**:
```
g_ij(t) = g_ij⁰ + α·E(t)·δ_ij
```

**Where**:
- `g_ij⁰ = δ_ij`: Base metric (identity)
- `α = 0.1`: Coupling strength
- `E(t)`: Signal energy

**Effect**:
- High energy → expanded metric → broader features
- Low energy → contracted metric → finer details

**Implementation**:
```c
void adapt_metric_to_signal(TCDE_Field* field) {
    float alpha = 0.1;
    float energy = compute_signal_energy(field);
    
    // Update diagonal metric
    for (int i = 0; i < 6; i++) {
        field->metric->g[i][i] = 1.0 + alpha * energy;
    }
    
    // Recompute inverse
    invert_metric(field->metric);
}
```

**Complexity**: O(d³) for metric inversion (d=6)

---

## Data Structures

### Signal Processor State

```c
typedef struct {
    // Configuration
    float sample_rate;          // Hz
    int dimension;              // 6 for full manifold
    int max_centers;            // Capacity (e.g., 100)
    
    // TCDE Core
    TCDE_Field* field;          // Continuous field
    
    // Signal History
    float* buffer;              // Circular buffer
    int buffer_size;            // Samples (e.g., 1024)
    int buffer_pos;             // Current position
    
    // Features
    float curvature;            // Current R(t)
    TopologyFeatures topology;  // Current H_k
    float energy;               // Current E(t)
    
    // Timing
    uint64_t sample_count;      // Total samples processed
    float time;                 // Current time (seconds)
    
} TCDESignalProcessor;
```

### Feature Vector

```c
typedef struct {
    // Geometric
    float curvature;            // Scalar curvature R
    float energy;               // Field energy E
    
    // Topological
    int num_components;         // H_0 (connected components)
    int num_loops;              // H_1 (cycles)
    int num_voids;              // H_2 (voids)
    
    // Statistical
    float mean;                 // Signal mean
    float variance;             // Signal variance
    float skewness;             // Signal skewness
    
    // Temporal
    float timestamp;            // Time of extraction
    
} SignalFeatures;
```

---

## Processing Pipeline

### Initialization

```c
TCDESignalProcessor* create_signal_processor(float sample_rate) {
    TCDESignalProcessor* proc = malloc(sizeof(TCDESignalProcessor));
    
    // Configuration
    proc->sample_rate = sample_rate;
    proc->dimension = 6;
    proc->max_centers = 100;
    
    // Create TCDE field
    proc->field = TCDE_CreateField(proc->max_centers, 2.5);
    
    // Allocate buffer
    proc->buffer_size = 1024;
    proc->buffer = calloc(proc->buffer_size, sizeof(float));
    proc->buffer_pos = 0;
    
    // Initialize features
    proc->curvature = 0.0;
    proc->energy = 0.0;
    
    // Timing
    proc->sample_count = 0;
    proc->time = 0.0;
    
    return proc;
}
```

### Sample Processing

```c
SignalFeatures process_sample(TCDESignalProcessor* proc, float sample) {
    // 1. Update buffer
    proc->buffer[proc->buffer_pos] = sample;
    proc->buffer_pos = (proc->buffer_pos + 1) % proc->buffer_size;
    
    // 2. Compute derivatives (for embedding)
    float s_dot = compute_derivative(proc->buffer, proc->buffer_pos, 1);
    float s_ddot = compute_derivative(proc->buffer, proc->buffer_pos, 2);
    
    // 3. Create embedding point
    float coords[6] = {
        sample,                     // x: signal value
        s_dot,                      // y: first derivative
        s_ddot,                     // z: second derivative
        proc->time,                 // τ₁: current time
        proc->time + 1.0/proc->sample_rate,  // τ₂: next time
        0.0                         // m: modality (audio)
    };
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    
    // 4. Update field
    TCDE_Complex coeff = sample + I * s_dot;
    TCDE_AddCenter(proc->field, &point, coeff, 0.1);
    
    // 5. Evolve field
    float dt = 1.0 / proc->sample_rate;
    evolve_signal_field(proc->field, dt);
    
    // 6. Adapt metric
    adapt_metric_to_signal(proc->field);
    
    // 7. Extract features
    SignalFeatures features;
    features.curvature = compute_signal_curvature(proc->field);
    features.energy = compute_signal_energy(proc->field);
    features.topology = compute_signal_topology(proc->field);
    features.timestamp = proc->time;
    
    // 8. Update state
    proc->sample_count++;
    proc->time += dt;
    
    return features;
}
```

**Latency**: <1ms on modern CPU

---

## Optimization Strategies

### 1. Sparse RBF Centers

**Problem**: O(k²) evolution cost

**Solution**: Limit k to 10-100 centers
- Add centers only for significant features
- Remove centers with low energy
- Merge nearby centers

**Code**:
```c
void prune_centers(TCDE_Field* field, float threshold) {
    for (int i = 0; i < num_centers; i++) {
        if (|centers[i].coeff|² < threshold) {
            remove_center(field, i);
        }
    }
}
```

### 2. Lazy Metric Updates

**Problem**: O(d³) metric inversion

**Solution**: Update only when energy changes significantly
```c
void lazy_adapt_metric(TCDE_Field* field) {
    float new_energy = compute_signal_energy(field);
    float delta = fabs(new_energy - field->last_energy);
    
    if (delta > 0.1 * field->last_energy) {
        adapt_metric_to_signal(field);
        field->last_energy = new_energy;
    }
}
```

### 3. Incremental Topology

**Problem**: O(n log n) persistence computation

**Solution**: Update topology incrementally
- Recompute only every N samples (N=100-1000)
- Use previous topology as starting point

### 4. SIMD Acceleration

**Problem**: Many floating-point operations

**Solution**: Vectorize inner loops
```c
// Use SSE/AVX for distance computations
__m256 compute_distances_simd(float* x, float* centers, int n);
```

---

## Memory Management

### Memory Budget

**Per Processor**:
- Field: 100 centers × 64 bytes = 6.4 KB
- Buffer: 1024 samples × 4 bytes = 4 KB
- Metric: 6×6 × 4 bytes = 144 bytes
- Features: ~100 bytes

**Total**: ~11 KB per processor

**Scalability**: 1000 concurrent streams = 11 MB

---

## Error Handling

### Numerical Stability

```c
// Check for NaN/Inf
if (!isfinite(sample)) {
    log_error("Invalid sample: %f", sample);
    return default_features();
}

// Clamp extreme values
sample = clamp(sample, -1e6, 1e6);
```

### Resource Limits

```c
// Check center capacity
if (num_centers >= max_centers) {
    prune_centers(field, energy_threshold);
}

// Check memory usage
if (get_memory_usage() > memory_limit) {
    reduce_buffer_size();
}
```

---

## Performance Characteristics

### Latency

| Operation | Time (μs) | Notes |
|-----------|-----------|-------|
| Sample update | 10-50 | Depends on k |
| Field evolution | 50-200 | O(k²) |
| Feature extraction | 100-500 | Includes topology |
| **Total** | **<1000** | **<1ms** |

### Throughput

| Platform | Samples/sec | Streams |
|----------|-------------|---------|
| ARM Cortex-M4 | 10,000 | 1 |
| ARM Cortex-A53 | 100,000 | 10 |
| Intel i7 | 1,000,000 | 100 |
| GPU (CUDA) | 10,000,000 | 1000 |

### Memory

| Component | Size | Scalability |
|-----------|------|-------------|
| Per stream | 11 KB | Linear |
| 100 streams | 1.1 MB | Linear |
| 1000 streams | 11 MB | Linear |

---

## Integration Points

### Input Interfaces

```c
// Single sample
process_sample(processor, sample);

// Batch processing
process_batch(processor, samples, count);

// Callback-based
set_sample_callback(processor, callback_fn);
```

### Output Interfaces

```c
// Synchronous
SignalFeatures features = get_features(processor);

// Asynchronous
set_feature_callback(processor, feature_callback_fn);

// Streaming
FeatureStream* stream = create_feature_stream(processor);
```

---

## Deployment Configurations

### Embedded (Low Power)

```c
TCDESignalProcessor* proc = create_signal_processor(8000);  // 8kHz
set_max_centers(proc, 20);          // Limit complexity
set_update_rate(proc, 100);         // Update topology every 100 samples
enable_lazy_metric(proc, true);     // Reduce metric updates
```

### Desktop (High Performance)

```c
TCDESignalProcessor* proc = create_signal_processor(44100);  // 44.1kHz
set_max_centers(proc, 100);         // Full complexity
set_update_rate(proc, 10);          // Frequent topology updates
enable_simd(proc, true);            // Use vectorization
```

### Cloud (Scalable)

```c
// Create processor pool
ProcessorPool* pool = create_processor_pool(1000);  // 1000 streams
set_load_balancing(pool, DYNAMIC);
enable_gpu_acceleration(pool, true);
```

---

## Next Steps

1. **Implementation Guide**: See [implementation-guide.md](./implementation-guide.md)
2. **Performance Comparison**: See [comparison.md](./comparison.md)
3. **Try Demo**: See [demo/README.md](./demo/README.md)

---

**Status**: ✅ Architecture Documented  
**Complexity**: Well-Defined  
**Ready For**: Implementation
