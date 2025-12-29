# Optimization with TCDE - Implementation Guide

**Domain**: Continuous Optimization  
**Date**: 10 Novembre 2025  
**Status**: Step-by-Step Design (Implementation Pending)

---

## Prerequisites

### Knowledge
- Optimization theory basics
- C programming
- Linear algebra
- TCDE fundamentals

### Software
```bash
- GCC 9+ or Clang 10+
- CMake 3.15+
- TCDE Core library
- BLAS/LAPACK (optional)
- Python 3.8+ (for testing)
- SciPy (for comparison)
```

---

## Step 1: Setup

### Install Dependencies

```bash
# TCDE Core (if not already installed)
git clone https://github.com/tcde/tcde-core.git
cd tcde-core && mkdir build && cd build
cmake .. && make && sudo make install

# Optimization benchmarks
pip install scipy numpy matplotlib
```

---

## Step 2: Create Optimizer Structure

### Header File

Create `tcde_optimizer.h`:

```c
#ifndef TCDE_OPTIMIZER_H
#define TCDE_OPTIMIZER_H

#include <tcde/tcde_core.h>

// Objective function type
typedef double (*ObjectiveFunc)(const double* x, int n, void* params);

// Constraint function type
typedef double (*ConstraintFunc)(const double* x, int n, int idx, void* params);

// Optimizer configuration
typedef struct {
    ObjectiveFunc objective;
    void* obj_params;
    
    int dimension;
    double* lower_bounds;
    double* upper_bounds;
    
    int num_ineq_constraints;
    ConstraintFunc ineq_constraints;
    void* constraint_params;
    
    // Evolution parameters
    double diffusion;
    double drift_strength;
    double constraint_penalty;
    double dt;
    int max_iterations;
    double tolerance;
    
} OptimizerConfig;

// Result structure
typedef struct {
    double* solution;
    double objective_value;
    int iterations;
    bool converged;
    double* energy_history;
} OptimizationResult;

// Optimizer state
typedef struct {
    OptimizerConfig config;
    TCDE_Field* field;
    OptimizationResult current_best;
    int iteration;
} TCDEOptimizer;

// API
TCDEOptimizer* TCDE_CreateOptimizer(OptimizerConfig* config);
void TCDE_DestroyOptimizer(TCDEOptimizer* opt);
OptimizationResult TCDE_Optimize(TCDEOptimizer* opt);
void TCDE_UpdateConstraints(TCDEOptimizer* opt, ConstraintFunc new_constraints);

#endif
```

---

## Step 3: Implement Core Functions

### Objective Mapping

```c
static void map_objective_to_field(TCDEOptimizer* opt) {
    // Latin Hypercube Sampling for initial points
    int num_samples = 50;
    double** samples = lhs_sample(opt->config.dimension, num_samples,
                                  opt->config.lower_bounds,
                                  opt->config.upper_bounds);
    
    // Evaluate and create centers
    double f_min = INFINITY;
    for (int i = 0; i < num_samples; i++) {
        double f_val = opt->config.objective(samples[i], 
                                            opt->config.dimension,
                                            opt->config.obj_params);
        f_min = fmin(f_min, f_val);
    }
    
    // Create RBF centers
    for (int i = 0; i < num_samples; i++) {
        double f_val = opt->config.objective(samples[i], 
                                            opt->config.dimension,
                                            opt->config.obj_params);
        
        // Map to 6D (embed first 6 dimensions, pad if needed)
        float coords[6] = {0};
        for (int j = 0; j < fmin(6, opt->config.dimension); j++) {
            coords[j] = samples[i][j];
        }
        
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        
        // Boltzmann weight
        double beta = 1.0;
        TCDE_Complex coeff = exp(-beta * (f_val - f_min)) + 0.0 * I;
        
        TCDE_AddCenter(opt->field, &point, coeff, 0.1);
    }
    
    free_samples(samples, num_samples);
}
```

### Evolution Step

