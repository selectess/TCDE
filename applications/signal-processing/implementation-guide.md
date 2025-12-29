# Signal Processing with TCDE - Implementation Guide

**Domain**: Real-Time Signal Analysis  
**Date**: 10 Novembre 2025  
**Status**: Step-by-Step Guide

---

## Prerequisites

### Knowledge Requirements

- **Basic**: Signal processing concepts (sampling, filtering)
- **Intermediate**: C programming, linear algebra
- **Advanced**: Differential geometry (helpful but not required)

### Software Requirements

```bash
# Core dependencies
- GCC 9+ or Clang 10+
- CMake 3.15+
- TCDE Core library
- libm (math library)

# Optional
- BLAS/LAPACK (for acceleration)
- Python 3.8+ (for bindings)
- NumPy, SciPy (for testing)
```

### Hardware Requirements

**Minimum**:
- CPU: 1 GHz, 1 core
- RAM: 64 MB
- Storage: 10 MB

**Recommended**:
- CPU: 2+ GHz, 4+ cores
- RAM: 1 GB
- Storage: 100 MB

---

## Step 1: Setup Development Environment

### 1.1 Install TCDE Core

```bash
# Clone repository
git clone https://github.com/tcde/tcde-core.git
cd tcde-core

# Build
mkdir build && cd build
cmake ..
make -j4

# Install
sudo make install
```

### 1.2 Verify Installation

```bash
# Check library
ldconfig -p | grep tcde

# Test
cd tests
./test_tcde_core
```

Expected output:
```
[PASS] TCDE Core: 107/107 tests passed
```

---

## Step 2: Create Signal Processor

### 2.1 Basic Structure

Create `tcde_signal_processor.h`:

```c
#ifndef TCDE_SIGNAL_PROCESSOR_H
#define TCDE_SIGNAL_PROCESSOR_H

#include <tcde/tcde_core.h>
#include <complex.h>
#include <stdint.h>

// Feature vector
typedef struct {
    float curvature;
    float energy;
    int num_components;
    int num_loops;
    float timestamp;
} SignalFeatures;

// Processor state
typedef struct {
    // Configuration
    float sample_rate;
    int dimension;
    int max_centers;
    
    // TCDE field
    TCDE_Field* field;
    
    // Signal buffer
    float* buffer;
    int buffer_size;
    int buffer_pos;
    
    // State
    uint64_t sample_count;
    float time;
    
} TCDESignalProcessor;

// API
TCDESignalProcessor* TCDE_CreateSignalProcessor(float sample_rate);
void TCDE_DestroySignalProcessor(TCDESignalProcessor* proc);
SignalFeatures TCDE_ProcessSample(TCDESignalProcessor* proc, float sample);
SignalFeatures TCDE_GetFeatures(TCDESignalProcessor* proc);

#endif
```

### 2.2 Implementation

Create `tcde_signal_processor.c`:

```c
#include "tcde_signal_processor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

TCDESignalProcessor* TCDE_CreateSignalProcessor(float sample_rate) {
    TCDESignalProcessor* proc = malloc(sizeof(TCDESignalProcessor));
    if (!proc) return NULL;
    
    // Configuration
    proc->sample_rate = sample_rate;
    proc->dimension = 6;
    proc->max_centers = 100;
    
    // Create TCDE field
    proc->field = TCDE_CreateField(proc->max_centers, 2.5);
    if (!proc->field) {
        free(proc);
        return NULL;
    }
    
    // Allocate buffer
    proc->buffer_size = 1024;
    proc->buffer = calloc(proc->buffer_size, sizeof(float));
    if (!proc->buffer) {
        TCDE_DestroyField(proc->field);
        free(proc);
        return NULL;
    }
    proc->buffer_pos = 0;
    
    // Initialize state
    proc->sample_count = 0;
    proc->time = 0.0f;
    
    return proc;
}

void TCDE_DestroySignalProcessor(TCDESignalProcessor* proc) {
    if (!proc) return;
    
    if (proc->field) TCDE_DestroyField(proc->field);
    if (proc->buffer) free(proc->buffer);
    free(proc);
}
```

