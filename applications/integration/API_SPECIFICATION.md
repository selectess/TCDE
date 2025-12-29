# TCDE Real-World Applications - API Specification

**Date**: 10 Novembre 2025  
**Version**: 1.0  
**Status**: Phase 4 - Integration  
**Protocol**: ZERO TOLERANCE

---

## Overview

This document specifies the APIs for integrating TCDE into real-world applications. Three API levels are provided: C (native), Python (bindings), and REST (web services).

---

## 1. C API (Native)

### 1.1 Signal Processing API

#### Core Functions

```c
/**
 * @brief Create signal processor
 * @param sample_rate Sampling rate in Hz
 * @param window_size Analysis window size
 * @return Processor instance or NULL on error
 */
TCDESignalProcessor* TCDE_CreateSignalProcessor(
    int sample_rate,
    int window_size
);

/**
 * @brief Process single sample
 * @param processor Processor instance
 * @param sample Input sample value
 * @return Processing result
 */
SignalFeatures TCDE_ProcessSample(
    TCDESignalProcessor* processor,
    float sample
);

/**
 * @brief Process batch of samples
 * @param processor Processor instance
 * @param samples Input samples array
 * @param count Number of samples
 * @param features Output features array (pre-allocated)
 * @return Number of samples processed
 */
int TCDE_ProcessBatch(
    TCDESignalProcessor* processor,
    const float* samples,
    int count,
    SignalFeatures* features
);

/**
 * @brief Destroy processor
 * @param processor Processor to destroy
 */
void TCDE_DestroySignalProcessor(
    TCDESignalProcessor* processor
);
```

#### Data Structures

```c
typedef struct {
    float energy;           // Signal energy
    float curvature;        // Geometric curvature
    int components;         // Topological components (H0)
    float mean;             // Mean value
    float variance;         // Variance
    float timestamp;        // Timestamp (seconds)
} SignalFeatures;

typedef struct {
    int sample_rate;        // Sampling rate (Hz)
    int window_size;        // Analysis window size
    TCDE_Field* field;      // Internal TCDE field
    float* buffer;          // Sample buffer
    int buffer_pos;         // Buffer position
} TCDESignalProcessor;
```

---

### 1.2 Optimization API

#### Core Functions

```c
/**
 * @brief Create optimizer
 * @param dimension Problem dimension
 * @param bounds_min Lower bounds array
 * @param bounds_max Upper bounds array
 * @param objective Objective function
 * @return Optimizer instance or NULL on error
 */
TCDEOptimizer* TCDE_CreateOptimizer(
    int dimension,
    const float* bounds_min,
    const float* bounds_max,
    float (*objective)(const float* x, int n)
);

/**
 * @brief Optimize function
 * @param optimizer Optimizer instance
 * @return Optimization result
 */
OptimizationResult TCDE_Optimize(
    TCDEOptimizer* optimizer
);

/**
 * @brief Adapt to new constraints
 * @param optimizer Optimizer instance
 * @param new_bounds_min New lower bounds
 * @param new_bounds_max New upper bounds
 * @return Updated optimization result
 */
OptimizationResult TCDE_AdaptConstraints(
    TCDEOptimizer* optimizer,
    const float* new_bounds_min,
    const float* new_bounds_max
);

/**
 * @brief Destroy optimizer
 * @param optimizer Optimizer to destroy
 */
void TCDE_DestroyOptimizer(
    TCDEOptimizer* optimizer
);
```

#### Data Structures

```c
typedef struct {
    float* solution;        // Best solution found
    float value;            // Function value at solution
    int iterations;         // Iterations to convergence
    float time_seconds;     // Time to convergence
    bool converged;         // Convergence flag
    float gradient_norm;    // Final gradient norm
} OptimizationResult;

typedef struct {
    int dimension;          // Problem dimension
    float* bounds_min;      // Lower bounds
    float* bounds_max;      // Upper bounds
    TCDE_Field* field;      // Internal TCDE field
    float* current_point;   // Current best point
    float current_value;    // Current best value
    float (*objective)(const float* x, int n);
} TCDEOptimizer;
```

---

### 1.3 Pattern Recognition API

#### Core Functions

