# Optimization with TCDE - Architecture

**Domain**: Continuous Optimization  
**Date**: 10 Novembre 2025  
**Status**: Theoretical Design

---

## System Architecture

### High-Level Overview

```
┌────────────────────────────────────────────────────────────────┐
│                    TCDE Optimizer                              │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐    │
│  │  Objective   │───▶│   TCDE       │───▶│  Solution    │    │
│  │  Function    │    │   Core       │    │  Extraction  │    │
│  └──────────────┘    └──────────────┘    └──────────────┘    │
│         │                    │                    │           │
│         │                    │                    │           │
│  ┌──────▼──────┐    ┌───────▼────────┐  ┌────────▼───────┐  │
│  │ Landscape    │    │  Field         │  │  Optimum       │  │
│  │ Mapping      │    │  Evolution     │  │  Tracking      │  │
│  │ - f(x)→Φ(x)  │    │  - ∂Φ/∂t       │  │  - x* finder   │  │
│  └──────────────┘    │  - Metric g    │  │  - Convergence │  │
│                      │  - Constraints │  │  - Quality     │  │
│                      └────────────────┘  └────────────────┘  │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

---

## Core Components

### 1. Objective Landscape Mapping

**Purpose**: Transform optimization problem into geometric field

**Input**: 
- Objective function `f: ℝⁿ → ℝ`
- Constraints `g_i(x) ≤ 0`, `h_j(x) = 0`
- Search bounds `x ∈ [a, b]ⁿ`

**Output**: 
- Field `Φ(x,t) ∈ ℂ` in M⁶
- Adaptive metric `g_ij(x)`

**Mapping Strategy**:

```
Option 1: Boltzmann Distribution
Φ(x) = exp(-β·f(x))
where β = temperature parameter

Option 2: Inverted Landscape
Φ(x) = 1/(1 + f(x) - f_min)
where f_min = current best

Option 3: RBF Approximation
Φ(x) = Σᵢ cᵢ·ψ(||x - xᵢ||_g)
where cᵢ = exp(-β·f(xᵢ))
```

**Data Structure**:
```c
typedef struct {
    // Objective function
    double (*objective)(const double* x, int n, void* params);
    void* obj_params;
    
    // Constraints
    int num_ineq;  // Number of g_i(x) ≤ 0
    int num_eq;    // Number of h_j(x) = 0
    double (*ineq_constraints)(const double* x, int n, int i, void* params);
    double (*eq_constraints)(const double* x, int n, int j, void* params);
    void* constraint_params;
    
    // Search space
    int dimension;
    double* lower_bounds;
    double* upper_bounds;
    
    // Mapping parameters
    double beta;  // Temperature
    double f_min; // Best value found
    
} ObjectiveLandscape;
```

**Algorithm**:
```c
void map_objective_to_field(ObjectiveLandscape* landscape, TCDE_Field* field) {
    // 1. Sample objective function
    int num_samples = 100;  // Initial sampling
    double** samples = latin_hypercube_sampling(landscape->dimension, 
                                                num_samples,
                                                landscape->lower_bounds,
                                                landscape->upper_bounds);
    
    // 2. Evaluate objective at samples
    for (int i = 0; i < num_samples; i++) {
        double f_val = landscape->objective(samples[i], landscape->dimension, 
                                           landscape->obj_params);
        
        // 3. Create RBF center
        float coords[6];
        embed_solution_to_6d(samples[i], landscape->dimension, coords);
        TCDE_Point point = TCDE_CreatePoint(6, coords);
        
        // 4. Coefficient from objective value
        double phi_val = exp(-landscape->beta * (f_val - landscape->f_min));
        TCDE_Complex coeff = phi_val + 0.0 * I;
        
        // 5. Add to field
        TCDE_AddCenter(field, &point, coeff, 0.1);
    }
}
```

**Complexity**: O(n×d) where n = samples, d = dimension

---

### 2. Field Evolution Engine

**Purpose**: Evolve field toward optimal solution

**Evolution Equation**:
```
∂Φ/∂t = D∇²Φ - ∇(f·Φ) + λ·P_constraints(Φ)
```

**Components**:
- `D∇²Φ`: Diffusion (exploration)
- `-∇(f·Φ)`: Drift toward low f(x) (exploitation)
- `P_constraints`: Constraint projection

**Discretization**:
```
Φ(t+Δt) = Φ(t) + Δt·[D·L·Φ(t) - ∇f·Φ(t) + λ·P(Φ(t))]
```

**Implementation**:
```c
typedef struct {
    double diffusion;      // D coefficient
    double drift_strength; // Gradient weight
    double constraint_penalty; // λ
    double dt;            // Time step
    int max_iterations;   // Stopping criterion
    double tolerance;     // Convergence threshold
} EvolutionParams;