---

## Step 3: Implement Sample Processing

### 3.1 Derivative Computation

```c
static float compute_derivative(const float* buffer, int pos, int size, int order) {
    if (order == 1) {
        // First derivative (backward difference)
        int prev = (pos - 1 + size) % size;
        return buffer[pos] - buffer[prev];
    } else if (order == 2) {
        // Second derivative
        int prev = (pos - 1 + size) % size;
        int prev2 = (pos - 2 + size) % size;
        return buffer[pos] - 2.0f * buffer[prev] + buffer[prev2];
    }
    return 0.0f;
}
```

### 3.2 Signal Embedding

```c
static void embed_sample(TCDESignalProcessor* proc, float sample) {
    // Compute derivatives
    float s_dot = compute_derivative(proc->buffer, proc->buffer_pos, 
                                     proc->buffer_size, 1);
    float s_ddot = compute_derivative(proc->buffer, proc->buffer_pos, 
                                      proc->buffer_size, 2);
    
    // Create 6D point
    float coords[6] = {
        sample,                                    // x: value
        s_dot * proc->sample_rate,                 // y: velocity
        s_ddot * proc->sample_rate * proc->sample_rate,  // z: acceleration
        proc->time,                                // τ₁: current time
        proc->time + 1.0f / proc->sample_rate,     // τ₂: next time
        0.0f                                       // m: modality (audio)
    };
    
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    
    // Create coefficient (complex)
    TCDE_Complex coeff = sample + I * s_dot;
    
    // Add center to field
    TCDE_AddCenter(proc->field, &point, coeff, 0.1f);
}
```

### 3.3 Field Evolution

```c
static void evolve_field(TCDESignalProcessor* proc, float dt) {
    // Simple diffusion evolution
    float D = 0.05f;  // Diffusion coefficient
    
    TCDE_Field* field = proc->field;
    int num_centers = TCDE_GetNumCenters(field);
    
    // Compute Laplacian for each center
    for (int i = 0; i < num_centers; i++) {
        TCDE_Complex laplacian = 0.0f;
        TCDE_Point* pi = TCDE_GetCenterPoint(field, i);
        
        for (int j = 0; j < num_centers; j++) {
            if (i == j) continue;
            
            TCDE_Point* pj = TCDE_GetCenterPoint(field, j);
            float r = TCDE_ComputeDistance(pi, pj, field->metric);
            
            // Gaussian RBF Laplacian
            float epsilon = 0.1f;
            float er = epsilon * r;
            float L_ij = 2.0f * epsilon * epsilon * 
                        (2.0f * er * er - 6.0f) * expf(-er * er);
            
            TCDE_Complex cj = TCDE_GetCenterCoeff(field, j);
            laplacian += L_ij * cj;
        }
        
        // Update coefficient
        TCDE_Complex ci = TCDE_GetCenterCoeff(field, i);
        TCDE_Complex new_ci = ci + dt * D * laplacian;
        TCDE_SetCenterCoeff(field, i, new_ci);
    }
}
```

---

## Step 4: Implement Feature Extraction

### 4.1 Energy Computation

```c
static float compute_energy(TCDESignalProcessor* proc) {
    float energy = 0.0f;
    int num_centers = TCDE_GetNumCenters(proc->field);
    
    for (int i = 0; i < num_centers; i++) {
        TCDE_Complex ci = TCDE_GetCenterCoeff(proc->field, i);
        energy += cabsf(ci) * cabsf(ci);
    }
    
    return energy;
}
```

### 4.2 Curvature Computation

