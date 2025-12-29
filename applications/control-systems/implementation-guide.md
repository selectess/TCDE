# Control Systems with TCDE - Implementation Guide

**Date**: 10 Novembre 2025  
**Status**: Design Guide

## Quick Implementation

### Step 1: Controller Structure

```c
// tcde_controller.h
typedef struct {
    TCDE_Field* field;
    double* state;
    double* control;
    int state_dim;
    int control_dim;
} TCDEController;

TCDEController* TCDE_CreateController(int state_dim, int control_dim);
void TCDE_ControlStep(TCDEController* ctrl, const double* state);
double* TCDE_GetControl(TCDEController* ctrl);
```

### Step 2: Implementation

```c
// tcde_controller.c
TCDEController* TCDE_CreateController(int state_dim, int control_dim) {
    TCDEController* ctrl = malloc(sizeof(TCDEController));
    ctrl->field = TCDE_CreateField(50, 2.5);
    ctrl->state_dim = state_dim;
    ctrl->control_dim = control_dim;
    ctrl->control = calloc(control_dim, sizeof(double));
    return ctrl;
}

void TCDE_ControlStep(TCDEController* ctrl, const double* state) {
    // Embed state
    float coords[6] = {0};
    for (int i = 0; i < fmin(6, ctrl->state_dim); i++) {
        coords[i] = state[i];
    }
    TCDE_Point point = TCDE_CreatePoint(6, coords);
    TCDE_Complex coeff = coords[0] + I * coords[1];
    TCDE_AddCenter(ctrl->field, &point, coeff, 0.1);
    
    // Evolve
    evolve_field(ctrl->field, 0.01);
    
    // Extract control
    TCDE_Complex grad[6];
    compute_field_gradient(ctrl->field, grad);
    for (int i = 0; i < ctrl->control_dim; i++) {
        ctrl->control[i] = -creal(grad[i]);
    }
}
```

### Step 3: Test (Inverted Pendulum)

```python
import numpy as np
from tcde_controller import Controller

# System dynamics
def pendulum_dynamics(state, control):
    theta, omega = state
    u = control[0]
    g, l, m = 9.81, 1.0, 1.0
    theta_dot = omega
    omega_dot = -g/l * np.sin(theta) + u/(m*l**2)
    return np.array([theta_dot, omega_dot])

# Create controller
controller = Controller(state_dim=2, control_dim=1)

# Simulation
state = np.array([np.pi/4, 0.0])  # Initial: 45 degrees
dt = 0.01

for t in np.arange(0, 10, dt):
    # Get control
    control = controller.control_step(state)
    
    # Apply dynamics
    state_dot = pendulum_dynamics(state, control)
    state += state_dot * dt
    
    print(f"t={t:.2f}, theta={state[0]:.3f}, u={control[0]:.3f}")
```

## Comparison with PID

```python
# PID controller
class PIDController:
    def __init__(self, Kp, Ki, Kd):
        self.Kp, self.Ki, self.Kd = Kp, Ki, Kd
        self.integral = 0
        self.prev_error = 0
    
    def control(self, error, dt):
        self.integral += error * dt
        derivative = (error - self.prev_error) / dt
        self.prev_error = error
        return self.Kp * error + self.Ki * self.integral + self.Kd * derivative

# Compare
pid = PIDController(Kp=10, Ki=1, Kd=5)  # Manual tuning required
tcde = Controller(state_dim=2, control_dim=1)  # No tuning

# Run both and compare settling time, overshoot
```

---

**Status**: ✅ Implementation Guide Complete  
**Note**: DESIGN GUIDE - Validation through simulation required