```c
/**
 * @brief Create pattern recognizer
 * @param pattern_width Expected pattern width
 * @param pattern_height Expected pattern height
 * @param topology_dim Topology feature dimension
 * @return Recognizer instance or NULL on error
 */
TCDEPatternRecognizer* TCDE_CreateRecognizer(
    int pattern_width,
    int pattern_height,
    int topology_dim
);

/**
 * @brief Learn pattern
 * @param recognizer Recognizer instance
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data
 * @param label Pattern class label
 * @return true on success, false on error
 */
bool TCDE_LearnPattern(
    TCDEPatternRecognizer* recognizer,
    int width,
    int height,
    const float* data,
    int label
);

/**
 * @brief Recognize pattern
 * @param recognizer Recognizer instance
 * @param width Pattern width
 * @param height Pattern height
 * @param data Pattern data
 * @return Recognition result
 */
RecognitionResult TCDE_RecognizePattern(
    TCDEPatternRecognizer* recognizer,
    int width,
    int height,
    const float* data
);

/**
 * @brief Destroy recognizer
 * @param recognizer Recognizer to destroy
 */
void TCDE_DestroyRecognizer(
    TCDEPatternRecognizer* recognizer
);
```

#### Data Structures

```c
typedef struct {
    int predicted_label;    // Predicted class
    float confidence;       // Confidence score (0-1)
    float* class_scores;    // Scores for all classes
    int num_classes;        // Number of classes
    float recognition_time_ms;  // Recognition time
} RecognitionResult;

typedef struct {
    int pattern_width;      // Expected pattern width
    int pattern_height;     // Expected pattern height
    int topology_dim;       // Topology feature dimension
    PatternDatabase* database;  // Learned patterns
    int total_recognitions; // Total recognitions
    int correct_recognitions;  // Correct recognitions
} TCDEPatternRecognizer;
```

---

## 2. Python API (Bindings)

### 2.1 Installation

```python
# Install TCDE Python bindings
pip install tcde

# Or build from source
python setup.py install
```

### 2.2 Signal Processing

```python
import tcde

# Create processor
processor = tcde.SignalProcessor(
    sample_rate=44100,
    window_size=1024
)

# Process single sample
features = processor.process_sample(0.5)
print(f"Energy: {features.energy}")
print(f"Curvature: {features.curvature}")

# Process batch
import numpy as np
samples = np.sin(2 * np.pi * 440 * np.linspace(0, 1, 44100))
features_batch = processor.process_batch(samples)

# Cleanup
processor.close()
```

### 2.3 Optimization

```python
import tcde
import numpy as np

# Define objective function
def rosenbrock(x):
    return (1 - x[0])**2 + 100 * (x[1] - x[0]**2)**2

# Create optimizer
optimizer = tcde.Optimizer(
    dimension=2,
    bounds_min=[-5, -5],
    bounds_max=[5, 5],
    objective=rosenbrock
)

# Optimize
result = optimizer.optimize()
print(f"Solution: {result.solution}")
print(f"Value: {result.value}")
print(f"Converged: {result.converged}")

# Adapt constraints
result2 = optimizer.adapt_constraints(
    bounds_min=[-2, -2],
    bounds_max=[2, 2]
)

# Cleanup
optimizer.close()
```

### 2.4 Pattern Recognition

```python
import tcde
import numpy as np

# Create recognizer
recognizer = tcde.PatternRecognizer(
    pattern_width=28,
    pattern_height=28,
    topology_dim=10
)

# Learn patterns
for i in range(10):
    pattern = generate_pattern(i)  # Your pattern generator
    recognizer.learn_pattern(pattern, label=i)

# Recognize pattern
test_pattern = generate_test_pattern()
result = recognizer.recognize_pattern(test_pattern)
print(f"Predicted: {result.predicted_label}")
print(f"Confidence: {result.confidence}")

# Get accuracy
accuracy = recognizer.get_accuracy()
print(f"Accuracy: {accuracy * 100}%")

# Cleanup
recognizer.close()
```

---

## 3. REST API (Web Services)

### 3.1 Base URL

```
http://localhost:8080/api/v1/tcde
```

### 3.2 Signal Processing Endpoints

#### POST /signal/process

Process signal samples.

**Request**:
```json
{
  "sample_rate": 44100,
  "samples": [0.0, 0.5, 1.0, 0.5, 0.0]
}
```

**Response**:
```json
{
  "status": "success",
  "features": [
    {
      "energy": 0.0,
      "curvature": 0.0,
      "components": 0,
      "mean": 0.4,
      "variance": 0.2,
      "timestamp": 0.0
    },
    ...
  ],
  "processing_time_ms": 0.221
}
```

#### POST /signal/detect-anomalies

Detect anomalies in signal.