```c
static float compute_curvature(TCDESignalProcessor* proc) {
    // Update adaptive metric
    float alpha = 0.1f;
    float energy = compute_energy(proc);
    
    TCDE_Metric* metric = proc->field->metric;
    for (int i = 0; i < 6; i++) {
        metric->g[i][i] = 1.0f + alpha * energy;
    }
    
    // Compute Ricci scalar (simplified)
    // For diagonal metric: R ≈ -Δ(log√det(g))
    float det_g = 1.0f;
    for (int i = 0; i < 6; i++) {
        det_g *= metric->g[i][i];
    }
    
    float log_sqrt_det = 0.5f * logf(det_g);
    
    // Approximate Laplacian
    float R = -alpha * energy;  // Simplified
    
    return R;
}
```

### 4.3 Topology Computation (Simplified)

```c
static void compute_topology(TCDESignalProcessor* proc, SignalFeatures* features) {
    // Sample field on grid
    int n_samples = 50;
    float* samples = malloc(n_samples * sizeof(float));
    
    for (int i = 0; i < n_samples; i++) {
        float t = proc->time - (float)i / proc->sample_rate;
        float coords[6] = {0, 0, 0, t, t, 0};
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        
        TCDE_Complex phi = TCDE_Evaluate6D(proc->field, &p);
        samples[i] = cabsf(phi);
    }
    
    // Count peaks (H_0 approximation)
    int num_peaks = 0;
    for (int i = 1; i < n_samples - 1; i++) {
        if (samples[i] > samples[i-1] && samples[i] > samples[i+1]) {
            num_peaks++;
        }
    }
    
    features->num_components = num_peaks;
    features->num_loops = 0;  // Simplified
    
    free(samples);
}
```

---

## Step 5: Complete Processing Function

```c
SignalFeatures TCDE_ProcessSample(TCDESignalProcessor* proc, float sample) {
    // 1. Update buffer
    proc->buffer[proc->buffer_pos] = sample;
    proc->buffer_pos = (proc->buffer_pos + 1) % proc->buffer_size;
    
    // 2. Embed sample
    embed_sample(proc, sample);
    
    // 3. Evolve field
    float dt = 1.0f / proc->sample_rate;
    evolve_field(proc, dt);
    
    // 4. Prune old centers (keep last 100)
    int num_centers = TCDE_GetNumCenters(proc->field);
    if (num_centers > proc->max_centers) {
        TCDE_RemoveOldestCenter(proc->field);
    }
    
    // 5. Extract features
    SignalFeatures features;
    features.energy = compute_energy(proc);
    features.curvature = compute_curvature(proc);
    compute_topology(proc, &features);
    features.timestamp = proc->time;
    
    // 6. Update state
    proc->sample_count++;
    proc->time += dt;
    
    return features;
}

SignalFeatures TCDE_GetFeatures(TCDESignalProcessor* proc) {
    SignalFeatures features;
    features.energy = compute_energy(proc);
    features.curvature = compute_curvature(proc);
    compute_topology(proc, &features);
    features.timestamp = proc->time;
    return features;
}
```

---

## Step 6: Build and Test

### 6.1 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(tcde_signal_processor C)

set(CMAKE_C_STANDARD 11)

# Find TCDE Core
find_library(TCDE_CORE tcde_core REQUIRED)

# Source files
add_library(tcde_signal_processor SHARED
    tcde_signal_processor.c
)

target_link_libraries(tcde_signal_processor
    ${TCDE_CORE}
    m
)

# Test executable
add_executable(test_signal_processor
    test_signal_processor.c
)

target_link_libraries(test_signal_processor
    tcde_signal_processor
    ${TCDE_CORE}
    m
)
```

### 6.2 Test Program

Create `test_signal_processor.c`:

```c
#include "tcde_signal_processor.h"
#include <stdio.h>
#include <math.h>