void evolve_optimization_field(TCDE_Field* field, 
                               ObjectiveLandscape* landscape,
                               EvolutionParams* params) {
    for (int iter = 0; iter < params->max_iterations; iter++) {
        // 1. Compute Laplacian (diffusion)
        compute_field_laplacian(field);
        
        // 2. Compute objective gradient at centers
        for (int i = 0; i < field->num_centers; i++) {
            double* x = extract_solution_from_center(field, i);
            double* grad_f = compute_objective_gradient(landscape, x);
            
            // Update coefficient
            TCDE_Complex ci = field->centers[i].coeff;
            TCDE_Complex drift = -params->drift_strength * 
                                 (grad_f[0] + I * grad_f[1]) * ci;
            
            field->centers[i].coeff += params->dt * drift;
            
            free(grad_f);
            free(x);
        }
        
        // 3. Apply diffusion
        apply_diffusion(field, params->diffusion, params->dt);
        
        // 4. Project onto constraints
        project_onto_constraints(field, landscape, params->constraint_penalty);
        
        // 5. Check convergence
        if (check_convergence(field, params->tolerance)) {
            break;
        }
        
        // 6. Adapt metric
        adapt_metric_to_landscape(field, landscape);
    }
}
```

**Complexity**: O(k²×iter) where k = centers, iter = iterations

---

### 3. Constraint Handling Module

**Purpose**: Encode constraints in geometry

**Approach 1: Penalty Method**
```
Φ_constrained = Φ · exp(-λ·Σᵢ max(0, g_i(x))²)
```

**Approach 2: Metric Adaptation**
```
g_ij(x) = g_ij⁰ · (1 + α·Σᵢ max(0, g_i(x)))
```
Metric expands in infeasible regions → repels field

**Approach 3: Projection**
```
x_feasible = argmin ||x - x_current||²  s.t.  g_i(x) ≤ 0
```

**Implementation**:
```c
typedef enum {
    CONSTRAINT_PENALTY,
    CONSTRAINT_METRIC,
    CONSTRAINT_PROJECT
} ConstraintMethod;

void handle_constraints(TCDE_Field* field, 
                       ObjectiveLandscape* landscape,
                       ConstraintMethod method,
                       double penalty) {
    switch (method) {
        case CONSTRAINT_PENALTY:
            apply_penalty_method(field, landscape, penalty);
            break;
            
        case CONSTRAINT_METRIC:
            adapt_metric_for_constraints(field, landscape, penalty);
            break;
            
        case CONSTRAINT_PROJECT:
            project_centers_to_feasible(field, landscape);
            break;
    }
}

void apply_penalty_method(TCDE_Field* field, 
                         ObjectiveLandscape* landscape,
                         double lambda) {
    for (int i = 0; i < field->num_centers; i++) {
        double* x = extract_solution_from_center(field, i);
        
        // Compute constraint violation
        double violation = 0.0;
        for (int j = 0; j < landscape->num_ineq; j++) {
            double g_j = landscape->ineq_constraints(x, landscape->dimension, 
                                                     j, landscape->constraint_params);
            if (g_j > 0) {
                violation += g_j * g_j;
            }
        }
        
        // Apply penalty to coefficient
        double penalty_factor = exp(-lambda * violation);
        field->centers[i].coeff *= penalty_factor;
        
        free(x);
    }
}
```

---

### 4. Solution Extraction Module

**Purpose**: Extract optimal solution from field

**Strategy 1: Maximum Field Value**
```
x* = argmax_x |Φ(x)|
```

**Strategy 2: Weighted Centroid**
```
x* = Σᵢ wᵢ·xᵢ / Σᵢ wᵢ
where wᵢ = |cᵢ|²
```

**Strategy 3: Best Center**
```
x* = xᵢ where i = argmin_i f(xᵢ)
```

**Implementation**:
```c
typedef struct {
    double* solution;      // x* ∈ ℝⁿ
    double objective_value; // f(x*)
    double field_value;    // |Φ(x*)|
    bool is_feasible;      // Constraints satisfied?
    int iterations;        // Convergence iterations
    double convergence_rate; // Energy decrease rate
} OptimizationResult;