**Request**:
```json
{
  "sample_rate": 44100,
  "samples": [...],
  "threshold": 2.0
}
```

**Response**:
```json
{
  "status": "success",
  "anomalies": [
    {
      "index": 1050,
      "value": 5.0,
      "score": 3.5
    }
  ],
  "total_anomalies": 1
}
```

---

### 3.3 Optimization Endpoints

#### POST /optimize/create

Create optimization problem.

**Request**:
```json
{
  "dimension": 2,
  "bounds_min": [-5, -5],
  "bounds_max": [5, 5],
  "function": "rosenbrock"
}
```

**Response**:
```json
{
  "status": "success",
  "optimizer_id": "opt_12345",
  "message": "Optimizer created"
}
```

#### POST /optimize/run

Run optimization.

**Request**:
```json
{
  "optimizer_id": "opt_12345",
  "max_iterations": 10000,
  "tolerance": 1e-6
}
```

**Response**:
```json
{
  "status": "success",
  "solution": [1.0, 1.0],
  "value": 0.000000,
  "iterations": 5000,
  "time_seconds": 0.002,
  "converged": true
}
```

#### POST /optimize/adapt

Adapt constraints.

**Request**:
```json
{
  "optimizer_id": "opt_12345",
  "bounds_min": [-2, -2],
  "bounds_max": [2, 2]
}
```

**Response**:
```json
{
  "status": "success",
  "solution": [1.0, 1.0],
  "value": 0.000000,
  "adaptation_time_ms": 0.5
}
```

---

### 3.4 Pattern Recognition Endpoints

#### POST /pattern/learn

Learn pattern.

**Request**:
```json
{
  "recognizer_id": "rec_12345",
  "width": 28,
  "height": 28,
  "data": [...],  // Flattened array
  "label": 0
}
```

**Response**:
```json
{
  "status": "success",
  "message": "Pattern learned",
  "total_patterns": 10
}
```

#### POST /pattern/recognize

Recognize pattern.

**Request**:
```json
{
  "recognizer_id": "rec_12345",
  "width": 28,
  "height": 28,
  "data": [...]  // Flattened array
}
```

**Response**:
```json
{
  "status": "success",
  "predicted_label": 5,
  "confidence": 0.98,
  "class_scores": [0.01, 0.02, ..., 0.98, ...],
  "recognition_time_ms": 0.042
}
```

#### GET /pattern/accuracy

Get recognizer accuracy.

**Request**:
```
GET /pattern/accuracy?recognizer_id=rec_12345
```

**Response**:
```json
{
  "status": "success",
  "accuracy": 1.0,
  "total_recognitions": 100,
  "correct_recognitions": 100
}
```

---

## 4. Error Handling

### 4.1 C API Error Codes

```c
typedef enum {
    TCDE_SUCCESS = 0,
    TCDE_ERROR_INVALID_PARAM = -1,
    TCDE_ERROR_ALLOCATION = -2,
    TCDE_ERROR_NOT_INITIALIZED = -3,
    TCDE_ERROR_CONVERGENCE = -4,
    TCDE_ERROR_IO = -5
} TCDE_ErrorCode;
```

### 4.2 Python API Exceptions

```python
class TCDEError(Exception):
    """Base TCDE exception"""
    pass

class TCDEInvalidParameterError(TCDEError):
    """Invalid parameter error"""
    pass

class TCDEAllocationError(TCDEError):
    """Memory allocation error"""
    pass

class TCDEConvergenceError(TCDEError):
    """Convergence error"""
    pass
```

### 4.3 REST API Error Responses

```json
{
  "status": "error",
  "error_code": "INVALID_PARAMETER",
  "message": "Invalid dimension: must be > 0",
  "details": {
    "parameter": "dimension",
    "value": -1
  }
}
```

---

## 5. Performance Considerations

### 5.1 Threading

**C API**:
- Thread-safe: No (use separate instances per thread)
- Recommendation: Create one instance per thread

**Python API**:
- GIL: Released during computation
- Recommendation: Use multiprocessing for parallelism

**REST API**:
- Concurrent requests: Supported
- Max connections: Configurable (default: 100)

### 5.2 Memory Management

**C API**:
- Manual: Caller must free returned pointers
- Example: `free(result.solution)` after use

**Python API**:
- Automatic: Garbage collected
- Recommendation: Call `.close()` for immediate cleanup

**REST API**:
- Automatic: Server manages lifecycle
- Timeout: 5 minutes of inactivity

---

## 6. Examples

