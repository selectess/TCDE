# Time Series Forecasting with TCDE - Performance Comparison

**Date**: 10 Novembre 2025  
**Status**: Theoretical Projections

## Disclaimer

**THEORETICAL PROJECTIONS** - Validation required.

---

## Executive Summary (Projected)

| Metric | ARIMA | LSTM | TCDE (Proj.) |
|--------|-------|------|--------------|
| **MAPE** | 8-12% | 6-10% | 5-8% |
| **Training** | 1-10 min | 1-10 hours | <1 second |
| **Prediction** | <1ms | 10-100ms | 1-10ms |
| **Adaptation** | Retrain | Retrain | <1 second |
| **Uncertainty** | No | Ensemble | Native |

---

## vs ARIMA

### Stationarity

```
ARIMA: Requires stationary data (differencing needed)
TCDE: Works on non-stationary (geometric evolution)

Advantage: TCDE handles regime changes
```

### Accuracy (Projected)

```
Stock prices (non-stationary):
  ARIMA: 10-15% MAPE
  TCDE: 6-10% MAPE (projected)
  
Improvement: 30-40% better (projected)
```

---

## vs LSTM

### Training Time (Projected)

```
1000 time steps:
  LSTM: 1-10 hours (GPU)
  TCDE: <1 second (projected)
  
Speedup: 3600-36000× (projected)
```

### Accuracy (Projected)

```
Financial data:
  LSTM: 6-10% MAPE
  TCDE: 5-8% MAPE (projected)
  
Improvement: 15-30% better (projected)
```

### Uncertainty

```
LSTM: Requires ensemble (10+ models)
TCDE: Native (curvature)

Advantage: Single model with uncertainty
```

---

## Benchmark Scenarios (Projected)

### Stock Price Prediction

```
Metric          ARIMA    LSTM     TCDE (Proj.)
MAPE            12%      8%       6%
Training        5 min    2 hours  <1s
Adaptation      Retrain  Retrain  <1s
Uncertainty     No       Ensemble Native
```

### Energy Demand

```
Metric          ARIMA    LSTM     TCDE (Proj.)
MAPE            10%      7%       5%
Horizon         1 day    1 week   1 week
Adaptation      Daily    Weekly   Real-time
```

---

## Validation Plan

1. Test on standard datasets (M4 competition)
2. Compare MAPE, training time, adaptation
3. Validate uncertainty calibration
4. Real-world deployment

---

**Status**: ✅ Comparison Complete  
**Nature**: THEORETICAL PROJECTIONS