```c
static void evolve_step(TCDEOptimizer* opt) {
    TCDE_Field* field = opt->field;
    int k = TCDE_GetNumCenters(field);
    
    // 1. Compute Laplacian (diffusion)
    TCDE_Complex* laplacian = malloc(k * sizeof(TCDE_Complex));
    
    for (int i = 0; i < k; i++) {
        laplacian[i] = 0.0;
        TCDE_Point* pi = TCDE_GetCenterPoint(field, i);
        
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            
            TCDE_Point* pj = TCDE_GetCenterPoint(field, j);
            float r = TCDE_ComputeDistance(pi, pj, field->metric);
            
            // Gaussian RBF Laplacian
            float epsilon = 0.1;
            float er = epsilon * r;
            float L_ij = 2.0 * epsilon * epsilon * 
                        (2.0 * er * er - 6.0) * expf(-er * er);
            
            TCDE_Complex cj = TCDE_GetCenterCoeff(field, j);
            laplacian[i] += L_ij * cj;
        }
    }
    
    // 2. Apply diffusion
    for (int i = 0; i < k; i++) {
        TCDE_Complex ci = TCDE_GetCenterCoeff(field, i);
        TCDE_Complex new_ci = ci + opt->config.dt * opt->config.diffusion * laplacian[i];
        TCDE_SetCenterCoeff(field, i, new_ci);
    }
    
    free(laplacian);
    
    // 3. Apply drift (toward low objective)
    for (int i = 0; i < k; i++) {
        // Extract solution from center
        double* x = extract_solution(field, i, opt->config.dimension);
        
        // Numerical gradient
        double* grad = numerical_gradient(opt->config.objective, x, 
                                         opt->config.dimension,
                                         opt->config.obj_params);
        
        // Update coefficient (drift)
        TCDE_Complex ci = TCDE_GetCenterCoeff(field, i);
        TCDE_Complex drift = -opt->config.drift_strength * 
                            (grad[0] + I * (opt->config.dimension > 1 ? grad[1] : 0)) * ci;
        
        TCDE_Complex new_ci = ci + opt->config.dt * drift;
        TCDE_SetCenterCoeff(field, i, new_ci);
        
        free(x);
        free(grad);
    }
}
```

### Constraint Handling

```c
static void apply_constraints(TCDEOptimizer* opt) {
    if (opt->config.num_ineq_constraints == 0) return;
    
    TCDE_Field* field = opt->field;
    int k = TCDE_GetNumCenters(field);
    
    for (int i = 0; i < k; i++) {
        double* x = extract_solution(field, i, opt->config.dimension);
        
        // Compute total violation
        double violation = 0.0;
        for (int j = 0; j < opt->config.num_ineq_constraints; j++) {
            double g_j = opt->config.ineq_constraints(x, opt->config.dimension, 
                                                      j, opt->config.constraint_params);
            if (g_j > 0) {
                violation += g_j * g_j;
            }
        }
        
        // Penalty
        double penalty = exp(-opt->config.constraint_penalty * violation);
        TCDE_Complex ci = TCDE_GetCenterCoeff(field, i);
        TCDE_SetCenterCoeff(field, i, ci * penalty);
        
        free(x);
    }
}
```

### Solution Extraction

```c
static OptimizationResult extract_best_solution(TCDEOptimizer* opt) {
    OptimizationResult result;
    result.solution = malloc(opt->config.dimension * sizeof(double));
    result.objective_value = INFINITY;
    
    int k = TCDE_GetNumCenters(opt->field);
    
    for (int i = 0; i < k; i++) {
        double* x = extract_solution(opt->field, i, opt->config.dimension);
        double f_val = opt->config.objective(x, opt->config.dimension,
                                            opt->config.obj_params);
        
        if (f_val < result.objective_value) {
            result.objective_value = f_val;
            memcpy(result.solution, x, opt->config.dimension * sizeof(double));
        }
        
        free(x);
    }
    
    result.iterations = opt->iteration;
    result.converged = false;  // Set by main loop
    
    return result;
}
```

---

## Step 4: Main Optimization Loop

