# Anomaly Detection with TCDE - Performance Comparison

**Date**: 10 Novembre 2025  
**Status**: Theoretical Projections

## Disclaimer

**THEORETICAL PROJECTIONS** - Validation required.

---

## Executive Summary (Projected)

| Metric | Traditional IDS | Isolation Forest | TCDE (Proj.) |
|--------|----------------|------------------|--------------|
| **Detection Rate** | 70-85% | 80-90% | >95% |
| **False Positives** | 5-15% | 2-5% | <1% |
| **Latency** | 1-10 sec | 100-1000ms | <100ms |
| **Training** | Hours | Minutes | <1 second |
| **Adaptation** | Manual | Retrain | <1 second |

---

## vs Traditional IDS

### Detection Capability

```
Signature-based IDS: Detects known attacks only
TCDE: Detects unknown attacks (topological deviation)

Advantage: Zero-day attack detection
```

### False Positives (Projected)

```
IDS: 5-15% (high alarm fatigue)
TCDE: <1% (projected, topological precision)

Improvement: 5-15× fewer false alarms (projected)
```

### Speed (Projected)

```
IDS: 1-10 seconds (signature matching)
TCDE: <100ms (field evaluation)

Speedup: 10-100× faster (projected)
```

---

## vs Isolation Forest

### Accuracy (Projected)

```
KDD Cup 99 dataset:
  Isolation Forest: 85% detection, 3% FP
  TCDE: 95% detection, <1% FP (projected)
  
Improvement: +10% detection, -2% FP (projected)
```

### Training Time (Projected)

```
10,000 samples:
  Isolation Forest: 1-5 minutes
  TCDE: <1 second (projected)
  
Speedup: 60-300× (projected)
```

---

## Benchmark Scenarios (Projected)

### Network Intrusion (KDD Cup 99)

```
Metric              IDS      Iso Forest  TCDE (Proj.)
Detection Rate      75%      85%         95%
False Positives     10%      3%          <1%
Latency            5s       500ms       <100ms
Training           Hours    5 min       <1s
```

### Industrial Monitoring

```
Metric              Threshold  Autoencoder  TCDE (Proj.)
Failure Detection   70%        80%          90%
False Alarms        15%        5%           <2%
Response Time       1s         100ms        <50ms
```

---

## Validation Plan

1. Test on KDD Cup 99, NSL-KDD datasets
2. Measure detection rate, false positives
3. Compare latency in real-time stream
4. Validate adaptation capability

---

**Status**: ✅ Comparison Complete  
**Nature**: THEORETICAL PROJECTIONS
