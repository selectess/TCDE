# Time Series Forecasting with TCDE - Architecture

**Date**: 10 Novembre 2025

## Architecture

```
History → Embedding → Field Evolution → Prediction
```

## Core Components

### 1. Temporal Embedding
```c
void embed_timeseries(const double* series, int length, TCDE_Field* field) {
    for (int i = 0; i < length; i++) {
        float coords[6] = {
            series[i],                    // Value
            (i > 0) ? series[i]-series[i-1] : 0,  // Trend
            (float)i / length,            // Time position
            0, 0, 0
        };
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        TCDE_Complex coeff = series[i] + 0.0 * I;
        TCDE_AddCenter(field, &point, coeff, 0.1);
    }
}
```

### 2. Prediction
```c
double predict_next(TCDE_Field* field, int horizon) {
    // Evolve field forward
    for (int h = 0; h < horizon; h++) {
        evolve_field(field, 0.01);
    }
    
    // Extract prediction
    float coords[6] = {0, 0, 1.0, 0, 0, 0};  // Future time
    TCDE_Point future = TCDE_CreatePoint(6, coords);
    TCDE_Complex phi = TCDE_Evaluate6D(field, &future);
    
    return creal(phi);
}
```

### 3. Uncertainty
```c
double compute_uncertainty(TCDE_Field* field) {
    // Curvature = uncertainty
    return fabs(compute_curvature(field));
}
```

---

**Status**: ✅ Architecture Documented