```c
OptimizationResult TCDE_Optimize(TCDEOptimizer* opt) {
    opt->current_best.energy_history = malloc(opt->config.max_iterations * sizeof(double));
    
    for (opt->iteration = 0; opt->iteration < opt->config.max_iterations; opt->iteration++) {
        // 1. Evolution step
        evolve_step(opt);
        
        // 2. Apply constraints
        apply_constraints(opt);
        
        // 3. Track energy
        opt->current_best.energy_history[opt->iteration] = TCDE_ComputeEnergy(opt->field);
        
        // 4. Extract current best
        OptimizationResult current = extract_best_solution(opt);
        
        // 5. Update if improved
        if (current.objective_value < opt->current_best.objective_value) {
            if (opt->current_best.solution) free(opt->current_best.solution);
            opt->current_best = current;
        } else {
            free(current.solution);
        }
        
        // 6. Check convergence
        if (opt->iteration > 10) {
            double energy_change = fabs(
                opt->current_best.energy_history[opt->iteration] - 
                opt->current_best.energy_history[opt->iteration - 10]
            );
            
            if (energy_change < opt->config.tolerance) {
                opt->current_best.converged = true;
                break;
            }
        }
    }
    
    return opt->current_best;
}
```

---

## Step 5: Test on Benchmark Functions

### Rosenbrock Function

```c
double rosenbrock(const double* x, int n, void* params) {
    double sum = 0.0;
    for (int i = 0; i < n - 1; i++) {
        double term1 = x[i+1] - x[i]*x[i];
        double term2 = 1.0 - x[i];
        sum += 100.0 * term1 * term1 + term2 * term2;
    }
    return sum;
}

void test_rosenbrock() {
    OptimizerConfig config = {
        .objective = rosenbrock,
        .obj_params = NULL,
        .dimension = 2,
        .lower_bounds = (double[]){-5.0, -5.0},
        .upper_bounds = (double[]){5.0, 5.0},
        .num_ineq_constraints = 0,
        .diffusion = 0.05,
        .drift_strength = 0.1,
        .dt = 0.01,
        .max_iterations = 1000,
        .tolerance = 1e-6
    };
    
    TCDEOptimizer* opt = TCDE_CreateOptimizer(&config);
    OptimizationResult result = TCDE_Optimize(opt);
    
    printf("Rosenbrock Test:\n");
    printf("  Solution: (%.6f, %.6f)\n", result.solution[0], result.solution[1]);
    printf("  Objective: %.6e\n", result.objective_value);
    printf("  Iterations: %d\n", result.iterations);
    printf("  Converged: %s\n", result.converged ? "Yes" : "No");
    printf("  Known optimum: (1.0, 1.0) with f=0.0\n");
    
    TCDE_DestroyOptimizer(opt);
    free(result.solution);
    free(result.energy_history);
}
```

### Rastrigin Function

```c
double rastrigin(const double* x, int n, void* params) {
    double A = 10.0;
    double sum = A * n;
    for (int i = 0; i < n; i++) {
        sum += x[i]*x[i] - A * cos(2.0 * M_PI * x[i]);
    }
    return sum;
}
```

### Ackley Function

```c
double ackley(const double* x, int n, void* params) {
    double sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < n; i++) {
        sum1 += x[i] * x[i];
        sum2 += cos(2.0 * M_PI * x[i]);
    }
    return -20.0 * exp(-0.2 * sqrt(sum1 / n)) - exp(sum2 / n) + 20.0 + M_E;
}
```

---

## Step 6: Python Bindings

