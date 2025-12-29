# Anomaly Detection with TCDE - Architecture

**Date**: 10 Novembre 2025

## Architecture

```
Data Stream → Field → Deviation Measure → Alert
```

## Core Components

### 1. Normal Profile
```c
void learn_normal_profile(TCDE_Field* field, const double** normal_data, int n) {
    for (int i = 0; i < n; i++) {
        embed_data_point(normal_data[i], field);
    }
    // Field represents "normal" topology
}
```

### 2. Anomaly Detection
```c
bool detect_anomaly(TCDE_Field* normal_field, const double* data, double threshold) {
    // Create field for data point
    TCDE_Field* test_field = TCDE_CreateField(10, 2.5);
    embed_data_point(data, test_field);
    
    // Compute topological distance
    double deviation = topological_distance(normal_field, test_field);
    
    TCDE_DestroyField(test_field);
    return deviation > threshold;
}
```

### 3. Topological Distance
```c
double topological_distance(TCDE_Field* f1, TCDE_Field* f2) {
    double curv_diff = fabs(compute_curvature(f1) - compute_curvature(f2));
    double energy_diff = fabs(TCDE_ComputeEnergy(f1) - TCDE_ComputeEnergy(f2));
    return sqrt(curv_diff*curv_diff + energy_diff*energy_diff);
}
```

---

**Status**: ✅ Architecture Documented