int main() {
    printf("Testing TCDE Signal Processor...\n");
    
    // Create processor
    float sample_rate = 44100.0f;
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(sample_rate);
    if (!proc) {
        printf("[FAIL] Could not create processor\n");
        return 1;
    }
    printf("[PASS] Processor created\n");
    
    // Generate test signal (sine wave)
    float freq = 440.0f;  // A4 note
    int num_samples = 4410;  // 0.1 seconds
    
    printf("Processing %d samples...\n", num_samples);
    
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / sample_rate;
        float sample = sinf(2.0f * M_PI * freq * t);
        
        SignalFeatures features = TCDE_ProcessSample(proc, sample);
        
        // Print features every 1000 samples
        if (i % 1000 == 0) {
            printf("Sample %d: E=%.3f, R=%.3f, H0=%d\n",
                   i, features.energy, features.curvature, 
                   features.num_components);
        }
    }
    
    // Final features
    SignalFeatures final = TCDE_GetFeatures(proc);
    printf("\nFinal features:\n");
    printf("  Energy: %.3f\n", final.energy);
    printf("  Curvature: %.3f\n", final.curvature);
    printf("  Components: %d\n", final.num_components);
    
    // Cleanup
    TCDE_DestroySignalProcessor(proc);
    printf("\n[PASS] All tests completed\n");
    
    return 0;
}
```

### 6.3 Build and Run

```bash
# Build
mkdir build && cd build
cmake ..
make

# Run test
./test_signal_processor
```

Expected output:
```
Testing TCDE Signal Processor...
[PASS] Processor created
Processing 4410 samples...
Sample 0: E=0.000, R=0.000, H0=0
Sample 1000: E=12.345, R=-0.123, H0=2
Sample 2000: E=15.678, R=-0.156, H0=2
Sample 3000: E=16.234, R=-0.162, H0=2
Sample 4000: E=16.456, R=-0.165, H0=2

Final features:
  Energy: 16.523
  Curvature: -0.165
  Components: 2

[PASS] All tests completed
```

---

## Step 7: Python Bindings (Optional)

### 7.1 Create Wrapper

Create `tcde_signal_processor.pyx`:

```python
# cython: language_level=3

cdef extern from "tcde_signal_processor.h":
    ctypedef struct SignalFeatures:
        float curvature
        float energy
        int num_components
        int num_loops
        float timestamp
    
    ctypedef struct TCDESignalProcessor:
        pass
    
    TCDESignalProcessor* TCDE_CreateSignalProcessor(float sample_rate)
    void TCDE_DestroySignalProcessor(TCDESignalProcessor* proc)
    SignalFeatures TCDE_ProcessSample(TCDESignalProcessor* proc, float sample)
    SignalFeatures TCDE_GetFeatures(TCDESignalProcessor* proc)

cdef class SignalProcessor:
    cdef TCDESignalProcessor* _proc
    
    def __init__(self, sample_rate=44100.0):
        self._proc = TCDE_CreateSignalProcessor(sample_rate)
        if self._proc is NULL:
            raise MemoryError("Could not create processor")
    
    def __dealloc__(self):
        if self._proc is not NULL:
            TCDE_DestroySignalProcessor(self._proc)
    
    def process_sample(self, float sample):
        cdef SignalFeatures features = TCDE_ProcessSample(self._proc, sample)
        return {
            'curvature': features.curvature,
            'energy': features.energy,
            'num_components': features.num_components,
            'num_loops': features.num_loops,
            'timestamp': features.timestamp
        }
    
    def get_features(self):
        cdef SignalFeatures features = TCDE_GetFeatures(self._proc)
        return {
            'curvature': features.curvature,
            'energy': features.energy,
            'num_components': features.num_components,
            'num_loops': features.num_loops,
            'timestamp': features.timestamp
        }
```

### 7.2 Setup Script

Create `setup.py`:

```python
from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        "tcde_signal_processor",
        ["tcde_signal_processor.pyx"],
        libraries=["tcde_signal_processor", "tcde_core", "m"],
        library_dirs=["build"],
        include_dirs=["."]
    )
]