```python
# tcde_optimizer.pyx
cdef extern from "tcde_optimizer.h":
    ctypedef double (*ObjectiveFunc)(const double* x, int n, void* params)
    
    ctypedef struct OptimizerConfig:
        ObjectiveFunc objective
        int dimension
        double* lower_bounds
        double* upper_bounds
        double diffusion
        double drift_strength
        double dt
        int max_iterations
        double tolerance
    
    ctypedef struct OptimizationResult:
        double* solution
        double objective_value
        int iterations
        bint converged
    
    ctypedef struct TCDEOptimizer:
        pass
    
    TCDEOptimizer* TCDE_CreateOptimizer(OptimizerConfig* config)
    void TCDE_DestroyOptimizer(TCDEOptimizer* opt)
    OptimizationResult TCDE_Optimize(TCDEOptimizer* opt)

cdef class Optimizer:
    cdef TCDEOptimizer* _opt
    cdef object _objective_func
    
    def __init__(self, objective, dimension, bounds, **kwargs):
        self._objective_func = objective
        
        # Setup config
        cdef OptimizerConfig config
        config.dimension = dimension
        # ... (setup other parameters)
        
        self._opt = TCDE_CreateOptimizer(&config)
    
    def optimize(self):
        cdef OptimizationResult result = TCDE_Optimize(self._opt)
        
        # Convert to Python
        solution = [result.solution[i] for i in range(self._opt.config.dimension)]
        
        return {
            'solution': solution,
            'objective_value': result.objective_value,
            'iterations': result.iterations,
            'converged': result.converged
        }
```

---

## Step 7: Comparison with SciPy

```python
import numpy as np
from scipy.optimize import minimize
from tcde_optimizer import Optimizer
import time

def rosenbrock(x):
    return sum(100.0*(x[1:]-x[:-1]**2.0)**2.0 + (1-x[:-1])**2.0)

# TCDE
start = time.time()
tcde_opt = Optimizer(rosenbrock, dimension=2, bounds=[(-5, 5), (-5, 5)])
tcde_result = tcde_opt.optimize()
tcde_time = time.time() - start

# SciPy (L-BFGS-B)
start = time.time()
scipy_result = minimize(rosenbrock, x0=np.random.uniform(-5, 5, 2), 
                       method='L-BFGS-B', bounds=[(-5, 5), (-5, 5)])
scipy_time = time.time() - start

print("Comparison:")
print(f"TCDE:  f={tcde_result['objective_value']:.6e}, time={tcde_time:.3f}s, iter={tcde_result['iterations']}")
print(f"SciPy: f={scipy_result.fun:.6e}, time={scipy_time:.3f}s, iter={scipy_result.nit}")
```

---

## Step 8: Constrained Optimization Example

```c
// Constraint: x^2 + y^2 <= 1 (unit circle)
double circle_constraint(const double* x, int n, int idx, void* params) {
    return x[0]*x[0] + x[1]*x[1] - 1.0;  // g(x) <= 0
}

void test_constrained() {
    OptimizerConfig config = {
        .objective = rosenbrock,
        .dimension = 2,
        .lower_bounds = (double[]){-2.0, -2.0},
        .upper_bounds = (double[]){2.0, 2.0},
        .num_ineq_constraints = 1,
        .ineq_constraints = circle_constraint,
        .constraint_penalty = 10.0,
        // ... other params
    };
    
    TCDEOptimizer* opt = TCDE_CreateOptimizer(&config);
    OptimizationResult result = TCDE_Optimize(opt);
    
    // Verify constraint
    double constraint_val = circle_constraint(result.solution, 2, 0, NULL);
    printf("Constraint value: %.6f (should be <= 0)\n", constraint_val);
}
```

---

## Troubleshooting

### Issue: Slow Convergence

**Solutions**:
1. Increase drift_strength (0.1 → 0.5)
2. Decrease diffusion (0.05 → 0.01)
3. Adjust dt (0.01 → 0.001)
4. Add more initial samples

### Issue: Stuck in Local Minimum

**Solutions**:
1. Increase diffusion (exploration)
2. Use multi-start strategy
3. Adaptive sampling in promising regions
4. Temperature annealing

### Issue: Constraint Violations

**Solutions**:
1. Increase constraint_penalty (10 → 100)
2. Use projection method instead of penalty
3. Adapt metric to encode constraints

---

## Next Steps

1. **Performance Comparison**: See [comparison.md](./comparison.md)
2. **Try Demo**: See [demo/README.md](./demo/README.md) (when available)

---

**Status**: ✅ Implementation Guide Complete  
**Note**: This is a DESIGN GUIDE - actual implementation and validation pending  
**Estimated Implementation Time**: 2-3 days