### 6.1 Complete C Example

```c
#include "tcde_signal_processor.h"
#include <stdio.h>
#include <math.h>

int main() {
    // Create processor
    TCDESignalProcessor* proc = TCDE_CreateSignalProcessor(44100, 1024);
    if (!proc) {
        fprintf(stderr, "Failed to create processor\\n");
        return 1;
    }
    
    // Generate sine wave
    for (int i = 0; i < 44100; i++) {
        float sample = sinf(2.0f * M_PI * 440.0f * i / 44100.0f);
        SignalFeatures features = TCDE_ProcessSample(proc, sample);
        
        if (i % 1000 == 0) {
            printf("Sample %d: E=%.3f, R=%.3f, H0=%d\\n",
                   i, features.energy, features.curvature, features.components);
        }
    }
    
    // Cleanup
    TCDE_DestroySignalProcessor(proc);
    return 0;
}
```

### 6.2 Complete Python Example

```python
import tcde
import numpy as np
import matplotlib.pyplot as plt

# Signal Processing
processor = tcde.SignalProcessor(sample_rate=44100)
t = np.linspace(0, 1, 44100)
signal = np.sin(2 * np.pi * 440 * t)
features = processor.process_batch(signal)

plt.plot([f.energy for f in features])
plt.title("Signal Energy Over Time")
plt.show()

# Optimization
def sphere(x):
    return np.sum(x**2)

optimizer = tcde.Optimizer(
    dimension=10,
    bounds_min=[-10] * 10,
    bounds_max=[10] * 10,
    objective=sphere
)
result = optimizer.optimize()
print(f"Minimum found: {result.value}")

# Pattern Recognition
recognizer = tcde.PatternRecognizer(28, 28, 10)

# Train
for i in range(10):
    for j in range(10):
        pattern = generate_digit(i)
        recognizer.learn_pattern(pattern, label=i)

# Test
test_pattern = generate_digit(5)
result = recognizer.recognize_pattern(test_pattern)
print(f"Recognized as: {result.predicted_label}")
```

### 6.3 Complete REST Example

```python
import requests
import json

BASE_URL = "http://localhost:8080/api/v1/tcde"

# Create optimizer
response = requests.post(f"{BASE_URL}/optimize/create", json={
    "dimension": 2,
    "bounds_min": [-5, -5],
    "bounds_max": [5, 5],
    "function": "rosenbrock"
})
optimizer_id = response.json()["optimizer_id"]

# Run optimization
response = requests.post(f"{BASE_URL}/optimize/run", json={
    "optimizer_id": optimizer_id,
    "max_iterations": 10000
})
result = response.json()
print(f"Solution: {result['solution']}")
print(f"Value: {result['value']}")
```

---

## 7. Versioning

### 7.1 API Versioning

- **Current version**: v1
- **URL format**: `/api/v1/tcde/...`
- **Backward compatibility**: Guaranteed within major version

### 7.2 Library Versioning

- **Semantic versioning**: MAJOR.MINOR.PATCH
- **Current version**: 1.0.0
- **Breaking changes**: Only in major versions

---

## 8. Rate Limiting

### 8.1 REST API Limits

- **Requests per minute**: 1000
- **Concurrent connections**: 100
- **Max request size**: 10 MB
- **Max response size**: 50 MB

### 8.2 Quotas

- **Free tier**: 10,000 requests/day
- **Pro tier**: 1,000,000 requests/day
- **Enterprise**: Unlimited

---

## 9. Authentication

### 9.1 API Keys

```bash
# Include API key in header
curl -H "X-API-Key: your_api_key_here" \
     http://localhost:8080/api/v1/tcde/optimize/create
```

### 9.2 OAuth 2.0

```python
import requests
from requests_oauthlib import OAuth2Session

oauth = OAuth2Session(client_id, token=token)
response = oauth.post(f"{BASE_URL}/optimize/create", json={...})
```

---

## 10. Support

### 10.1 Documentation

- **API Reference**: https://tcde.dev/api/reference
- **Tutorials**: https://tcde.dev/tutorials
- **Examples**: https://github.com/tcde/examples

### 10.2 Community

- **Forum**: https://forum.tcde.dev
- **Discord**: https://discord.gg/tcde
- **Stack Overflow**: Tag `tcde`

---

**Status**: API Specification Complete ✅  
**Date**: 10 Novembre 2025  
**Version**: 1.0  
**Protocol**: ZERO TOLERANCE

---

**End of API Specification**