OptimizationResult extract_solution(TCDE_Field* field, 
                                   ObjectiveLandscape* landscape) {
    OptimizationResult result;
    result.solution = malloc(landscape->dimension * sizeof(double));
    
    // Find center with best objective value
    double best_f = INFINITY;
    int best_idx = -1;
    
    for (int i = 0; i < field->num_centers; i++) {
        double* x = extract_solution_from_center(field, i);
        double f_val = landscape->objective(x, landscape->dimension, 
                                           landscape->obj_params);
        
        if (f_val < best_f) {
            best_f = f_val;
            best_idx = i;
            memcpy(result.solution, x, landscape->dimension * sizeof(double));
        }
        
        free(x);
    }
    
    result.objective_value = best_f;
    result.field_value = cabs(field->centers[best_idx].coeff);
    result.is_feasible = check_feasibility(result.solution, landscape);
    
    return result;
}
```

---

### 5. Adaptive Metric Module

**Purpose**: Adjust metric based on problem structure

**Hessian-Based Adaptation**:
```
g_ij(x) = |H_ij(f)|  (Hessian of objective)
```
Metric reflects curvature of objective landscape

**Constraint-Based Adaptation**:
```
g_ij(x) = δ_ij + α·Σₖ ∇g_k(x)ᵢ·∇g_k(x)ⱼ
```
Metric encodes constraint gradients

**Implementation**:
```c
void adapt_metric_to_landscape(TCDE_Field* field, 
                               ObjectiveLandscape* landscape) {
    // Sample field to estimate Hessian
    int n_samples = 50;
    double** samples = sample_field_region(field, n_samples);
    
    // Compute average Hessian
    double** H_avg = allocate_matrix(landscape->dimension, landscape->dimension);
    
    for (int s = 0; s < n_samples; s++) {
        double** H = compute_hessian(landscape->objective, samples[s], 
                                     landscape->dimension);
        
        // Accumulate
        for (int i = 0; i < landscape->dimension; i++) {
            for (int j = 0; j < landscape->dimension; j++) {
                H_avg[i][j] += fabs(H[i][j]) / n_samples;
            }
        }
        
        free_matrix(H);
    }
    
    // Update metric (project to 6D)
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (i < landscape->dimension && j < landscape->dimension) {
                field->metric->g[i][j] = H_avg[i][j];
            } else {
                field->metric->g[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }
    
    // Ensure positive definiteness
    regularize_metric(field->metric, 0.01);
    
    free_matrix(H_avg);
    free_samples(samples, n_samples);
}
```

---

## Data Structures

### Optimizer State

```c
typedef struct {
    // Problem definition
    ObjectiveLandscape* landscape;
    
    // TCDE field
    TCDE_Field* field;
    
    // Evolution parameters
    EvolutionParams* params;
    
    // Current state
    OptimizationResult current_best;
    int iteration;
    double* energy_history;
    
    // Convergence tracking
    bool converged;
    double convergence_threshold;
    
    // Adaptation
    ConstraintMethod constraint_method;
    bool adaptive_metric;
    
} TCDEOptimizer;
```

### Benchmark Problem

```c
typedef struct {
    const char* name;
    int dimension;
    double (*function)(const double* x, int n);
    double* lower_bounds;
    double* upper_bounds;
    double known_optimum;  // For validation
    double* optimum_location;
} BenchmarkProblem;

// Standard benchmarks
BenchmarkProblem rosenbrock = {
    .name = "Rosenbrock",
    .dimension = 2,
    .function = rosenbrock_function,
    .lower_bounds = (double[]){-5.0, -5.0},
    .upper_bounds = (double[]){5.0, 5.0},
    .known_optimum = 0.0,
    .optimum_location = (double[]){1.0, 1.0}
};
```

---

## Processing Pipeline

### Initialization

```c
TCDEOptimizer* create_optimizer(ObjectiveLandscape* landscape,
                               EvolutionParams* params) {
    TCDEOptimizer* opt = malloc(sizeof(TCDEOptimizer));
    
    // Store problem
    opt->landscape = landscape;
    opt->params = params;
    
    // Create TCDE field
    opt->field = TCDE_CreateField(100, 2.5);  // 100 centers
    
    // Map objective to field
    map_objective_to_field(landscape, opt->field);
    
    // Initialize state
    opt->iteration = 0;
    opt->converged = false;
    opt->convergence_threshold = params->tolerance;
    opt->energy_history = malloc(params->max_iterations * sizeof(double));
    
    // Extract initial best
    opt->current_best = extract_solution(opt->field, landscape);
    
    return opt;
}
```

### Optimization Loop

```c
OptimizationResult optimize(TCDEOptimizer* opt) {
    while (opt->iteration < opt->params->max_iterations && !opt->converged) {
        // 1. Evolve field one step
        evolve_optimization_field(opt->field, opt->landscape, opt->params);
        
        // 2. Handle constraints
        handle_constraints(opt->field, opt->landscape, 
                          opt->constraint_method, 
                          opt->params->constraint_penalty);
        
        // 3. Adapt metric (optional)
        if (opt->adaptive_metric && opt->iteration % 10 == 0) {
            adapt_metric_to_landscape(opt->field, opt->landscape);
        }
        
        // 4. Extract current best
        OptimizationResult current = extract_solution(opt->field, opt->landscape);
        
        // 5. Update if improved
        if (current.objective_value < opt->current_best.objective_value) {
            free(opt->current_best.solution);
            opt->current_best = current;
        } else {
            free(current.solution);
        }
        
        // 6. Track energy
        opt->energy_history[opt->iteration] = TCDE_ComputeEnergy(opt->field);
        
        // 7. Check convergence
        if (opt->iteration > 10) {
            double energy_change = fabs(opt->energy_history[opt->iteration] - 
                                       opt->energy_history[opt->iteration - 10]);
            if (energy_change < opt->convergence_threshold) {
                opt->converged = true;
            }
        }
        
        opt->iteration++;
    }
    
    return opt->current_best;
}
```

---

## Optimization Strategies

### Strategy 1: Exploration-Exploitation Balance

**Temperature Schedule**:
```c
void update_temperature(TCDEOptimizer* opt) {
    // Simulated annealing-like
    double T_0 = 1.0;
    double T_min = 0.01;
    double alpha = 0.95;
    
    opt->landscape->beta = 1.0 / (T_0 * pow(alpha, opt->iteration));
    opt->landscape->beta = fmax(opt->landscape->beta, 1.0 / T_min);
}
```

### Strategy 2: Multi-Start

**Parallel Fields**:
```c
OptimizationResult multi_start_optimize(ObjectiveLandscape* landscape,
                                       int num_starts) {
    OptimizationResult best;
    best.objective_value = INFINITY;
    
    for (int i = 0; i < num_starts; i++) {
        // Random initialization
        TCDEOptimizer* opt = create_optimizer_random(landscape);
        
        // Optimize
        OptimizationResult result = optimize(opt);
        
        // Track best
        if (result.objective_value < best.objective_value) {
            if (best.solution) free(best.solution);
            best = result;
        } else {
            free(result.solution);
        }
        
        destroy_optimizer(opt);
    }
    
    return best;
}
```

### Strategy 3: Adaptive Sampling

**Add Centers in Promising Regions**:
```c
void adaptive_sampling(TCDEOptimizer* opt) {
    // Find region with high field value
    double* x_high = find_high_field_region(opt->field);
    
    // Sample around it
    for (int i = 0; i < 10; i++) {
        double* x_new = perturb_solution(x_high, 0.1);
        
        // Evaluate
        double f_new = opt->landscape->objective(x_new, 
                                                opt->landscape->dimension,
                                                opt->landscape->obj_params);
        
        // Add if promising
        if (f_new < opt->current_best.objective_value * 1.1) {
            add_center_at_solution(opt->field, x_new, f_new, opt->landscape);
        }
        
        free(x_new);
    }
    
    free(x_high);
}
```

---

## Performance Characteristics (Theoretical)

### Computational Complexity

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Field initialization | O(n×d) | n samples, d dimensions |
| Evolution step | O(k²) | k centers |
| Constraint check | O(k×c) | c constraints |
| Solution extraction | O(k) | Find best center |
| Metric adaptation | O(k×d²) | Hessian estimation |
| **Total per iteration** | **O(k²+k×d²)** | Dominated by evolution |

### Memory Requirements

| Component | Size | Notes |
|-----------|------|-------|
| Field | k × 64 bytes | k centers |
| Metric | 6×6 × 8 bytes | 288 bytes |
| History | iter × 8 bytes | Energy tracking |
| **Total** | **~6.4 KB** | For k=100 |

### Convergence Properties (Theoretical)

**Energy Decrease**:
```
E(t+Δt) ≤ E(t) - γ·||∇E||²
```
Guaranteed decrease (under conditions)

**Convergence Rate**:
```
||x(t) - x*|| ≤ C·exp(-λt)
```
Exponential convergence (for convex f)

---

## Next Steps

1. **Implementation Guide**: See [implementation-guide.md](./implementation-guide.md)
2. **Performance Comparison**: See [comparison.md](./comparison.md)
3. **Try POC**: See [demo/README.md](./demo/README.md) (when available)

---

**Status**: ✅ Architecture Documented  
**Complexity**: Well-Defined (O(k²) per iteration)  
**Ready For**: Implementation and validation
