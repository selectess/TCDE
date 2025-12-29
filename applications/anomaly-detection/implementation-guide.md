# Anomaly Detection with TCDE - Implementation Guide

**Date**: 10 Novembre 2025

## Quick Implementation

```c
// tcde_anomaly_detector.h
typedef struct {
    TCDE_Field* normal_profile;
    double threshold;
    int num_features;
} TCDEAnomalyDetector;

TCDEAnomalyDetector* TCDE_CreateDetector(int num_features);
void TCDE_LearnNormal(TCDEAnomalyDetector* det, const double** data, int n);
bool TCDE_IsAnomaly(TCDEAnomalyDetector* det, const double* data);
double TCDE_GetAnomalyScore(TCDEAnomalyDetector* det, const double* data);
```

## Python Example

```python
from tcde_anomaly_detector import AnomalyDetector
import numpy as np

# Create detector
detector = AnomalyDetector(num_features=10)

# Learn from normal data
normal_data = np.loadtxt('normal_network_traffic.csv')
detector.learn_normal(normal_data)

# Detect anomalies in stream
for packet in network_stream:
    features = extract_features(packet)
    
    if detector.is_anomaly(features):
        score = detector.get_anomaly_score(features)
        print(f"ALERT: Anomaly detected (score={score:.3f})")
        log_alert(packet, score)
```

## Comparison with Isolation Forest

```python
from sklearn.ensemble import IsolationForest
import time

# Isolation Forest
start = time.time()
iso_forest = IsolationForest()
iso_forest.fit(normal_data)
iso_time = time.time() - start

# TCDE
start = time.time()
detector = AnomalyDetector(num_features=10)
detector.learn_normal(normal_data)
tcde_time = time.time() - start

print(f"Isolation Forest training: {iso_time:.2f}s")
print(f"TCDE training: {tcde_time:.2f}s")

# Test detection
test_data = load_test_data()  # Mix of normal and anomalies

iso_predictions = iso_forest.predict(test_data)
tcde_predictions = [detector.is_anomaly(x) for x in test_data]

# Compare accuracy, false positives
```

---

**Status**: ✅ Implementation Guide Complete
