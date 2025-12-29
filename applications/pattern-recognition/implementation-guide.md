# Pattern Recognition with TCDE - Implementation Guide

**Domain**: Topological Pattern Recognition  
**Date**: 10 Novembre 2025  
**Status**: Design Guide (POC Implementation Planned)

---

## Quick Start

### Step 1: Setup

```bash
# Install TCDE Core
git clone https://github.com/tcde/tcde-core.git
cd tcde-core && mkdir build && cd build
cmake .. && make && sudo make install

# Python dependencies
pip install numpy pillow scipy
```

### Step 2: Basic Recognizer

```c
// tcde_pattern_recognizer.h
#ifndef TCDE_PATTERN_RECOGNIZER_H
#define TCDE_PATTERN_RECOGNIZER_H

#include <tcde/tcde_core.h>

typedef struct {
    double curvature;
    double energy;
    int num_components;
    char* label;
} PatternSignature;

typedef struct {
    PatternSignature* signatures;
    int num_signatures;
    int capacity;
} TCDEPatternRecognizer;

TCDEPatternRecognizer* TCDE_CreateRecognizer(void);
void TCDE_LearnPattern(TCDEPatternRecognizer* rec, const float* data, 
                       int size, const char* label);
const char* TCDE_RecognizePattern(TCDEPatternRecognizer* rec, 
                                 const float* data, int size);
void TCDE_DestroyRecognizer(TCDEPatternRecognizer* rec);

#endif
```

### Step 3: Implementation

```c
// tcde_pattern_recognizer.c
#include "tcde_pattern_recognizer.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

TCDEPatternRecognizer* TCDE_CreateRecognizer(void) {
    TCDEPatternRecognizer* rec = malloc(sizeof(TCDEPatternRecognizer));
    rec->capacity = 1000;
    rec->signatures = malloc(rec->capacity * sizeof(PatternSignature));
    rec->num_signatures = 0;
    return rec;
}

void TCDE_LearnPattern(TCDEPatternRecognizer* rec, const float* data, 
                       int size, const char* label) {
    // 1. Create field
    TCDE_Field* field = TCDE_CreateField(100, 2.5);
    
    // 2. Embed data (simple: use first 6 dimensions or pad)
    for (int i = 0; i < fmin(size/6, 100); i++) {
        float coords[6];
        for (int j = 0; j < 6; j++) {
            coords[j] = (i*6+j < size) ? data[i*6+j] : 0.0f;
        }
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = coords[0] + I * coords[1];
        TCDE_AddCenter(field, &point, coeff, 0.1);
    }
    
    // 3. Extract signature
    PatternSignature sig;
    sig.curvature = compute_curvature(field);
    sig.energy = TCDE_ComputeEnergy(field);
    sig.num_components = count_components(field);
    sig.label = strdup(label);
    
    // 4. Store
    rec->signatures[rec->num_signatures++] = sig;
    
    TCDE_DestroyField(field);
}

const char* TCDE_RecognizePattern(TCDEPatternRecognizer* rec, 
                                 const float* data, int size) {
    // 1. Create field for query
    TCDE_Field* field = TCDE_CreateField(100, 2.5);
    
    // 2. Embed (same as learning)
    for (int i = 0; i < fmin(size/6, 100); i++) {
        float coords[6];
        for (int j = 0; j < 6; j++) {
            coords[j] = (i*6+j < size) ? data[i*6+j] : 0.0f;
        }
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = coords[0] + I * coords[1];
        TCDE_AddCenter(field, &point, coeff, 0.1);
    }
    
    // 3. Extract signature
    double query_curv = compute_curvature(field);
    double query_energy = TCDE_ComputeEnergy(field);
    int query_comp = count_components(field);
    
    // 4. Find nearest neighbor
    double min_dist = INFINITY;
    const char* best_label = NULL;
    
    for (int i = 0; i < rec->num_signatures; i++) {
        double dist = pow(query_curv - rec->signatures[i].curvature, 2) +
                     pow(query_energy - rec->signatures[i].energy, 2) +
                     pow(query_comp - rec->signatures[i].num_components, 2);
        
        if (dist < min_dist) {
            min_dist = dist;
            best_label = rec->signatures[i].label;
        }
    }
    
    TCDE_DestroyField(field);
    return best_label;
}
```

### Step 4: Test on MNIST

