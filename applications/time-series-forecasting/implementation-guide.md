# Time Series Forecasting with TCDE - Implementation Guide

**Date**: 10 Novembre 2025

## Quick Implementation

```c
// tcde_forecaster.h
typedef struct {
    TCDE_Field* field;
    double* history;
    int history_length;
} TCDEForecaster;

TCDEForecaster* TCDE_CreateForecaster(int history_length);
void TCDE_UpdateHistory(TCDEForecaster* fc, double new_value);
double TCDE_Predict(TCDEForecaster* fc, int horizon);
double TCDE_GetUncertainty(TCDEForecaster* fc);
```

## Python Example

```python
from tcde_forecaster import Forecaster
import numpy as np

# Create forecaster
fc = Forecaster(history_length=100)

# Load time series
data = np.loadtxt('stock_prices.csv')

# Train on history
for value in data[:100]:
    fc.update_history(value)

# Predict next 10 steps
predictions = []
uncertainties = []

for h in range(1, 11):
    pred = fc.predict(horizon=h)
    unc = fc.get_uncertainty()
    predictions.append(pred)
    uncertainties.append(unc)

# Plot with confidence intervals
import matplotlib.pyplot as plt
plt.plot(predictions, label='Prediction')
plt.fill_between(range(10), 
                 np.array(predictions) - np.array(uncertainties),
                 np.array(predictions) + np.array(uncertainties),
                 alpha=0.3, label='Uncertainty')
plt.legend()
plt.show()
```

## Comparison with LSTM

```python
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense
import time

# LSTM
start = time.time()
model = Sequential([
    LSTM(50, input_shape=(100, 1)),
    Dense(1)
])
model.compile(optimizer='adam', loss='mse')
model.fit(X_train, y_train, epochs=50, verbose=0)
lstm_time = time.time() - start

# TCDE
start = time.time()
fc = Forecaster(history_length=100)
for value in data[:100]:
    fc.update_history(value)
tcde_time = time.time() - start

print(f"LSTM training: {lstm_time:.2f}s")
print(f"TCDE training: {tcde_time:.2f}s")
print(f"Speedup: {lstm_time/tcde_time:.0f}×")
```

---

**Status**: ✅ Implementation Guide Complete