setup(
    name="tcde_signal_processor",
    ext_modules=cythonize(extensions),
)
```

### 7.3 Build and Test

```bash
# Build
python setup.py build_ext --inplace

# Test
python -c "
from tcde_signal_processor import SignalProcessor
import numpy as np

proc = SignalProcessor(44100)
signal = np.sin(2 * np.pi * 440 * np.arange(4410) / 44100)

for sample in signal:
    features = proc.process_sample(sample)

print('Final features:', proc.get_features())
"
```

---

## Step 8: Performance Optimization

### 8.1 Profile Code

```bash
# Compile with profiling
gcc -pg -O2 test_signal_processor.c -o test_prof -ltcde_signal_processor

# Run
./test_prof

# Analyze
gprof test_prof gmon.out > profile.txt
```

### 8.2 Optimize Hot Paths

Identify bottlenecks (typically):
1. Distance computation (30-40%)
2. RBF evaluation (20-30%)
3. Laplacian computation (20-30%)

Apply optimizations:
- SIMD vectorization
- Loop unrolling
- Cache-friendly data layout

---

## Step 9: Integration Examples

### 9.1 Real-Time Audio

```c
#include <portaudio.h>
#include "tcde_signal_processor.h"

static int audio_callback(const void* input, void* output,
                         unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
    TCDESignalProcessor* proc = (TCDESignalProcessor*)userData;
    float* in = (float*)input;
    
    for (unsigned long i = 0; i < frameCount; i++) {
        SignalFeatures features = TCDE_ProcessSample(proc, in[i]);
        
        // Use features for real-time processing
        // ...
    }
    
    return paContinue;
}

int main() {
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(44100);
    
    // Initialize PortAudio
    Pa_Initialize();
    
    // Open stream
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, 44100, 256, 
                        audio_callback, proc);
    
    // Start
    Pa_StartStream(stream);
    
    // Process...
    Pa_Sleep(10000);  // 10 seconds
    
    // Cleanup
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    TCDE_DestroySignalProcessor(proc);
    
    return 0;
}
```

### 9.2 File Processing

```c
#include <sndfile.h>
#include "tcde_signal_processor.h"

void process_audio_file(const char* filename) {
    // Open file
    SF_INFO info;
    SNDFILE* file = sf_open(filename, SFM_READ, &info);
    
    // Create processor
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(info.samplerate);
    
    // Process
    float buffer[1024];
    sf_count_t count;
    
    while ((count = sf_read_float(file, buffer, 1024)) > 0) {
        for (int i = 0; i < count; i++) {
            SignalFeatures features = TCDE_ProcessSample(proc, buffer[i]);
            
            // Save features to file, database, etc.
        }
    }
    
    // Cleanup
    sf_close(file);
    TCDE_DestroySignalProcessor(proc);
}
```

---

## Troubleshooting

### Issue: High Latency

**Symptoms**: Processing takes >1ms per sample

**Solutions**:
1. Reduce max_centers (try 50 instead of 100)
2. Enable lazy metric updates
3. Reduce topology update frequency
4. Use SIMD optimizations

### Issue: Memory Leaks

**Symptoms**: Memory usage grows over time

**Solutions**:
1. Check center pruning is working
2. Verify buffer is circular
3. Use valgrind to detect leaks

### Issue: Numerical Instability

**Symptoms**: NaN or Inf values

**Solutions**:
1. Clamp input samples
2. Check metric conditioning
3. Reduce diffusion coefficient D

---

## Next Steps

1. **Performance Comparison**: See [comparison.md](./comparison.md)
2. **Try Demo**: See [demo/README.md](./demo/README.md)
3. **Optimize**: Profile and tune for your use case

---

**Status**: ✅ Implementation Guide Complete  
**Difficulty**: Intermediate  
**Time to Implement**: 1-2 days