```python
# test_mnist.py
import numpy as np
from PIL import Image
from tcde_pattern_recognizer import PatternRecognizer

# Load MNIST subset (100 images per class)
def load_mnist_subset():
    # Download from http://yann.lecun.com/exdb/mnist/
    # Or use: from tensorflow.keras.datasets import mnist
    pass

# Create recognizer
recognizer = PatternRecognizer()

# Learn from training set (100 examples per class)
for image, label in training_data:
    # Flatten image to 1D array
    data = image.flatten().astype(np.float32) / 255.0
    recognizer.learn_pattern(data, str(label))

# Test on test set
correct = 0
total = 0

for image, label in test_data:
    data = image.flatten().astype(np.float32) / 255.0
    prediction = recognizer.recognize_pattern(data)
    
    if prediction == str(label):
        correct += 1
    total += 1

accuracy = correct / total
print(f"Accuracy: {accuracy:.2%}")
```

---

## Advanced Features

### Persistent Homology

```c
// Simplified persistence computation
int count_components(TCDE_Field* field) {
    // Sample field on grid
    int grid_size = 20;
    float* samples = malloc(grid_size * grid_size * sizeof(float));
    
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            float coords[6] = {i/(float)grid_size, j/(float)grid_size, 
                              0, 0, 0, 0};
            TCDE_Point p = TCDE_CreatePoint(6, coords);
            TCDE_Complex phi = TCDE_Evaluate6D(field, &p);
            samples[i*grid_size + j] = cabsf(phi);
        }
    }
    
    // Count peaks (simplified H_0)
    int components = 0;
    for (int i = 1; i < grid_size-1; i++) {
        for (int j = 1; j < grid_size-1; j++) {
            int idx = i*grid_size + j;
            if (samples[idx] > samples[idx-1] && 
                samples[idx] > samples[idx+1] &&
                samples[idx] > samples[idx-grid_size] &&
                samples[idx] > samples[idx+grid_size]) {
                components++;
            }
        }
    }
    
    free(samples);
    return components;
}
```

### Curvature Computation

```c
double compute_curvature(TCDE_Field* field) {
    // Update adaptive metric
    float alpha = 0.1;
    float energy = TCDE_ComputeEnergy(field);
    
    for (int i = 0; i < 6; i++) {
        field->metric->g[i][i] = 1.0 + alpha * energy;
    }
    
    // Simplified Ricci scalar
    double R = -alpha * energy;
    return R;
}
```

---

## Benchmarking

### Comparison Script

```python
import time
import numpy as np
from sklearn.svm import SVC
from sklearn.neural_network import MLPClassifier
from tcde_pattern_recognizer import PatternRecognizer

# Load data
X_train, y_train = load_mnist_subset(100)  # 100 per class
X_test, y_test = load_mnist_test()

# TCDE
start = time.time()
tcde = PatternRecognizer()
for x, y in zip(X_train, y_train):
    tcde.learn_pattern(x.flatten(), str(y))
tcde_train_time = time.time() - start

start = time.time()
tcde_predictions = [tcde.recognize_pattern(x.flatten()) for x in X_test]
tcde_test_time = time.time() - start
tcde_accuracy = np.mean([p == str(y) for p, y in zip(tcde_predictions, y_test)])

# SVM
start = time.time()
svm = SVC()
svm.fit(X_train.reshape(len(X_train), -1), y_train)
svm_train_time = time.time() - start

start = time.time()
svm_predictions = svm.predict(X_test.reshape(len(X_test), -1))
svm_test_time = time.time() - start
svm_accuracy = np.mean(svm_predictions == y_test)

# Results
print("Comparison Results:")
print(f"TCDE: Train={tcde_train_time:.2f}s, Test={tcde_test_time:.2f}s, Acc={tcde_accuracy:.2%}")
print(f"SVM:  Train={svm_train_time:.2f}s, Test={svm_test_time:.2f}s, Acc={svm_accuracy:.2%}")
```

---

## Troubleshooting

### Issue: Low Accuracy

**Solutions**:
1. Increase number of centers (100 → 200)
2. Adjust feature weights in distance function
3. Add more topological features
4. Normalize input data properly

### Issue: Slow Recognition

**Solutions**:
1. Reduce centers (100 → 50)
2. Use approximate nearest neighbor
3. Cache signatures
4. Parallelize matching

---

## Next Steps

1. **Performance Comparison**: See [comparison.md](./comparison.md)
2. **Try POC**: Implement and test on MNIST
3. **Optimize**: Profile and tune parameters

---

**Status**: ✅ Implementation Guide Complete  
**Note**: DESIGN GUIDE - POC implementation pending  
**Estimated Time**: 2-3 days for POC
